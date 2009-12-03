/**********************************************************************
*<
FILE: WalkPaint.cpp

DESCRIPTION:	Walkmesh paint class

HISTORY:
  1.6 - Initial Implementation
      - Borrowed paint routines heavily from the Max Vertex Paint Sample  

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma warning( disable:4800 )

#include "nwn2utility.h"
#include <meshadj.h>
#include <meshdelta.h>

using namespace std;

#define PBLOCK_REF	0
#define CID_PAINT				(CID_USER+0x439c)

const Class_ID WALKMESHMODIFIER_CLASS_ID = Class_ID(0x738f550e, 0xcf2d46e0);

static const char *MDBUtil = "MDBUtility";
static const char *MDBCommon = "MDBCommon";
static   HIMAGELIST hButtonImages = NULL;

static void LoadImages() {
   if (hButtonImages) return;
   HBITMAP hBitmap, hMask;
   hButtonImages = ImageList_Create(15, 14, ILC_MASK, 2, 0);   // 17 is kluge to center square. -SA
   hBitmap     = LoadBitmap (hInstance,MAKEINTRESOURCE(IDB_BUTTONS));
   hMask       = LoadBitmap (hInstance,MAKEINTRESOURCE(IDB_BUTTON_MASK));
   ImageList_Add(hButtonImages, hBitmap, hMask);
   DeleteObject(hBitmap);
   DeleteObject(hMask);
}

static const int SEL_OBJECT = 0;
static const int SEL_VERTEX = 1;
static const int SEL_EDGE = 2;
static const int SEL_FACE = 3;
static const int SEL_POLY = 4;
static const int SEL_ELEMENT = 5;

static const int NS_VERTEX = 0;
static const int NS_EDGE = 1;
static const int NS_FACE = 2;

static const int NWalkBits = 6;
class WalkMeshLocalData : public LocalModData, public IMeshSelectData 
{
public:
   WalkMeshLocalData(Mesh& m);
   WalkMeshLocalData();
   ~WalkMeshLocalData();
   LocalModData*	Clone();

   Mesh * GetMesh() { return mesh; }

   void	SetCache(Mesh& m);
   void	FreeCache();

   AdjEdgeList *GetAdjEdgeList ();
   AdjFaceList *GetAdjFaceList ();

   BitArray GetFaceSel(int level) { return uiFlags[level]; }

   // LocalModData
   void* GetInterface(ULONG id) { if (id == I_MESHSELECTDATA) return(IMeshSelectData*)this; else return LocalModData::GetInterface(id); }

   // Selection sets
   BitArray vertSel;
   BitArray edgeSel;

   // Lists of named selection sets
   GenericNamedSelSetList vselSet;
   GenericNamedSelSetList fselSet;
   GenericNamedSelSetList eselSet;

   BitArray GetVertSel() { return vertSel; }
   BitArray GetFaceSel() { return uiFlags[0]; }
   BitArray GetEdgeSel() { return edgeSel; }

   void SetVertSel(BitArray &set, IMeshSelect *imod, TimeValue t);
   void SetFaceSel(BitArray &set, IMeshSelect *imod, TimeValue t);
   void SetEdgeSel(BitArray &set, IMeshSelect *imod, TimeValue t);

   GenericNamedSelSetList & GetNamedVertSelList () { return vselSet; }
   GenericNamedSelSetList & GetNamedEdgeSelList () { return eselSet; }
   GenericNamedSelSetList & GetNamedFaceSelList () { return fselSet; }
   GenericNamedSelSetList & GetNamedSel (int nsl) {
      if (nsl==NS_VERTEX) return vselSet;
      if (nsl==NS_EDGE) return eselSet;
      return fselSet;
   }

   MeshTempData *temp;
   Mesh*	mesh;
   Tab<BitArray> uiFlags;
   BOOL held;
};


class WalkMeshModifier : public Modifier, public IMeshSelect, public FlagUser
{
public:
	WalkMeshModifier();
	~WalkMeshModifier();

   void DeleteThis() { delete this; }
   void GetClassName(TSTR& s) { s = "WalkMeshModifier"; }  
   virtual Class_ID ClassID() { return WALKMESHMODIFIER_CLASS_ID; }		
   RefTargetHandle Clone(RemapDir& remap);
   TCHAR *GetObjectName() { return "WalkMeshModifier"; }

   //From Modifier
   ChannelMask ChannelsUsed()  {return PART_GEOM|PART_TOPO;}
   ChannelMask ChannelsChanged() {return PART_SELECT|PART_SUBSEL_TYPE|PART_TOPO|PART_GEOM ;} // RB 5/27/97: Had to include topo channel because in edge select mode this modifier turns on hidden edges -- which may cause the edge list to be rebuilt, which is effectively a change in topology since the edge list is now part of the topo channel.
   bool ModifyTriObject(TriObject* tobj, TimeValue t, ModContext &mc);
   void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
   Interval GetValidity(TimeValue t) { return FOREVER; }
   Interval LocalValidity(TimeValue t);
   Class_ID InputType() {return mapObjectClassID;}
   void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

   // From BaseObject
   BOOL ChangeTopology() {return FALSE;}
   //BOOL DependOnTopology(ModContext &mc);
   virtual IOResult SaveLocalData(ISave *isave, LocalModData *ld);  
   virtual IOResult LoadLocalData(ILoad *iload, LocalModData **pld);

   int Display(TimeValue t, INode* inode, ViewExp *vpt, int flagst, ModContext *mc);

	void			selectionChanged();
   void        saveState();
   BOOL			dlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

   // ParamBlock2 access:
   int NumParamBlocks () { return 1; }
   IParamBlock2* GetParamBlock(int i) 
   { 
      if (i == 0) return pblock;
      return NULL;
   }
   IParamBlock2* GetParamBlockByID(BlockID id) { 
      if (pblock->ID() == id) return pblock;
      return NULL; 
   }

   CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;} 

   virtual	int NumRefs();
   virtual  RefTargetHandle GetReference(int i);
   virtual  void SetReference(int i, RefTargetHandle rtarg);
   virtual  RefResult   NotifyRefChanged(Interval,RefTargetHandle,PartID &,RefMessage);

   int NumSubs() {return 1;}
   Animatable* SubAnim(int i) { return GetReference(i); }
   TSTR SubAnimName(int i) {return GetString (IDS_PARAMS);}

   void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
   void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);

   // IMeshSelect
   virtual DWORD GetSelLevel() { return IMESHSEL_FACE; }
   virtual void SetSelLevel(DWORD level) {}
   virtual void LocalDataChanged() {
      NotifyDependents(FOREVER, PART_SELECT, REFMSG_CHANGE);
      //InvalidateVDistances ();
   }

   void *GetInterface (ULONG id) { if (id==I_MESHSELECT) return (IMeshSelect *) this; else return Modifier::GetInterface(id); }

public:
	IParamBlock2 *pblock;
   IParamMap2 *pmapParam;
	Interface		*mIP;
   ICustButton* iPaintButton;
   static BOOL oldShowEnd;
   int selLevel;
};

class SelectRestore : public RestoreObj {
public:
   BitArray usel, rsel;
   BitArray *sel;
   WalkMeshModifier *mod;
   WalkMeshLocalData *d;
   int level;

   SelectRestore(WalkMeshModifier *m, WalkMeshLocalData *d);
   SelectRestore(WalkMeshModifier *m, WalkMeshLocalData *d, int level);
   void Restore(int isUndo);
   void Redo();
   int Size() { return 1; }
   void EndHold() {d->held=FALSE;}
   TSTR Description() { return TSTR(_T("SelectRestore")); }
};


/****************************************************************************
*
* 3D Paint Implementation
*
***************************************************************************/

