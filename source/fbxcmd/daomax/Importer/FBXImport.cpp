/**********************************************************************
*<
FILE: ImporterCore.cpp

DESCRIPTION:	Core Import helper routines

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "DAOImport.h"
#if VERSION_3DSMAX >= ((7000<<16)+(15<<8)+0) // Version 7
#  include <IFrameTagManager.h>
#endif
#include <notetrck.h>
#include "FBXImport.h"
#include "maxconvert.h"

#include <stdexcept>
using std::runtime_error;

extern ClassDesc2* GetDAOImportDesc();
extern "C" KFbxSdkManager* GetFbxManager();


typedef std::map<std::string, INodeTab, ltstr> NameNodeMap;

static const char *FBXImport = "FBXImport";
static const char *FBXCommon = "FBXCommon";

const float FramesPerSecond = 30.0f;
const float FramesIncrement = 1.0f/30.0f;
const int TicksPerFrame = GetTicksPerFrame();

inline TimeValue TimeToFrame(float t) {
	return TimeValue(t * FramesPerSecond * TicksPerFrame);
}
inline float FrameToTime(TimeValue t) {
	return float(t) / (FramesPerSecond * TicksPerFrame);
}


struct FBXImportImpl
{
	FBXImportImpl(FBXImporter *owner);
	~FBXImportImpl();


	void ImportObject(INode* parent, KFbxNode* lNode, LPCTSTR name = NULL);
	void ImportNode(INode* parent, KFbxNode* lNode, LPCTSTR name = NULL);
	void ImportSkeleton( INode* parent, KFbxSkeleton* lSkel, LPCSTR name = NULL);
	void ImportMesh( INode* parent, KFbxMesh* lMesh, LPCSTR name = NULL);
	void ImportMaterialObject( INode* parent, KFbxMesh* pMesh, LPCTSTR matName, LPCTSTR texName);
	Text GetDiffuseMaterialName(KFbxLayerContainer* lLayerContainer);
	bool GetDiffuseMaterialAndTextureName(KFbxLayerContainer* lLayerContainer, Text &matName, Text& texName);

	//void ImportBones( INode* parent, KFbxXMatrix pm, NODERef bones);
	INode *CreateBone(const char* name, Point3 startPos, Point3 endPos, Point3 zAxis);


	Control* MakePosition(Control *tmCont, Class_ID clsid);
	Control* MakePositionXYZ(Control *tmCont, Class_ID clsid);
	Control* MakeRotation(Control *tmCont, Class_ID rotClsid, Class_ID rollClsid);
	Control* MakeScale(Control *tmCont, Class_ID clsid);

	BOOL clearAnimation;
	BOOL enableScale;
	float scaleFactor;
	BOOL enableAnimation;
	BOOL flipUV;

	FBXImporter *o;
	Interface *gi;
};

FBXImporter::FBXImporter(const TCHAR *Name,ImpInterface *I,Interface *GI, BOOL SuppressPrompts)
: BaseImporter()
{
	BaseInit(Name,I,GI,SuppressPrompts);
}

FBXImporter::FBXImporter()
{
}

void FBXImporter::Initialize()
{
}

FBXImportImpl::FBXImportImpl(FBXImporter *owner) : o(owner)
{
	gi = o->gi;

	ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");
	clearAnimation = TRUE;
	enableScale = owner->GetIniValue<BOOL>(FBXImport, "EnableScale", 1);
	scaleFactor = owner->GetIniValue<float>(FBXImport, "ScaleFactor", 1.0f);
	enableAnimation = owner->GetIniValue<BOOL>(FBXImport, "EnableAnimation", 0);
	flipUV = owner->GetIniValue<BOOL>(FBXImport, "FlipUV", 1);
	
}

FBXImportImpl::~FBXImportImpl()
{
}

#pragma region DoImport
bool FBXImporter::DoImport()
{
	bool lStatus = false;
	if ( KFbxScene* pScene = KFbxScene::Create(GetFbxManager(), PathFindFileName(name.c_str())) )
	{
		int lFileMajor, lFileMinor, lFileRevision;
		bool lStatus;
		char lPassword[1024];

		KFbxSdkManager* pSdkManager = GetFbxManager();

		int lFileFormat = -1;
		int lFileFormatSubType = -1;

		LPCTSTR pFilename = this->name.c_str();
		// Create an importer.
		KFbxImporter* lImporter = KFbxImporter::Create(pSdkManager,"");

		if (lFileFormat == -1)
		{
			if (!pSdkManager->GetIOPluginRegistry()->DetectFileFormat(pFilename, lFileFormat))
			{
				// Unrecognizable file format. Try to fall back to native format.
				lFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeReaderFormat();
			}
		}
		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(pFilename, lFileFormat);
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if( !lImportStatus )
		{
			KFbxLog::LogError("Call to KFbxImporter::Initialize() failed.");
			KFbxLog::LogError("Error returned: %s", lImporter->GetLastErrorString());

			if (lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
				lImporter->GetLastErrorID() == KFbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
			{
				KFbxLog::LogError("FBX version number for file %s is %d.%d.%d", pFilename, lFileMajor, lFileMinor, lFileRevision);
			}

			return false;
		}

		if (lImporter->IsFBX())
		{
			KFbxLog::LogDebug("FBX version number for file %s is %d.%d.%d", pFilename, lFileMajor, lFileMinor, lFileRevision);

			// Set the import states. By default, the import states are always set to 
			// true. The code below shows how to change these states.
			IOSREF.SetBoolProp(IMP_FBX_MATERIAL,        true);
			IOSREF.SetBoolProp(IMP_FBX_TEXTURE,         true);
			IOSREF.SetBoolProp(IMP_FBX_LINK,            true);
			IOSREF.SetBoolProp(IMP_FBX_SHAPE,           true);
			IOSREF.SetBoolProp(IMP_FBX_GOBO,            true);
			IOSREF.SetBoolProp(IMP_FBX_ANIMATION,       true);
			IOSREF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(pScene);

		if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
		{
			printf("Please enter password: ");

			lPassword[0] = '\0';

			scanf("%s", lPassword);
			KString lString(lPassword);

			IOSREF.SetStringProp(IMP_FBX_PASSWORD,      lString);
			IOSREF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);
			lStatus = lImporter->Import(pScene);

			//lImportOptions->SetOption(KFBXSTREAMOPT_FBX_PASSWORD, lString);
			//lImportOptions->SetOption(KFBXSTREAMOPT_FBX_PASSWORD_ENABLE, true);
			//lStatus = lImporter->Import(pScene, lImportOptions);

			if(lStatus == false && lImporter->GetLastErrorID() == KFbxIO::ePASSWORD_ERROR)
			{
				KFbxLog::LogError("\nPassword is wrong, import aborted.");
			}
		}
		else if (lStatus == false)
		{
			{
				KFbxIO::EError errcode = lImporter->GetLastErrorID();
				LPCSTR errstr = lImporter->GetLastErrorString();
				KFbxLog::LogError("Importer Error %d: %s", errcode, errstr);
			}
			{
				KFbxDocument::EError errcode = pScene->GetLastErrorID();
				LPCSTR errstr = pScene->GetLastErrorString();
				KFbxLog::LogError("Document Error %d: %s", errcode, errstr);
			}
		}
		if (lStatus == true)
		{
			try
			{
				FBXImportImpl impl(this);
				impl.ImportNode( NULL, pScene->GetRootNode(), NULL );
			}
			catch (...)
			{
			}
		}

		// Destroy the importer.
		pScene->Destroy();
		lImporter->Destroy();
	}
	return lStatus;
}
#pragma endregion

void FBXImportImpl::ImportObject( INode* parent, KFbxNode* lNode, LPCTSTR name /*= NULL*/ )
{
	LPCTSTR cname = lNode->GetName();
	if ( KFbxNodeAttribute *pAttr = lNode->GetNodeAttribute() )
	{
		KFbxNodeAttribute::EAttributeType lAttributeType = pAttr->GetAttributeType();
		switch (lAttributeType)
		{
		case KFbxNodeAttribute::eUNIDENTIFIED:
		case KFbxNodeAttribute::eNULL:
			ImportNode(parent, lNode, name);
			break;

		case KFbxNodeAttribute::eMARKER:
			break;

		case KFbxNodeAttribute::eSKELETON:
			ImportSkeleton(parent, (KFbxSkeleton*)pAttr, name);
			break;

		case KFbxNodeAttribute::eMESH:
			ImportMesh(parent, (KFbxMesh*)pAttr, name);
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
		ImportNode(parent, lNode, name);
	}
}


void FBXImportImpl::ImportNode( INode* parent, KFbxNode* lNode, LPCTSTR name /*= NULL*/ )
{
	if (name == NULL) name = lNode->GetName();

	// Calc Bounding Box
	KFbxVector4 lT, lR, lS;
	lNode->GetDefaultT(lT);
	lNode->GetDefaultR(lR);
	lNode->GetDefaultS(lS);
	KFbxQuaternion qr; qr.ComposeSphericalXYZ(lR);

	KFbxXMatrix tm;
	tm.SetT(lT), tm.SetQ(qr), tm.SetS(lS);
	Matrix3 tm3 = TOMATRIX3(tm);

	Point3 p = TOPOINT3(lT);
	Quat q = TOQUAT(qr);
	PosRotScale prs = prsDefault;
	Point3 pp;
	Point3 zAxis(0,0,0);
	float scale = Average(lS);

	int len = lNode->GetChildCount();
	if (len > 0) {
		float len = 0.0f;
		for (int lChild = 0; lChild < lNode->GetChildCount(); ++lChild)
			len += lNode->GetDefaultT(lT).Length();
		len /= float(lNode->GetChildCount());
		pp = p + Point3(len, 0.0f, 0.0f); // just really need magnitude as rotation will take care of positioning
	}
	else if (parent)
	{
		pp = p;
	}
	INode *bone = gi->GetINodeByName(name);
	if (bone)
	{
		// Is there a better way of "Affect Pivot Only" behaviors?
		INode *pinode = bone->GetParentNode();
		if (pinode)
			bone->Detach(0,1);
		PosRotScaleNode(bone, p, q, scale, prs);
		if (pinode)
			pinode->AttachChild(bone, 1);
	}
	else
	{
		if (bone = CreateBone(name, p, pp, zAxis))
		{
			PosRotScaleNode(bone, p, q, scale, prs);
			//bone->SetNodeTM(0, mm);
			//bone->Hide(node->GetVisibility() ? FALSE : TRUE);
		}
		if (bone != NULL && parent != NULL)
		{
			parent->AttachChild(bone, 1);
		}
	}
	for( int i = 0; i < lNode->GetChildCount(); i++)
	{
		ImportObject(bone, lNode->GetChild(i));
	}
}

void FBXImportImpl::ImportSkeleton( INode* parent, KFbxSkeleton* lSkel, LPCSTR name )
{
	ImportNode(parent, lSkel->GetNode(), name);
}


Text FBXImportImpl::GetDiffuseMaterialName(KFbxLayerContainer* lLayerContainer)
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

bool FBXImportImpl::GetDiffuseMaterialAndTextureName(KFbxLayerContainer* lLayerContainer, Text &matName, Text& texName)
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


void FBXImportImpl::ImportMesh( INode* parent, KFbxMesh* pMesh, LPCSTR name )
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
	KFbxQuaternion q; q.ComposeSphericalXYZ(lR);
	{
		Text matName, texName;
		if (GetDiffuseMaterialAndTextureName(pMesh, matName, texName))
		{
			ImportMaterialObject(parent, pMesh, matName, texName);
		}
	}

	KFbxLayerElement::EMappingMode lMappingMode = KFbxLayerElement::eNONE;

	int lVertexCount = pMesh->GetControlPointsCount();
	{
		KFbxVector4* pVerts = pMesh->GetControlPoints();
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
			KFbxVector4& v = pVerts[iVertex];
			sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
		}
	}

	if (KFbxLayerElementUV const* pUVs = pMesh->GetLayer(0)->GetUVs())
	{
		KFbxLayerElementArrayTemplate<KFbxVector2>& uvs = pUVs->GetDirectArray();
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex) {
			KFbxVector2 uvw = uvs[iVertex];
			if (uvw[0] < 0.0f || uvw[0] > 1.0f) uvw[0] -= floor(uvw[0]);
			if (uvw[1] < 0.0f || uvw[1] > 1.0f) uvw[1] -= floor(uvw[1]);
			if (flipUV) uvw[1] = 1.0f-uvw[1];
			sstr.AppendFormat("%g %g\n", (double)uvw[0], (double)uvw[1]);
		}
	}
	if (KFbxLayerElementNormal const* pNorms = pMesh->GetLayer(0)->GetNormals())
	{
		KFbxLayerElementArrayTemplate<KFbxVector4>& array = pNorms->GetDirectArray();
		Text sstr;
		for (int iVertex=0; iVertex < lVertexCount; ++iVertex){
			KFbxVector4& v = array[iVertex];
			sstr.AppendFormat("%g %g %g %g\n", (double)v[0], (double)v[1], (double)v[2], 1.0);
		}
	}
	{
		int lPolygonCount = pMesh->GetPolygonCount();
		int lVertices = 0;
		for (int lPolygon = 0; lPolygon < lPolygonCount; lPolygon++){
			lVertices += pMesh->GetPolygonSize(lPolygon);
		}
		Text sstr;
		for (int lPolygon = 0; lPolygon < lPolygonCount; lPolygon++){
			int lPolygonSize = pMesh->GetPolygonSize(lPolygon);
			//KFBXLOG_ASSERT(lPolygonSize == 3);
			for (int lIdx = 0; lIdx < lPolygonSize; lIdx++) {
				int lControlPointIndex = pMesh->GetPolygonVertex(lPolygon, lIdx);
				sstr.AppendFormat("%d ", lControlPointIndex);
			}
			sstr.append('\n');
		}
	}
}


