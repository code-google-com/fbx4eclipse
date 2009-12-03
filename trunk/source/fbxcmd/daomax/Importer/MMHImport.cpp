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
#include "MMHImport.h"
#include "MMHFormat.h"
#include "maxconvert.h"

#include <stdexcept>
using std::runtime_error;

extern ClassDesc2* GetDAOImportDesc();

using namespace DAO;
using namespace DAO::GFF;
using namespace DAO::MMH;

typedef std::map<std::string, INodeTab, ltstr> NameNodeMap;

static const char *MMHImport = "MMHImport";
static const char *MSHImport = "MSHImport";
static const char *MSHCommon = "MSHCommon";

const float FramesPerSecond = 30.0f;
const float FramesIncrement = 1.0f/30.0f;
const int TicksPerFrame = GetTicksPerFrame();

inline TimeValue TimeToFrame(float t) {
	return TimeValue(t * FramesPerSecond * TicksPerFrame);
}
inline float FrameToTime(TimeValue t) {
	return float(t) / (FramesPerSecond * TicksPerFrame);
}


struct MMHImportImpl
{
	MMHImportImpl(MMHImporter *owner);
	~MMHImportImpl();


	void ImportSkeleton();
	void ImportBones( INode* parent, Matrix44 pm, NODERef bones);


	INode *CreateBone(const char* name, Point3 startPos, Point3 endPos, Point3 zAxis);
	//INodeTab ImportSkeleton(Skeleton& skel);
	void OrderBones(INodeTab& bones);
	void ImportAnimations();
	void ClearAnimation();
	void ClearAnimation(INode* node);
	void ClearAnimation(Control* c);

	//void ImportPosition(Control* c, TransformTrack& track, float time, MMHArray<granny_real32>& defaultKeys);
	//void ImportRotation(Control* c, TransformTrack& track, float time, MMHArray<granny_real32>& defaultKeys);
	//void ImportScale(Control* c, TransformTrack& track, float time, MMHArray<granny_real32>& defaultKeys);

	Control* MakePosition(Control *tmCont, Class_ID clsid);
	Control* MakePositionXYZ(Control *tmCont, Class_ID clsid);
	Control* MakeRotation(Control *tmCont, Class_ID rotClsid, Class_ID rollClsid);
	Control* MakeScale(Control *tmCont, Class_ID clsid);

	DAO::MMH::MMHFile file;
	DAO::MMH::MDLHPtr root;
	DAODumpStream log;
	DAODumpStream logb;


	BOOL clearAnimation;
	BOOL enableScale;
	float scaleFactor;
	BOOL enableAnimation;

	MMHImporter *o;
	Interface *gi;
};

MMHImporter::MMHImporter(const TCHAR *Name,ImpInterface *I,Interface *GI, BOOL SuppressPrompts)
: BaseImporter()
{
	BaseInit(Name,I,GI,SuppressPrompts);
}

MMHImporter::MMHImporter()
{
}

void MMHImporter::Initialize()
{
}

MMHImportImpl::MMHImportImpl(MMHImporter *owner) : o(owner), root(NULL)
{
	gi = o->gi;

	ParamBlockDesc2 *params = GetDAOImportDesc()->GetParamBlockDescByName("parameters");
	clearAnimation = TRUE;
	enableScale = owner->GetIniValue<BOOL>(MSHImport, "EnableScale", 1);
	scaleFactor = owner->GetIniValue<float>(MSHImport, "ScaleFactor", 1.0f);
	enableAnimation = owner->GetIniValue<BOOL>(MMHImport, "EnableAnimation", 0);
}

MMHImportImpl::~MMHImportImpl()
{
}

bool MMHImporter::DoImport()
{
	MMHImportImpl impl(this);

	try
	{
		impl.file.open(name);
		impl.root = impl.file.get_Root();

		Text txtA, txtB;
		txtA.Format("C:\\tmp\\%s-1.log", PathFindFileName(name.c_str()));
		impl.log.Open(txtA.c_str(), false);

		txtB.Format("C:\\tmp\\%s-2.log", PathFindFileName(name.c_str()));
		impl.logb.Open(txtB.c_str(), false);

		impl.ImportSkeleton();
		//impl.ImportAnimations();
	}
	catch (std::exception &e)
	{
		throw e;
	}
	return true;
}


void MMHImportImpl::ImportSkeleton()
{
	GFFListRef list = root->get_children();
	DAOArray< NODEPtr > bones;
	if ( DynamicCast(list, bones) )
	{
		Matrix44 wtm(
			Vector4f( 0.000000,1.000000,0.000000,0.00000),
			Vector4f(-1.000000,0.000000,0.000000,0.00000),
			Vector4f( 0.000000,0.000000,1.000000,0.00000),
			Vector4f( 0.000000,0.000000,0.000000,1.00000));

		for (size_t i=0, n=bones.size(); i<n; ++i)
		{
			ImportBones(NULL, wtm, bones[i]);
		}
	}
}

