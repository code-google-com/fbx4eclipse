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
	void ExportMesh( KFbxMesh* lMesh, LPCSTR name = NULL);

	template <class T>
	T GetProp(LPCTSTR name, T defaultValue)
	{
		//IOSREF.GetBoolProp(name, default);
		return defaultValue;
	}
	Text GetDiffuseMaterialName(KFbxLayerContainer* lLayerContainer);

	void ExportMaterialObject( KFbxMesh* pMesh, LPCTSTR matName, LPCTSTR texName);
	bool GetDiffuseMaterialAndTextureName(KFbxLayerContainer* lLayerContainer, Text &matName, Text& texName);

	int GetFirstFreeBone();
	void BuildBoneMap(KFbxNode* lNode);

	void AddBoneIndex( KFbxNode* lNode, bool force );
	//////////////////////////////////////////////////////////////////////////

	AppSettings exportSettings;
	DAOXmlWriterPtr mmhwriter, mshwriter;
	DAOStreamPtr mmhfile, mshfile;
	Text mshfname, mmhfname;
	bool flipUV;

	typedef std::map<KFbxNode*, int> Node2IndexMap;
	typedef std::vector<KFbxNode*> Index2NodeMap;

	Node2IndexMap node2Index;
	Index2NodeMap index2Node;
	Text currentId;

	Text GetNextId();
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


MMHExportImpl::MMHExportImpl( DAOWriter *owner, KFbxScene* scene, KFbxStreamOptions* options, LPCSTR filename ) 
	: base(owner, scene, options, filename)
	, exportSettings("MMHExport")
	, mmhwriter(NULL), mshwriter(NULL)
	, mmhfile(NULL), mshfile(NULL)
{
	Text name;
	currentId = "_a";

	flipUV = GetProp<bool>("FlipUV", true);

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

	mshfname = mmhfname;
	PathRemoveExtension(mshfname);
	mshfname.append(".msh");
	name = mshfname; name.append(".xml");
	mshfile.swap(DAOStream::Create(name, false));
	if (mshfile.isNull())
		KFbxLog::LogError("Could not create file: %s", mshfname.c_str());
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
		case KFbxNodeAttribute::eNULL:
			ExportNode(lNode, name);
			break;

		case KFbxNodeAttribute::eMARKER:
			break;

		case KFbxNodeAttribute::eSKELETON:
			ExportSkeleton((KFbxSkeleton*)pAttr, name);
			break;

		case KFbxNodeAttribute::eMESH:
			ExportMesh((KFbxMesh*)pAttr, name);
			break;

		case KFbxNodeAttribute::eNURB:
		case KFbxNodeAttribute::ePATCH:
		case KFbxNodeAttribute::eCAMERA:
		case KFbxNodeAttribute::eCAMERA_STEREO:
		case KFbxNodeAttribute::eCAMERA_SWITCHER:
		case KFbxNodeAttribute::eLIGHT:
		case KFbxNodeAttribute::eOPTICAL_REFERENCE:
		case KFbxNodeAttribute::eOPTICAL_MARKER:
		case KFbxNodeAttribute::eNURBS_CURVE:
		case KFbxNodeAttribute::eTRIM_NURBS_SURFACE:
		case KFbxNodeAttribute::eBOUNDARY:
		case KFbxNodeAttribute::eNURBS_SURFACE:
		case KFbxNodeAttribute::eSHAPE:
		case KFbxNodeAttribute::eLODGROUP:
		case KFbxNodeAttribute::eSUBDIV:
			break;
		}
	}
	else
	{
		ExportNode(lNode, name);
	}
}

