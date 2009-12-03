/**********************************************************************
*<
FILE: ExporterCore.cpp

DESCRIPTION:	Core Export helper routines

CREATED BY: tazpn (Theo)

HISTORY: 

*>	Copyright (c) 2006, All Rights Reserved.
**********************************************************************/
#include "stdafx.h"
#include "writer.h"
#include "exporterBase.h"
#include "MMHFormat.h"
#include "MSHFormat.h"
#include "../common/appsettings.h"
#include "../common/fbxconvert.h"
#include "../common/DAXmlWriter.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
using namespace DAO::MESH;

static const char *MMHExport = "MMHExport";
static const char *MSHExport = "MSHExport";
static const char *MSHCommon = "MSHCommon";

typedef std::map<std::string, KArrayKFbxNode, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;

typedef std::map<std::string, GFFFile, ltstr> NameFileMap;
typedef NameFileMap::iterator NameFileItr;

class MMHExportImpl : public ExporterBase
{
	typedef ExporterBase base;
public:
	MMHExportImpl(DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, LPCSTR fname);

	bool DoExport();
	bool CalcBoundingBox(KFbxNode* lNode, KFbxVector4& bmin, KFbxVector4& bmax );
	void ExportObject(KFbxNode* lNode, LPCTSTR name = NULL);
	void ExportNode(KFbxNode* lNode, LPCTSTR name = NULL, bool bbox = false);
	void ExportSkeleton( KFbxSkeleton* lSkel, LPCSTR name = NULL);
	void StartExportNode(KFbxNode* lNode, LPCTSTR name = NULL);
	void EndExportNode(KFbxNode* lNode);
	void ExportMesh( KFbxMesh* lMesh, LPCSTR name = NULL);

	void ExportNodeTransform( KFbxNode* lNode, bool global = false);
	template <class T>
	T GetProp(LPCTSTR name, T defaultValue)
	{
		return mmhExportSettings.GetSetting(name, defaultValue);
	}

	Text GetDiffuseMaterialName(KFbxLayerContainer* lLayerContainer);

	void ExportMaterialObject( KFbxMesh* pMesh, LPCTSTR matName, LPCTSTR texName);
	bool GetDiffuseMaterialAndTextureName(KFbxLayerContainer* lLayerContainer, Text &matName, Text& texName);

	int GetFirstFreeBone();
	void BuildBoneMap(KFbxNode* lNode);

	void AddBoneIndex( KFbxNode* lNode, bool force );

	void UpdateTangentSpace(KFbxMesh *pMesh);
	bool NeedTangentSpace(KFbxMesh *pMesh);

	void RecomputeMeshVertices( KFbxMesh* pMesh );
	bool HasSharedVertices( KFbxMesh* pMesh );

	void RescaleMesh( KFbxMesh* pMesh, KFbxXMatrix & lm );

	//////////////////////////////////////////////////////////////////////////

	AppSettings mmhExportSettings, mshExportSettings;
	DAOXmlWriterPtr mmhwriter, mshwriter;
	DAOStreamPtr mmhfile, mshfile;
	Text mshfname, mmhfname;
	bool flipUV;
	bool allowSharedVertices;

	typedef std::map<KFbxNode*, int> Node2IndexMap;
	typedef std::vector<KFbxNode*> Index2NodeMap;

	Node2IndexMap node2Index;
	Index2NodeMap index2Node;
	Text currentId;
	int meshId;
	int nodeId;

	Text GetNextId();

	void CollapseTransforms( KFbxNode* lRoot, KFbxXMatrix gm );

};

template <class T>
inline fbxVectorTemplate3<T> CalcDim(const fbxVectorTemplate3<T>& arg1, const fbxVectorTemplate3<T>& arg2)
{
	return fbxVectorTemplate3<T>((T)abs(arg2[0]-arg1[0]), (T)abs(arg2[1]-arg1[1]), (T)abs(arg2[2]-arg1[2]) );
}

template <class T>
inline fbxVectorTemplate3<T> CalcMid(const fbxVectorTemplate3<T>& arg1, const fbxVectorTemplate3<T>& arg2)
{
	return fbxVectorTemplate3<T>((T)((arg2[0]+arg1[0])/(T)2), (T)((arg2[1]+arg1[1])/(T)2), (T)((arg2[2]+arg1[2])/(T)2) );
}
template <class T>
inline fbxVectorTemplate4<T> CalcDim(const fbxVectorTemplate4<T>& arg1, const fbxVectorTemplate4<T>& arg2)
{
	return fbxVectorTemplate4<T>((T)abs(arg2[0]-arg1[0]), (T)abs(arg2[1]-arg1[1]), (T)abs(arg2[2]-arg1[2]), (T)abs(arg2[3]-arg1[3]) );
}

template <class T>
inline fbxVectorTemplate4<T> CalcMid(const fbxVectorTemplate4<T>& arg1, const fbxVectorTemplate4<T>& arg2)
{
	return fbxVectorTemplate4<T>((T)((arg2[0]+arg1[0])/(T)2), (T)((arg2[1]+arg1[1])/(T)2), (T)((arg2[2]+arg1[2])/(T)2), (T)((arg2[3]+arg1[3])/(T)2)  );
}



bool DAOWriter::WriteMMH( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, LPCSTR filename )
{
	KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
	bool            lIsAScene = (lScene != NULL);
	bool            lResult = false;
	if(lIsAScene)
	{
		MMHExportImpl impl(this, lScene, pStreamOptions, filename);
		lResult = impl.DoExport();
	}
	return lResult;
}


// Recursive function to get a node's global default position.
// As a prerequisite, parent node's default local position must be already set.
KFbxXMatrix GetGlobalDefaultPosition(KFbxNode* pNode)
{
	KFbxXMatrix lLocalPosition;
	KFbxXMatrix lGlobalPosition;
	KFbxXMatrix lParentGlobalPosition;

	KFbxVector4 lT, lR, lS;
	lLocalPosition.SetT(pNode->GetDefaultT(lT));
	lLocalPosition.SetR(pNode->GetDefaultR(lR));
	lLocalPosition.SetS(pNode->GetDefaultS(lS));

	if (pNode->GetParent())
	{
		lParentGlobalPosition = GetGlobalDefaultPosition(pNode->GetParent());
		lGlobalPosition = lParentGlobalPosition * lLocalPosition;
	}
	else
	{
		lGlobalPosition = lLocalPosition;
	}

	return lGlobalPosition;
}

// Function to get a node's global default position.
// As a prerequisite, parent node's default local position must be already set.
void SetGlobalDefaultPosition(KFbxNode* pNode, KFbxXMatrix pGlobalPosition)
{
	KFbxXMatrix lLocalPosition;
	KFbxXMatrix lParentGlobalPosition;

	if (pNode->GetParent())
	{
		lParentGlobalPosition = GetGlobalDefaultPosition(pNode->GetParent());
		lLocalPosition = lParentGlobalPosition.Inverse() * pGlobalPosition;
	}
	else
	{
		lLocalPosition = pGlobalPosition;
	}

	pNode->SetDefaultT(lLocalPosition.GetT());
	pNode->SetDefaultR(lLocalPosition.GetR());
	pNode->SetDefaultS(lLocalPosition.GetS());
}