void FBXImportImpl::ImportMaterialObject( INode* parent, KFbxMesh* pMesh, LPCTSTR matName, LPCTSTR texPath )
{
	FixedString<CHAR, MAX_PATH> texName = PathFindFileName(texPath);
	PathRenameExtension(texName, ".dds");
}


INode *FBXImportImpl::CreateBone(const char* name, Point3 startPos, Point3 endPos, Point3 zAxis)
{
	if (FPInterface * fpBones = GetCOREInterface(Interface_ID(0x438aff72, 0xef9675ac)))
	{
		FunctionID createBoneID = fpBones->FindFn(TEXT("createBone"));
		FPValue result;
		FPParams params (3, TYPE_POINT3, &startPos, TYPE_POINT3, &endPos, TYPE_POINT3, &zAxis);     
		FPStatus status = fpBones->Invoke(createBoneID, result, &params);
		if (status == FPS_OK && result.type == TYPE_INODE)
		{
			if (INode *n = result.n)
			{
				n->SetName(const_cast<TCHAR*>(name));
				float len = Length(endPos-startPos);
				float width = max(0.5f, min(3.0f, len * 0.25f));
				if (Object* o = n->GetObjectRef())
				{
					setMAXScriptValue(o->GetReference(0), "width", 0, width);
					setMAXScriptValue(o->GetReference(0), "height", 0, width);
				}
				n->BoneAsLine(1);
				n->ShowBone(2);
				n->IgnoreExtents(TRUE);
			}
			return result.n;
		}
		fpBones->ReleaseInterface();
	}
	return NULL;
}
#if 0
void FBXImportImpl::ImportBones( INode* parent, KFbxXMatrix pm, NODERef node)
{
	int indent = log.IncreaseIndent();
	try 
	{
		Text name = !node.isNull() ? node->get_name() : Text();

		GFFListRef childList = node->get_children();

		Vector4f trans = node->GetLocalTranslation();
		float len = trans.Length();
		Quaternion quat = node->GetLocalRotation();
		float scale = node->GetLocalScale();

		KFbxXMatrix tm(trans, quat, scale);
		KFbxXMatrix im = pm * tm;

		Matrix3 tm3 = TOMATRIX3(tm);
		Matrix3 pm3(true), pm3p(true);
		pm3p = TOMATRIX3(pm);
		if (parent) pm3 = parent->GetNodeTM(0);

		Matrix3 im3 = pm3 * tm3;
		Matrix3 im3r = tm3 * pm3;

		Matrix3 mm = im3r;
		Point3 p = TOPOINT3(im.GetTrans());
		Quat q = TOQUAT(im.GetRot());
		PosRotScale prs = prsDefault; //(PosRotScale)(prsPos|prsRot);

		Point3 pp;
		Point3 zAxis(0,0,0);

		DAOArray<NODEPtr> children;
		bool hasChildren = DynamicCast(childList, children);
		if (hasChildren) {
			float len = 0.0f;
			for (DAOArray<NODEPtr>::iterator itr=children.begin(), end = children.end(); itr != end; ++itr) {
				Vector4f trans = node->GetLocalTranslation();
				len += trans.Length();
			}
			len /= float(children.size());
			pp = p + Point3(len, 0.0f, 0.0f); // just really need magnitude as rotation will take care of positioning
		}
		else if (parent)
		{
			pp = p + Point3(len/3.0f, 0.0f, 0.0f);
		}
		INode *bone = gi->GetINodeByName(name.c_str());
		if (bone)
		{
			// Is there a better way of "Affect Pivot Only" behaviors?
			INode *pinode = bone->GetParentNode();
			if (pinode)
				bone->Detach(0,1);
			PosRotScaleNode(bone, p, q, scale, prs);
			if (pinode)
				pinode->AttachChild(bone, 1);
		}
		else
		{
			if (bone = CreateBone(name, p, pp, zAxis))
			{
				
				//PosRotScaleNode(bone, p, q, scale, prs);
				bone->SetNodeTM(0, mm);
				//bone->Hide(node->GetVisibility() ? FALSE : TRUE);
			}
			if (bone != NULL && parent != NULL)
			{
				parent->AttachChild(bone, 1);
			}
		}

		Matrix3 ltm = GetNodeLocalTM(bone, 0);
		Point3 lp = ltm.GetTrans();
		Quat lq(ltm);

		if (bone)
		{
			for (size_t i=0, n=children.size(); i<n; ++i)
				ImportBones(bone, im, NODERef(children[i]));
		}
	}
	catch( std::exception & ) 
	{
	}
	catch( ... ) 
	{
	}
	log.DecreaseIndent();
}