void MMHExportImpl::ExportNode(KFbxNode* lNode, LPCTSTR name, bool bbox)
{
	if (name == NULL) name = lNode->GetName();

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

	// Calc Bounding Box
	KFbxVector4 min, max;
	if ( bbox && CalcBoundingBox(lNode, min, max) )
	{
		Text sstr;
		sstr.AppendFormat("%g %g %g %g %g %g %g %g"
			, (double)min[0], (double)min[1], (double)min[2], 1.0
			, (double)max[0], (double)max[1], (double)max[2], 1.0);
		mmhwriter->StartElement("BoundingBox");
		mmhwriter->WriteCDATA(sstr);
		mmhwriter->EndElement();
	}
	KFbxVector4 lT, lR, lS;
	lNode->GetDefaultT(lT);
	lNode->GetDefaultR(lR);
	lNode->GetDefaultS(lS);
	KFbxQuaternion q = ComposeSphericalXYZ(lR);

	//KFbxXMatrix qm;
	//qm.SetR(lR);
	//KFbxQuaternion q = qm.GetQ();
	//
	{
		mmhwriter->StartElement("Translation");
		mmhwriter->WriteFormatCDATA("%g %g %g %g", (double)lT[0], (double)lT[1], (double)lT[2], 1.0);
		mmhwriter->EndElement();
	}
	{
		mmhwriter->StartElement("Rotation");
		mmhwriter->WriteFormatCDATA("%g %g %g %g", (double)q[0], (double)q[1], (double)q[2], (double)q[3]);
		mmhwriter->EndElement();
	}

	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		ExportObject(lNode->GetChild(i));
	}

	mmhwriter->EndElement(); // Node
}

