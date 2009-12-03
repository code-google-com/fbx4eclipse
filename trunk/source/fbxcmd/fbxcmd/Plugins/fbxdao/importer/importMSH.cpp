/**********************************************************************
*<
FILE: ImporterCore.cpp

DESCRIPTION:	Core Import helper routines

CREATED BY: tazpn (Theo)

HISTORY: 

*>	Copyright (c) 2006, All Rights Reserved.
**********************************************************************/
#include "stdafx.h"
#include "reader.h"
#include "importerBase.h"
#include "MMHFormat.h"
#include "MSHFormat.h"
#include "MAOFormat.h"
#include <float.h>
#include "../common/fbxconvert.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
using namespace DAO::MESH;

typedef std::map<std::string, KArrayKFbxNode, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;
typedef KArrayTemplate<KFbxCluster*> KArrayKFbxCluster;

class MSHImportImpl: public ImporterBase
{
	typedef ImporterBase base;
public:
	MSHImportImpl(DAOReader *owner, KFbxScene* scene, KFbxStreamOptions* options, DAO::GFF::GFFFile &file);

	bool DoImport();
	KFbxNode * DoImport(KFbxNode * pNode, DAO::MMH::MSHHRef meshRef, KArrayKFbxNode& bones);
	KFbxNode *ImportMESH( KFbxNode* pParentNode , CHNKRef chnk 
		, DAOMemStream& idxStream , DAOMemStream& vertStream
		, MSHHRef meshRef, KArrayKFbxNode& bones );

	void AddVertexPosition( KFbxMesh* pMesh, int idx, const DECLValue& value );
	void AddVertexNormal( KFbxMesh* pMesh, int idx, const DECLValue& value );
	void AddVertexTexCoord( KFbxMesh* pMesh, int idx, const DECLValue& value );
	void AddVertexTangent( KFbxMesh* pMesh, int idx, const DECLValue& value );
	void AddVertexBinormal( KFbxMesh* pMesh, int idx, const DECLValue& value );
	void AddVertexColor( KFbxMesh* pMesh, int idx, const DECLValue& value );
	void AddVertexUnused( KFbxMesh* pMesh, int idx, const DECLValue& value );

	KFbxTexture* CreateTexture(LPCTSTR name);
	KFbxTexture* CreateNormalBump(LPCTSTR name, KFbxTexture* nmap);
	KFbxSurfaceMaterial* CreateMaterial(DAO::MMH::MSHHRef meshRef, KFbxMesh* pMesh);

	void LoadMaterial( KFbxMesh* pMesh, KFbxSurfaceMaterial * lMaterial, DAO::MAO::MaterialObjectRef matObj );

	DAO::MESH::MESHPtr root;
	BOOL enableScale;
	bool flipUV;
	float scaleFactor;
	bool normalUnderBump;
};


bool DAOReader::ReadMESH( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAO::GFF::GFFFile &file)
{
	KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
	bool            lIsAScene = (lScene != NULL);
	bool            lResult = false;
	if(lIsAScene)
	{
		MSHImportImpl impl(this, lScene, pStreamOptions, file);		
		lResult = impl.DoImport();
	}
	return lResult;
}


KFbxNode * DAOReader::ImportMESH(KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAO::GFF::GFFFile &file,
						   KFbxNode * pNode, DAO::MMH::MSHHRef meshRef, KArrayKFbxNode& bones )
{
	KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
	MSHImportImpl impl(this, lScene, pStreamOptions, file);
	return impl.DoImport(pNode, meshRef, bones);
}


