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
#include "../common/appsettings.h"
#include "../common/fbxconvert.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;
using namespace DAO::MESH;

static const char *MMHImport = "MMHImport";
static const char *MSHImport = "MSHImport";
static const char *MSHCommon = "MSHCommon";

typedef std::map<std::string, KArrayKFbxNode, ltstr> NameNodeMap;
typedef NameNodeMap::iterator NameNodeItr;

typedef std::map<std::string, GFFFile, ltstr> NameFileMap;
typedef NameFileMap::iterator NameFileItr;

class MMHImportImpl : public ImporterBase
{
	typedef ImporterBase base;
public:
	MMHImportImpl(DAOReader *owner, KFbxScene* scene, KFbxStreamOptions* options, DAO::GFF::GFFFile &file);

	bool DoImport();
	void ImportSkeleton();
	void LoadDependencies();
	void ImportNode( KFbxNode * parent, NODERef node);
	void ImportMesh(KFbxNode *pNode, MSHHRef node);

	KFbxNode *CreateBone(const char* name
		, KFbxVector4& startPos, KFbxVector4& endPos, KFbxVector4& zAxis
		, KFbxSkeleton::ESkeletonType type);

	//////////////////////////////////////////////////////////////////////////


	KFbxXMatrix GetNodeLocalTM(KFbxNode *n);

	void SetGlobalDefaultPosition(KFbxNode* pNode, KFbxXMatrix pGlobalPosition);
	KFbxXMatrix GetGlobalDefaultPosition(KFbxNode* pNode);
	//////////////////////////////////////////////////////////////////////////

	AppSettings importSettings;
	AppSettings environmentSettings;


    NameFileMap dependencies;
	DAO::MESH::MESHFile mshFile;
	DAO::MMH::MDLHPtr root;
	KArrayKFbxNode bones;

	BOOL clearAnimation;
	BOOL enableScale;
	float scaleFactor;
	BOOL enableAnimation;
	bool hideBones;
};


bool DAOReader::ReadMMH( KFbxDocument* pDocument, KFbxStreamOptions* pStreamOptions, DAO::GFF::GFFFile &file )
{
	KFbxScene*      lScene = KFbxCast<KFbxScene>(pDocument);
	bool            lIsAScene = (lScene != NULL);
	bool            lResult = false;
	if(lIsAScene)
	{
		MMHImportImpl impl(this, lScene, pStreamOptions, file);		
		lResult = impl.DoImport();
	}
	return lResult;
}


MMHImportImpl::MMHImportImpl( DAOReader *owner, KFbxScene* scene, KFbxStreamOptions* options, DAO::GFF::GFFFile &file ) 
	: base(owner, scene, options, file)
	, root(NULL)
	, environmentSettings("Environment")
	, importSettings("MMHImport")
{
	root.swap( StructPtrCast<MDLH>( gffFile.get_RootStruct() ) );
}


bool MMHImportImpl::DoImport()
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

	// Locate configuration;
	ResourceManager::LoadSettings(importSettings);

	clearAnimation = TRUE;
	enableScale = GetProp<bool>("EnableScale", true);
	scaleFactor = GetProp<float>("ScaleFactor", 1.f);
	enableAnimation = GetProp<bool>("EnableAnimation", false);
	hideBones = GetProp<bool>("HideBones", true);

	globals.SetSystemUnit( KFbxSystemUnit(scaleFactor) );

	bones.Resize( root->get_totalBones() );

	LoadDependencies();

	ImportSkeleton();

	return true;
}



// Function to get a node's global default position.
// As a prerequisite, parent node's default local position must be already set.
void MMHImportImpl::SetGlobalDefaultPosition(KFbxNode* pNode, KFbxXMatrix pGlobalPosition)
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

// Recursive function to get a node's global default position.
// As a prerequisite, parent node's default local position must be already set.
KFbxXMatrix MMHImportImpl::GetGlobalDefaultPosition(KFbxNode* pNode)
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

static Vector4f GetLocalTranslation(NODERef node) {
	if (node.isNull()) return Vector4f();
	TRSLPtr val = GetFirstOfType<TRSL>(node->get_children());
	return (!val.isNull()) ? val->get_translation() : Vector3f();
}

static Quaternion GetLocalRotation(NODERef node) {
	if (node.isNull()) return Quaternion();
	ROTAPtr val = GetFirstOfType<ROTA>(node->get_children());
	return (!val.isNull()) ? val->get_rotation() : Quaternion();
}

static float GetLocalScale(NODERef node) {
	if (node.isNull()) return 1.0f;
	SCALPtr val = GetFirstOfType<SCAL>(node->get_children());
	return (!val.isNull()) ? val->get_scale() : 1.0f;
}



void MMHImportImpl::ImportSkeleton()
{
	GFFListRef list = root->get_children();
	DAOArray< NODEPtr > bones;
	if ( DynamicCast(list, bones) )
	{
		for (size_t i=0, n=bones.size(); i<n; ++i)
		{
			ImportNode(NULL, bones[i]);
		}
	}
}