void MMHExportImpl::ExportSkeleton( KFbxSkeleton* lSkel, LPCSTR name )
{
	KFbxNode *lNode = lSkel->GetNode();
	if (name == NULL) name = lSkel->GetName();

	mmhwriter->StartElement("Node");
	mmhwriter->WriteAttribute("Name", name);
	int iBoneIndex = ~0;
	Node2IndexMap::iterator itr = node2Index.find(lNode);
	if (itr != node2Index.end())
	{
		iBoneIndex = (*itr).second;
		if (iBoneIndex < 0)
			iBoneIndex = ~iBoneIndex;
	}	
	if (iBoneIndex >= 0)
	{
		mmhwriter->WriteFormatAttribute("BoneIndex", "%d", iBoneIndex);
	}

	KFbxVector4 min, max;
	if ( CalcBoundingBox(lNode, min, max) )
	{
		fbxDouble3 bdim = CalcDim(min, max);
		fbxDouble3 bmid = CalcMid(min, max);

		KFbxVector4 lT, lR, lS;
		lNode->GetDefaultR(lR);
		KFbxQuaternion q = ComposeSphericalXYZ(lR);

		mmhwriter->StartElement("CollisionObject");
		mmhwriter->WriteAttribute( "Kinematic", "true");
		{
			mmhwriter->StartElement("Shape");
			mmhwriter->WriteFormatAttribute( "Name", "Box_%s", name );
			mmhwriter->WriteAttribute("Type", "Box");
			mmhwriter->WriteAttribute("AllowEmitterSpawn", "1");
			mmhwriter->WriteAttribute("Fadeable", "false");
			mmhwriter->WriteFormatAttribute("DimY", "%g", bdim[1]);
			mmhwriter->WriteFormatAttribute("DimZ", "%g", bdim[2]);
			mmhwriter->WriteFormatAttribute("DimX", "%g", bdim[0]);
			mmhwriter->WriteAttribute("GROUP_MASK_PLACEABLES", "true");
			mmhwriter->WriteAttribute("GROUP_MASK_WALKABLE", "false");
			mmhwriter->WriteAttribute("GROUP_MASK_NONWALKABLE", "true");
			mmhwriter->WriteFormatAttribute("Rotation", "%g %g %g %g", (double)q[0], (double)q[1], (double)q[2], (double)q[3]);
			mmhwriter->WriteFormatAttribute("Position", "%g %g %g %g", (double)bmid[0], (double)bmid[1], (double)bmid[2], 1.0);
			mmhwriter->EndElement();
		}
		mmhwriter->EndElement();
	}
	//mmhwriter->WriteAttribute("SoundMaterialType", "0");

	mmhwriter->StartElement("Export");
	mmhwriter->WriteAttribute("TagName", "Translation");
	mmhwriter->WriteFormatAttribute("ExportName", "%s_Translation", name);
	mmhwriter->WriteAttribute("ControllerType", "Vector3");
	mmhwriter->EndElement();

	mmhwriter->StartElement("Export");
	mmhwriter->WriteAttribute("TagName", "Rotation");
	mmhwriter->WriteFormatAttribute("ExportName", "%s_Rotation", name);
	mmhwriter->WriteAttribute("ControllerType", "Quaternion");
	mmhwriter->EndElement();

	KFbxVector4 lT, lR, lS;
	lNode->GetDefaultT(lT);
	lNode->GetDefaultR(lR);
	lNode->GetDefaultS(lS);
	
	KFbxQuaternion q = ComposeSphericalXYZ(lR);
	{
		mmhwriter->StartElement("Translation");
		mmhwriter->WriteFormatCDATA("%g %g %g %g", (double)lT[0], (double)lT[1], (double)lT[2], 1.0);
		mmhwriter->EndElement();
	}
	{
		mmhwriter->StartElement("Rotation");
		mmhwriter->WriteFormatCDATA("%g %g %g %g", (double)q[0], (double)q[1], (double)q[2], (double)q[3]);
		mmhwriter->EndElement();
	}

	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		ExportObject(lNode->GetChild(i));
	}

	mmhwriter->EndElement(); // Node
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
	if (name == NULL) name = pMesh->GetName();
	KFbxNode* lNode = pMesh->GetNode();

	pMesh->ComputeBBox();
	fbxDouble3 bmin = pMesh->BBoxMin.Get();
	fbxDouble3 bmax = pMesh->BBoxMax.Get();
	fbxDouble3 bdim = CalcDim(bmin, bmax);
	fbxDouble3 bmid = CalcMid(bmin, bmax);

	KFbxVector4 lT, lR, lS;
	lNode->GetDefaultT(lT);
	lNode->GetDefaultR(lR);
	lNode->GetDefaultS(lS);
	KFbxQuaternion q = ComposeSphericalXYZ(lR);

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
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
			KFbxVector4& v = pVerts[iVertex];
			sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}

	if (KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs())
	{
		KFbxLayerElementArrayTemplate<KFbxVector2>& uvs = pUVs->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "TEXCOORD");
		mshwriter->WriteAttribute("Type", "Float2");
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
			KFbxVector2 uvw = uvs[iVertex];
			if (uvw[0] < 0.0f || uvw[0] > 1.0f) uvw[0] -= floor(uvw[0]);
			if (uvw[1] < 0.0f || uvw[1] > 1.0f) uvw[1] -= floor(uvw[1]);
			if (flipUV) uvw[1] = 1.0f-uvw[1];
			sstr.AppendFormat("%g %g\n", (double)uvw[0], (double)uvw[1]);
		}
		mshwriter->WriteCDATA(sstr);

		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementNormal const* pNorms = pMesh->GetLayer(0)->GetNormals())
	{
		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pNorms->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "NORMAL");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
			KFbxVector4& v = array[iVertex];
			sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementTangent const* pTangents = pMesh->GetLayer(0)->GetTangents())
	{
		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pTangents->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "TANGENT");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
			KFbxVector4& v = array[iVertex];
			sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementBinormal const* pBinorms = pMesh->GetLayer(0)->GetBinormals())
	{	
		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pBinorms->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "BINORMAL");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
			KFbxVector4& v = array[iVertex];
			sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
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
		for (int lPolygon = 0; lPolygon < lPolygonCount; lPolygon++){
			int lPolygonSize = pMesh->GetPolygonSize(lPolygon);
			KFBXLOG_ASSERT(lPolygonSize == 3);
			for (int lIdx = 0; lIdx < lPolygonSize; lIdx++) {
				int lControlPointIndex = pMesh->GetPolygonVertex(lPolygon, lIdx);
				sstr.AppendFormat("%d ", lControlPointIndex);
			}
			sstr.append('\n');
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	mshwriter->EndElement(); // MeshGroup
}

void MMHExportImpl::ExportMaterialObject( KFbxMesh* pMesh, LPCTSTR matName, LPCTSTR texPath )
{
	Text matFName = matName;
	matFName.append(".mao");
	DAOStreamPtr maofile( DAOStream::Create(matFName, false) );
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
			node2Index[lNode] = iBoneIndex;
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