void MMHImportImpl::ImportBones( INode* parent, Matrix44 pm, NODERef node)
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

		Matrix44 tm(trans, quat, scale);
		Matrix44 im = pm * tm;

		Matrix3 tm3 = TOMATRIX3(tm);
		Matrix3 pm3(true), pm3p(true);
		pm3p = TOMATRIX3(pm);
		if (parent) pm3 = parent->GetNodeTM(0);

		Matrix3 im3 = pm3 * tm3;
		Matrix3 im3r = tm3 * pm3;

#if 0
		//Vector3f ypr;
		//Text txtPos, txtRot, txtYpr, txtScale, txtFormat;
		//txtPos.Format("(%7.4f, %7.4f, %7.4f)", trans[0], trans[1], trans[2]);
		//txtRot.Format("(%7.4f, %7.4f, %7.4f, %7.4f)", quat.w, quat.x, quat.y, quat.z);
		//ypr = quat.AsEulerYawPitchRoll();
		//txtYpr.Format("(%7.4f, %7.4f, %7.4f)", TODEG(ypr[0]), TODEG(ypr[1]), TODEG(ypr[2]));
		//txtScale.Format("%7.4f", scale);
		//txtFormat.Format("%%-%ds | %%-36s | %%-46s | %%-36s | %%-10s\n", max(60 - indent*2, 1) );
		//log.Indent();
		//log.PrintF(txtFormat, name.c_str(), txtPos.c_str(), txtRot.c_str(), txtYpr.c_str(), txtScale.c_str());
		Matrix44& dm = im;

		Text txtFormat;
		Text txtPos0, txtPos1, txtPos2, txtPos3;
		Vector4f r0 = dm.GetRow(0);
		Vector4f r1 = dm.GetRow(1);
		Vector4f r2 = dm.GetRow(2);
		Vector4f r3 = dm.GetRow(3);
		txtPos0.Format("[%7.4f,%7.4f,%7.4f]", r0.x, r0.y, r0.z);
		txtPos1.Format("[%7.4f,%7.4f,%7.4f]", r1.x, r1.y, r1.z);
		txtPos2.Format("[%7.4f,%7.4f,%7.4f]", r2.x, r2.y, r2.z);
		txtPos3.Format("[%7.4f,%7.4f,%7.4f]", r3.x, r3.y, r3.z);

		txtFormat.Format("%%-%ds | %%s %%s %%s %%s\n", max(60 - indent*2, 1) );
		log.Indent();
		log.PrintF(txtFormat, name.c_str(), txtPos0.c_str(), txtPos1.c_str(), txtPos2.c_str(), txtPos3.c_str());
#endif
		//quat = quat.Inverse();
		//if (enableScale)
		//	scale *= scaleFactor;
		//Matrix3 im = TOMATRIX3(trans, quat, scale);

		//Point3 pim = im.GetTrans();
		//Quat qim(im);

		//Matrix3 wtm(true); // world transform matrix
		//if (parent) 
		//{
		//	wtm = parent->GetNodeTM(0);
		//}
		//im = im * wtm;
		//Matrix3 mm = TOMATRIX3(im);

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

INode *MMHImportImpl::CreateBone(const char* name, Point3 startPos, Point3 endPos, Point3 zAxis)
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

Matrix3 MMHImportImpl::GetLocalTransform(Skeleton& skel, size_t index)
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

Matrix3 MMHImportImpl::GetWorldTransform(Skeleton& skel, size_t index)
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


void MMHImportImpl::OrderBones(INodeTab& bones)
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
				node->SetUserPropInt("MSHBoneIndex", int(bones.Count()));
				bones.Append(1, &node);
			}
		}
		for (stringlist::const_iterator itr=boneMatch.begin(), end=boneMatch.end(); itr != end; ++itr)
		{
			INodeTab& map = nodes[*itr];
			for (size_t i = 0, n = map.Count(); i<n; ++i)
			{
				INode *node = map[i];
				node->SetUserPropInt("MSHBoneIndex", int(bones.Count()));
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
						node->SetUserPropInt("MSHBoneIndex", int(curIdx));
						++curIdx;
						break;
					}
				}
			}
		}
	}
}

void MMHImportImpl::ClearAnimation(Control *c)
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

void MMHImportImpl::ClearAnimation(INode *node)
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

void MMHImportImpl::ClearAnimation()
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

Control* MMHImportImpl::MakePosition(Control *tmCont, Class_ID clsid)
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

Control* MMHImportImpl::MakePositionXYZ(Control *tmCont, Class_ID clsid)
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

Control* MMHImportImpl::MakeRotation(Control *tmCont, Class_ID rotClsid, Class_ID rollClsid)
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

Control* MMHImportImpl::MakeScale(Control *tmCont, Class_ID clsid)
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


void MMHImportImpl::ImportAnimations()
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
		MMHArray<granny_real32> defaultKeys(nkeys);
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

void MMHImportImpl::ImportPosition(Control* c, TransformTrack& track, float time, MMHArray<granny_real32>& defaultKeys)
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

void MMHImportImpl::ImportRotation(Control* c, TransformTrack& track, float time, MMHArray<granny_real32>& defaultKeys)
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

void MMHImportImpl::ImportScale(Control* c, TransformTrack& track, float time, MMHArray<granny_real32>& defaultKeys)
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