// Mouse proc
class PaintMouseProc : public MouseCallBack {
public:
   int		proc(HWND hWnd, int msg, int point, int flags, IPoint2 m); // Mouse callback
   void	SetModifier(WalkMeshModifier* pMod) { pModifier = pMod; }
   void	SetPickMode(BOOL bPick){bPickMode = bPick;}
   BOOL	GetPickMode(){return bPickMode;}
   void	DoPainting(HWND hWnd, IPoint2 m);
   void	DoPickColor(HWND hWnd, IPoint2 m);
   void	MaybeStartHold();
   void	MaybeEndHold();
   BOOL	IsValidPickNode(INode *node);


private:
   WalkMeshModifier*	pModifier;
   BOOL			wasHolding;
   BOOL			bPickMode;
};


class PaintCommandMode : public CommandMode {
public:
   PaintCommandMode();

   PaintMouseProc mouseProc;

   int							Class();
   virtual int					ID();
   MouseCallBack*				MouseProc(int *numPoints);
   BOOL						ChangeFG(CommandMode *oldMode);
   ChangeForegroundCallback*	ChangeFGProc();
   void						EnterMode();
   void						ExitMode();

   void						SetInterface(Interface* ip) { iInterface = ip; }
   void						SetModifier(WalkMeshModifier* pMod)		{ pModifier = pMod; mouseProc.SetModifier(pMod); }

private:
   Interface*					iInterface;
   WalkMeshModifier*			pModifier;
};