KFbxNode * MMHImportImpl::CreateBone( const char* name
									 , KFbxVector4& startPos, KFbxVector4& endPos, KFbxVector4& zAxis
									 , KFbxSkeleton::ESkeletonType type )
{
	KFbxSkeleton* lBoneAttribute = KFbxSkeleton::Create(mManager, name);
	lBoneAttribute->SetSkeletonType(type);


	KFbxNode* lBone = KFbxNode::Create(mManager, name);
	lBone->SetNodeAttribute(lBoneAttribute);    
	lBone->SetDefaultT(startPos);

	if (hideBones)
		lBone->SetVisibility(false);

	return lBone;
}

void MMHImportImpl::ImportNode( KFbxNode * parent, NODERef node)
{
	try 
	{
		if (node.isNull())
			return;

		Text name = node->get_name();
		if (name.isNull())
			return;

		GFFListRef childList = node->get_children();

		Vector4f trans = GetLocalTranslation(node);
		float len = trans.Length();
		Quaternion quat = GetLocalRotation(node);
		float scale = GetLocalScale(node);
		KFbxXMatrix im = TOMATRIX3(trans, quat, scale);
		KFbxVector4 p = im.GetT();
		KFbxQuaternion q = im.GetQ();

		PosRotScale prs = prsDefault;

		KFbxVector4 pp;
		KFbxVector4 zAxis(0,0,0);

		DAOArray<NODEPtr> children;
		bool hasChildren = DynamicCast(childList, children);
		if (hasChildren) {
			float len = 0.0f;
			for (DAOArray<NODEPtr>::iterator itr=children.begin(), end = children.end(); itr != end; ++itr) {
				Vector4f trans = GetLocalTranslation(node);
				len += trans.Length();
			}
			len /= float(children.size());
			pp = p + KFbxVector4(len, 0.0f, 0.0f); // just really need magnitude as rotation will take care of positioning
		}
		else if (parent)
		{
			pp = p + KFbxVector4(len/3.0f, 0.0f, 0.0f);
		}
		KFbxNode *bone = lScene->FindNodeByName(name.c_str());
		if (bone)
		{
			KFbxNode *pinode = bone->GetParent();
			PosRotScaleNode(bone, p, q, scale, prs);
		}
		else
		{
			KFbxSkeleton::ESkeletonType type = parent ? KFbxSkeleton::eLIMB_NODE : KFbxSkeleton::eROOT;
			if (bone = CreateBone(name, p, pp, zAxis, type))
			{
				PosRotScaleNode(bone, p, q, scale, prs);
				//bone->Hide(node->GetVisibility() ? FALSE : TRUE);
			}
			if (bone)
			{
				if ( parent != NULL)
				{
					parent->AddChild(bone);
				}
				else
				{
					KFbxNode* lRootNode = lScene->GetRootNode();
					lRootNode->AddChild(bone);
				}
			}
		}

		int boneIdx = node->get_boneIndex();
		if (boneIdx >= 0 && boneIdx < bones.GetCount()) {
			bones.SetAt(boneIdx, bone);
			SetPropInt(bone, "DAOBoneIndex", boneIdx);
		}

		if (bone)
		{
			for (size_t i=0, n=children.size(); i<n; ++i)
				ImportNode(bone, NODERef(children[i]));
		}

		// Import meshes after child bones have been imported.
		DAOArray<MSHHPtr> meshRefs;
		if ( DynamicCast(childList, meshRefs) ) {
			for (size_t i=0, n=meshRefs.size(); i<n; ++i) {
				MSHHRef meshRef(meshRefs[i]);
				ImportMesh(bone, meshRef);
			}
		}
	}
	catch( std::exception & ) 
	{
	}
	catch( ... ) 
	{
	}
}

void MMHImportImpl::ImportMesh(KFbxNode *pNode, MSHHRef meshRef)
{
	try
	{
		if ( !mshFile.get_Root().isNull() )
		{
			// Create bone list
			GFFListRef usedBoneList( meshRef->get_meshBonesUsed() );
			int nBones = usedBoneList->count();
			KArrayKFbxNode usedBones;
			usedBones.Resize(nBones);
			for (size_t iBone=0; iBone < nBones; ++iBone) {
				int iUsedBone = usedBoneList->asInt32(iBone);
				usedBones.SetAt(iBone, bones[iUsedBone]);
			}
			KFbxNode *result = o->ImportMESH(lScene, pStreamOptions, mshFile, pNode, meshRef, usedBones);
			if (result != NULL)
			{

			}
		}
	}
	catch( std::exception & ) 
	{
	}
	catch( ... ) 
	{
	}
}

void MMHImportImpl::LoadDependencies()
{
	try
	{
		// locate mode FDL file and can get all filenames for this MMH

		Text meshFileName = root->get_modelHierarchyModelDataName();
		if (meshFileName.Length() > 0)
			base::OpenFile(mshFile, meshFileName);
	}
	catch (std::exception&)
	{
	}
}