#if 0

Matrix3 FBXImportImpl::GetLocalTransform(Skeleton& skel, size_t index)
{
	Matrix3 m3(true);
	if (index >= 0)
	{
		Bone& bone = skel.Bones[index];
		m3.SetRotate( Inverse(bone.Transform.Rotation) );
		m3.SetTrans(bone.Transform.Origin);
		m3 *= ScaleMatrix(Point3(bone.Transform.Scale.m[0][0], bone.Transform.Scale.m[1][1], bone.Transform.Scale.m[2][2]));

		RHMatrix4& r = bone.InverseWorldTransform;
		Matrix3 tm(
			Point3(r.m[0][0], r.m[0][1], r.m[0][2]),
			Point3(r.m[1][0], r.m[1][1], r.m[1][2]),
			Point3(r.m[2][0], r.m[2][1], r.m[2][2]),
			Point3(r.m[3][0], r.m[3][1], r.m[3][2])
			);
		Matrix3 itm = Inverse(tm);
		//m3 = itm;
	}
	return m3;
}

Matrix3 FBXImportImpl::GetWorldTransform(Skeleton& skel, size_t index)
{
	Bone& bone = skel.Bones[index];
	if (bone.ParentIndex >= 0)
	{
		return GetLocalTransform(skel, index) * GetWorldTransform(skel, bone.ParentIndex);
	}
	else
	{
		return GetLocalTransform(skel, index) * wtm;
	}
}