/****************************************************************************
*
* Walk Mesh Class Description
*
***************************************************************************/

extern ClassDesc2* GetWalkMeshModifierDesc();
static ParamBlockDesc2& get_walkmesh_param_blk();

class WalkMeshModifierClassDesc : public ClassDesc2
{
	public:
   WalkMeshModifierClassDesc() {
      get_walkmesh_param_blk().SetClassDesc(this);
   }
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { 
      return new WalkMeshModifier();
   }
	const TCHAR *	ClassName() { return GetString(IDS_WALKMESH_CLASS_NAME); }
	SClass_ID		SuperClassID() { return WSM_CLASS_ID; }
	Class_ID		ClassID() { return WALKMESHMODIFIER_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("WalkMeshModifier"); }
	HINSTANCE		HInstance() { return hInstance; }
};



class WalkMeshPBAccessor : public PBAccessor
{ 
public:
   TSTR iniFileName;
   bool iniFileValid;

   WalkMeshPBAccessor() : iniFileValid(false)
   {
      Interface *gi = GetCOREInterface();
      TCHAR iniName[MAX_PATH];
#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
      if (gi) {
         LPCTSTR pluginDir = gi->GetDir(APP_PLUGCFG_DIR);
         PathCombine(iniName, pluginDir, "NWN2Utils.ini");
      } 
      else 
#endif
      {
         GetModuleFileName(NULL, iniName, _countof(iniName));
         if (LPTSTR fname = PathFindFileName(iniName))
            *fname = NULL;
         PathAddBackslash(iniName);
         PathAppend(iniName, "plugcfg");
         PathAppend(iniName, "NWN2Utils.ini");
      }
      iniFileName = iniName;
      iniFileValid = (_access(iniName, 0) != -1);
   }

   // Generic IniFile reading routine
   template<typename T>
   T GetIniValue(LPCTSTR Section, LPCTSTR Setting, T Default){
      if (!iniFileValid) 
         return Default;
      return ::GetIniValue<T>(Section, Setting, Default, iniFileName);
   }

   // Generic IniFile reading routine
   template<typename T>
   void SetIniValue(LPCTSTR Section, LPCTSTR Setting, T value){
      if (!iniFileValid) 
         return;
      ::SetIniValue<T>(Section, Setting, value, iniFileName);
   }

   virtual void Get(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval &valid) 
   { 
      switch (id)
      {
      case WLK_PB_PAINTTYPE:   v.i = ((WalkMeshModifier*)owner)->selLevel; /*v.i = GetIniValue<BOOL>(MDBUtil, "PaintType", v.i);*/ break;
      }
   }

   // set from v
   void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t) 
   { 
      switch (id)
      {
      case WLK_PB_PAINTTYPE:   ((WalkMeshModifier*)owner)->selLevel = v.i; /*SetIniValue<BOOL>(MDBUtil, "PaintType", v.i);*/ break;
      }
   }
};

static WalkMeshPBAccessor walkMeshPBAccessor;