MMHExportImpl::MMHExportImpl( DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, LPCSTR filename ) 
	: base(owner, scene, options, filename)
	, mmhExportSettings("MMHExport")
	, mshExportSettings("MSHExport")
	, mmhwriter(NULL), mshwriter(NULL)
	, mmhfile(NULL), mshfile(NULL)
{
	Text name;
	currentId = "_a";
	meshId = nodeId = 1;

	ResourceManager::LoadSettings(mmhExportSettings);
	ResourceManager::LoadSettings(mshExportSettings);
	flipUV = mshExportSettings.GetSetting<bool>("FlipUV", true);
	allowSharedVertices = mshExportSettings.GetSetting<bool>("AllowSharedVertices", false);

	mmhfname = filename;
	LPCTSTR ext = PathFindExtension(mmhfname);
	if (ext != NULL && _tcsicmp(ext, ".xml") == 0)
		PathRemoveExtension(mmhfname);
	PathRemoveExtension(mmhfname);
	mmhfname.append(".mmh");
	name = mmhfname; name.append(".xml");
	mmhfile.swap( DAOStream::Create(name, false) );
	if (mmhfile.isNull())
		KFbxLog::LogError("Could not create file: %s", mmhfname.c_str());
	else
		KFbxLog::LogInfo("Exporting: %s", mmhfname.c_str() );


	mshfname = mmhfname;
	PathRemoveExtension(mshfname);
	mshfname.append(".msh");
	name = mshfname; name.append(".xml");
	mshfile.swap(DAOStream::Create(name, false));
	if (mshfile.isNull())
		KFbxLog::LogError("Could not create file: %s", mshfname.c_str());
	else
		KFbxLog::LogInfo("Exporting: %s", mshfname.c_str() );
}

bool MMHExportImpl::CalcBoundingBox(KFbxNode* lNode, KFbxVector4& bmin, KFbxVector4& bmax )
{
	bool ok = false;
	if ( KFbxNodeAttribute *pAttr = lNode->GetNodeAttribute() )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
		switch (lAttributeType)
		{
		case KFbxNodeAttribute::eMESH:      
		case KFbxNodeAttribute::eNURB:      
		case KFbxNodeAttribute::ePATCH:     
			if ( KFbxGeometryBase *pBase = static_cast<KFbxGeometryBase *>(pAttr) )
			{
				pBase->ComputeBBox();
				fbxDouble3 min = pBase->BBoxMin.Get();
				fbxDouble3 max = pBase->BBoxMax.Get();
				for (int i=0;i<3;++i){
					if (min[i] < bmin[i]) bmin[i] = min[i];
					if (max[i] > bmax[i]) bmax[i] = max[i];
				}
				ok = true;					
			}
			break;
		}   
	}

	KFbxMesh* lMesh = (KFbxMesh*) lNode->GetNodeAttribute ();

    for( int i = 0; i < lNode->GetChildCount(); i++)
    {
        ok |= CalcBoundingBox(lNode->GetChild(i), bmin, bmax);
    }
	return ok;
}

bool MMHExportImpl::DoExport()
{
	if (mmhfile.isNull() || mshfile.isNull())
		return false;

	KFbxGlobalSettings& globals = lScene->GetGlobalSettings();

	KFbxAxisSystem SceneAxisSystem = globals.GetAxisSystem();
	KFbxSystemUnit::KFbxUnitConversionOptions options;
	options.mConvertLightIntensity = true;
	options.mConvertRrsNodes = true;

	KFbxAxisSystem OurAxisSystem(KFbxAxisSystem::ZAxis, KFbxAxisSystem::ParityEven, KFbxAxisSystem::RightHanded);
	if( SceneAxisSystem != OurAxisSystem ) {
		OurAxisSystem.ConvertScene(lScene);
	}

	double scaleFactor = GetProp<double>("ScaleFactor", 1.0);
	// Convert Unit System to what is used in this example, if needed
	KFbxSystemUnit SceneSystemUnit = lScene->GetGlobalSettings().GetSystemUnit();
	if( SceneSystemUnit.GetScaleFactor() != scaleFactor )
	{
		KFbxSystemUnit OurSystemUnit(scaleFactor);
		OurSystemUnit.ConvertScene(lScene);
	}
	else
	{
		KFbxSystemUnit OurSystemUnit(scaleFactor);
		OurSystemUnit.ConvertScene(lScene);
	}


	bool ok = true;
	mmhwriter = DAOXmlWriter::Create(this->mmhfile);
	mmhwriter->SetIndent(1);
	mmhwriter->SetIndentString("  ");
	mmhwriter->StartDocument(NULL, "utf-8", NULL);
	mmhwriter->StartElement("ModelHierarchy");
	mmhwriter->WriteAttribute("Name", mmhfname);
	mmhwriter->WriteAttribute("ModelDataName", mshfname);

	mshwriter = DAOXmlWriter::Create(this->mshfile);
	mshwriter->SetIndent(1);
	mshwriter->SetIndentString("  ");
	mshwriter->StartDocument(NULL, "utf-8", NULL);
	mshwriter->StartElement("ModelMeshData");
	mshwriter->WriteAttribute("Name", mshfname);
	mshwriter->WriteAttribute("Version", "1");

	KFbxNode* lRoot = lScene->GetRootNode();
	BuildBoneMap(lRoot);

	// Skip scene if first and only child is named GOB
	if (lRoot->GetChildCount() == 1) {
		//LPCSTR name = lRoot->GetChildName(0);
		KFbxNode* lNode = lRoot->GetChild(0);
		if (lNode != NULL && _tcsicmp(lNode->GetName(), "GOB") == 0)
			lRoot = lRoot->GetChild(0);
	}
	KFbxXMatrix gm = lRoot->GetGlobalFromDefaultTake();
	//KFbxXMatrix wm = lRoot->GetGlobalState();
	CollapseTransforms( lRoot, gm );

	ExportNode( lRoot, "GOB", true );

	mshwriter->EndElement(); // ModelMeshData
	mshwriter->EndDocument();
	//mshwriter->Flush();
	mshwriter.swap( DAOXmlWriterPtr(NULL) );
	this->mshfile->Close();

	mmhwriter->EndElement(); // ModelHierarchy
	mmhwriter->EndDocument();
	//mmhwriter->Flush();
	mmhwriter.swap( DAOXmlWriterPtr(NULL) );
	this->mmhfile->Close();

	return true;
}