bool MSHImportImpl::DoImport()
{
	KFbxNode* lRootNode = lScene->GetRootNode();
	KFbxNodeAttribute * lRootNodeAttribute = KFbxNull::Create(mManager,"");
	lRootNode->SetNodeAttribute(lRootNodeAttribute);

	KFbxGlobalSettings& globals = lScene->GetGlobalSettings();
	
	//globals.SetOriginalUpAxis(  KFbxAxisSystem(KFbxAxisSystem::ZAxis, KFbxAxisSystem::ParityEven, KFbxAxisSystem::RightHanded) );
	globals.SetAxisSystem( KFbxAxisSystem(KFbxAxisSystem::ZAxis, KFbxAxisSystem::ParityEven, KFbxAxisSystem::RightHanded) );

	IOSREF.GetBoolProp(IMP_FBX_MATERIAL,        true);
	IOSREF.GetBoolProp(IMP_FBX_TEXTURE,         true);
	IOSREF.GetBoolProp(IMP_FBX_LINK,            true);
	IOSREF.GetBoolProp(IMP_FBX_SHAPE,           true);
	IOSREF.GetBoolProp(IMP_FBX_GOBO,            true);
	IOSREF.GetBoolProp(IMP_FBX_ANIMATION,       true);
	IOSREF.GetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);

	enableScale = GetProp<bool>("EnableScale", true);
	scaleFactor = GetProp<float>("ScaleFactor", 1.f);
	flipUV = GetProp<bool>("FlipUV", true);
	normalUnderBump = GetProp<bool>("NormalUnderBump", true);
	
	globals.SetSystemUnit( KFbxSystemUnit(scaleFactor) );


	DAO::MMH::MSHHRef meshRef(NULL);
	KArrayKFbxNode bones;
	DoImport(lRootNode, meshRef, bones);
	return true;
}