// Parameter and ParamBlock IDs
static ParamBlockDesc2 walkmesh_param_blk ( 
   walkmesh_params, _T("Parameters"),  0, NULL, P_AUTO_CONSTRUCT + P_AUTO_UI + P_MULTIMAP, wlk_mdb_params,
   //rollout
   1,
   wlk_mdb_params,	IDD_WM_MOD_PANEL,  IDS_PARAMS, 0, 0, NULL, 

	WLK_PB_PAINTTYPE, 	_T("paintType"),	TYPE_INT, 0, IDS_WM_PAINTTYPE,
		p_default, 		0, 
		p_range, 		0, 4, 
		p_ui, 			wlk_mdb_params,	TYPE_RADIO, 5, IDC_RDO_NO_COLL, IDC_RDO_AXIS_ALIGNED_BOX, IDC_RDO_SPHERE, IDC_RDO_CAPSULE, IDC_RDO_PROXY_MESH,
		p_accessor,		&walkMeshPBAccessor,
		end,

   end,
   end
   );

static WalkMeshModifierClassDesc WalkMeshModifierDesc;
ClassDesc2* GetWalkMeshModifierDesc() { return &WalkMeshModifierDesc; }
ParamBlockDesc2& get_walkmesh_param_blk() {
   return walkmesh_param_blk;
}

class WalkMeshModifierDlgProc : public ParamMap2UserDlgProc {
public:
   WalkMeshModifier *mod;
   WalkMeshModifierDlgProc(WalkMeshModifier* m) {mod = m;}		

   INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);		
   void DeleteThis() {delete this;}		
};

INT_PTR WalkMeshModifierDlgProc::DlgProc (TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   int numPoints;
   switch (msg) 
   {
   case WM_INITDIALOG:
      LoadImages();
      mod->iPaintButton = GetICustButton(GetDlgItem(hWnd, IDC_PAINT));
      mod->iPaintButton->SetType(CBT_CHECK);
      mod->iPaintButton->SetHighlightColor(GREEN_WASH);
      mod->iPaintButton->SetCheck(mod->mIP->GetCommandMode()->ID() == CID_PAINT && 
         !((PaintMouseProc *)mod->mIP->GetCommandMode()->MouseProc(&numPoints))->GetPickMode());
      mod->iPaintButton->SetImage(hButtonImages,0,0,0,0,15,14);
      mod->iPaintButton->SetTooltip (TRUE, GetString (IDS_PAINT));
      break;

   case WM_DESTROY:
      if (mod && mod->iPaintButton != NULL) {
         ReleaseICustButton(mod->iPaintButton);
         mod->iPaintButton = NULL;
      }
      break;

   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDC_PAINT:
         {
            //mod->ActivatePaint(mod->iPaintButton->IsChecked());
            break;
         }
      default: break;
      }
      break;

   default:
      return FALSE;
   }

   return TRUE;
}

//--- WalkMeshModifier -------------------------------------------------------

WalkMeshModifier::WalkMeshModifier()
{
	mIP = NULL;	
	pblock = NULL;
   pmapParam = NULL;
   iPaintButton = NULL;
   selLevel = 0;
   WalkMeshModifierDesc.MakeAutoParamBlocks(this);
}

WalkMeshModifier::~WalkMeshModifier()
{
   walkmesh_param_blk.SetUserDlgProc();
   if (pmapParam) {
      DestroyCPParamMap2 (pmapParam);
      pmapParam = NULL;
   }
   if (iPaintButton != NULL) {
      ReleaseICustButton(iPaintButton);
      iPaintButton = NULL;
   }
}

RefTargetHandle WalkMeshModifier::Clone(RemapDir& remap) {
   WalkMeshModifier *mod = new WalkMeshModifier();
   mod->ReplaceReference(0,remap.CloneRef(GetReference(0)));
   //mod->ReplaceReference(1,remap.CloneRef(GetReference(1)));
   BaseClone(this, mod, remap);
   return mod;
}


int WalkMeshModifier::NumRefs()
{
   return 1;
}

RefTargetHandle WalkMeshModifier::GetReference(int i)
{
   if (i==0) return pblock;
   return NULL;
}

void WalkMeshModifier::SetReference(int i, RefTargetHandle rtarg)
{
   if (i==0) pblock = (IParamBlock2*)rtarg;
}