void MMHExportImpl::ExportObject(KFbxNode* lNode, LPCTSTR name)
{
	LPCTSTR cname = lNode->GetName();
	if ( KFbxNodeAttribute *pAttr = lNode->GetNodeAttribute() )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
		switch (lAttributeType)
		{
		case KFbxNodeAttribute::eUNIDENTIFIED:
			KFbxLog::LogWarn("Skipping '%s' as type is unidentified.", cname );  
			break;

		case KFbxNodeAttribute::eNULL:
			KFbxLog::LogVerbose("Exporting NULL '%s' as Node", cname );  
			ExportNode(lNode, NULL);
			break;

		case KFbxNodeAttribute::eMARKER:
			KFbxLog::LogWarn("Skipping '%s' as MARKER is not supported.", cname );  
			break;

		case KFbxNodeAttribute::eSKELETON:
			KFbxLog::LogVerbose("Exporting Skeleton '%s' as Bone", cname );  
			ExportSkeleton((KFbxSkeleton*)pAttr, NULL);
			break;

		case KFbxNodeAttribute::eMESH:
			if ( KFbxMesh* pMesh = (KFbxMesh*)pAttr )
			{
				if (!pMesh->IsTriangleMesh())
				{
					KFbxLog::LogInfo("Mesh '%s' is not a triangle mesh.  Triangulating...", cname );
					KFbxGeometryConverter lConverter(mManager);
					lConverter.TriangulateInPlace(lNode);
				}
				KFbxLog::LogVerbose("Exporting Mesh '%s'", cname );
				ExportMesh((KFbxMesh*)lNode->GetNodeAttribute(), NULL);

			}
			break;

		case KFbxNodeAttribute::eNURB:					
			KFbxLog::LogWarn("NURB is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
			{
				KFbxGeometryConverter lConverter(mManager);
				lConverter.TriangulateInPlace(lNode);
				ExportMesh((KFbxMesh*)lNode->GetNodeAttribute(), NULL);
			}
			break;

		case KFbxNodeAttribute::ePATCH:
			KFbxLog::LogWarn("PATCH is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
			{
				KFbxGeometryConverter lConverter(mManager);
				lConverter.TriangulateInPlace(lNode);
				ExportMesh((KFbxMesh*)lNode->GetNodeAttribute(), NULL);
			}
			break;

		case KFbxNodeAttribute::eCAMERA:
			KFbxLog::LogVerbose("Skipping '%s' as CAMERA is not supported.", cname );  
			break;

		case KFbxNodeAttribute::eCAMERA_STEREO:
			KFbxLog::LogVerbose("Skipping '%s' as CAMERA_STEREO is not supported.", cname );  
			break;

		case KFbxNodeAttribute::eCAMERA_SWITCHER:
			KFbxLog::LogVerbose("Skipping '%s' as CAMERA_SWITCHER is not supported.", cname );  
			break;

		case KFbxNodeAttribute::eLIGHT:
			KFbxLog::LogWarn("Skipping '%s' as LIGHT is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eOPTICAL_REFERENCE:
			KFbxLog::LogWarn("Skipping '%s' as OPTICAL_REFERENCE is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eOPTICAL_MARKER:
			KFbxLog::LogWarn("Skipping '%s' as OPTICAL_MARKER is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eNURBS_CURVE:
			KFbxLog::LogWarn("Skipping '%s' as NURBS_CURVE is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eTRIM_NURBS_SURFACE:
			KFbxLog::LogWarn("Skipping '%s' as TRIM_NURBS_SURFACE is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eBOUNDARY:
			KFbxLog::LogWarn("Skipping '%s' as BOUNDARY is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eNURBS_SURFACE:
			KFbxLog::LogWarn("Skipping '%s' as NURBS_SURFACE is not supported.", cname );  
			break;
		case KFbxNodeAttribute::eSHAPE:
			KFbxLog::LogWarn("Skipping '%s' as SHAPE is not supported.", cname );  
			break;

		case KFbxNodeAttribute::eLODGROUP:
			KFbxLog::LogWarn("Skipping '%s' as LODGROUP is not supported.", cname );  
			break;

		case KFbxNodeAttribute::eSUBDIV:
			KFbxLog::LogWarn("Skipping '%s' as SUBDIV is not supported.", cname );  
			break;
		default:
			KFbxLog::LogWarn("Skipping unknown attribute for object '%s'.", cname );  
			break;
		}
	}
	else
	{
		KFbxLog::LogVerbose("Exporting '%s' as Node", cname );  
		ExportNode(lNode, NULL);
	}
}

void MMHExportImpl::ExportNode(KFbxNode* lNode, LPCTSTR name, bool root)
{
	StartExportNode(lNode, NULL);
	// Calc Bounding Box
	KFbxVector4 min, max;
	if ( root && CalcBoundingBox(lNode, min, max) )
	{
		Text sstr;
		sstr.AppendFormat("%g %g %g %g %g %g %g %g"
			, (double)min[0], (double)min[1], (double)min[2], 1.0
			, (double)max[0], (double)max[1], (double)max[2], 1.0);
		mmhwriter->StartElement("BoundingBox");
		mmhwriter->WriteCDATA(sstr);
		mmhwriter->EndElement();
	}	
	ExportNodeTransform(lNode, root);
	EndExportNode(lNode);
}

void MMHExportImpl::ExportSkeleton( KFbxSkeleton* lSkel, LPCSTR name )
{
	KFbxNode *lNode = lSkel->GetNode();
	StartExportNode(lNode, NULL);
	ExportNodeTransform(lNode);
	EndExportNode(lNode);
}

Text MMHExportImpl::GetDiffuseMaterialName(KFbxLayerContainer* lLayerContainer)
{
	Text retval;
	if (lLayerContainer){
		KFbxNode* pNode = lLayerContainer->GetNode();
		int lNbMat = pNode->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
		for (int lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++){
			KFbxSurfaceMaterial *lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex));
			if(lMaterial){                                                         
				KFbxProperty lProperty = lMaterial->FindProperty(KFbxSurfaceMaterial::sDiffuse);
				if(lProperty.IsValid()){
					retval = lProperty.GetName();
					break;
				}
			}
		}
	} 
	return retval;
}

bool MMHExportImpl::GetDiffuseMaterialAndTextureName(KFbxLayerContainer* lLayerContainer, Text &matName, Text& texName)
{
	if (lLayerContainer){
		KFbxNode* pNode = lLayerContainer->GetNode();
		int lNbMat = pNode->GetSrcObjectCount(KFbxSurfaceMaterial::ClassId);
		for (int lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++){
			KFbxSurfaceMaterial *lMaterial = KFbxCast <KFbxSurfaceMaterial>(pNode->GetSrcObject(KFbxSurfaceMaterial::ClassId, lMaterialIndex));
			if(lMaterial){                                                         
				KFbxProperty lProperty = lMaterial->FindProperty(KFbxSurfaceMaterial::sDiffuse);
				if(lProperty.IsValid()){
					matName = lMaterial->GetName();
					int lNbTex = lProperty.GetSrcObjectCount(KFbxTexture::ClassId);
					for (int lTextureIndex = 0; lTextureIndex < lNbTex; lTextureIndex++){
						KFbxTexture* lTexture = KFbxCast <KFbxTexture> (lProperty.GetSrcObject(KFbxTexture::ClassId, lTextureIndex)); 
						if(lTexture) {
							KString lFileName = lTexture->GetFileName();
							texName = lFileName;
							return true;
						}
					}
				}
			}
		}
	} 
	return false;
}


struct SkinWeight {
	int index;
	float weight;
};

namespace std {
template<>
struct less<SkinWeight> : public binary_function<SkinWeight, SkinWeight, bool>
{
	bool operator()(const SkinWeight& lhs, const SkinWeight& rhs) {
		if (lhs.weight == 0.0) {
			if (rhs.weight == 0.0) {
				return rhs.index < lhs.index;
			} else {
				return true;
			}
			return false;
		} else if ( rhs.weight == lhs.weight ) {
			return lhs.index < rhs.index;
		} else {
			return rhs.weight < lhs.weight;
		}
	}
};
struct equalsSkinWeightIndex : binary_function<SkinWeight, int, bool>
{
	bool operator()(SkinWeight& lhs, int rhs) const { return lhs.index == rhs; }
	bool operator()(int lhs, const SkinWeight& rhs) const { return lhs == rhs.index; }
	bool operator()(int lhs, int rhs) const { return lhs == rhs; }
};
struct equalsInt : binary_function<int, int, bool>
{
	bool operator()(int lhs, int rhs) const { return lhs == rhs; }
};
}

typedef vector<SkinWeight> SkinWeightList;
typedef vector<SkinWeightList> BoneWeightList;

void MMHExportImpl::ExportMesh( KFbxMesh* pMesh, LPCSTR name )
{
	TCHAR buffer[32];
	if (pMesh == NULL) return;
	KFbxNode* lNode = pMesh->GetNode();

	bool exportNode = ( lNode->GetChildCount() > 0 );
	if (lNode->GetChildCount() > 0)
	{
		StartExportNode(lNode, NULL);
		ExportNodeTransform(lNode);
	}

	if (name == NULL || name[0] == 0) name = pMesh->GetName();
	if ((name == NULL || name[0] == 0) && lNode != NULL) name = lNode->GetName();
	if (name == NULL || name[0] == 0)
	{
		sprintf(buffer, "Mesh%02d", meshId++);
		name = buffer;
	}

	if (!allowSharedVertices)
	{
		if (NULL == pMesh->GetLayer(0)->GetNormals())
			pMesh->ComputeVertexNormals();
		if (HasSharedVertices(pMesh))
		{
			try
			{
				KFbxLog::LogDebug("Splitting mesh '%s'", name);
				pMesh->SplitPoints();
			}
			catch (...)
			{
				KFbxLog::LogWarn("Splitting mesh '%s' failed.  Trying alternate algorithm.", name);
				RecomputeMeshVertices(pMesh);
			}
		}
		if (NeedTangentSpace(pMesh))
		{
			KFbxLog::LogVerbose("Updating Tangent Space for '%s'", name);
			UpdateTangentSpace(pMesh);		
		}
	}
	else
	{
		if (NULL == pMesh->GetLayer(0)->GetNormals())
			pMesh->ComputeVertexNormals();
		if (NeedTangentSpace(pMesh))
		{
			KFbxLog::LogVerbose("Updating Tangent Space for '%s'", name);
			UpdateTangentSpace(pMesh);		
		}
	}
	
	pMesh->ComputeBBox();
	fbxDouble3 bmin = pMesh->BBoxMin.Get();
	fbxDouble3 bmax = pMesh->BBoxMax.Get();
	fbxDouble3 bdim = CalcDim(bmin, bmax);
	fbxDouble3 bmid = CalcMid(bmin, bmax);

	// Calculate 
	Text istr, wstr, busedStr;

	int lVertexCount = pMesh->GetControlPointsCount();
	int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
	if (lSkinCount > 0)
	{	
		vector<int> bonesUsed;
		if ( KFbxSkin* pSkin = (KFbxSkin *)pMesh->GetDeformer(0, KFbxDeformer::eSKIN) ) {
			int lClusterCount = pSkin->GetClusterCount();
			BoneWeightList weights;
			weights.resize( lVertexCount );

			for (int j = 0; j != lClusterCount; ++j) 
			{
				KFbxCluster* lCluster = pSkin->GetCluster(j);
				KFbxNode* pBone = lCluster->GetLink();
				int iBoneIndex = 0;
				Node2IndexMap::iterator itr = node2Index.find(pBone);
				if (itr != node2Index.end())
				{
					iBoneIndex = (*itr).second;
					if (iBoneIndex < 0)
						iBoneIndex = ~iBoneIndex;
				}
				int iUsedBone = 0;
				vector<int>::iterator vitr = std::find_if(bonesUsed.begin(), bonesUsed.end(), std::bind2nd(equalsInt(), iBoneIndex));
				if ( bonesUsed.end() == vitr )
				{
					iUsedBone = bonesUsed.size();
					bonesUsed.push_back(iBoneIndex);
					busedStr.AppendFormat("%d ", iBoneIndex);
				}
				else
				{
					iUsedBone = (*vitr);
				}
				int lIndexCount = lCluster->GetControlPointIndicesCount();
				int* lIndices = lCluster->GetControlPointIndices();
				double* lWeights = lCluster->GetControlPointWeights();
				for (int i = 0; i < lIndexCount; ++i )
				{
					SkinWeight sw;
					int idx = lIndices[i];
					sw.index = iUsedBone;
					sw.weight = lWeights[i];
					SkinWeightList& bw = weights[idx];
					if ( bw.end() == std::find_if(bw.begin(), bw.end(), std::bind2nd(equalsSkinWeightIndex(), iUsedBone)) )
						bw.push_back(sw);					
				}

			}
			for (int i = 0; i != lVertexCount; ++i) {
				SkinWeightList& bw = weights[i];
				std::sort(bw.begin(), bw.end(), less<SkinWeight>());
				int k, n = min(4, bw.size());
				for (k=0; k<n; ++k) {
					istr.AppendFormat("%d ", bw[k].index);
					wstr.AppendFormat("%g ", bw[k].weight);
				}
				for ( ; k<4; ++k) istr.AppendFormat("0 "), wstr.AppendFormat("0 ");
				istr.append("\n"), wstr.append("\n");
			}
		}
		// Empty whitespace at the end can cause Graphicsprocessormmh to crash
		istr.Trim(), wstr.Trim(), busedStr.Trim();
	}

	{
		mmhwriter->StartElement("NodeMesh");
		mmhwriter->WriteAttribute( "Name", name);
		if (!busedStr.isNull())
			mmhwriter->WriteAttribute( "BonesUsed", busedStr);
		mmhwriter->WriteAttribute( "ID", GetNextId()); //????
		mmhwriter->WriteAttribute( "MeshGroupName", name);
		Text matName, texName;
		if (GetDiffuseMaterialAndTextureName(pMesh, matName, texName))
		{
			mmhwriter->WriteAttribute( "MaterialObject", matName);
			ExportMaterialObject(pMesh, matName, texName);
		}
		//mmhwriter->WriteAttribute( "CastRuntimeShadow", "1");
		//mmhwriter->WriteAttribute( "ReceiveRuntimeShadow", "1");
		//mmhwriter->WriteAttribute( "CastBakedShadow", "1");
		//mmhwriter->WriteAttribute( "ReceiveBakedShadow", "1");
		mmhwriter->WriteAttribute( "CutAway", "0");
		mmhwriter->WriteAttribute( "PunchThrough", "0");
		{
			mmhwriter->StartElement("Attribute");
			mmhwriter->WriteAttribute( "AttributeName", "BaseLight" );
			mmhwriter->WriteAttribute( "SourceName", "BaseLight" );
			mmhwriter->EndElement();
		}

		if (!exportNode)
			ExportNodeTransform(lNode);

		mmhwriter->EndElement();
	}


	mshwriter->StartElement("MeshGroup");
	mshwriter->WriteAttribute("Name", name);
	mshwriter->WriteAttribute("Optimize", "All");

	KFbxLayerElement::EMappingMode lMappingMode = KFbxLayerElement::eNONE;

	{
		KFbxVector4* pVerts = pMesh->GetControlPoints();

		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "POSITION");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					KFbxVector4 &v = pVerts[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
				KFbxVector4& v = pVerts[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}

	if (KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs())
	{
		lMappingMode = pUVs->GetMappingMode();

		KFbxLayerElementArrayTemplate<KFbxVector2>& uvs = pUVs->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "TEXCOORD");
		mshwriter->WriteAttribute("Type", "Float2");

		Text sstr;
		int lPolygonCount = pMesh->GetPolygonCount();
		for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
		{
			int lPolySize = pMesh->GetPolygonSize(lPolygon);
			for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
			{
				int iUV;
				//if (lMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX) {
				//	iUV = pMesh->GetTextureUVIndex(lPolygon, iPoly);
				//} else {
				//	iUV = pMesh->GetPolygonVertex(lPolygon, iPoly);
				//}
				iUV = pMesh->GetPolygonVertex(lPolygon, iPoly);
				KFbxVector2 uvw = uvs[iUV];
				if (uvw[0] < 0.0f || uvw[0] > 1.0f) uvw[0] -= floor(uvw[0]);
				if (uvw[1] < 0.0f || uvw[1] > 1.0f) uvw[1] -= floor(uvw[1]);
				if (flipUV) uvw[1] = 1.0f-uvw[1];
				sstr.AppendFormat("%g %g\n", (double)uvw[0], (double)uvw[1]);
			}
		}
		mshwriter->WriteCDATA(sstr);

		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementNormal const* pNorms = pMesh->GetLayer(0)->GetNormals())
	{
		lMappingMode = pNorms->GetMappingMode();

		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pNorms->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "NORMAL");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					KFbxVector4 &v = array[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
				KFbxVector4& v = array[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementTangent const* pTangents = pMesh->GetLayer(0)->GetTangents())
	{
		lMappingMode = pTangents->GetMappingMode();

		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pTangents->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "TANGENT");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					KFbxVector4 &v = array[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
				KFbxVector4& v = array[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementBinormal const* pBinorms = pMesh->GetLayer(0)->GetBinormals())
	{	
		lMappingMode = pBinorms->GetMappingMode();
		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pBinorms->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "BINORMAL");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					KFbxVector4 &v = array[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
				KFbxVector4& v = array[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (lSkinCount > 0)
	{	
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "BLENDINDICES");
		mshwriter->WriteAttribute("Type", "Short4");
		mshwriter->WriteCDATA(istr);
		mshwriter->EndElement(); // Data

		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "BLENDWEIGHT");
		mshwriter->WriteAttribute("Type", "Float4");
		mshwriter->WriteCDATA(wstr);
		mshwriter->EndElement(); // Data
	}
	{
		int lPolygonCount = pMesh->GetPolygonCount();
		int lVertices = 0;
		for (int lPolygon = 0; lPolygon < lPolygonCount; lPolygon++){
			lVertices += pMesh->GetPolygonSize(lPolygon);
		}
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("IndexCount", "%d", lVertices);
		mshwriter->WriteAttribute("IndexType", "Index32");
		mshwriter->WriteAttribute("Semantic", "Indices");
		Text sstr;
		int iCurVert = 0;
		for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
		{
			int lPolySize = pMesh->GetPolygonSize(lPolygon);
			for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
			{
				int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
				//sstr.AppendFormat( "%d ", iVert );
				sstr.AppendFormat( "%d ", iCurVert++ );
			}
			sstr.append('\n');
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	mshwriter->EndElement(); // MeshGroup

	if (exportNode) EndExportNode(lNode);
}

void MMHExportImpl::ExportMaterialObject( KFbxMesh* pMesh, LPCTSTR matName, LPCTSTR texPath )
{
	Text matFName = matName;
	matFName.append(".mao");
	DAOStreamPtr maofile( DAOStream::Create(matFName, false) );
	if (maofile.isNull())
		KFbxLog::LogError("Could not create file: %s", matFName.c_str());
	else
		KFbxLog::LogInfo("Exporting: %s", matFName.c_str() );

	DAOXmlWriterPtr writer = DAOXmlWriter::Create(maofile);
	writer->SetIndent(1);
	writer->SetIndentString("  ");
	writer->StartDocument(NULL, "utf-8", NULL);
	writer->StartElement("MaterialObject");
	writer->WriteAttribute("Name", matName);

	writer->StartElement("Material");
	writer->WriteAttribute("Name", "Prop.mat");
	writer->EndElement();
	writer->StartElement("DefaultSemantic");
	writer->WriteAttribute("Name", "Blend");
	writer->EndElement();

	FixedString<CHAR, MAX_PATH> texName = PathFindFileName(texPath);
	PathRenameExtension(texName, ".dds");

	writer->StartElement("Texture");
	writer->WriteAttribute("Name", "mml_tDiffuse");
	writer->WriteAttribute("ResName", texName);
	writer->EndElement();

	writer->EndElement();
	writer->EndDocument();
	//writer->Flush();
	writer.swap( DAOXmlWriterPtr(NULL) );
	maofile->Close();
}

void MMHExportImpl::BuildBoneMap(KFbxNode* lNode)
{
	AddBoneIndex(lNode, false);

	if ( KFbxNodeAttribute *pAttr = lNode->GetNodeAttribute() )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
		switch (lAttributeType)
		{
		case KFbxNodeAttribute::eMESH:
			if ( KFbxMesh*pMesh = (KFbxMesh*)pAttr )
			{
				int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
				for (int lSkinIdx=0; lSkinIdx < lSkinCount; ++lSkinIdx) {
					if ( KFbxSkin* pSkin = (KFbxSkin *)pMesh->GetDeformer(lSkinIdx, KFbxDeformer::eSKIN) ) {
						int lClusterCount = pSkin->GetClusterCount();
						for (int j = 0; j != lClusterCount; ++j) {
							KFbxCluster* lCluster = pSkin->GetCluster(j);
							if ( KFbxNode* pBone = lCluster->GetLink() )
							{
								AddBoneIndex(pBone, true);
							}
						}
					}
				}
			}
			break;
		}
	}
	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		BuildBoneMap(lNode->GetChild(i));
	}
}

int MMHExportImpl::GetFirstFreeBone()
{
	int i=1;
	for ( ; i<index2Node.size(); ++i){
		KFbxNode* node = index2Node[i];
		if (node == NULL)
			return i;
	}
	return i;
}

void MMHExportImpl::AddBoneIndex( KFbxNode* lNode, bool force )
{
	LPCTSTR name = lNode->GetName();
	// Search for DAOBoneIndex custom property
	int iBoneIndex = ~0;
	if (!TryGetUserPropInt(lNode, "DAOBoneIndex", iBoneIndex))
	{
		if ( !TryGetPropInt(lNode, "DAOBoneIndex", iBoneIndex) )
		{
			iBoneIndex = ~0;
		}
	}
	if (iBoneIndex >= 0)
	{
		Node2IndexMap::iterator itr = node2Index.find(lNode);
		if (itr == node2Index.end())
		{
			node2Index[lNode] = iBoneIndex;
			if (iBoneIndex >= index2Node.size())
				index2Node.resize(iBoneIndex+1, NULL);
			// relocate a bone which also uses this index
			KFbxNode* pBone = index2Node[iBoneIndex];
			if (pBone != NULL && pBone != lNode)
			{
				int idx = GetFirstFreeBone();
				if (idx >= index2Node.size())
					index2Node.resize(idx+1, NULL);
				index2Node[idx] = pBone;
			}
			index2Node[iBoneIndex] = lNode;
		}
	}
	else if (force)
	{
		Node2IndexMap::iterator itr = node2Index.find(lNode);
		if (itr == node2Index.end())
		{
			int idx = GetFirstFreeBone();
			if (idx >= index2Node.size())
				index2Node.resize(idx+1, NULL);
			node2Index[lNode] = idx;
			index2Node[idx] = lNode;
		}
	}
}

Text MMHExportImpl::GetNextId()
{
	Text id = currentId;
	int len = currentId.Length();
	if (currentId[len-1] == 'z')
	{
		currentId.append('a');
	}
	else
	{
		++currentId[len-1];
	}
	return id;
}

static inline KFbxVector4 Scale(const KFbxVector4& v, float s )
{
	KFbxVector4 res(v);
	res *= s;
	return res;
}
static inline KFbxVector4 Normalize(const KFbxVector4& v )
{
	KFbxVector4 res(v);
	res.Normalize();
	return res;
}

bool MMHExportImpl::NeedTangentSpace(KFbxMesh *pMesh)
{
	KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs();
	KFbxLayerElementTangent* pTangentLayer = pMesh->GetLayer(0)->GetTangents();
	KFbxLayerElementBinormal* pBinormLayer = pMesh->GetLayer(0)->GetBinormals();

	return (pUVs != NULL) && ((pTangentLayer == NULL) || (pBinormLayer == NULL));
}

void MMHExportImpl::UpdateTangentSpace(KFbxMesh *pMesh)
{
	int lVertexCount = pMesh->GetControlPointsCount();
	KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs();
	if (pUVs == NULL)
		return;

	KFbxLayerElement::EMappingMode lMappingMode = pMesh->GetLayer(0)->GetUVs()->GetMappingMode();
	KFbxLayerElementArrayTemplate<KFbxVector2>& uvs = pUVs->GetDirectArray();

	KFbxLayer* lLayer = pMesh->GetLayer(0);
	KFbxLayerElementTangent* pTangentLayer = lLayer->GetTangents();
	if (pTangentLayer == NULL)
	{
		pTangentLayer = KFbxLayerElementTangent::Create(pMesh, "Tangents");
		lLayer->SetTangents(pTangentLayer);
	}
	pTangentLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
	pTangentLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElementArrayTemplate<KFbxVector4>& aTangents = pTangentLayer->GetDirectArray();
	aTangents.Resize(lVertexCount);

	KFbxLayerElementBinormal* pBinormLayer = pMesh->GetLayer(0)->GetBinormals();
	if (pBinormLayer == NULL)
	{
		pBinormLayer = KFbxLayerElementBinormal::Create(pMesh, "Binormals");
		lLayer->SetBinormals(pBinormLayer);
	}
	pBinormLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
	pBinormLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElementArrayTemplate<KFbxVector4>& aBinorms = pBinormLayer->GetDirectArray();
	aBinorms.Resize(lVertexCount);

	KFbxVector4 emptyVert(0.0, 0.0, 0.0, 0.0);
	for ( int i = 0; i < lVertexCount; i++ ) {	
		aBinorms.SetAt(i, emptyVert);
		aTangents.SetAt(i, emptyVert);
	}

	KFbxVector4* pVerts = pMesh->GetControlPoints();
	int lPolygonCount = pMesh->GetPolygonCount();

	// Handle Tangents and Binormals
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
	{
		int lPolySize = pMesh->GetPolygonSize(lPolygon);

		// get vertex numbers
		int i0 = pMesh->GetPolygonVertex(lPolygon, 0);
		int i1 = pMesh->GetPolygonVertex(lPolygon, 1);
		int i2 = pMesh->GetPolygonVertex(lPolygon, 2);

		KFbxVector4& v0 = pVerts[i0];
		KFbxVector4& v1 = pVerts[i1];
		KFbxVector4& v2 = pVerts[i2];

		KFbxVector4 side_0 = v0 - v1;
		KFbxVector4 side_1 = v2 - v1;

		int iuv0, iuv1, iuv2;
		if (lMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			iuv0 = pMesh->GetTextureUVIndex(lPolygon, 0);
			iuv1 = pMesh->GetTextureUVIndex(lPolygon, 1);
			iuv2 = pMesh->GetTextureUVIndex(lPolygon, 2);
		}
		else // KFbxLayerElement::eBY_CONTROL_POINT
		{
			iuv0 = pMesh->GetPolygonVertex(lPolygon, 0);
			iuv1 = pMesh->GetPolygonVertex(lPolygon, 1);
			iuv2 = pMesh->GetPolygonVertex(lPolygon, 2);
		}

		KFbxVector2& uv0 = uvs[iuv0];
		KFbxVector2& uv1 = uvs[iuv0];
		KFbxVector2& uv2 = uvs[iuv0];

		float delta_U_0 = uvs[iuv0][0] - uvs[iuv1][0];
		float delta_U_1 = uvs[iuv2][0] - uvs[iuv1][0];
		float delta_V_0 = uvs[iuv0][1] - uvs[iuv1][1];
		float delta_V_1 = uvs[iuv2][1] - uvs[iuv1][1];

		KFbxVector4 face_tangent = Normalize( Scale(side_0, delta_V_1) - Scale(side_1, delta_V_0) );
		KFbxVector4 face_bi_tangent = Normalize( Scale(side_0, delta_U_1) - Scale(side_1, delta_U_0) ); 
		KFbxVector4 face_normal = Normalize( side_0.CrossProduct( side_1 ) );
		
		float face_winding = 1.0f;
		if ( (face_tangent.CrossProduct(face_bi_tangent)).DotProduct( face_normal ) < 0 ) {
			face_bi_tangent = -face_bi_tangent;
			face_winding = 1.0f;
		}
		else {
			face_winding = -1.0f;
		}
		// no duplication, just smoothing
		for (int i=0; i<lPolySize; ++i) {
			int idx = pMesh->GetPolygonVertex(lPolygon, i);
			aTangents.SetAt( idx, aTangents.GetAt(idx) + face_tangent);
			aBinorms.SetAt( idx, aBinorms.GetAt(idx) + face_bi_tangent);
		}
	}

	// for each.getPosition(), normalize the Tangent and Binormal
	for ( int i = 0; i < lVertexCount; i++ ) {	
		aTangents.SetAt( i, Normalize(aTangents.GetAt(i)) );
		aBinorms.SetAt( i, Normalize(aBinorms.GetAt(i)) );
	}
}

bool MMHExportImpl::HasSharedVertices( KFbxMesh* pMesh )
{
	int lVertexCount = pMesh->GetControlPointsCount();
	std::vector<int> indexCount;
	indexCount.resize(lVertexCount);

	bool hasSharedVertices = false;
	int lPolygonCount = pMesh->GetPolygonCount();
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
	{
		int lPolySize = pMesh->GetPolygonSize(lPolygon);
		for (int i=0; i<lPolySize; ++i)
		{
			int idx = pMesh->GetPolygonVertex(lPolygon, 0);
			int cnt = indexCount[idx];
			hasSharedVertices |= ( cnt > 0 );
			indexCount[idx] = cnt + 1;
		}
	}
	return hasSharedVertices;
}

void MMHExportImpl::RecomputeMeshVertices( KFbxMesh* pMesh )
{
	int lVertexCount = pMesh->GetControlPointsCount();
	int newVertexCount = 0;
	int lPolygonCount = pMesh->GetPolygonCount();
	// Handle Tangents and Binormals
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
		newVertexCount += pMesh->GetPolygonSize(lPolygon);

	KFbxLayerElementArrayTemplate<KFbxVector4> verts(EFbxType::eDOUBLE4);
	KFbxLayerElementArrayTemplate<KFbxVector4> norms(EFbxType::eDOUBLE4);
	KFbxLayerElementArrayTemplate<KFbxVector2> uvs(EFbxType::eDOUBLE2);
	verts.Resize(newVertexCount);
	norms.Resize(newVertexCount);
	uvs.Resize(newVertexCount);

	bool hadNormals = false;
	KFbxLayerElementArrayTemplate<KFbxVector4> oldNormals(EFbxType::eDOUBLE4);
	if (KFbxLayerElementNormal const* pNorms = pMesh->GetLayer(0)->GetNormals()){
		oldNormals = pNorms->GetDirectArray();
		hadNormals = true;
	}
	bool hadUVs = false;
	KFbxLayerElementArrayTemplate<KFbxVector2> oldUVs(EFbxType::eDOUBLE2);
	if (KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs()){
		oldUVs = pUVs->GetDirectArray();
		hadUVs = true;
	}

	KFbxVector4* pVerts = pMesh->GetControlPoints();
	int lVertIdx = 0;
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
	{
		int lPolySize = pMesh->GetPolygonSize(lPolygon);
		for (int i=0; i<lPolySize; ++i)
		{
			int idx = pMesh->GetPolygonVertex(lPolygon, 0);
			verts.SetAt(lVertIdx, pVerts[idx]);
			if (hadNormals)
				norms.SetAt(lVertIdx, oldNormals[idx]);
			if (hadUVs)
				uvs.SetAt(lVertIdx, oldUVs[idx]);
			++lVertIdx;
		}
	}
	for ( int lPolygon = lPolygonCount-1; lPolygon >= 0; --lPolygon )   // for each face
		pMesh->RemovePolygon(lPolygon);

	lVertIdx = 0;
	for ( int lPolygon =0; lPolygon < lPolygonCount; ++lPolygon)
	{
		pMesh->BeginPolygon(lPolygon);
		pMesh->AddPolygon(lVertIdx, lVertIdx); ++lVertIdx;
		pMesh->AddPolygon(lVertIdx, lVertIdx); ++lVertIdx;
		pMesh->AddPolygon(lVertIdx, lVertIdx); ++lVertIdx;
		pMesh->EndPolygon ();
	}

	pMesh->SetControlPointCount(newVertexCount);

	KFbxLayer* lLayer = pMesh->GetLayer(0);
	KFbxLayerElementNormal* pNormLayer = pMesh->GetLayer(0)->GetNormals();
	if (pNormLayer == NULL)
	{
		pNormLayer = KFbxLayerElementNormal::Create(pMesh, "");
		pNormLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
		pNormLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
		lLayer->SetNormals(pNormLayer);
	}
	KFbxLayerElementArrayTemplate<KFbxVector4>& aNorms = pNormLayer->GetDirectArray();
	aNorms.Resize(newVertexCount);

	KFbxLayerElementUV* lUVLayer = pMesh->GetLayer(0)->GetUVs();
	if (lUVLayer == NULL)
	{
		lUVLayer=KFbxLayerElementUV::Create(pMesh, "UVChannel_1");
		lUVLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
		lUVLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
		lLayer->SetUVs(lUVLayer, KFbxLayerElement::eDIFFUSE_TEXTURES);
	}
	KFbxLayerElementArrayTemplate<KFbxVector2>& aUVs = lUVLayer->GetDirectArray();
	aUVs.Resize(newVertexCount);

	pVerts = pMesh->GetControlPoints();
	for (int i=0;i<newVertexCount; ++i)
	{
		KFbxVector2 uv = uvs[i];
		KFbxVector4 v = verts[i];
		KFbxVector4 n = norms[i];

		pVerts[i] = verts[i];
		aUVs.SetAt(i, uvs[i]);
		aNorms.SetAt(i, norms[i]);

		uv = aUVs[i];
		v = pVerts[i];
		n = aNorms[i];

	}
	//pMesh->ComputeVertexNormals();
	if (NULL == pMesh->GetLayer(0)->GetNormals())
		pMesh->ComputeVertexNormals();
	UpdateTangentSpace(pMesh);
}

void MMHExportImpl::StartExportNode( KFbxNode* lNode, LPCTSTR name /*= NULL*/ )
{
	TCHAR buffer[32];
	if (name == NULL) name = lNode->GetName();
	if (name == NULL || name[0] == 0)
	{
		sprintf(buffer, "Node%02d", nodeId++);
		name = buffer;
	}
	mmhwriter->StartElement("Node");
	mmhwriter->WriteAttribute("Name", name);
	//mmhwriter->WriteAttribute("SoundMaterialType", "0");

	mmhwriter->StartElement("Export");
	mmhwriter->WriteFormatAttribute("TagName", "%sTranslation", name);
	mmhwriter->WriteFormatAttribute("ExportName", "%s_%sTranslation", name, name);
	mmhwriter->WriteFormatAttribute("ControllerType", "%s_Vector3", name);
	mmhwriter->EndElement();

	mmhwriter->StartElement("Export");
	mmhwriter->WriteFormatAttribute("TagName", "%sRotation", name);
	mmhwriter->WriteFormatAttribute("ExportName", "%s_%sRotation", name, name);
	mmhwriter->WriteFormatAttribute("ControllerType", "%s_Quaternion", name);
	mmhwriter->EndElement();
}

void MMHExportImpl::EndExportNode( KFbxNode* lNode )
{
	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		ExportObject(lNode->GetChild(i));
	}
	mmhwriter->EndElement(); // Node
}

inline bool NotEquals(float x, float y, float tol = FLT_EPSILON * 100)
{
	return (fabs(x-y) > tol);
}

void MMHExportImpl::ExportNodeTransform( KFbxNode* lNode, bool global )
{
	{
		KFbxVector4 lT, lR, lS;
		double scl = 1.0;
		if (global)
		{
			lS = lNode->GetDefaultS(lS);
			scl = (double)Average(lS);
			if ( NotEquals(lS[0], lS[1]) || NotEquals(lS[0], lS[2]) ) {
				KFbxLog::LogWarn("Node '%s' has non-uniform scale.  Transforms will be collapsed into the node.", lNode->GetName() );
				scl = 1.0;
			}
		}
		else
		{
			lS = lNode->GetDefaultS(lS);
			scl = (double)Average(lS);
			if ( NotEquals(lS[0], lS[1]) || NotEquals(lS[0], lS[2]) ) {
				KFbxLog::LogWarn("Node '%s' has non-uniform scale.  Transforms will be collapsed into the node.", lNode->GetName() );
				scl = 1.0;
			}
		}
		KFbxQuaternion q = ComposeSphericalXYZ(lR);
		{
			mmhwriter->StartElement("Translation");
			Text value = FormatText("%g %g %g %g", (double)lT[0], (double)lT[1], (double)lT[2], (double)lT[3]);
			value.Trim();
			mmhwriter->WriteCDATA( value );
			mmhwriter->EndElement();
		}
		{
			mmhwriter->StartElement("Rotation");
			Text value = FormatText("%g %g %g %g", (double)q[0], (double)q[1], (double)q[2], (double)q[3]);
			value.Trim();
			mmhwriter->WriteCDATA( value );
			mmhwriter->EndElement();
		}
		if ( scl != 1.0 )
		{
			mmhwriter->StartElement("Scale");
			Text value = FormatText("%g", scl);
			value.Trim();
			mmhwriter->WriteCDATA( value );
			mmhwriter->EndElement();
		}
	}
}


void MMHExportImpl::RescaleMesh( KFbxMesh* pMesh, KFbxXMatrix & lm )
{
	int lVertexCount = pMesh->GetControlPointsCount();
	KFbxNode* lNode = pMesh->GetNode();
	KFbxXMatrix m = lNode->GetGlobalFromDefaultTake();
	KFbxLayer* lLayer = pMesh->GetLayer(0);
	KFbxLayerElementNormal* pNormLayer = pMesh->GetLayer(0)->GetNormals();
	if (pNormLayer == NULL) {
		pMesh->ComputeVertexNormals();
		pNormLayer = pMesh->GetLayer(0)->GetNormals();
	}
	KFbxLayerElementArrayTemplate<KFbxVector4>& aNorms = pNormLayer->GetDirectArray();

	KFbxVector4* pVerts = pMesh->GetControlPoints();
	KFbxLayerElementArrayTemplate<KFbxVector4> oldVerts(EFbxType::eDOUBLE4);
	oldVerts.Resize(aNorms.GetCount());
	for (int i=0;i<lVertexCount; ++i) {
		KFbxXMatrix mv;
		mv.SetT(pVerts[i]);
		mv = m * mv;
		oldVerts.SetAt(i, pVerts[i]);
		pVerts[i] = mv.GetT();
	}

	KFbxLayerElement::EMappingMode lMappingMode = pNormLayer->GetMappingMode();
	KFbxLayerElement::EReferenceMode lReferenceMode = pNormLayer->GetReferenceMode();
	int iCurVert = 0;
	int lPolygonCount = pMesh->GetPolygonCount();
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
	{
		int lPolySize = pMesh->GetPolygonSize(lPolygon);
		for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
		{
			int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
			KFbxVector4 v = oldVerts.GetAt(iVert);
			KFbxVector4 n = aNorms.GetAt(iCurVert);
			KFbxXMatrix mn; mn.SetT(v + n);
			mn = m * mn;
			KFbxVector4 n2, v2 = pVerts[iVert];
			n2 = mn.GetT() - v2;
			n2.Normalize();
			aNorms.SetAt(iCurVert, n2);
			++iCurVert;
		}
	}
}

void MMHExportImpl::CollapseTransforms( KFbxNode* lNode, KFbxXMatrix lm )
{
	KFbxXMatrix m = lNode->GetGlobalFromDefaultTake();
	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		KFbxNode *lChild = lNode->GetChild(i);
		LPCTSTR cname = lChild->GetName();
		if ( KFbxNodeAttribute *pAttr = lChild->GetNodeAttribute() )
		{
			KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
			switch (lAttributeType)
			{
			case KFbxNodeAttribute::eMESH:
				if ( KFbxMesh* pMesh = (KFbxMesh*)pAttr )
				{
					if (!pMesh->IsTriangleMesh())
					{
						KFbxLog::LogInfo("Mesh '%s' is not a triangle mesh.  Triangulating...", cname );
						KFbxGeometryConverter lConverter(mManager);
						lConverter.TriangulateInPlace(lChild);
					}
					RescaleMesh((KFbxMesh*)lChild->GetNodeAttribute(), m);
				}
				break;

			case KFbxNodeAttribute::eNURB:					
				KFbxLog::LogWarn("NURB is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
				{
					KFbxGeometryConverter lConverter(mManager);
					lConverter.TriangulateInPlace(lChild);
					RescaleMesh((KFbxMesh*)lChild->GetNodeAttribute(), m);
				}
				break;

			case KFbxNodeAttribute::ePATCH:
				KFbxLog::LogWarn("PATCH is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
				{
					KFbxGeometryConverter lConverter(mManager);
					lConverter.TriangulateInPlace(lChild);
					RescaleMesh((KFbxMesh*)lChild->GetNodeAttribute(), m);
				}
				break;
			}
		}
		CollapseTransforms(lChild, m);
	}

	KFbxXMatrix identityMatrix;
	SetGlobalDefaultPosition(lNode, identityMatrix);
	lNode->SetDefaultTakeFromLocalState(false);
}