KFbxNode * MSHImportImpl::DoImport(KFbxNode * pNode, DAO::MMH::MSHHRef meshRef, KArrayKFbxNode& bones)
{
	DAOMemStream vertStream;
	DAOMemStream idxStream;
	GFFListRef idxList = root->get_indexdata();
	GFFListRef vertList = root->get_vertexdata();
	idxStream.Open(idxList->pdata(), idxList->size());
	vertStream.Open(vertList->pdata(), vertList->size());

	Text meshGroupName = (!meshRef.isNull()) ? meshRef->get_meshGroupName() : Text();

	if (pNode == NULL)
		pNode = lScene->GetRootNode();

	DAOArray<CHNKPtr> chunks;
	if ( DynamicCast<CHNK>(root->get_chunks(), chunks) ) {
		for (DAOArray<CHNKPtr>::iterator itr = chunks.begin(); itr != chunks.end(); ++itr) {
			CHNKRef chnk(*itr);
			Text nodeName = chnk->get_name();
			if (!nodeName.isNull()) {
				if (meshGroupName.isNull() || nodeName.Compare(meshGroupName, true) == 0) {
					if ( KFbxNode *pMeshNode = ImportMESH(pNode, chnk, idxStream, vertStream, meshRef, bones) ) {
						return pMeshNode;
					}
				}
			}
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

MSHImportImpl::MSHImportImpl( DAOReader *owner, KFbxScene* scene, KFbxStreamOptions* options, DAO::GFF::GFFFile &file )
	: base(owner, scene, options, file), root(NULL)
{
	root.swap( StructPtrCast<DAO::MESH::MESH>( gffFile.get_RootStruct() ) );
}

//////////////////////////////////////////////////////////////////////////

template <typename T, typename U> 
void AssignValue(T& dst, const U& src) {
	KFBXLOG_ASSERT( !"Assigment not specified.");
}

template <typename T>  void AssignValue(T& dst, const T& src) { dst = src; }

template<> void AssignValue(float& dst, const float& src)          { dst = src; }
template<> void AssignValue(KFbxVector2& dst, const Vector2f& src) { dst.Set(src[0], src[1]); }
template<> void AssignValue(KFbxVector2& dst, const Float16_2& src) { dst.Set(src[0], src[1]); }
template<> void AssignValue(KFbxVector4& dst, const Vector2f& src) { dst.Set(src[0], src[1], 0.0); }
template<> void AssignValue(KFbxVector4& dst, const Vector3f& src) { dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(KFbxVector4& dst, const Vector4f& src) { dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(KFbxVector4& dst, const Float16_4& src) { dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(KFbxColor& dst, const Vector3f& src)   { dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(KFbxColor& dst, const Vector4f& src)   { dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(KFbxColor& dst, const ARGB4& src)      { dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(KFbxColor& dst, const Float16_4& src) { dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(KFbxVector4& dst, const Signed_10_10_10& src) { dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(KFbxVector4& dst, const Unsigned_10_10_10& src) { dst.Set(src[0], src[1], src[2]); }
template<> void AssignValue(KFbxColor& dst, const Signed_10_10_10& src) { dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(KFbxColor& dst, const Unsigned_10_10_10& src) { dst.Set(src[0], src[1], src[2], src[3]); }
template<> void AssignValue(fbxDouble3& dst, const Color4& src)   { dst = fbxDouble3(src[0], src[1], src[2]); }

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


inline Vector4f GetLocalTranslation(DAO::GFF::GFFListRef list) {
	if (list.isNull()) return Vector4f();
	TRSLPtr val = GetFirstOfType<TRSL>(list);
	return (!val.isNull()) ? val->get_translation() : Vector3f();
}

inline Quaternion GetLocalRotation(DAO::GFF::GFFListRef list) {
	if (list.isNull()) return Quaternion();
	ROTAPtr val = GetFirstOfType<ROTA>(list);
	return (!val.isNull()) ? val->get_rotation() : Quaternion();
}

inline float GetLocalScale(DAO::GFF::GFFListRef list) {
	if (list.isNull()) return 1.0f;
	SCALPtr val = GetFirstOfType<SCAL>(list);
	return (!val.isNull()) ? val->get_scale() : 1.0f;
}


KFbxNode *MSHImportImpl::ImportMESH(  KFbxNode* pParentNode
									, CHNKRef chnk
									, DAOMemStream& idxStream
									, DAOMemStream& vertStream
									, DAO::MMH::MSHHRef meshRef
									, KArrayKFbxNode& bones
									)
{
	LPCTSTR nodeName = chnk->get_name().c_str();
	KFbxMesh* pMesh = KFbxMesh::Create(mManager, nodeName);

	int nverts = chnk->get_Vertexcount();
	unsigned long vsize = chnk->get_Vertexsize();
	unsigned long voff = chnk->get_Vertexoffset();

	pMesh->InitControlPoints(nverts);

	// Set the normals on Layer 0.
	KFbxLayer* lLayer = pMesh->GetLayer(0);
	if (lLayer == NULL) {
		pMesh->CreateLayer();
		lLayer = pMesh->GetLayer(0);
	}

	KFbxSkin* lSkin = NULL;
	KArrayKFbxCluster clusters;
	bool hasSkin = bones.GetCount() > 0;

	GFFListRef declList = chnk->get_Vertexdeclarator();
	DAOArray<DECLPtr> decls;
	if ( DynamicCast<DECL>(declList, decls) ) 
	{
		// skin is processed after 
		DECLRef positionDecl(NULL), blendIdxDecl(NULL), blendWgtDecl(NULL);
		DECLRef normalDecl(NULL), binormalDecl(NULL), tangentDecl(NULL);
		DECLRef colorDecl(NULL), texcoordDecl(NULL);

		for (DAOArray<DECLPtr>::iterator itr = decls.begin(); itr != decls.end(); ++itr) {
			DECLRef decl(*itr);
			int strmIdx = decl->get_Stream();
			if (strmIdx == -1)
				continue;

			KFBXLOG_ASSERT( 0 == strmIdx );
			if ( 0 != strmIdx ) continue;
			
			KFBXLOG_ASSERT( 0 == decl->get_usageIndex() );
			if ( 0 != decl->get_usageIndex() ) continue;

			KFBXLOG_ASSERT( 0 == decl->get_Method() );
			if ( 0 != decl->get_Method() ) continue;

			switch ( decl->get_usage() )
			{
			case DECLUSAGE_POSITION:		positionDecl = decl; break;
			case DECLUSAGE_NORMAL:			normalDecl = decl; break;
			case DECLUSAGE_TEXCOORD:		texcoordDecl = decl; break;
			case DECLUSAGE_TANGENT:			tangentDecl = decl; break;
			case DECLUSAGE_BINORMAL:		binormalDecl = decl; break;
			case DECLUSAGE_BLENDWEIGHT:		blendWgtDecl = decl; break;
			case DECLUSAGE_BLENDINDICES:	blendIdxDecl = decl; break;
			case DECLUSAGE_COLOR:			colorDecl = decl; break;
			}
		}

		if (!positionDecl.isNull())
		{
			DECLTYPE type = positionDecl->get_dataType();
			long offset = positionDecl->get_Offset();
			KFbxVector4* pVerts = pMesh->GetControlPoints();
			for (int i = 0; i<nverts; ++i) {
				vertStream.Seek(SEEK_SET, i * vsize + offset + voff);
				KFbxVector4& vert = pVerts[i];
				AssignValue( vert, DECLValue(type, vertStream.dataptr()) );
				vert = vert;
			}
		}

		if (!normalDecl.isNull())
		{
			// We want to have one normal for each vertex (or control point),
			// so we set the mapping mode to eBY_CONTROL_POINT.
			KFbxLayerElementNormal* pNormLayer = KFbxLayerElementNormal::Create(pMesh, "");
			pNormLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
			pNormLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
			KFbxLayerElementArrayTemplate<KFbxVector4>& aNorms = pNormLayer->GetDirectArray();
			aNorms.Resize(nverts);
			for (int i = 0; i<nverts; ++i) {
				vertStream.Seek(SEEK_SET, i * vsize + normalDecl->get_Offset() + voff);
				KFbxVector4 norm;
				AssignValue( norm, DECLValue(normalDecl->get_dataType(), vertStream.dataptr()) );
				aNorms.SetAt(i, norm);
			}
			lLayer->SetNormals(pNormLayer);
		}

		if (!texcoordDecl.isNull())
		{
			KFbxLayerElementUV* lUVLayer=KFbxLayerElementUV::Create(pMesh, "UVChannel_1");
			lUVLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
			lUVLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
			KFbxLayerElementArrayTemplate<KFbxVector2>& aUVs = lUVLayer->GetDirectArray();
			aUVs.Resize(nverts);
			for (int i = 0; i<nverts; ++i) {
				KFbxVector2 uvw;
				vertStream.Seek(SEEK_SET, i * vsize + texcoordDecl->get_Offset() + voff);
				AssignValue(uvw, DECLValue(texcoordDecl->get_dataType(), vertStream.dataptr()) );

				if (uvw[0] < 0.0f || uvw[0] > 1.0f) uvw[0] -= floor(uvw[0]);
				if (uvw[1] < 0.0f || uvw[1] > 1.0f) uvw[1] -= floor(uvw[1]);
				if (flipUV) uvw[1] = 1.0f-uvw[1];

				//if (enableScale) 
				//	uvw.Set(uvw[0] * scaleFactor, uvw[1] * scaleFactor);
				aUVs.SetAt(i, uvw);
			}
			lLayer->SetUVs(lUVLayer, KFbxLayerElement::eDIFFUSE_TEXTURES);
		}

		if (!tangentDecl.isNull())
		{
			KFbxLayerElementTangent* pTangentLayer = KFbxLayerElementTangent::Create(pMesh, "Tangents");
			pTangentLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
			pTangentLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
			KFbxLayerElementArrayTemplate<KFbxVector4>& aTangents = pTangentLayer->GetDirectArray();
			aTangents.Resize(nverts);
			for (int i = 0; i<nverts; ++i) {
				vertStream.Seek(SEEK_SET, i * vsize + tangentDecl->get_Offset() + voff);
				KFbxVector4 tan;
				AssignValue(tan, DECLValue(tangentDecl->get_dataType(), vertStream.dataptr()) );
				aTangents.SetAt(i, tan);
			}
			lLayer->SetTangents(pTangentLayer);
		}

		if (!binormalDecl.isNull())
		{
			KFbxLayerElementBinormal* pBinormLayer = KFbxLayerElementBinormal::Create(pMesh, "Binormals");
			pBinormLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
			pBinormLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
			KFbxLayerElementArrayTemplate<KFbxVector4>& aBinorms = pBinormLayer->GetDirectArray();
			aBinorms.Resize(nverts);
			for (int i = 0; i<nverts; ++i) {
				vertStream.Seek(SEEK_SET, i * vsize + binormalDecl->get_Offset() + voff);
				KFbxVector4 binorm;
				AssignValue( binorm, DECLValue(binormalDecl->get_dataType(), vertStream.dataptr()) );
				aBinorms.SetAt(i, binorm);
			}
			lLayer->SetBinormals(pBinormLayer);
		}


		if (!colorDecl.isNull())
		{
			KFbxLayerElementVertexColor* pLayer = KFbxLayerElementVertexColor::Create(pMesh, "VertexColors");
			pLayer->SetMappingMode(KFbxLayerElement::eBY_CONTROL_POINT);
			pLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
			KFbxLayerElementArrayTemplate<KFbxColor>& aColors = pLayer->GetDirectArray();
			aColors.Resize(nverts);
			for (int i = 0; i<nverts; ++i) {
				vertStream.Seek(SEEK_SET, i * vsize + colorDecl->get_Offset() + voff);
				KFbxColor color;
				AssignValue( color, DECLValue(colorDecl->get_dataType(), vertStream.dataptr()) );
				aColors.SetAt(i, color);
			}
			lLayer->SetVertexColors(pLayer);
		}

		if (hasSkin && !blendIdxDecl.isNull() && !blendWgtDecl.isNull())
		{
			clusters.Resize( bones.GetCount() );

			int idxOffset = blendIdxDecl->get_Offset();
			int wgtOffset = blendWgtDecl->get_Offset();

			for (int i = 0; i<nverts; ++i) {
				Short4 blendIdx;
				KFbxVector4 weights;
				vertStream.Seek(SEEK_SET, i * vsize + idxOffset + voff);
				AssignValue( blendIdx, DECLValue(blendIdxDecl->get_dataType(), vertStream.dataptr()) );
				vertStream.Seek(SEEK_SET, i * vsize + wgtOffset + voff);
				AssignValue( weights, DECLValue(blendWgtDecl->get_dataType(), vertStream.dataptr()) );

				for (int idx=0;idx<4;++idx) {
					short boneIdx = blendIdx[idx];
					float weight = weights[idx];
					if (weight == 0.0)
						break;

					K_ASSERT(boneIdx >= 0 && boneIdx < bones.GetCount());

					if (boneIdx >= 0 && boneIdx < bones.GetCount())
					{
						KFbxNode * pBone = bones.GetAt(boneIdx);
						KFbxCluster * pCluster = clusters.GetAt(boneIdx);
						if (lSkin == NULL) {
							lSkin = KFbxSkin::Create(mManager, "");
						}
						if (pCluster == NULL) {
							pCluster = KFbxCluster::Create(mManager,"");
							pCluster->SetLink(pBone);
							pCluster->SetLinkMode(KFbxCluster::eTOTAL1);
							lSkin->AddCluster(pCluster);
							clusters.SetAt(boneIdx, pCluster);
						}
						pCluster->AddControlPointIndex(i, weight);
					}
				}
			}
		}
	}
	KFBXLOG_ASSERT( 0 == chnk->get_Indexformat());
	KFBXLOG_ASSERT( 0 == chnk->get_Basevertexindex());
	KFBXLOG_ASSERT( 0 == chnk->get_Minindex());

	idxStream.Seek(SEEK_SET, sizeof(short) * chnk->get_Startindex());
	UShort3 * pFaces = (UShort3*)idxStream.dataptr();
	int nfaces = chnk->get_Indexcount() / 3;
	for (int i=0; i<nfaces; ++i) {
		UShort3& t = pFaces[i];
		pMesh->BeginPolygon(i);
		pMesh->AddPolygon(t[0]);
		pMesh->AddPolygon(t[1]);
		pMesh->AddPolygon(t[2]);
		pMesh->EndPolygon ();
	}

	KFbxNode* pNode = KFbxNode::Create(mManager, nodeName);
	pNode->SetNodeAttribute(pMesh);
	pNode->SetShadingMode(KFbxNode::eTEXTURE_SHADING);

	pNode->RotationActive = true;
	pNode->PreRotation.Set( fbxDouble3(-90.0, 0.0, 0.0) );
	pNode->InheritType.Set( eINHERIT_RSrs );

	if (!meshRef.isNull()){
		// Set attributes
		this->SetUserPropString(pNode, "DAO_ID", meshRef->get_id());
		this->SetUserPropInt(pNode, "DAO_RuntimeShadow", meshRef->get_meshCastRuntimeShadow());
		this->SetUserPropInt(pNode, "DAO_BakedShadow", meshRef->get_meshCastBakedShadow());
		this->SetUserPropInt(pNode, "DAO_ReceiveRuntimeShadow", meshRef->get_meshReceiveRuntimeShadow());
		this->SetUserPropInt(pNode, "DAO_ReceiveBakedShadow", meshRef->get_meshReceiveBakedShadow());
		this->SetUserPropInt(pNode, "DAO_CutAway", meshRef->get_meshCutAway());
		this->SetUserPropInt(pNode, "DAO_PunchThrough", meshRef->get_meshCastRuntimeShadow());
		this->SetUserPropInt(pNode, "DAO_IsVFX", meshRef->get_meshIsVfxMesh());
		this->SetUserPropInt(pNode, "DAO_VariationTint", meshRef->get_useVariationTint());

		Text matObj = meshRef->get_materialObject();
		if (!matObj.isNull())
		{
			lLayer->GetMaterials()->SetMappingMode(KFbxLayerElement::eALL_SAME);
			lLayer->GetMaterials()->SetReferenceMode(KFbxLayerElement::eINDEX_TO_DIRECT);
			CreateMaterial(meshRef, pMesh);
		}

		// Set Offset
		GFFListRef list = meshRef->get_children();

		Vector4f pos = GetLocalTranslation(list);
		Quaternion rot = GetLocalRotation(list);
		float scl = GetLocalScale(list);
		KFbxXMatrix m = TOMATRIX3(pos, rot, scl);
		PosRotScaleNode(pNode, m);
	}

	pParentNode->AddChild(pNode);

	if (lSkin != NULL ) {

		// Now we have the Patch and the skeleton correctly positioned,
		// set the Transform and TransformLink matrix accordingly.
		KFbxXMatrix lXMatrix = pNode->GetGlobalFromDefaultTake(KFbxNode::eSOURCE_SET);

		for (int i=0; i<clusters.GetCount(); ++i)
		{
			if (KFbxCluster* pCluster = clusters[i])
			{
				KFbxNode *pBone = bones[i];
				pCluster->SetTransformMatrix(lXMatrix);
				KFbxXMatrix lXMatrixLink = pBone->GetGlobalFromDefaultTake(KFbxNode::eSOURCE_SET);
				pCluster->SetTransformLinkMatrix(lXMatrixLink);
			}
		}

		pMesh->AddDeformer(lSkin);
	}


	return pNode;
}


KFbxTexture* MSHImportImpl::CreateTexture(LPCTSTR name)
{
	KFbxTexture* lTexture = KFbxTexture::Create(mManager, name);

	_tstring fullname = ResourceManager::FindFile(name);

	lTexture->SetRelativeFileName(name);
	lTexture->SetFileName(fullname.c_str()); // Resource file is in current directory.
	lTexture->SetTextureUse(KFbxTexture::eSTANDARD);
	lTexture->SetMappingType(KFbxTexture::eUV);
	lTexture->SetMaterialUse(KFbxTexture::eMODEL_MATERIAL);
	lTexture->SetSwapUV(false);
	lTexture->SetTranslation(0.0, 0.0);
	lTexture->SetScale(1.0, 1.0);
	lTexture->SetRotation(0.0, 0.0);

	return lTexture;
}

KFbxTexture* MSHImportImpl::CreateNormalBump(LPCTSTR name, KFbxTexture* nmap)
{
#if 0
	KFbxNode *texmap = (KFbxNode*)o->gi->CreateInstance(TEXMAP_CLASS_ID, GNORMAL_CLASS_ID);
	if(texmap != NULL)
	{
		texmap->SetName(name);
		texmap->SetSubTexmap(0, nmap);
		return texmap;
	}
#endif
	return nmap;
}

static bool isValidFloat( float value )
{
	switch (_fpclass(value))
	{
	case _FPCLASS_SNAN:
	case _FPCLASS_QNAN:
	case _FPCLASS_NINF:
	case _FPCLASS_PINF:
		return false;
	}
	return true;
}

static bool isValidColor( const Color4& value )
{
	return isValidFloat(value.r) && isValidFloat(value.b) && isValidFloat(value.g) && isValidFloat(value.a);
}

KFbxSurfaceMaterial* MSHImportImpl::CreateMaterial(DAO::MMH::MSHHRef meshRef, KFbxMesh* pMesh)
{
	KFbxSurfaceMaterial *m = NULL;
	if (meshRef.isNull())
		return NULL;

	Text name = meshRef->get_materialObject();
	//KFbxSurfacePhong *lMaterial = KFbxSurfacePhong::Create(mManager, name);
	KFbxSurfacePhong *lMaterial = KFbxSurfacePhong::Create(mManager, name);
	lMaterial->Reset();

	Color4 matColor = meshRef->get_meshMaterialColor();
	KString lShadingName = "phong";
	lMaterial->GetShadingModel().Set(lShadingName);

	if ( isValidColor(matColor) ) {
		fbxDouble3 color;
		AssignValue(color, matColor);
		lMaterial->GetAmbientColor().Set( color );
		lMaterial->GetAmbientFactor().Set( 1.0 );
		lMaterial->GetDiffuseColor().Set( color );
		lMaterial->GetDiffuseFactor().Set( 1.0 );
	}
	lMaterial->GetEmissiveColor().Set(fbxDouble3(0.0, 0.0, 0.0));
	lMaterial->GetAmbientColor().Set(fbxDouble3(0.8, 0.8, 0.8));
	lMaterial->GetDiffuseColor().Set(fbxDouble3(0.8, 0.8, 0.8));
	lMaterial->GetSpecularColor().Set(fbxDouble3(0.0, 0.0, 0.0));
	lMaterial->GetTransparencyFactor().Set(0.0);
	lMaterial->GetShininess().Set(0.5);

	lMaterial->GetShadingModel().Set(KString("phong"));


	if (!name.isNull()) {
		name.append(".mao");

		DAOStreamPtr maoStream = ResourceManager::OpenStream(name);
		if (maoStream.isNull()) {
			KFbxLog::LogWarn("Unable to locate material object file: %s", name.c_str());
		} else  {
			try
			{
				DAO::MAO::MAOFile maoFile;
				maoFile.open(*maoStream);
				DAO::MAO::MaterialObjectRef matObj = maoFile.get_Object();
				if (matObj.isNull())
					KFbxLog::LogWarn("Unable to process material object: %s", name.c_str());
				else
				{
					_tstring matFileName = matObj->Material;
					matFileName.append(".xml");
					try
					{
						DAOStreamPtr matStream = ResourceManager::OpenStream(matFileName.c_str());
						if (matStream.isNull()) {
							KFbxLog::LogWarn("Unable to locate material file: %s", matFileName.c_str());
						} else  {
							DAO::MAT::MATFile matFile;
							matFile.open(*matStream);
							matObj->loadSettingsFromMaterial( matFile.get_Object() );
						}
					}
					catch (...)
					{
						KFbxLog::LogWarn("Exception loading material file: %s", matFileName.c_str());
					}
					LoadMaterial(pMesh, lMaterial, matObj);
				}
			}
			catch (...)
			{
				KFbxLog::LogWarn("Exception loading material object file: %s", name.c_str());
			}
		}

	}

	//get the node of mesh, add material for it.
	if (pMesh != NULL)
	{
		KFbxNode* lNode = pMesh->GetNode();
		if(lNode)             
			lNode->AddMaterial(lMaterial);
	}

	m = lMaterial;
	return m;
}

void MSHImportImpl::LoadMaterial(KFbxMesh* pMesh, KFbxSurfaceMaterial * lMaterial, DAO::MAO::MaterialObjectRef matObj )
{
	Text diffuse = matObj->asTexture(_T("mml_tDiffuse")).resName.c_str();
	Text normal = matObj->asTexture(_T("mml_tNormalMap")).resName.c_str();
	//_tstring filterColor = matObj->asText(_T("mml_tTintMask"));

	//lMaterial->GetSpecularColor().Set( Convert(mtl.Ks) );
	//lMaterial->GetSpecularFactor().Set(mtl.specularValue );

	if ( KFbxSurfacePhong* lPhong = KFbxCast<KFbxSurfacePhong>(lMaterial) )
	{
		lPhong->GetEmissiveColor().Set( fbxDouble3(0.0, 0.0, 0.0) );
		lPhong->GetEmissiveFactor().Set( 0.0f );


		// Generate primary and secondary colors.

		float opacity = matObj->asFloat32(_T("mml_fOpacityMultiplier"));
		if (opacity >= 0.0 && opacity <= 1.0)
		{
			//lMaterial->GetTransparentColor().Set( fbxDouble3(1.0, 1.0, 1.0) );
			lPhong->GetTransparencyFactor().Set(1.0 - opacity);
		}
	}


	//double value = pow(2.0, double(mtl.specularPower));
	//lMaterial->GetShininess().Set( value );

	KFbxLayer* lLayer = pMesh->GetLayer(0);

	

	if (!diffuse.isNull()) {
		KFbxLayerElementTexture* lTextureDiffuseLayer = lLayer->GetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES);

		// Set texture mapping for diffuse channel.
		if (!lTextureDiffuseLayer)
		{
			lTextureDiffuseLayer = KFbxLayerElementTexture::Create(pMesh, "");
			lLayer->SetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES, lTextureDiffuseLayer);
			lTextureDiffuseLayer->SetMappingMode(KFbxLayerElement::eALL_SAME);
			lTextureDiffuseLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
			//lLayer->SetDiffuseTextures(lTextureDiffuseLayer);
			//lLayer->SetTextures(KFbxLayerElement::eDIFFUSE_TEXTURES, lTextureDiffuseLayer);
		}
		KFbxTexture* lTexture = CreateTexture(diffuse);
		lTextureDiffuseLayer->GetDirectArray().Add( lTexture );

		KFbxProperty lProperty = KFbxProperty::Create(lMaterial,KFbxSurfaceMaterial::sDiffuse, DTVector3D, "", true, NULL);
		int lNbTex = lProperty.GetSrcObjectCount(KFbxTexture::ClassId);
		lProperty.ConnectSrcObject(lTexture);
	}
	if (!normal.isNull()) {
		KFbxLayerElementTexture* lTextureLayer=KFbxLayerElementTexture::Create(pMesh, "");
		lTextureLayer->SetMappingMode(KFbxLayerElement::eALL_SAME);
		lTextureLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);

		// TODO: 100% seems like too much for true normal bumps. Use default value
		//m->SetTexmapAmt(ID_BU, 1.0f, 0); 
		KFbxTexture* tex = CreateTexture(normal);
		if (normalUnderBump) {
			tex = CreateNormalBump(normal, tex);
			lLayer->SetTextures(KFbxLayerElement::eNORMALMAP_TEXTURES, lTextureLayer);
			lTextureLayer->GetDirectArray().Add( tex );
		} else {
			lLayer->SetTextures(KFbxLayerElement::eBUMP_TEXTURES, lTextureLayer);
			lTextureLayer->GetDirectArray().Add( tex );
		}	
		KFbxProperty lProperty = KFbxProperty::Create(lMaterial,KFbxSurfaceMaterial::sNormalMap, DTVector3D, "", true, NULL);
		int lNbTex = lProperty.GetSrcObjectCount(KFbxTexture::ClassId);
		lProperty.ConnectSrcObject(tex);

	}
	/*if (!filterColor.isNull()) {
		KFbxLayerElementTexture* lTextureLayer=KFbxLayerElementTexture::Create(pMesh, "");
		lTextureLayer->SetMappingMode(KFbxLayerElement::eALL_SAME);
		lTextureLayer->SetReferenceMode(KFbxLayerElement::eDIRECT);
		lLayer->SetSpecularTextures(lTextureLayer);
		lTextureLayer->GetDirectArray().Add( CreateTexture(filterColor) );
	}*/
}