RefResult WalkMeshModifier::NotifyRefChanged(
   Interval changeInt, 
   RefTargetHandle hTarget, 
   PartID& partID, 
   RefMessage message)
{
   switch (message) 
   {
   case REFMSG_CHANGE:
      ParamID changing_param = pblock->LastNotifyParamID();
      walkmesh_param_blk.InvalidateUI(changing_param);
      break;
   }
   return REF_SUCCEED;
}

Interval WalkMeshModifier::LocalValidity(TimeValue t) 
{ 
   // If we are being edited, return NEVER to forces a cache to be built after previous modifier.
   if (TestAFlag(A_MOD_BEING_EDITED))
      return NEVER;
   return GetValidity(t); 
}

bool WalkMeshModifier::ModifyTriObject(TriObject* tobj, TimeValue t, ModContext &mc) 
{
#if 1
   WalkMeshLocalData *d  = (WalkMeshLocalData*)mc.localData;
   if (!d) mc.localData = d = new WalkMeshLocalData(tobj->GetMesh());

   Interval outValid;
   outValid = tobj->ChannelValidity (t, SELECT_CHAN_NUM);

   BitArray faceSel = d->GetFaceSel(0);
   faceSel.SetSize(tobj->GetMesh().getNumFaces(),TRUE);
   tobj->GetMesh().faceSel = faceSel;
   tobj->GetMesh().selLevel = MESH_FACE;
   tobj->GetMesh().ClearVSelectionWeights ();

   // Update the cache used for display, hit-testing:
   if (!d->GetMesh()) d->SetCache(tobj->GetMesh());
   else *(d->GetMesh()) = tobj->GetMesh();

   // Set display flags - but be sure to turn off vertex display in stack result if
   // "Show End Result" is turned on - we want the user to just see the Mesh Select
   // level vertices (from the Display method).
   tobj->GetMesh().dispFlags = 0;
   tobj->GetMesh().SetDispFlag (DISP_SELFACES);
   tobj->SetChannelValidity (SELECT_CHAN_NUM, outValid);
#else
   tobj->ReadyChannelsForMod(GEOM_CHANNEL|TOPO_CHANNEL|VERTCOLOR_CHANNEL|TEXMAP_CHANNEL);
   Mesh* mesh = &tobj->GetMesh();
   if (!mesh) return false;
   UVVert *mv;
   TVFace *mf;

   WalkMeshLocalData *d  = (WalkMeshLocalData*)mc.localData;
   if (!d) mc.localData = d = new WalkMeshLocalData(tobj->GetMesh());
   if (!d->GetMesh()) d->SetCache(*mesh);

   MeshDelta md(*mesh);

   int chan = 0;
   // If the incoming Mesh had no vertex colors, this will add a default map to start with.
   // The default map has the same topology as the Mesh (so one color per vertex),
   // with all colors set to white.
   if (!mesh->mapSupport(-chan)) {
      md.AddMap (-chan);
      mv = NULL;
      mf = NULL;
   } else {
      mv = mesh->mapVerts(-chan);
      mf = mesh->mapFaces(-chan);
   }

   VertColor nvc(1.0,0.0,0.0);
   VertColor white(1.0,1.0,1.0);

   // Create a new Color
   md.Map(-chan).VCreate (&nvc);
   md.Map(-chan).VCreate (&white);

   // Enumerate the faces on the mesh.
   for (int f=0, nf = mesh->getNumFaces(); f < nf; ++f)
   {
      Face& face = mesh->faces[f];
      for (int i=0; i<2; ++i)
      {
         DWORD ivtx = face.getVert(i);
         if (ivtx%2)
            md.Map(-chan).Set(ivtx, nvc);
         else
            md.Map(-chan).Set(ivtx, white);
      }
   }

   md.Apply(*mesh);
   // sca 5/1/01 - this line isn't needed!  It's handled by things like PartsChanged.
   //NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
   tobj->UpdateValidity(VERT_COLOR_CHAN_NUM, Interval(t,t));
#endif
   return true;
}