void FBXImportImpl::OrderBones(INodeTab& bones)
{
	if (info.Skeletons.size() == 1)
	{
		NameNodeMap nodes;
		INodeTab rv = ImportSkeleton(*info.Skeletons[0]);
		// Strip out auto-generated bones.  Place in order found in ini file
		for (size_t i = 0, n = rv.Count(); i<n; ++i)
		{
			INode *node = rv[i];

			bool found = false;
			for (stringlist::const_iterator itr=boneMatch.begin(), end=boneMatch.end(); itr != end; ++itr)
			{
				if (wildmatch(*itr, node->GetName())) {
					nodes[*itr].Append(1, &node);
					found = true;
				}
			}
			if (!found)
			{
				node->SetUserPropInt("FBXBoneIndex", int(bones.Count()));
				bones.Append(1, &node);
			}
		}
		for (stringlist::const_iterator itr=boneMatch.begin(), end=boneMatch.end(); itr != end; ++itr)
		{
			INodeTab& map = nodes[*itr];
			for (size_t i = 0, n = map.Count(); i<n; ++i)
			{
				INode *node = map[i];
				node->SetUserPropInt("FBXBoneIndex", int(bones.Count()));
				bones.Append(1, &node);
			}
		}
		// When in face mode, swap the Face Bones for the first N bones in the skeleton.  
		//   Some of the later bones like Head, Neck, Ribcage are still used so we cannot 
		//   discard the whole skeleton.
		if (enableFaceMode)
		{
			size_t curIdx = 0;
			for (size_t i = 0, n = bones.Count(); i<n; ++i)
			{
				INode *node = bones[i];
				for (stringlist::const_iterator itr=faceBoneMatch.begin(), end=faceBoneMatch.end(); itr != end; ++itr)
				{
					if (wildmatch(*itr, node->GetName())) {
						bones[i] = bones[curIdx];
						bones[curIdx] = node;
						node->SetUserPropInt("FBXBoneIndex", int(curIdx));
						++curIdx;
						break;
					}
				}
			}
		}
	}
}

