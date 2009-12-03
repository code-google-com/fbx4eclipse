#include "stdafx.h"
#include "program.h"
#include <sys/stat.h>
#include <io.h>
#include <shellapi.h>
using namespace std;

#include "daostream.h"
#include "GFFFormat.h"
#include "MMHFormat.h"
#include "MSHFormat.h"
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MESH;
using namespace DAO::MMH;

#ifndef ASSERT
#ifdef _DEBUG
#include <crtdbg.h>
#define ASSERT _ASSERTE
#else
#define ASSERT(exp)
#endif
#endif

//////////////////////////////////////////////////////////////////////////
namespace {

template <typename T, typename U> 
void AssignValue(T& dst, const U& src) {
	ASSERT( !"Assignment not specified.");
}

template<> void AssignValue(float& dst, const float& src)				{ dst = src; }
template<> void AssignValue(Vector2f& dst, const Vector2f& src)			{ dst.Set(src[0], src[1]); }
template<> void AssignValue(Vector2f& dst, const Float16_2& src)		{ dst.Set(src[0], src[1]); }
template<> void AssignValue(Vector4f& dst, const Vector2f& src)			{ dst.Set(src[0], src[1], 0.0); }
template<> void AssignValue(Vector4f& dst, const Vector3f& src)			{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Vector4f& dst, const Vector4f& src)			{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Vector4f& dst, const Float16_4& src)		{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const Vector3f& src)			{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Color4& dst, const Vector4f& src)			{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const ARGB4& src)				{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const Float16_4& src)			{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Vector4f& dst, const Signed_10_10_10& src)	{ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Vector4f& dst, const Unsigned_10_10_10& src){ dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(Color4& dst, const Signed_10_10_10& src)	{ dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(Color4& dst, const Unsigned_10_10_10& src)	{ dst.Set(src[0], src[1], src[2], src[3]); }


void Dump(IDAODumpStream& out, LPCTSTR name, const UShort3& val)
{
	char buffer[256];
	sprintf(buffer, "[%4hu,%4hu,%4hu]", val[0], val[1], val[2]);
	Dump(out, name, buffer);
}
void Dump(IDAODumpStream& out, LPCTSTR name, const UShort4& val)
{
	char buffer[256];
	sprintf(buffer, "[%4hu,%4hu,%4hu,%4hu]", val[0], val[1], val[2], val[3]);
	Dump(out, name, buffer);
}


template <class T>
void AssignValue(T& dst, const DECLValue& src) {
	switch (src.get_type())
	{
	case DECLTYPE_FLOAT1:  AssignValue(dst, src.asFloat()); break;
	case DECLTYPE_FLOAT2:  AssignValue(dst, src.asVector2f()); break;
	case DECLTYPE_FLOAT3:  AssignValue(dst, src.asVector3f()); break;
	case DECLTYPE_FLOAT4:  AssignValue(dst, src.asVector4f()); break;
	case DECLTYPE_COLOR:  AssignValue(dst, src.asColor()); break;
	case DECLTYPE_UBYTE4:  AssignValue(dst, src.asUByte4()); break;
	case DECLTYPE_SHORT2:  AssignValue(dst, src.asShort2()); break;
	case DECLTYPE_SHORT4:  AssignValue(dst, src.asShort4()); break;
	case DECLTYPE_UBYTE4N:  AssignValue(dst, src.asUByte4()); break;
	case DECLTYPE_SHORT2N:  AssignValue(dst, src.asShort2()); break;
	case DECLTYPE_SHORT4N:  AssignValue(dst, src.asShort4()); break;
	case DECLTYPE_USHORT2N:  AssignValue(dst, src.asUShort2()); break;
	case DECLTYPE_USHORT4N:  AssignValue(dst, src.asUShort4()); break;
	case DECLTYPE_UDEC3:  AssignValue(dst, src.asU101010()); break;
	case DECLTYPE_DEC3N:  AssignValue(dst, src.asS101010()); break;
	case DECLTYPE_FLOAT16_2:  AssignValue(dst, src.asFloat16_2()); break;
	case DECLTYPE_FLOAT16_4:  AssignValue(dst, src.asFloat16_4()); break;
	case DECLTYPE_UNUSED:  
		break;
	}
}

}


typedef std::map<std::string, KArrayKFbxNode, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;

typedef std::map<std::string, GFFFile, ltstr> NameFileMap;
typedef NameFileMap::iterator NameFileItr;

class MMHExportImpl : public ExporterBase
{
	typedef ExporterBase base;
public:
	MMHExportImpl(DAOWriter *owner, MDLHRef scene, LPCSTR fname);

	bool DoExport();
	bool CalcBoundingBox(NODERef lNode, Vector4f& bmin, Vector4f& bmax );
	void ExportObject(NODERef lNode, LPCTSTR name = NULL);
	void ExportNode(NODERef lNode, LPCTSTR name = NULL, bool bbox = false);
	void ExportSkeleton( NODERef lSkel, LPCSTR name = NULL);
	void StartExportNode(NODERef lNode, LPCTSTR name = NULL);
	void EndExportNode(NODERef lNode);
	void ExportMesh( MESHRef lMesh, LPCSTR name = NULL);

	void ExportNodeTransform( NODERef lNode, bool global = false);

	template <class T>
	T GetProp(LPCTSTR name, T defaultValue)
	{
		return mmhExportSettings.GetSetting(name, defaultValue);
	}

	Text GetDiffuseMaterialName(KFbxLayerContainer* lLayerContainer);

	void ExportMaterialObject( MESHRef pMesh, LPCTSTR matName, LPCTSTR texName);
	bool GetDiffuseMaterialAndTextureName(KFbxLayerContainer* lLayerContainer, Text &matName, Text& texName);

	int GetFirstFreeBone();
	void BuildBoneMap(NODERef lNode);

	void AddBoneIndex( NODERef lNode, bool force );

	void UpdateTangentSpace(KFbxMesh *pMesh);
	bool NeedTangentSpace(KFbxMesh *pMesh);

	void RecomputeMeshVertices( MESHRef pMesh );
	bool HasSharedVertices( MESHRef pMesh );

	void RescaleMesh( MESHRef pMesh, Matrix44 lm );
	void CollapseTransforms( NODERef lRoot, Matrix44 gm );
	void SetLocalStateFromDefaultTake( NODERef lRoot, bool pRecursive );

	//////////////////////////////////////////////////////////////////////////

	MDLHRef scene;

	DAOXmlWriterPtr mmhwriter, mshwriter;
	DAOStreamPtr mmhfile, mshfile;
	Text mshfname, mmhfname, phyfname;
	bool flipUV;
	bool allowSharedVertices;
	bool forceByPolygonVertexMapping;
	bool forceRecalculateTangentSpace;
	bool collapseTransforms;

	typedef std::map<NODERef, int> Node2IndexMap;
	typedef std::vector<NODERef> Index2NodeMap;

	Node2IndexMap node2Index;
	Index2NodeMap index2Node;
	Text currentId;
	int meshId;
	int nodeId;
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


// Recursive function to get a node's global default position.
// As a prerequisite, parent node's default local position must be already set.
Matrix44 GetGlobalDefaultPosition(NODERef pNode)
{
	Matrix44 lLocalPosition;
	Matrix44 lGlobalPosition;
	Matrix44 lParentGlobalPosition;

	Vector4f lT, lR, lS;
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
void SetGlobalDefaultPosition(NODERef pNode, Matrix44 pGlobalPosition)
{
	Matrix44 lLocalPosition;
	Matrix44 lParentGlobalPosition;

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


MMHExportImpl::MMHExportImpl( DAOWriter *owner, MDLHRef scene, LPCSTR filename ) 
: base(owner, scene, options, filename)
, mmhwriter(NULL), mshwriter(NULL)
, mmhfile(NULL), mshfile(NULL)
{
	mmhfname = filename;
	LPCTSTR ext = PathFindExtension(mmhfname);
	if (ext != NULL && _tcsicmp(ext, ".xml") == 0)
		PathRemoveExtension(mmhfname);
	PathRemoveExtension(mmhfname);
	mmhfname.append(".mmh");
	name = mmhfname; name.append(".xml");
	mmhfile.swap( DAOStream::Create(name, false) );
	if (mmhfile.isNull())
		KFbxLog::LogError("Could not create file: %s", name.c_str());
	else
		KFbxLog::LogInfo("Exporting: %s", name.c_str() );

	mshfname = mmhfname;
	PathRemoveExtension(mshfname);
	mshfname.append(".msh");
	name = mshfname; name.append(".xml");
	mshfile.swap(DAOStream::Create(name, false));
	if (mshfile.isNull())
		KFbxLog::LogError("Could not create file: %s", name.c_str());
	else
		KFbxLog::LogInfo("Exporting: %s", name.c_str() );
}

bool MMHExportImpl::CalcBoundingBox(NODERef lNode, Vector4f& bmin, Vector4f& bmax )
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

	MESHRef lMesh = (MESHRef) lNode->GetNodeAttribute ();

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

	NODERef lRoot = lScene->GetRootNode();
	BuildBoneMap(lRoot);

	// Skip scene if first and only child is named GOB
	if (lRoot->GetChildCount() == 1) {
		NODERef lNode = lRoot->GetChild(0);
		if (lNode != NULL && _tcsicmp(lNode->GetName(), "GOB") == 0)
			lRoot = lRoot->GetChild(0);
	}
	SetLocalStateFromDefaultTake(lRoot, true);

	Matrix44 gm = lRoot->GetGlobalFromDefaultTake();
	CollapseTransforms( lRoot, gm );
	//SetLocalTransform( lRoot, Matrix44() );

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

void MMHExportImpl::ExportObject(NODERef lNode, LPCTSTR name)
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
			ExportSkeleton((NODERef)pAttr, NULL);
			break;

		case KFbxNodeAttribute::eMESH:
			if ( MESHRef pMesh = (MESHRef)pAttr )
			{
				if (!pMesh->IsTriangleMesh())
				{
					KFbxLog::LogInfo("Mesh '%s' is not a triangle mesh.  Triangulating...", cname );
					KFbxGeometryConverter lConverter(mManager);
					lConverter.TriangulateInPlace(lNode);
				}
				KFbxLog::LogVerbose("Exporting Mesh '%s'", cname );
				ExportMesh((MESHRef)lNode->GetNodeAttribute(), NULL);

			}
			break;

		case KFbxNodeAttribute::eNURB:					
			KFbxLog::LogWarn("NURB is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
			{
				KFbxGeometryConverter lConverter(mManager);
				lConverter.TriangulateInPlace(lNode);
				ExportMesh((MESHRef)lNode->GetNodeAttribute(), NULL);
			}
			break;

		case KFbxNodeAttribute::ePATCH:
			KFbxLog::LogWarn("PATCH is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
			{
				KFbxGeometryConverter lConverter(mManager);
				lConverter.TriangulateInPlace(lNode);
				ExportMesh((MESHRef)lNode->GetNodeAttribute(), NULL);
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

void MMHExportImpl::ExportNode(NODERef lNode, LPCTSTR name, bool root)
{
	StartExportNode(lNode, NULL);
	// Calc Bounding Box
	Vector4f min, max;
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

void MMHExportImpl::ExportSkeleton( NODERef lSkel, LPCSTR name )
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
		NODERef pNode = lLayerContainer->GetNode();
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
		NODERef pNode = lLayerContainer->GetNode();
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

void MMHExportImpl::ExportMesh( MESHRef pMesh, LPCSTR name )
{
	TCHAR buffer[32];
	if (pMesh == NULL) return;
	NODERef lNode = pMesh->GetNode();

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

	//if (NULL == pMesh->GetLayer(0)->GetUVs())
	//{
	//	KFbxLog::LogWarn("Mesh '%s' did not have Diffuse Texture UVs.  Initializing defaults.", name);
	//	pMesh->InitTextureUV(pMesh->GetControlPointsCount(), KFbxLayerElement::eDIFFUSE_TEXTURES);
	//}

	if (!allowSharedVertices)
	{
		if (NULL == pMesh->GetLayer(0)->GetNormals())
			pMesh->ComputeVertexNormals();
		if (HasSharedVertices(pMesh))
		{
			KFbxLog::LogDebug("Splitting mesh '%s'", name);
			//RecomputeMeshVertices(pMesh);
			pMesh->BuildMeshEdgeArray();
			pMesh->SplitPoints();
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
				NODERef pBone = lCluster->GetLink();
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

	KFbxLayerElement::EMappingMode lVertMappingMode = forceByPolygonVertexMapping ? KFbxLayerElement::eBY_POLYGON_VERTEX : KFbxLayerElement::eBY_CONTROL_POINT;
	{
		Vector4f* pVerts = pMesh->GetControlPoints();

		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "POSITION");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lVertMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					Vector4f &v = pVerts[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
				Vector4f& v = pVerts[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}

	if (KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs())
	{
		KFbxLayerElement::EMappingMode lUVMappingMode = pUVs->GetMappingMode();

		KFbxLayerElementArrayTemplate<KFbxVector2>& uvs = pUVs->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "TEXCOORD");
		mshwriter->WriteAttribute("Type", "Float2");

		Text sstr;
		if (lVertMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
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
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
				KFbxVector2 uvw = uvs[iVertex];
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
		KFbxLayerElement::EMappingMode lNormMappingMode = pNorms->GetMappingMode();

		KFbxLayerElementArrayTemplate<Vector4f>& array = pNorms->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "NORMAL");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lVertMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					Vector4f &v = array[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
				Vector4f& v = array[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementTangent const* pTangents = pMesh->GetLayer(0)->GetTangents())
	{
		KFbxLayerElement::EMappingMode lTangentMappingMode = pTangents->GetMappingMode();

		KFbxLayerElementArrayTemplate<Vector4f>& array = pTangents->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "TANGENT");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lVertMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					Vector4f &v = array[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
				Vector4f& v = array[iVertex];
				sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
			}
		}
		mshwriter->WriteCDATA(sstr);
		mshwriter->EndElement(); // Data
	}
	if (KFbxLayerElementBinormal const* pBinorms = pMesh->GetLayer(0)->GetBinormals())
	{	
		KFbxLayerElement::EMappingMode lBinormMappingMode = pBinorms->GetMappingMode();

		KFbxLayerElementArrayTemplate<Vector4f>& array = pBinorms->GetDirectArray();
		mshwriter->StartElement("Data");
		mshwriter->WriteFormatAttribute("ElementCount", "%d", lVertexCount);
		mshwriter->WriteAttribute("Semantic", "BINORMAL");
		mshwriter->WriteAttribute("Type", "Float4");
		Text sstr;
		if (lVertMappingMode == KFbxLayerElement::eBY_POLYGON_VERTEX)
		{
			int lPolygonCount = pMesh->GetPolygonCount();
			for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
			{
				int lPolySize = pMesh->GetPolygonSize(lPolygon);
				for ( int iPoly=0; iPoly < lPolySize; ++iPoly)
				{
					int iVert = pMesh->GetPolygonVertex(lPolygon, iPoly);
					Vector4f &v = array[iVert];
					sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
				}
			}
		}
		else
		{
			for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
				Vector4f& v = array[iVertex];
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

void MMHExportImpl::ExportMaterialObject( MESHRef pMesh, LPCTSTR matName, LPCTSTR texPath )
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
	writer.swap( DAOXmlWriterPtr(NULL) );
	maofile->Close();
}

void MMHExportImpl::BuildBoneMap(NODERef lNode)
{
	AddBoneIndex(lNode, false);

	if ( KFbxNodeAttribute *pAttr = lNode->GetNodeAttribute() )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
		switch (lAttributeType)
		{
		case KFbxNodeAttribute::eMESH:
			if ( MESHRefpMesh = (MESHRef)pAttr )
			{
				int lSkinCount = pMesh->GetDeformerCount(KFbxDeformer::eSKIN);
				for (int lSkinIdx=0; lSkinIdx < lSkinCount; ++lSkinIdx) {
					if ( KFbxSkin* pSkin = (KFbxSkin *)pMesh->GetDeformer(lSkinIdx, KFbxDeformer::eSKIN) ) {
						int lClusterCount = pSkin->GetClusterCount();
						for (int j = 0; j != lClusterCount; ++j) {
							KFbxCluster* lCluster = pSkin->GetCluster(j);
							if ( NODERef pBone = lCluster->GetLink() )
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
		NODERef node = index2Node[i];
		if (node == NULL)
			return i;
	}
	return i;
}

void MMHExportImpl::AddBoneIndex( NODERef lNode, bool force )
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
			NODERef pBone = index2Node[iBoneIndex];
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

static inline Vector4f Scale(const Vector4f& v, float s )
{
	Vector4f res(v);
	res *= s;
	return res;
}
static inline Vector4f Normalize(const Vector4f& v )
{
	Vector4f res(v);
	res.Normalize();
	return res;
}

bool MMHExportImpl::NeedTangentSpace(KFbxMesh *pMesh)
{
	KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs();
	KFbxLayerElementTangent* pTangentLayer = pMesh->GetLayer(0)->GetTangents();
	KFbxLayerElementBinormal* pBinormLayer = pMesh->GetLayer(0)->GetBinormals();

	return forceRecalculateTangentSpace || ((pUVs != NULL) && ((pTangentLayer == NULL) || (pBinormLayer == NULL)));
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
	KFbxLayerElementArrayTemplate<Vector4f>& aTangents = pTangentLayer->GetDirectArray();
	aTangents.Resize(lVertexCount);

	KFbxLayerElementBinormal* pBinormLayer = pMesh->GetLayer(0)->GetBinormals();
	if (pBinormLayer == NULL)
	{
		pBinormLayer = KFbxLayerElementBinormal::Create(pMesh, "Binormals");
		lLayer->SetBinormals(pBinormLayer);
	}
	pBinormLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
	pBinormLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
	KFbxLayerElementArrayTemplate<Vector4f>& aBinorms = pBinormLayer->GetDirectArray();
	aBinorms.Resize(lVertexCount);

	Vector4f emptyVert(0.0, 0.0, 0.0, 0.0);
	for ( int i = 0; i < lVertexCount; i++ ) {	
		aBinorms.SetAt(i, emptyVert);
		aTangents.SetAt(i, emptyVert);
	}

	Vector4f* pVerts = pMesh->GetControlPoints();
	int lPolygonCount = pMesh->GetPolygonCount();

	// Handle Tangents and Binormals
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
	{
		int lPolySize = pMesh->GetPolygonSize(lPolygon);

		// get vertex numbers
		int i0 = pMesh->GetPolygonVertex(lPolygon, 0);
		int i1 = pMesh->GetPolygonVertex(lPolygon, 1);
		int i2 = pMesh->GetPolygonVertex(lPolygon, 2);

		Vector4f& v0 = pVerts[i0];
		Vector4f& v1 = pVerts[i1];
		Vector4f& v2 = pVerts[i2];

		Vector4f side_0 = v0 - v1;
		Vector4f side_1 = v2 - v1;

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

		Vector4f face_tangent = Normalize( Scale(side_0, delta_V_1) - Scale(side_1, delta_V_0) );
		Vector4f face_bi_tangent = Normalize( Scale(side_0, delta_U_1) - Scale(side_1, delta_U_0) ); 
		Vector4f face_normal = Normalize( side_0.CrossProduct( side_1 ) );

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

bool MMHExportImpl::HasSharedVertices( MESHRef pMesh )
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
			int idx = pMesh->GetPolygonVertex(lPolygon, i);
			KFBXLOG_ASSERT( idx >= 0 && idx < lVertexCount );
			if (idx >= 0)
			{
				int cnt = indexCount[idx];
				hasSharedVertices |= ( cnt > 0 );
				indexCount[idx] = cnt + 1;
			}
		}
	}
	return hasSharedVertices;
}

void MMHExportImpl::RecomputeMeshVertices( MESHRef pMesh )
{
	int lVertexCount = pMesh->GetControlPointsCount();
	int newVertexCount = 0;
	int lPolygonCount = pMesh->GetPolygonCount();
	// Handle Tangents and Binormals
	for ( int lPolygon = 0; lPolygon < lPolygonCount; ++lPolygon )   // for each face
		newVertexCount += pMesh->GetPolygonSize(lPolygon);

	KFbxLayerElementArrayTemplate<Vector4f> verts(EFbxType::eDOUBLE4);
	KFbxLayerElementArrayTemplate<Vector4f> norms(EFbxType::eDOUBLE4);
	KFbxLayerElementArrayTemplate<KFbxVector2> uvs(EFbxType::eDOUBLE2);
	verts.Resize(newVertexCount);
	norms.Resize(newVertexCount);
	uvs.Resize(newVertexCount);

	bool hadNormals = false;
	KFbxLayerElementArrayTemplate<Vector4f> oldNormals(EFbxType::eDOUBLE4);
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

	Vector4f* pVerts = pMesh->GetControlPoints();
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
	KFbxLayerElementArrayTemplate<Vector4f>& aNorms = pNormLayer->GetDirectArray();
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
		Vector4f v = verts[i];
		Vector4f n = norms[i];

		pVerts[i] = verts[i];
		aUVs.SetAt(i, uvs[i]);
		aNorms.SetAt(i, norms[i]);

		uv = aUVs[i];
		v = pVerts[i];
		n = aNorms[i];

	}
	if (NULL == pMesh->GetLayer(0)->GetNormals())
		pMesh->ComputeVertexNormals();
	UpdateTangentSpace(pMesh);
}

void MMHExportImpl::StartExportNode( NODERef lNode, LPCTSTR name /*= NULL*/ )
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

void MMHExportImpl::EndExportNode( NODERef lNode )
{
	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		ExportObject(lNode->GetChild(i));
	}
	mmhwriter->EndElement(); // Node
}

inline bool NotEquals(float x, float y, float tol = FLT_EPSILON * 1000)
{
	return (fabs(x-y) > tol);
}

void MMHExportImpl::ExportNodeTransform( NODERef lNode, bool global )
{
	{
		Vector4f lT, lR, lS;
		double scl = 1.0;
		if (global)
		{
			Matrix44 m = GetLocalTransformFromDefaultTake(lNode);
			lT = m.GetT(), lR = m.GetR(), lS = m.GetS();
			scl = (double)Average(lS);
			if ( NotEquals(lS[0], lS[1]) || NotEquals(lS[0], lS[2]) ) {
				KFbxLog::LogWarn("Node '%s' has non-uniform scale.  Transforms will be collapsed into the node.", lNode->GetName() );
				scl = 1.0;
			}
		}
		else
		{
			// TODO: Why doesn't this work??? Have to use the default take to get local transforms
			//Matrix44 m = GetLocalTransformFromDefaultTake(lNode);
			Matrix44 m = GetLocalTransform(lNode);
			lT = m.GetT(), lR = m.GetR(), lS = m.GetS();
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

void MMHExportImpl::RescaleMesh( MESHRef pMesh, Matrix44 tm )
{
	NODERef lNode = pMesh->GetNode();

	if ( !collapseTransforms )
	{
		Vector4f lS = tm.GetS();
		if ( NotEquals(lS[0], lS[1]) || NotEquals(lS[0], lS[2]) ) {
			KFbxLog::LogWarn("Node '%s' has non-uniform scale.  Transforms will be collapsed into the node.", lNode->GetName() );
		} else {
			return;
		}
	}

	int lVertexCount = pMesh->GetControlPointsCount();
	KFbxLayer* lLayer = pMesh->GetLayer(0);
	KFbxLayerElementNormal* pNormLayer = pMesh->GetLayer(0)->GetNormals();
	if (pNormLayer == NULL) {
		pMesh->ComputeVertexNormals();
		pNormLayer = pMesh->GetLayer(0)->GetNormals();
	}
	KFbxLayerElementArrayTemplate<Vector4f>& aNorms = pNormLayer->GetDirectArray();

	Vector4f* pVerts = pMesh->GetControlPoints();
	KFbxLayerElementArrayTemplate<Vector4f> oldVerts(EFbxType::eDOUBLE4);
	oldVerts.Resize(aNorms.GetCount());
	for (int i=0;i<lVertexCount; ++i) {
		Matrix44 mv;
		mv.SetT(pVerts[i]);
		mv = tm * mv;
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
			Vector4f v = oldVerts.GetAt(iVert);
			Vector4f n = aNorms.GetAt(iCurVert);
			Matrix44 mn; mn.SetT(v + n);
			mn = tm * mn;
			Vector4f n2, v2 = pVerts[iVert];
			n2 = mn.GetT() - v2;
			n2.Normalize();
			aNorms.SetAt(iCurVert, n2);
			++iCurVert;
		}
	}

	Matrix44 pivotMatrix;
	SetLocalTransform( lNode, pivotMatrix );
}

void MMHExportImpl::CollapseTransforms( NODERef lNode, Matrix44 m )
{
	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		KFbxNode *lChild = lNode->GetChild(i);

		Matrix44 lm = GetLocalTransformFromDefaultTake(lChild);
		Matrix44 tm = m * lm;

		LPCTSTR cname = lChild->GetName();
		if ( KFbxNodeAttribute *pAttr = lChild->GetNodeAttribute() )
		{
			KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
			switch (lAttributeType)
			{
			case KFbxNodeAttribute::eMESH:
				if ( MESHRef pMesh = (MESHRef)pAttr )
				{
					if (!pMesh->IsTriangleMesh())
					{
						KFbxLog::LogInfo("Mesh '%s' is not a triangle mesh.  Triangulating...", cname );
						KFbxGeometryConverter lConverter(mManager);
						lConverter.TriangulateInPlace(lChild);
					}
					RescaleMesh((MESHRef)lChild->GetNodeAttribute(), tm);
				}
				break;

			case KFbxNodeAttribute::eNURB:					
				KFbxLog::LogWarn("NURB is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
				{
					KFbxGeometryConverter lConverter(mManager);
					lConverter.TriangulateInPlace(lChild);
					RescaleMesh((MESHRef)lChild->GetNodeAttribute(), tm);
				}
				break;

			case KFbxNodeAttribute::ePATCH:
				KFbxLog::LogWarn("PATCH is not directly supported and will be triangulated. Exporting Mesh '%s'", cname );  
				{
					KFbxGeometryConverter lConverter(mManager);
					lConverter.TriangulateInPlace(lChild);
					RescaleMesh((MESHRef)lChild->GetNodeAttribute(), tm);
				}
				break;
			}
		}
		CollapseTransforms(lChild, tm);
	}

	//SetLocalTransform( lNode, GetLocalTransformFromDefaultTake(lNode) );

	//Matrix44 pivotMatrix;
	//lNode->SetDefaultT(pivotMatrix.GetT());
	//lNode->SetDefaultR(pivotMatrix.GetR());
	//lNode->SetDefaultS(pivotMatrix.GetS());
}

void MMHExportImpl::SetLocalStateFromDefaultTake( NODERef lNode, bool bRecursive )
{
	Matrix44 m = GetLocalTransformFromDefaultTake(lNode);
	if (bRecursive) {
		for( int i = 0; i < lNode->GetChildCount(); i++) {
			SetLocalStateFromDefaultTake( lNode->GetChild(i), bRecursive );
		}
	}
	lNode->SetDefaultT(m.GetT());
	lNode->SetDefaultR(m.GetR());
	lNode->SetDefaultS(m.GetS());
}




void DumpMesh(  DAODumpStream& log
				, CHNKRef chnk
				, DAOMemStream& idxStream
				, DAOMemStream& vertStream
				)
{
	LPCTSTR nodeName = chnk->get_name().c_str();
	int nverts = chnk->get_Vertexcount();
	unsigned long vsize = chnk->get_Vertexsize();
	unsigned long voff = chnk->get_Vertexoffset();

	GFFListRef declList = chnk->get_Vertexdeclarator();
	DAOArray<DECLPtr> decls;
	if ( DynamicCast<DECL>(declList, decls) ) {
		for (DAOArray<DECLPtr>::iterator itr = decls.begin(); itr != decls.end(); ++itr) {
			DECLRef decl(*itr);
			int strmIdx = decl->get_Stream();
			if (strmIdx == -1)
				continue;

			ASSERT( 0 == strmIdx );
			ASSERT( 0 == decl->get_usageIndex() );
			ASSERT( 0 == decl->get_Method() );

			switch ( decl->get_usage() )
			{
			case DECLUSAGE_POSITION:		
				{
					DECLTYPE type = decl->get_dataType();
					long offset = decl->get_Offset();
					DAOArray< Vector4f > verts;
					verts.resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + offset);
						Vector4f& vert = verts[i];
						AssignValue( vert, DECLValue(type, vertStream.dataptr()) );
						vert = vert;
					}
					Dump(log, "Position", verts);
				}
				break;
#if 0
			case DECLUSAGE_NORMAL:			
				{
					// We want to have one normal for each vertex (or control point),
					// so we set the mapping mode to eBY_CONTROL_POINT.
					LayerElementNormal* pNormLayer = LayerElementNormal::Create(pMesh, "");
					pNormLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pNormLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetNormals(pNormLayer);
					LayerElementArrayTemplate<Vector4>& aNorms = pNormLayer->GetDirectArray();
					aNorms.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aNorms.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;

			case DECLUSAGE_TEXCOORD:
				{
					LayerElementUV* lUVLayer=LayerElementUV::Create(pMesh, "UVChannel_1");
					lUVLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					lUVLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetUVs(lUVLayer, LayerElement::eDIFFUSE_TEXTURES);
					LayerElementArrayTemplate<Vector2>& aUVs = lUVLayer->GetDirectArray();
					aUVs.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						Vector2 &uvw = aUVs.GetAt(i);
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue(uvw, DECLValue(decl->get_dataType(), vertStream.dataptr()) );

						if (uvw[0] < 0.0f || uvw[0] > 1.0f) uvw[0] -= floor(uvw[0]);
						if (uvw[1] < 0.0f || uvw[1] > 1.0f) uvw[1] -= floor(uvw[1]);
						if (flipUV) uvw[1] = 1.0f-uvw[1];

						//if (enableScale) 
						//	uvw.Set(uvw[0] * scaleFactor, uvw[1] * scaleFactor);
					}
				}
				break;
			case DECLUSAGE_TANGENT:			
				{
					LayerElementTangent* pTangentLayer = LayerElementTangent::Create(pMesh, "Tangents");
					pTangentLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pTangentLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetTangents(pTangentLayer);
					LayerElementArrayTemplate<Vector4>& aTangents = pTangentLayer->GetDirectArray();
					aTangents.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aTangents.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;

			case DECLUSAGE_BINORMAL:		
				{
					LayerElementBinormal* pBinormLayer = LayerElementBinormal::Create(pMesh, "Binormals");
					pBinormLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pBinormLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetBinormals(pBinormLayer);
					LayerElementArrayTemplate<Vector4>& aBinorms = pBinormLayer->GetDirectArray();
					aBinorms.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aBinorms.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;
				//case DECLUSAGE_BLENDWEIGHT:		AddVertexBlendWeight(pMesh, decl, value);	break;
				//case DECLUSAGE_BLENDINDICES:	AddVertexBlendIndices(pMesh, decl, value);	break;
				//case DECLUSAGE_PSIZE:			AddVertexPSize(pMesh, decl, value);			break;
				//case DECLUSAGE_TESSFACTOR:		AddVertexTessFactor(pMesh, decl, value);	break;
				//case DECLUSAGE_POSITIONT:		AddVertexPositionT(pMesh, decl, value);		break;
			case DECLUSAGE_COLOR:			
				{
					LayerElementVertexColor* pLayer = LayerElementVertexColor::Create(pMesh, "VertexColors");
					pLayer->SetMappingMode(LayerElement::eBY_CONTROL_POINT);
					pLayer->SetReferenceMode(LayerElement::eDIRECT);
					lLayer->SetVertexColors(pLayer);
					LayerElementArrayTemplate<Color>& aColors = pLayer->GetDirectArray();
					aColors.Resize(nverts);
					for (int i = 0; i<nverts; ++i) {
						vertStream.Seek(SEEK_SET, voff + i * vsize + decl->get_Offset());
						AssignValue( aColors.GetAt(i), DECLValue(decl->get_dataType(), vertStream.dataptr()) );
					}
				}
				break;
				//case DECLUSAGE_FOG:				AddVertexFog(pMesh, decl, value);			break;
				//case DECLUSAGE_DEPTH:			AddVertexDepth(pMesh, decl, value);			break;
				//case DECLUSAGE_SAMPLE:			AddVertexSample(pMesh, decl, value);		break;
			case DECLUSAGE_UNUSED:			
				break;
#endif
			}
		}
	}

	ASSERT( 0 == chnk->get_Indexformat());
	ASSERT( 0 == chnk->get_Basevertexindex());
	ASSERT( 0 == chnk->get_Minindex());

	DAOArray< long > verts;
	verts.resize(nverts);

	idxStream.Seek(SEEK_SET, sizeof(short) * chnk->get_Startindex());
	UShort3 * pFaces = (UShort3*)idxStream.dataptr();
	int nfaces = chnk->get_Indexcount() / 3;
	DumpArray(log, "Indices", pFaces, nfaces);
	for (int i=0; i<nfaces; ++i) {
		UShort3& t = pFaces[i];
		++verts[t[0]];
		++verts[t[1]];
		++verts[t[2]];
		//pMesh->BeginPolygon(i);
		//pMesh->AddPolygon(t[0]);
		//pMesh->AddPolygon(t[1]);
		//pMesh->AddPolygon(t[2]);
		//pMesh->EndPolygon ();
	}
	Dump(log, "VertInfo", verts);

}
//
//void ImportBones( DAODumpStream& log, Matrix44 pm, NODERef node)
//{
//	int indent = log.IncreaseIndent();
//	try 
//	{
//		Text name = !node.isNull() ? node->get_name() : Text();
//
//		GFFListRef childList = node->get_children();
//
//		Vector4f trans = node->GetLocalTranslation();
//		Quaternion quat = node->GetLocalRotation();
//		float scale = node->GetLocalScale();
//		Matrix44 tm(trans, quat, scale);
//
//		float len = trans.Length();
//
//
//		Matrix44 im = pm * tm;
//
//		//[-0.161288,0.000001,0.986907]
//		//[-0.000001,-1.000000,0.000001]
//		//[0.986907,-0.000001,0.161288]
//		//[-0.002451,0.000000,0.371050]
//
//		Matrix44& dm = im;
//
//		Text txtPos, txtRot, txtYpr, txtScale, txtFormat, txtMatrix;
//		txtPos.Format("(%7.4f, %7.4f, %7.4f)", trans[0], trans[1], trans[2]);
//		txtRot.Format("(%7.4f, %7.4f, %7.4f, %7.4f)", quat.w, quat.x, quat.y, quat.z);
//		txtFormat.Format("%%-%ds | %%-24s | %%-32s\n", max(60 - indent*2, 1) );
//		//log.Indent();
//		//log.PrintF(txtFormat, name.c_str(), txtPos.c_str(), txtRot.c_str());
//
//
//		Text txtPos0, txtPos1, txtPos2, txtPos3;
//		Vector4f r0 = dm.GetRow(0);
//		Vector4f r1 = dm.GetRow(1);
//		Vector4f r2 = dm.GetRow(2);
//		Vector4f r3 = dm.GetRow(3);
//		txtPos0.Format("[%7.4f,%7.4f,%7.4f]", r0.x, r0.y, r0.z);
//		txtPos1.Format("[%7.4f,%7.4f,%7.4f]", r1.x, r1.y, r1.z);
//		txtPos2.Format("[%7.4f,%7.4f,%7.4f]", r2.x, r2.y, r2.z);
//		txtPos3.Format("[%7.4f,%7.4f,%7.4f]", r3.x, r3.y, r3.z);
//
//		txtFormat.Format("%%-%ds | %%s %%s %%s %%s\n", max(60 - indent*2, 1) );
//		log.Indent();
//		log.PrintF(txtFormat, name.c_str(), txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());
//
//
//		DAOArray< NODEPtr > children;
//		if ( DynamicCast(childList, children) ) {
//			for (size_t i=0, n=children.size(); i<n; ++i)
//				ImportBones(log, im, NODERef(children[i]));
//		}
//	}
//	catch( std::exception & ) 
//	{
//	}
//	catch( ... ) 
//	{
//	}
//	log.DecreaseIndent();
//}

void DumpMesh(MESHRef root)
{
	DAODumpStream out(NULL, false);

	DAOMemStream vertStream;
	DAOMemStream idxStream;
	GFFListRef idxList = root->get_indexdata();
	GFFListRef vertList = root->get_vertexdata();
	idxStream.Open(idxList->pdata(), idxList->size());
	vertStream.Open(vertList->pdata(), vertList->size());

	DAOArray<CHNKPtr> chunks;
	if ( DynamicCast<CHNK>(root->get_chunks(), chunks) ) {
		for (DAOArray<CHNKPtr>::iterator itr = chunks.begin(); itr != chunks.end(); ++itr) {
			CHNKRef chnk(*itr);
			DumpMesh(out, chnk, idxStream, vertStream);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

static void HelpString(Cmd::HelpType type){
	switch (type)
	{
	case Cmd::htShort: cout << "Decompile - Decompile MMH or MSH back to XML source files"; break;
	case Cmd::htLong:  
		{
			TCHAR fullName[MAX_PATH], exeName[MAX_PATH];
			GetModuleFileName(NULL, fullName, MAX_PATH);
			_tsplitpath(fullName, NULL, NULL, exeName, NULL);
			cout << "Usage: " << exeName << " Decompile [-opts[modifiers]] <mmh/msh file>" << endl 
				<< "  Simple decompile of DA:O MMH or MESH files." << endl
				<< endl
				<< "<Switches>" << endl
				<< "  v         Verbose Help (Lists specific versions)" << endl
				<< endl
				;
		}
		break;
	}
}

static bool ExecuteCmd(Program &prog)
{
	TCHAR infile[MAX_PATH], fullpath[MAX_PATH];
	int argc = prog.argc;
	TCHAR **argv = prog.argv;
	bool verboseHelp = false;
	TCHAR path[MAX_PATH];

	if (argc == 0)
		return false;

	infile[0] = 0;
	path[0] = 0;
	for (int i = 0; i < argc; i++)
	{
		TCHAR *arg = argv[i];
		if (arg == NULL)
			continue;
		if (arg[0] == '-' || arg[0] == '/')
		{
			switch (arg[1])
			{
			case 'v':
				verboseHelp = true;
				break;

			default:
				_fputts( _T("ERROR: Unknown argument specified \""), stderr );
				_fputts( arg, stderr );
				_fputts( _T("\".\n"), stderr );
				return false;
			}
		}
		else if (infile[0] == 0)
		{
			_tcscpy(infile, arg);
		}
	}
	if (infile[0] == 0)
	{
		_ftprintf( stderr, _T("File not specified.\n") );
		return false;
	}

	TCHAR drive[MAX_PATH], dir[MAX_PATH], fname[MAX_PATH], ext[MAX_PATH];
	GetFullPathName(infile, _countof(fullpath), fullpath, NULL);
	_tsplitpath(fullpath, drive, dir, fname, ext);
	if (path[0] == 0) {
		_tmakepath(path, drive, dir, fname, NULL);
	}

	struct _stat64 fstats;
	if ( -1 == _tstat64(fullpath, &fstats) ){
		_ftprintf( stderr, _T("Path is not a valid file or directory.\n") );
		return false;
	}

	if ( (fstats.st_mode & _S_IFDIR) != _S_IFDIR ) // file not directory
	{
		LPCSTR ext = PathFindExtension(fullpath);
		if (_tcsicmp(ext, _T("MMH")) == 0)
		{
			MMHFile file;
			file.open(fullpath);
			MDLHPtr root = file.get_Root();
			//DumpMesh(  );

			MMHExportImpl impl(this, MDLHRef(root), fullpath);
			lResult = impl.DoExport();
		}
		else if (_tcsicmp(ext, _T("MSH")) == 0)
		{
			MESHFile file;
			file.open(fullpath);
			MESHPtr root = file.get_Root();
			DumpMesh( MESHRef(root) );

		}

	}

	return false;
}

REGISTER_COMMAND(Decompile, HelpString, ExecuteCmd);