void WalkMeshModifier::ModifyObject (TimeValue t, ModContext &mc, ObjectState *os, INode *inode) 
{
   OutputDebugString("WalkMeshModifier::ModifyObject\n");

   if (inode) 
   {
      if (!inode->GetCVertMode())
      {
         inode->SetCVertMode(TRUE);
         NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
         //mIP->RedrawViews(mIP->GetTime());
      }
   }

   if (os->obj->IsSubClassOf(triObjectClassID)) { // handle TriObjects
      TriObject *tobj = (TriObject*)os->obj;
      ModifyTriObject(tobj, t, mc);
   } else if(os->obj->CanConvertToType(triObjectClassID)) { // Convert to a tri mesh if possible
      TriObject  *triOb = (TriObject *)os->obj->ConvertToType(t, triObjectClassID);
      // Now stuff this into the pipeline!
      os->obj = triOb;
      ModifyTriObject(triOb, t, mc);
   }
}

void WalkMeshModifier::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{
	OutputDebugString("WalkMeshModifier::NotifyInputChanged\n");
   Modifier::NotifyInputChanged(changeInt, partID, message, mc);
}

BOOL WalkMeshModifier::oldShowEnd = FALSE;
void WalkMeshModifier::BeginEditParams(IObjParam  *ip, ULONG flags,Animatable *prev)
{
   Modifier::BeginEditParams(ip, flags, prev);
	mIP = ip;

   TimeValue t = ip->GetTime();
   NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
   NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
   SetAFlag(A_MOD_BEING_EDITED);
   
   WalkMeshModifierDesc.BeginEditParams(ip,this,flags,prev);
   //walkmesh_param_blk.SetUserDlgProc(bv_mesh, new WalkMeshModifierDlgProc(this));

	pmapParam = pblock->GetMap(walkmesh_params);
	//UpdateBVDialogs();

   // Set show end result.
   oldShowEnd = ip->GetShowEndResult() ? TRUE : FALSE;
   //ip->SetShowEndResult (GetFlag (VP_DISP_END_RESULT));

   // Force an eval to update caches.
   NotifyDependents(FOREVER, PART_VERTCOLOR, REFMSG_CHANGE);
}
	
void WalkMeshModifier::EndEditParams (IObjParam *ip,ULONG flags,Animatable *next)
{
   //ActivatePaint(FALSE);

   Modifier::EndEditParams(ip,flags,next);
   //saveState();
	mIP = NULL;

   // NOTE: This flag must be cleared before sending the REFMSG_END_EDIT
   ClearAFlag(A_MOD_BEING_EDITED);
   TimeValue t = ip->GetTime();
   NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
   NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);

   walkmesh_param_blk.SetUserDlgProc();

   if (flags&END_EDIT_REMOVEUI) {
		pmapParam = NULL;
		//UpdateBVDialogs();
	}

   //SetFlag (VP_DISP_END_RESULT, ip->GetShowEndResult() ? TRUE : FALSE);
   ip->SetShowEndResult(oldShowEnd);

   // For PB2 we ask the ClassDesc2 to take care of the EndEditParams - NH
   WalkMeshModifierDesc.EndEditParams(ip,this,flags,next);
}

IOResult WalkMeshModifier::SaveLocalData(ISave *isave, LocalModData *ld) 
{ 
   return IO_OK; 
}

IOResult WalkMeshModifier::LoadLocalData(ILoad *iload, LocalModData **pld) 
{ 
   return IO_OK; 
}