void FBXImportImpl::ClearAnimation(Control *c)
{
	if (c != NULL)
	{
		if (c->IsColorController())
			return;

		if (IKeyControl *ikeys = GetKeyControlInterface(c)){
			ikeys->SetNumKeys(0);
		}
#if VERSION_3DSMAX > ((5000<<16)+(15<<8)+0) // Version 5
		if (Control *sc = c->GetWController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
#endif
		if (Control *sc = c->GetXController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
		if (Control *sc = c->GetYController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
		if (Control *sc = c->GetZController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
		if (Control *sc = c->GetRotationController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
		if (Control *sc = c->GetPositionController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
		if (Control *sc = c->GetScaleController()) { 
			if (sc != c) ClearAnimation(sc); 
		}
	}
}

void FBXImportImpl::ClearAnimation(INode *node)
{
	if (node != NULL)
	{
		if (node->HasNoteTracks()){
			for (int i = node->NumNoteTracks()-1; i>=0; --i ){
				if (NoteTrack *nt = node->GetNoteTrack(i))
					node->DeleteNoteTrack(nt, TRUE);
			}
		}
		node->DeleteKeys(TRACK_DOALL);
		ClearAnimation(node->GetTMController());
		for (int i=0, n=node->NumberOfChildren(); i<n; ++i){
			ClearAnimation(node->GetChildNode(i));
		}
	}
}

void FBXImportImpl::ClearAnimation()
{
	if (clearAnimation)
	{
#if VERSION_3DSMAX >= ((7000<<16)+(15<<8)+0) // Version 7
		if (IFrameTagManager *tagMgr = (IFrameTagManager*)GetCOREInterface(FRAMETAGMANAGER_INTERFACE)) {

			int n = tagMgr->GetTagCount();
			for (int i=n-1; i>=0; --i){
				tagMgr->DeleteTag( tagMgr->GetTagID(i) );
			}
		}
#endif
		ClearAnimation(o->gi->GetRootNode());
	}
}

//////////////////////////////////////////////////////////////////////////

const Class_ID IPOS_CONTROL_CLASS_ID = Class_ID(0x118f7e02,0xffee238a);
enum {
	IPOS_X_REF	=	0,
	IPOS_Y_REF	=	1,
	IPOS_Z_REF	=	2,
	IPOS_W_REF	=	3,
};

Control* FBXImportImpl::MakePosition(Control *tmCont, Class_ID clsid)
{
	Interface *ip = o->gi;
	if (Control *c = tmCont->GetPositionController()) {
		if (c->ClassID()!=clsid) {
			if (Control *tmpCtrl = (Control*)ip->CreateInstance(CTRL_POSITION_CLASS_ID, clsid)){
				if (!tmCont->SetPositionController(tmpCtrl))
					tmpCtrl->DeleteThis();
				else
					c = tmpCtrl;
			}
		}
		return c;
	}
	return NULL;
}

Control* FBXImportImpl::MakePositionXYZ(Control *tmCont, Class_ID clsid)
{
	Interface *ip = o->gi;
	// First make the controller and XYZ Independent position controller, then fix individuals
	if (Control *c = MakePosition(tmCont, IPOS_CONTROL_CLASS_ID)){
		if (Control *x = c->GetXController()){
			if (x->ClassID()!= clsid) {
				if (Control *tmp = (Control*)ip->CreateInstance(CTRL_FLOAT_CLASS_ID, clsid))
					c->SetReference(IPOS_X_REF, tmp);
			}
		}
		if (Control *y = c->GetYController()){
			if (y->ClassID()!= clsid) {
				if (Control *tmp = (Control*)ip->CreateInstance(CTRL_FLOAT_CLASS_ID, clsid))
					c->SetReference(IPOS_Y_REF, tmp);
			}
		}
		if (Control *z = c->GetZController()){
			if (z->ClassID()!= clsid) {
				if (Control *tmp = (Control*)ip->CreateInstance(CTRL_FLOAT_CLASS_ID, clsid))
					c->SetReference(IPOS_Z_REF, tmp);
			}
		}
		return c;
	}
	return NULL;
}

Control* FBXImportImpl::MakeRotation(Control *tmCont, Class_ID rotClsid, Class_ID rollClsid)
{
	Interface *ip = o->gi;
	if (Control *c = tmCont->GetRotationController()) {
		if (c->ClassID()!=rotClsid) {
			if (Control *tmpCtrl = (Control*)ip->CreateInstance(CTRL_ROTATION_CLASS_ID, rotClsid)) {
				if (!tmCont->SetRotationController(tmpCtrl))
					tmpCtrl->DeleteThis();
				else
					c = tmpCtrl;
			}
		}
		if (Control *r = tmCont->GetRollController()) {
			if (r->ClassID()!=rollClsid) {
				if (Control *r = (Control*)ip->CreateInstance(CTRL_FLOAT_CLASS_ID,rollClsid))
					if (!tmCont->SetRollController(r))
						r->DeleteThis();
			}
		}
		return c;
	}
	return NULL;
}

Control* FBXImportImpl::MakeScale(Control *tmCont, Class_ID clsid)
{
	Interface *ip = o->gi;
	if (Control *c = tmCont->GetScaleController()) {
		if (c->ClassID()!=clsid) {
			if (Control *tmpCtrl = (Control*)ip->CreateInstance(CTRL_SCALE_CLASS_ID, clsid)){
				if (!tmCont->SetScaleController(tmpCtrl))
					tmpCtrl->DeleteThis();
				else
					c = tmpCtrl;
			}
		}
		return c;
	}
	return NULL;
}


void FBXImportImpl::ImportAnimations()
{
	if (info.Animations.size() == 0 || !enableAnimation)
		return;

	ClearAnimation();

	//for (int anim=0, nanim=info.Animations.size(); anim<nanim; ++anim)
	//{
	//	Animation& anim = (*info.Animations[anim]);
	//}
	Interval range; range.SetInstant(0);

	float time = FrameToTime(0);
	for (int ianim=0, nanim=info.Animations.size(); ianim<nanim; ++ianim)
	{
		Animation& anim = (*info.Animations[ianim]);
		TimeValue animEnd = TimeToFrame(time + anim.Duration);
		if (animEnd > range.End())
			range.SetEnd(animEnd);
		// Build Default Time
		int nkeys = (int)(anim.Duration / anim.TimeStep);
		FBXArray<granny_real32> defaultKeys(nkeys);
		granny_real32 curtime = 0.0f;
		for (int ikeys=0; ikeys<nkeys; ++ikeys, curtime += anim.TimeStep)
			defaultKeys[ikeys] = curtime;

		for (int grp=0, ngrp=anim.TrackGroups.size(); grp<ngrp; ++grp)
		{
			TrackGroup& group = (*anim.TrackGroups[grp]);
			if (INode *root = o->gi->GetINodeByName(group.Name))
			{
				Point3 s( group.InitialPlacement.Scale.m[0][0]
				, group.InitialPlacement.Scale.m[1][1]
				, group.InitialPlacement.Scale.m[2][2] );
				for (int itrack=0, ntrack=group.TransformTracks.size(); itrack<ntrack; ++itrack)
				{
					TransformTrack& track = group.TransformTracks[itrack];
					if (INode *node = o->gi->GetINodeByName(track.Name))
					{
						if (Control *c = node->GetTMController())
						{
							DWORD flags=INHERIT_ALL;
							c->SetInheritanceFlags(INHERIT_ALL,FALSE);

							ImportPosition(c, track, time, defaultKeys);
							ImportRotation(c, track, time, defaultKeys);
							ImportScale(c, track, time, defaultKeys);
						}
					}
				}
				Matrix3 rot(true); group.InitialPlacement.Rotation.MakeMatrix(rot);
				Matrix3 m = TransMatrix(group.InitialPlacement.Origin) * Inverse(rot) * ScaleMatrix( s );
				PosRotScaleNode(root, m);
				// TODO: Move to initial transform
			}
		}
	}
	o->gi->SetAnimRange(range);
}

void FBXImportImpl::ImportPosition(Control* c, TransformTrack& track, float time, FBXArray<granny_real32>& defaultKeys)
{
	// Better use linear for now
	if (Control *subCtrl = MakePositionXYZ(c, Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID,0))) 
	{
		IKeyControl *xkeys = GetKeyControlInterface(subCtrl->GetXController());
		IKeyControl *ykeys = GetKeyControlInterface(subCtrl->GetYController());
		IKeyControl *zkeys = GetKeyControlInterface(subCtrl->GetZController());
		if (xkeys && ykeys && zkeys) 
		{
			if (Point3CurveData* posData = dynamic_cast<Point3CurveData*>(track.PositionCurve.ToPointer()))
			{
				NWArray<float> times = posData->GetKnots();
				NWArray<Point3> points = posData->GetPoint3Data();
				if (times.size() == 0 && points.size() != 0)
					times = defaultKeys;

				xkeys->SetNumKeys(times.size());
				ykeys->SetNumKeys(times.size());
				zkeys->SetNumKeys(times.size());
				for (int i=0; i<times.size(); ++i)
				{
					IBezFloatKey rKey;
					memset(&rKey, 0, sizeof(rKey));
					rKey.time = TimeToFrame(times[i] + time);
					SetInTanType(rKey.flags, BEZKEY_SLOW);
					SetOutTanType(rKey.flags, BEZKEY_SLOW);
					rKey.val = points[i].x;
					xkeys->SetKey(i, &rKey);
					rKey.val = points[i].y;
					ykeys->SetKey(i, &rKey);
					rKey.val = points[i].z;
					zkeys->SetKey(i, &rKey);
				}
				xkeys->SortKeys();
				ykeys->SortKeys();
				zkeys->SortKeys();
			}
		}
	}
}

void FBXImportImpl::ImportRotation(Control* c, TransformTrack& track, float time, FBXArray<granny_real32>& defaultKeys)
{
	if (RotationCurveData* rotData = dynamic_cast<RotationCurveData*>(track.OrientationCurve.ToPointer()))
	{
		if (rotData->GetKeyType() == QUADRATIC_KEY)
		{
			if (Control *subCtrl = MakeRotation(c, Class_ID(HYBRIDINTERP_ROTATION_CLASS_ID,0), Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID,0))) 
			{
				IKeyControl *keys = GetKeyControlInterface(subCtrl);
				if (keys) 
				{
					NWArray<float> times = rotData->GetKnots();
					NWArray<Quat> points = rotData->GetQuatData();
					if (times.size() == 0 && points.size() != 0)
						times = defaultKeys;

					keys->SetNumKeys(times.size());
					for (int i=0; i<times.size(); ++i)
					{
						Quat q = points[i];
						q.w = -q.w;
						IBezQuatKey rKey;
						memset(&rKey, 0, sizeof(rKey));
						rKey.time = TimeToFrame(times[i] + time);
						SetInTanType(rKey.flags, BEZKEY_SLOW);
						SetOutTanType(rKey.flags, BEZKEY_SLOW);
						rKey.val = q;
						keys->SetKey(i, &rKey);
					}
					keys->SortKeys();
				}
			}
		}
		else if (rotData->GetKeyType() == XYZ_ROTATION_KEY)
		{
			if (Control *subCtrl = MakeRotation(c, Class_ID(EULER_CONTROL_CLASS_ID,0), Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID,0)))
			{
				IKeyControl *xkeys = GetKeyControlInterface(subCtrl->GetXController());
				IKeyControl *ykeys = GetKeyControlInterface(subCtrl->GetYController());
				IKeyControl *zkeys = GetKeyControlInterface(subCtrl->GetZController());
				if (xkeys && ykeys && zkeys) 
				{
					NWArray<float> times = rotData->GetKnots();
					NWArray<Point3> points = rotData->GetPoint3Data();
					if (times.size() == 0 && points.size() != 0)
						times = defaultKeys;

					xkeys->SetNumKeys(times.size());
					ykeys->SetNumKeys(times.size());
					zkeys->SetNumKeys(times.size());
					for (int i=0; i<times.size(); ++i)
					{
						Point3 &p = points[i];
						//Quat q; Point3 p;
						//EulerToQuat(points[i], q);
						//q = invQ * q;
						//QuatToEuler(q, p);

						IBezFloatKey rKey;
						memset(&rKey, 0, sizeof(rKey));
						rKey.time = TimeToFrame(times[i] + time);
						SetInTanType(rKey.flags, BEZKEY_SLOW);
						SetOutTanType(rKey.flags, BEZKEY_SLOW);
						rKey.val = p.x;
						xkeys->SetKey(i, &rKey);
						rKey.val = p.y;
						ykeys->SetKey(i, &rKey);
						rKey.val = p.z;
						zkeys->SetKey(i, &rKey);
					}
					xkeys->SortKeys();
					ykeys->SortKeys();
					zkeys->SortKeys();
				}
			}
		}
	}
}

void FBXImportImpl::ImportScale(Control* c, TransformTrack& track, float time, FBXArray<granny_real32>& defaultKeys)
{
	if (Control *subCtrl = MakeScale(c, Class_ID(HYBRIDINTERP_FLOAT_CLASS_ID,0))) 
	{
		if (IKeyControl *keys = GetKeyControlInterface(subCtrl)) 
		{
			if (Matrix3CurveData* posData = dynamic_cast<Matrix3CurveData*>(track.ScaleShearCurve.ToPointer()))
			{
				NWArray<float> times = posData->GetKnots();
				NWArray<Matrix3> values = posData->GetMatrix3Data();
				if (times.size() == 0 && values.size() != 0)
					times = defaultKeys;

				keys->SetNumKeys(times.size());
				for (int i=0; i<times.size(); ++i)
				{
					Matrix3 &m = values[i];
					IBezScaleKey rKey;
					memset(&rKey, 0, sizeof(rKey));
					rKey.time = TimeToFrame(times[i] + time);
					SetInTanType(rKey.flags, BEZKEY_SLOW);
					SetOutTanType(rKey.flags, BEZKEY_SLOW);
					rKey.val = Point3(m.GetRow(0)[0], m.GetRow(1)[1], m.GetRow(2)[2] );
					keys->SetKey(i, &rKey);
				}
				keys->SortKeys();
			}
		}
	}
}
#endif
#endif