int WalkMeshModifier::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc) {
   if (!mIP->GetShowEndResult ()) return 0;
   if (!IsEnabled()) return 0;
   if (!mc->localData) return 0;
   if (!selLevel) return 0;

#if 1
   WalkMeshLocalData *modData = (WalkMeshLocalData *) mc->localData;
   Mesh *mesh = modData->GetMesh();
   if (!mesh) return 0;

   // Set up GW
   GraphicsWindow *gw = vpt->getGW();
   Matrix3 tm = inode->GetObjectTM(t);
   int savedLimits;
   gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
   gw->setTransform(tm);

   // We need to draw a "gizmo" version of the mesh:
   Point3 colSel=GetSubSelColor();
   Point3 colTicks=GetUIColor (COLOR_VERT_TICKS);
   Point3 colGiz=GetUIColor(COLOR_GIZMOS);
   Point3 colGizSel=GetUIColor(COLOR_SEL_GIZMOS);
   gw->setColor (LINE_COLOR, colGiz);

   //AdjEdgeList *ae = modData->GetAdjEdgeList ();
   //Point3 rp[3];
   //int i, ect = ae->edges.Count();

   //int es[3];
   //gw->startSegments();
   //for (i=0; i<ect; i++) {
   //   MEdge & me = ae->edges[i];
   //   if (me.Hidden (mesh->faces)) continue;
   //   es[0] = (me.Visible (mesh->faces)) ? GW_EDGE_VIS : GW_EDGE_INVIS;
   //   gw->setColor (LINE_COLOR, (ae->edges[i].AFaceSelected (modData->GetFaceSel(0))) ? colGizSel : colGiz);
   //   rp[0] = mesh->verts[me.v[0]];
   //   rp[1] = mesh->verts[me.v[1]];
   //   gw->segment(rp, (es[0] ==GW_EDGE_VIS) ? 1 : 0);
   //}
   //gw->endSegments();


   //gw->startTriangles();
   //
   //Point3 rp[3];
   //Point3 rgb[3] = { Point3(1.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f), Point3(1.0f, 0.0f, 0.0f) };

   //AdjFaceList* afl = modData->GetAdjFaceList();
   //for (int i=0, n=afl->list.Count(); i<n; ++i)
   //{
   //   AdjFace& af = afl->list[i];
   //   rp[0] = mesh->verts[ af.f[0] ];
   //   rp[1] = mesh->verts[ af.f[1] ];
   //   rp[2] = mesh->verts[ af.f[2] ];
   //   gw->setColor (FILL_COLOR, rgb[0]);
   //   gw->triangle(rp, rgb);
   //}
   //gw->endTriangles();

   gw->setRndLimits(savedLimits);
#endif
   return 0;
}

/****************************************************************************
*
* Walk Mesh Paint Data Implementation
*
***************************************************************************/
WalkMeshLocalData::WalkMeshLocalData(Mesh& m) : mesh(NULL), temp(NULL), held(FALSE)
{
   SetCache(m);
}

WalkMeshLocalData::WalkMeshLocalData() : mesh(NULL), temp(NULL), held(FALSE)
{
}

WalkMeshLocalData::~WalkMeshLocalData()
{
   FreeCache();
}

LocalModData*	WalkMeshLocalData::Clone()
{
   WalkMeshLocalData* d = new WalkMeshLocalData();
   return d;
}

void	WalkMeshLocalData::SetCache(Mesh& m)
{
   FreeCache();
   this->mesh = new Mesh(m);
   int n = m.getNumFaces();
   uiFlags.SetCount(NWalkBits);
   for (int i=0; i<NWalkBits; ++i) {
      BitArray& ba = uiFlags[i];
      ba.SetSize(n);
      if (i == 0 || i == 6) 
         ba.SetAll();
      else
         ba.ClearAll();
   }
}

void	WalkMeshLocalData::FreeCache()
{
   if (this->temp) delete this->temp;
   if (this->mesh) delete this->mesh;
   this->temp = NULL;
   this->mesh = NULL;
   uiFlags.ZeroCount();
}

AdjEdgeList *WalkMeshLocalData::GetAdjEdgeList () {
   if (!mesh) return NULL;
   if (!temp) temp = new MeshTempData;
   temp->SetMesh (mesh);
   return temp->AdjEList ();
}

AdjFaceList *WalkMeshLocalData::GetAdjFaceList () {
   if (!mesh) return NULL;
   if (!temp) temp = new MeshTempData;
   temp->SetMesh (mesh);
   return temp->AdjFList ();
}

void WalkMeshLocalData::SetVertSel(BitArray &set, IMeshSelect *imod, TimeValue t) {
   WalkMeshModifier *mod = (WalkMeshModifier *) imod;
   if (theHold.Holding()) theHold.Put (new SelectRestore (mod, this, SEL_VERTEX));
   vertSel = set;
   if (mesh) mesh->vertSel = set;
   //InvalidateVDistances ();
}

void WalkMeshLocalData::SetFaceSel(BitArray &set, IMeshSelect *imod, TimeValue t) {
   WalkMeshModifier *mod = (WalkMeshModifier *) imod;
   if (theHold.Holding()) theHold.Put (new SelectRestore (mod, this, SEL_FACE));
   uiFlags[0] = set;
   if (mesh) mesh->faceSel = set;
   //InvalidateVDistances ();
}

void WalkMeshLocalData::SetEdgeSel(BitArray &set, IMeshSelect *imod, TimeValue t) {
   WalkMeshModifier *mod = (WalkMeshModifier *) imod;
   if (theHold.Holding()) theHold.Put (new SelectRestore (mod, this, SEL_EDGE));
   edgeSel = set;
   if (mesh) mesh->edgeSel = set;
   //InvalidateVDistances ();
}

#if 0
void WalkMeshLocalData::SynchVerts(Mesh &m) 
{
   if (nverts) delete [] nverts;
   numnverts = m.getNumVerts();
   nverts = new NVert[numnverts];

   for (int i = 0 ; i < mesh->getNumFaces() ; i++) {  
      // for each vertex of each face
      for (int j = 0 ; j < 3 ; j++) {     
         int iCur = nverts[mesh->faces[i].v[j]].faces.Count();

         // Tell the vertex, which to which face it belongs and which 
         // of the three face v-indices corresponds to the vertex
         nverts[mesh->faces[i].v[j]].faces.SetCount(iCur+1);
         nverts[mesh->faces[i].v[j]].whichVertex.SetCount(iCur+1);
         nverts[mesh->faces[i].v[j]].faces[iCur] = i;
         nverts[mesh->faces[i].v[j]].whichVertex[iCur] = j;

         // Do the same for vertex color maps:
         for (int chan=0; chan<3; chan++) {
            TVFace *mf = mesh->mapFaces(-chan);
            if (!mf) continue;

            NVert & nvcv = nvcverts[chan][mf[i].t[j]];
            iCur = nvcv.faces.Count();
            nvcv.faces.SetCount(iCur+1);
            nvcv.whichVertex.SetCount(iCur+1);
            nvcv.faces[iCur] = i;
            nvcv.whichVertex[iCur] = j;
         }
      }
   }
}
#endif

// SelectRestore --------------------------------------------------

SelectRestore::SelectRestore(WalkMeshModifier *m, WalkMeshLocalData *data) {
   mod     = m;
   level   = mod->selLevel;
   d       = data;
   d->held = TRUE;
   switch (level) {
   case SEL_OBJECT: DbgAssert(0); break;
   case SEL_VERTEX: usel = d->vertSel; break;
   case SEL_EDGE:
      usel = d->edgeSel;
      break;
   default:
      usel = d->uiFlags[0];
      break;
   }
}

SelectRestore::SelectRestore(WalkMeshModifier *m, WalkMeshLocalData *data, int sLevel) {
   mod     = m;
   level   = sLevel;
   d       = data;
   d->held = TRUE;
   switch (level) {
   case SEL_OBJECT: DbgAssert(0); break;
   case SEL_VERTEX: usel = d->vertSel; break;
   case SEL_EDGE:
      usel = d->edgeSel; break;
   default:
      usel = d->uiFlags[0]; break;
   }
}

void SelectRestore::Restore(int isUndo) {
   if (isUndo) {
      switch (level) {			
      case SEL_VERTEX:
         rsel = d->vertSel; break;
      case SEL_FACE: 
      case SEL_POLY:
      case SEL_ELEMENT:
         rsel = d->uiFlags[0]; break;
      case SEL_EDGE:
         rsel = d->edgeSel; break;
      }
   }
   switch (level) {		
   case SEL_VERTEX:
      d->vertSel = usel; break;
   case SEL_FACE:
   case SEL_POLY:
   case SEL_ELEMENT:
      d->uiFlags[0] = usel; break;
   case SEL_EDGE:
      d->edgeSel = usel; break;
   }
   mod->LocalDataChanged ();
}

void SelectRestore::Redo() {
   switch (level) {		
   case SEL_VERTEX:
      d->vertSel = rsel; break;
   case SEL_FACE:
   case SEL_POLY:
   case SEL_ELEMENT:
      d->uiFlags[0] = rsel; break;
   case SEL_EDGE:
      d->edgeSel = rsel; break;
   }
   mod->LocalDataChanged ();
}
