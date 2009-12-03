#include "NWN2Utility.h"
#include "MDBFormat.h"
#include <utilapi.h> 

#pragma warning( disable:4800 )

#include <map>

using namespace std;

extern HINSTANCE hInstance;

#define MDBUtility_CLASS_ID	Class_ID(0x05152a30, 0x82f44f91)

class MDBUtility : public UtilityObj 
{

public:

   MDBUtility();
   ~MDBUtility();		

   void			BeginEditParams(Interface *ip,IUtil *iu);
   void			EndEditParams(Interface *ip,IUtil *iu);
   void			SelectionSetChanged(Interface *ip,IUtil *iu);
   void			Init(HWND hWnd);
   void			Destroy(HWND hWnd);
   void			DeleteThis() { }

   void			selectionChanged();
   void        enableGUI();

   void        Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t);

   void        showInfo(BOOL show, int selFlags);
   void        showType(BOOL show, int selFlags);
   void        showFlag(BOOL show, int selFlags);
   void        showSkel(BOOL show, int selFlags);
   void        showHelm(BOOL show, int selFlags);
   void        showHair(BOOL show, int selFlags);

   INT_PTR dlgInfoProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

   int getSelFlags();

private:
   IUtil			*mIU;
   Interface	*mIP;
   IParamBlock2 *pb;

   bool        mNoUpdate;

   HWND			mInfoPanel;
   IParamMap2*  mTypeMap;
   IParamMap2*  mFlagMap;
   IParamMap2*  mSkelMap;
   IParamMap2*  mHairMap;
   IParamMap2*  mHelmMap;

   HWND			mTypePanel;
   HWND			mFlagPanel;
   HWND			mSkelPanel;
   HWND			mHairPanel;
   HWND			mHelmPanel;

};

enum SelFlags
{
   MDB_SEL_NONE      = 0,
   MDB_SEL_MULT      = 0x00000001,
   MDB_SEL_MESH      = 0x00000002,
   MDB_SEL_DUMY      = 0x00000004,
   MDB_SEL_SKIN      = 0x00000008,

   MDB_SEL_TYPE_MISS = 0x00010000,
   MDB_SEL_TYPE_RIGD = 0x00020000,
   MDB_SEL_TYPE_SKIN = 0x00040000,
   MDB_SEL_TYPE_WALK = 0x00080000,
   MDB_SEL_TYPE_HOOK = 0x00100000,
   MDB_SEL_TYPE_HELM = 0x00200000,
   MDB_SEL_TYPE_HAIR = 0x00400000,
   MDB_SEL_TYPE_COL2 = 0x00800000,
   MDB_SEL_TYPE_COL3 = 0x01000000,
   MDB_SEL_TYPE_MASK = 0x0FFF0000,
};

enum MDBUtilType
{
   MDB_TYPE_NONE = 0,
   MDB_TYPE_SKIN = 1,
   MDB_TYPE_RIGD = 2,
   MDB_TYPE_WALK = 3,
   MDB_TYPE_HOOK = 4,
   MDB_TYPE_COL2 = 5,
   MDB_TYPE_COL3 = 6,
   MDB_TYPE_HAIR = 7,
   MDB_TYPE_HELM = 8,
};

static MDBUtility plugin;

class MDBUtilityClassDesc : public ClassDesc2
{
public:
   MDBUtilityClassDesc();
   ~MDBUtilityClassDesc();
   int 			   IsPublic() { return TRUE; }
   void *			Create(BOOL loading = FALSE) { return &plugin; }
   const TCHAR *	ClassName() { return GetString(IDS_MDBUTILITY); }
   SClass_ID		SuperClassID() { return UTILITY_CLASS_ID; }
   Class_ID		   ClassID() { return MDBUtility_CLASS_ID; }
   const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

   const TCHAR*	InternalName() { return _T("MDBUtility"); }
   HINSTANCE		HInstance() { return hInstance; }
};

enum { type_params, flag_params, skel_params, hair_params, helm_params };  // pblock ID
enum { PB_UTIL_TYPE, PB_FLAG_TRANS, PB_FLAG_HEAD, PB_FLAG_GLOW, PB_FLAG_ENVMAP, PB_FLAG_NOCAST, PB_FLAG_PROJTEX, PB_SKEL, PB_HAIR, PB_HELM };

static MDBUtilityClassDesc MDBUtilityDesc;
ClassDesc2* GetMDBUtilityDesc() { return &MDBUtilityDesc; }

class UtilPBAccessor : public PBAccessor
{ 
public:
   UtilPBAccessor() : m_util(NULL) {}

   MDBUtility* GetOwner() { return m_util; }
   void SetOwner(MDBUtility* value) { m_util = value; }

   // set from v
   void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t) 
   { 
      if (m_util)
      {
         m_util->Set(v, owner, id, tabIndex, t);
      }
   }

   MDBUtility* m_util;
};
static UtilPBAccessor UtilAccessor;

static ParamBlockDesc2 param_blk ( 
    type_params, _T("MDB Utility"),  0, &MDBUtilityDesc, P_CLASS_PARAMS | P_MULTIMAP,
    //rollout
    //5,
    //type_params,	IDD_WM_TYPE,  IDS_MDBUTIL_TYPE, 0, 0, NULL, 
    //flag_params,	IDD_WM_FLAGS, IDS_MDBUTIL_FLAG, 0, 0, NULL, 
    //skel_params,	IDD_WM_SKEL,  IDS_MDBUTIL_SKEL, 0, 0, NULL, 
    //hair_params,	IDD_WM_HAIR,  IDS_MDBUTIL_HAIR, 0, 0, NULL, 
    //helm_params,	IDD_WM_HELM,  IDS_MDBUTIL_HELM, 0, 0, NULL, 

    // params
    PB_UTIL_TYPE, 	_T("type"),	TYPE_INT, 0, IDS_TYPE,
       p_default, 	0, 
       p_range, 		0, 8, 
       p_ui, 			type_params,	TYPE_RADIO, 9, IDC_RDO_TYPE_NONE, IDC_RDO_TYPE_SKIN, IDC_RDO_TYPE_RIGD, IDC_RDO_TYPE_WALK, IDC_RDO_TYPE_HOOK, IDC_RDO_TYPE_COL2, IDC_RDO_TYPE_COL3, IDC_RDO_TYPE_HAIR, IDC_RDO_TYPE_HELM,
       p_accessor,	&UtilAccessor,
       end,

    PB_FLAG_TRANS, 	_T("trans"),	TYPE_INT, 0, IDS_FLAG_TRANS,
       p_default, 	0, 
       p_range, 		0, 1, 
       p_ui, 			flag_params,	TYPE_RADIO, 2, IDC_CHK_FLAGS_NOTRANS, IDC_CHK_FLAGS_TRANS, 
       p_accessor,	&UtilAccessor,
       end,

    PB_FLAG_HEAD, _T("head"), TYPE_BOOL, 0,	IDS_FLAG_HEAD,
       p_default,  FALSE,
       p_ui, 		flag_params,    TYPE_SINGLECHEKBOX,    IDC_CHK_FLAGS_HEAD, 
       p_accessor,	&UtilAccessor,
       end,

    PB_FLAG_GLOW, _T("glow"), TYPE_BOOL, 0,	IDS_FLAG_GLOW,
       p_default,  FALSE,
       p_ui, 		flag_params,    TYPE_SINGLECHEKBOX,    IDC_CHK_FLAGS_GLOW, 
       p_accessor,	&UtilAccessor,
       end,

    PB_FLAG_ENVMAP, _T("envMap"), TYPE_BOOL, 0,	IDS_FLAG_ENVMAP,
       p_default,  FALSE,
       p_ui, 		flag_params,    TYPE_SINGLECHEKBOX,    IDC_CHK_FLAGS_ENVMAP, 
       p_accessor,	&UtilAccessor,
       end,


    PB_FLAG_NOCAST, _T("noCast"), TYPE_BOOL, 0,	IDS_FLAG_NOCAST,
       p_default,  FALSE,
       p_ui, 		flag_params,    TYPE_SINGLECHEKBOX,    IDC_CHK_FLAGS_NOSHADOW, 
       p_accessor,	&UtilAccessor,
       end,

    PB_FLAG_PROJTEX, _T("projTex"), TYPE_BOOL, 0,	IDS_FLAG_PROJTEX,
       p_default,  FALSE,
       p_ui, 		flag_params,    TYPE_SINGLECHEKBOX,    IDC_CHK_FLAGS_PROJTEX, 
       p_accessor,	&UtilAccessor,
       end,

    PB_SKEL, _T("skeleton"),  TYPE_STRING, 0, IDS_SKEL,
       p_ui,      skel_params,   TYPE_EDITBOX,  IDC_TXT_SKEL,
       p_accessor,	&UtilAccessor,
       end,

    PB_HAIR, 	_T("hair"),	TYPE_INT, 0, IDS_HAIR,
       p_default, 	0, 
       p_range, 		0, 3, 
       p_ui, 			hair_params,	TYPE_RADIO, 4, IDC_RDO_HAIR_LOW, IDC_RDO_HAIR_SHORT, IDC_RDO_HAIR_PONY, IDC_RDO_HAIR_NONE,
       p_accessor,	&UtilAccessor,
       end,

    PB_HELM, 	_T("helm"),	TYPE_INT, 0, IDS_HELM,
       p_default, 	0, 
       p_range, 		0, 4,
       p_ui, 			helm_params,	TYPE_RADIO, 5, IDC_RDO_HELM_NONE, IDC_RDO_HELM_WHOLE, IDC_RDO_HELM_PARTIAL, IDC_RDO_HELM_FULL, IDC_RDO_HELM_FULL_BEARD,
       p_accessor,	&UtilAccessor,
       end,
    end
);

MDBUtilityClassDesc::MDBUtilityClassDesc()
{
   //param_blk.SetClassDesc(this);
}

MDBUtilityClassDesc::~MDBUtilityClassDesc()
{
   //param_blk.SetClassDesc(NULL);
}

//--- MDBUtility -------------------------------------------------------

MDBUtility::MDBUtility()
{
   mIU = NULL;
   mIP = NULL;
   mInfoPanel = NULL;
   mTypePanel = NULL;
   mFlagPanel = NULL;
   mSkelPanel = NULL;
   mHairPanel = NULL;
   mHelmPanel = NULL;

   mTypeMap = NULL;
   mFlagMap = NULL;
   mSkelMap = NULL;
   mHairMap = NULL;
   mHelmMap = NULL;

   UtilAccessor.SetOwner(this);
}

MDBUtility::~MDBUtility()
{

}

void MDBUtility::BeginEditParams(Interface *ip, IUtil *iu) 
{
   mIP = ip;
   mIU = iu;
   mNoUpdate = true;
   UtilAccessor.SetOwner(this);

   pb = param_blk.class_params;

   showInfo(TRUE, getSelFlags());

   selectionChanged();
   mNoUpdate = false;
}

void MDBUtility::EndEditParams(Interface *ip,IUtil *iu) 
{
   mNoUpdate = true;
   showInfo(FALSE, 0);

   UtilAccessor.SetOwner(NULL);
   mIU = NULL;
   mIP = NULL;
   pb = NULL;
   mNoUpdate = false;
}

void MDBUtility::SelectionSetChanged(Interface *ip, IUtil *iu)
{
   selectionChanged();
}

void MDBUtility::selectionChanged()
{
   if (!mIP) return;

   bool noUpdate = mNoUpdate;
   mNoUpdate = true;

   showInfo( TRUE, getSelFlags() );

   mNoUpdate = noUpdate;
}

int MDBUtility::getSelFlags()
{
   int mesh=0, dummy=0, skin=0;

   int flags = MDB_SEL_NONE;

   if (!mIP) return flags;

   for (int i=0; i<mIP->GetSelNodeCount(); i++) 
   {
      INode* node = mIP->GetSelNode(i);
      if (node != NULL)
      {
         TimeValue t = 0;
         ObjectState os = node->EvalWorldState(t); 

         TSTR typeStr;
         BOOL hasType = node->GetUserPropString("type", typeStr);

         if (node->IsBoneShowing())
         {
            ;
         }
         else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
         {
            TSTR objClass;
            os.obj->GetClassName(objClass);

            if (  os.obj 
               && (  os.obj->ClassID() == BONE_OBJ_CLASSID 
               || os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
               || os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
               )
               ) 
            {
               ; // ignore
            } 
            else if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
            {
               ++mesh;
               if (NULL != GetSkin(node))
                  ++skin;

               if (!hasType || typeStr.isNull())
                  flags |= MDB_SEL_TYPE_MISS;
               else if (stricmp(typeStr, "rigd") == 0)
                  flags |= MDB_SEL_TYPE_RIGD;
               else if (stricmp(typeStr, "skin") == 0)
                  flags |= MDB_SEL_TYPE_SKIN;
               else if (stricmp(typeStr, "walk") == 0)
                  flags |= MDB_SEL_TYPE_WALK;
               else if (stricmp(typeStr, "col2") == 0)
                  flags |= MDB_SEL_TYPE_COL2;
               else if (stricmp(typeStr, "col3") == 0)
                  flags |= MDB_SEL_TYPE_COL3;
            }
         }
         else if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
         {
            if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
            {
               ++dummy;

               if (!hasType || typeStr.isNull())
                  flags |= MDB_SEL_TYPE_MISS;
               else if (stricmp(typeStr, "hook") == 0)
                  flags |= MDB_SEL_TYPE_HOOK;
               else if (stricmp(typeStr, "helm") == 0)
                  flags |= MDB_SEL_TYPE_HELM;
               else if (stricmp(typeStr, "hair") == 0)
                  flags |= MDB_SEL_TYPE_HAIR;
            }
         }
      }
   }

   if ( (dummy + mesh) > 0 )
   {
      if ( (dummy + mesh) > 1 )
         flags |= MDB_SEL_MULT;
      if ( mesh > 0 )
         flags |= MDB_SEL_MESH;
      if ( dummy > 0 )
         flags |= MDB_SEL_DUMY;
      if ( skin > 0 )
         flags |= MDB_SEL_SKIN;
   }
   return flags;
}


//////////////////////////////////////////////////////////////////////////
/// Info
//////////////////////////////////////////////////////////////////////////

static INT_PTR CALLBACK MDBUtilityInfoDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   return plugin.dlgInfoProc(hWnd, msg, wParam, lParam);
}

void MDBUtility::showInfo(BOOL show, int selFlags)
{
   if (mIP == NULL)
      return;

   if (show)
   {
      if (mInfoPanel == NULL){
         mInfoPanel = mIP->AddRollupPage(
            hInstance,
            MAKEINTRESOURCE(IDD_WM_VERINFO),
            MDBUtilityInfoDlgProc,
            GetString(IDS_MDBUTIL_TYPE),
            ROLLUP_CAT_STANDARD);
      }
      TSTR version = GetFileVersion(NULL);
      SetWindowText( GetDlgItem(mInfoPanel, IDC_LBL_VERINFO),version );

      int count = mIP->GetSelNodeCount();
      if (count == 0)
      {
         SetWindowText( GetDlgItem(mInfoPanel, IDC_LBL_SELECTION), "none" );
         showType(FALSE, selFlags);
      }
      else if (count == 1)
      {
         INode* node = mIP->GetSelNode(0);
         SetWindowText( GetDlgItem(mInfoPanel, IDC_LBL_SELECTION), node ? node->GetName() : "???" );
         showType(TRUE, selFlags);
      }
      else 
      {
         SetWindowText( GetDlgItem(mInfoPanel, IDC_LBL_SELECTION), "multiple" );
         showType(TRUE, selFlags);
      }
   }
   else
   {
      showType(FALSE, selFlags);

      if (!mNoUpdate)
      {
         if (mInfoPanel){
            mIP->DeleteRollupPage(mInfoPanel);
            mInfoPanel = NULL;
         }
         if (mIU)
         {
            IUtil	*iu = mIU;
            mIU = NULL;
            iu->CloseUtility();
         }
      }
   }
}

INT_PTR MDBUtility::dlgInfoProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch (msg) 
   {
   case WM_DESTROY:
   case WM_NCDESTROY:
      mInfoPanel = NULL;
      break;

   case  WM_COMMAND:
      switch ( LOWORD(wParam) )
      {
      case IDC_CLOSE:
         showInfo(FALSE, 0);
         return TRUE;
      }
   case WM_PAINT:
      return FALSE;
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/// Type
//////////////////////////////////////////////////////////////////////////
void MDBUtility::showType(BOOL show, int selFlags)
{
   if (mIP == NULL)
      return;

   if (show)
   {
      if (mTypePanel == NULL){
         mTypeMap = CreateCPParamMap2(type_params, pb, mIP, hInstance, (char*)IDD_WM_TYPE, "MDB Object Type", 0, NULL, NULL);
         if (!mTypeMap) {
            return;
         }
         mTypeMap->Show(type_params, TRUE, 0);
         mTypePanel = mTypeMap->GetHWnd();
      }

      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_HELM) , (selFlags & MDB_SEL_DUMY) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_HAIR) , (selFlags & MDB_SEL_DUMY) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_HOOK) , (selFlags & MDB_SEL_DUMY) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_SKIN) , (selFlags & MDB_SEL_SKIN) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_COL2) , (selFlags & MDB_SEL_MESH) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_COL3) , (selFlags & MDB_SEL_MESH) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_RIGD) , (selFlags & MDB_SEL_MESH) );
      EnableWindow( GetDlgItem(mTypePanel, IDC_RDO_TYPE_WALK) , (selFlags & MDB_SEL_MESH) );

      BOOL bShowFlag = (selFlags & (MDB_SEL_TYPE_SKIN | MDB_SEL_TYPE_RIGD)) ? TRUE : FALSE;
      BOOL bShowHair = (selFlags & MDB_SEL_TYPE_HAIR) ? TRUE : FALSE;
      BOOL bShowSkel = (selFlags & MDB_SEL_TYPE_SKIN) ? TRUE : FALSE;
      BOOL bShowHelm = (selFlags & MDB_SEL_TYPE_HELM) ? TRUE : FALSE;

      int value = MDB_TYPE_NONE;
      if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_RIGD ) {
         value = MDB_TYPE_RIGD;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_SKIN ) {
         value = MDB_TYPE_SKIN;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_HAIR ) {
         value = MDB_TYPE_HAIR;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_HELM ) {
         value = MDB_TYPE_HELM;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_HOOK ) {
         value = MDB_TYPE_HOOK;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_WALK ) {
         value = MDB_TYPE_WALK;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_COL2 ) {
         value = MDB_TYPE_COL2;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_COL3 ) {
         value = MDB_TYPE_COL3;
      } else if ( (selFlags & MDB_SEL_TYPE_MASK) == MDB_SEL_TYPE_MISS ) {
         value = MDB_TYPE_NONE;
      } else {
         value = MDB_TYPE_NONE;
         for (int id=IDC_RDO_TYPE_NONE; id<IDC_RDO_TYPE_HELM; ++id) 
            CheckDlgButton(mTypePanel, id, FALSE);
      }

      pb->SetValue(PB_UTIL_TYPE, 0, value, 0);
      mTypeMap->Invalidate(PB_UTIL_TYPE);
      mTypeMap->Invalidate();

      showFlag(bShowFlag, selFlags);
      showHair(bShowHair, selFlags);
      showHelm(bShowHelm, selFlags);
      showSkel(bShowSkel, selFlags);
   }
   else
   {
      showHelm(FALSE, selFlags);
      showHair(FALSE, selFlags);
      showSkel(FALSE, selFlags);
      showFlag(FALSE, selFlags);

      if (mTypeMap){
         IParamMap2* map = mTypeMap;
         mTypeMap = NULL;
         mTypePanel = NULL;

         map->Show(type_params, FALSE, 0);
         DestroyCPParamMap2(map);
      }
   }
}


//////////////////////////////////////////////////////////////////////////
/// Flags
//////////////////////////////////////////////////////////////////////////

void MDBUtility::showFlag(BOOL show, int selFlags)
{
   if (mIP == NULL)
      return;

   if (show)
   {
      if (mFlagPanel == NULL){
         mFlagMap = CreateCPParamMap2(flag_params, pb, mIP, hInstance, (char*)IDD_WM_FLAGS, "MDB Material Flags", 0, NULL, NULL);
         if (!mFlagMap)
            return;
         mFlagMap->Show(flag_params, TRUE, 0);
         mFlagPanel = mFlagMap->GetHWnd();
      }

      int trans = 0;
      int head = 0;
      int glow = 0;
      int envmap = 0;
      int nocast = 0;
      int projtex = 0;

      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 

            TSTR typeStr;
            BOOL hasType = node->GetUserPropString("type", typeStr);

            if (node->IsBoneShowing())
            {
               ;
            }
            else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
            {
               TSTR objClass;
               os.obj->GetClassName(objClass);

               if (  os.obj 
                  && (  os.obj->ClassID() == BONE_OBJ_CLASSID 
                  || os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
                  || os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
                  )
                  ) 
               {
                  ; // ignore
               } 
               else if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
               {
                  int value = 0;
                  if ( node->GetUserPropInt("trans", value))
                     trans += value;
                  if ( node->GetUserPropInt("envmap", value))
                     envmap += value;
                  if ( node->GetUserPropInt("head", value))
                     head += value;
                  if ( node->GetUserPropInt("nolight", value))
                     glow += value;
                  if ( node->GetUserPropInt("nocast", value))
                     nocast += value;
                  if ( node->GetUserPropInt("projtexture", value))
                     projtex += value;
               }
            }
         }
      }

      pb->SetValue( PB_FLAG_TRANS,   0, int(max(0, min(1, trans))) );
      pb->SetValue( PB_FLAG_HEAD,    0, int(max(0, min(1, head))) );
      pb->SetValue( PB_FLAG_GLOW,    0, int(max(0, min(1, glow))) );
      pb->SetValue( PB_FLAG_ENVMAP,  0, int(max(0, min(1, envmap))) );
      pb->SetValue( PB_FLAG_NOCAST,  0, int(max(0, min(1, nocast))) );
      pb->SetValue( PB_FLAG_PROJTEX, 0, int(max(0, min(1, projtex))) );
   }
   else
   {
      if (mFlagMap){
         IParamMap2* map = mFlagMap;
         mFlagMap = NULL;
         mFlagPanel = NULL;

         map->Show(flag_params, FALSE, 0);
         DestroyCPParamMap2(map);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/// Skeleton
//////////////////////////////////////////////////////////////////////////

void MDBUtility::showSkel(BOOL show, int selFlags)
{
   if (mIP == NULL)
      return;
   if (show){
      if (mSkelPanel == NULL){
         mSkelMap = CreateCPParamMap2(skel_params, pb, mIP, hInstance, (char*)IDD_WM_SKEL, "MDB Skeleton", 0, NULL, NULL);
         if (!mSkelMap)
            return;
         mSkelMap->Show(skel_params, TRUE, 0);
         mSkelPanel = mSkelMap->GetHWnd();
      }

      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 

            if (node->IsBoneShowing())
            {
               ;
            }
            else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
            {
               TSTR objClass;
               os.obj->GetClassName(objClass);

               if (  os.obj 
                  && (  os.obj->ClassID() == BONE_OBJ_CLASSID 
                  || os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
                  || os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
                  )
                  ) 
               {
                  ; // ignore
               } 
               else if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
               {
                  if (NULL != GetSkin(node))
                  {
                     TSTR skel;
                     node->GetUserPropString("skelfile", skel);
                     pb->SetValue(PB_SKEL, 0, (char*)skel.data(), 0);
                  }
               }
            }
         }
      }
   }
   else
   {
      if (mSkelMap){
         IParamMap2* map = mSkelMap;
         mSkelMap = NULL;
         mSkelPanel = NULL;

         map->Show(skel_params, FALSE, 0);
         DestroyCPParamMap2(map);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/// Helm
//////////////////////////////////////////////////////////////////////////

void MDBUtility::showHelm(BOOL show, int selFlags)
{
   if (mIP == NULL)
      return;
   if (show){
      if (mHelmPanel == NULL){
         mHelmMap = CreateCPParamMap2(helm_params, pb, mIP, hInstance, (char*)IDD_WM_HELM, "MDB Helm Flags", 0, NULL, NULL);
         if (!mHelmMap)
            return;
         mHelmMap->Show(helm_params, TRUE, 0);
         mHelmPanel = mHelmMap->GetHWnd();
      }

      int flag = 0;
      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 
            if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
            {
               if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
               {
                  TSTR str;
                  if ( node->GetUserPropString("type", str) && _tcsicmp(str, "HELM") == 0)
                  {
                     if ( node->GetUserPropString("HelmFlag", str) )
                        flag = StringToEnum(str, NWN2::MDBHelmHairHidingBehaviors);
                     if ( node->GetUserPropString("flag", str) )
                        flag = _ttoi(str);
                  }
               }
            }
         }
      }
      pb->SetValue( PB_HELM, 0, max(0, min(4, flag)) );
      //CheckRadioButton(mHelmPanel, IDC_RDO_HELM_NONE, IDC_RDO_HELM_FULL_BEARD, IDC_RDO_HELM_NONE + flag);
   } else {
      if (mHelmMap){
         IParamMap2* map = mHelmMap;
         mHelmMap = NULL;
         mHelmPanel = NULL;
         map->Show(helm_params, FALSE, 0);
         DestroyCPParamMap2(map);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/// Hair
//////////////////////////////////////////////////////////////////////////

void MDBUtility::showHair(BOOL show, int selFlags)
{
   if (mIP == NULL)
      return;
   if (show){
      if (mHairPanel == NULL){
         mHairMap = CreateCPParamMap2(hair_params, pb, mIP, hInstance, (char*)IDD_WM_HAIR, "MDB Hair Flags", 0, NULL, NULL);
         if (!mHairMap)
            return;
         mHairMap->Show(hair_params, TRUE, 0);
         mHairPanel = mHairMap->GetHWnd();
      }

      int flag = 0;
      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 
            if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
            {
               if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
               {
                  TSTR str;
                  if ( node->GetUserPropString("type", str) && _tcsicmp(str, "HAIR") == 0)
                  {
                     if ( node->GetUserPropString("HairFlag", str) )
                        flag = StringToEnum(str, NWN2::MDBHairShorteningBehaviors);
                     if ( node->GetUserPropString("flag", str) )
                        flag = _ttoi(str);
                  }
               }
            }
         }
      }
      pb->SetValue( PB_HAIR, 0, max(0, min(3, flag)) );
      //CheckRadioButton(mHairPanel, IDC_RDO_HAIR_LOW, IDC_RDO_HAIR_NONE, IDC_RDO_HAIR_LOW + flag);
   } else {
      if (mHairMap){
         IParamMap2* map = mHairMap;
         mHairMap = NULL;
         mHairPanel = NULL;
         map->Show(hair_params, FALSE, 0);
         DestroyCPParamMap2(map);
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/// Set 
//////////////////////////////////////////////////////////////////////////

void MDBUtility::Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t) 
{ 
   if (mNoUpdate)
      return;

   switch (id)
   {
   case PB_UTIL_TYPE:
      {
         TSTR str;
         switch (v.i)
         {
         case 0: break;
         case 1: str = "skin"; break;
         case 2: str = "rigd"; break;
         case 3: str = "walk"; break;
         case 4: str = "hook"; break;
         case 5: str = "col2"; break;
         case 6: str = "col3"; break;
         case 7: str = "hair"; break;
         case 8: str = "helm"; break;
         }
         for (int i=0; i<mIP->GetSelNodeCount(); i++) 
         {
            INode* node = mIP->GetSelNode(i);
            if (node != NULL)
            {
               TimeValue t = 0;
               ObjectState os = node->EvalWorldState(t); 
               if (node->IsBoneShowing())
               {
                  ;
               }
               else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
               {
                  TSTR objClass;
                  os.obj->GetClassName(objClass);

                  if (  os.obj 
                     && (  os.obj->ClassID() == BONE_OBJ_CLASSID 
                     || os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
                     || os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
                     )
                     ) 
                  {
                     ; // ignore
                  } 
                  else if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
                  {
                     node->SetUserPropString("type", str);
                  }
               }
               else if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
               {
                  if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
                  {
                     node->SetUserPropString("type", str);
                  }
               }
            }
         }
         selectionChanged();
      } break;

   case PB_FLAG_TRANS:
   case PB_FLAG_HEAD:
   case PB_FLAG_GLOW:
   case PB_FLAG_ENVMAP:
   case PB_FLAG_NOCAST:
   case PB_FLAG_PROJTEX:
      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            bool ok = false;
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 
            if (node->IsBoneShowing())
            {
               ;
            }
            else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
            {
               TSTR objClass;
               os.obj->GetClassName(objClass);

               if (  os.obj 
                  && (  os.obj->ClassID() == BONE_OBJ_CLASSID 
                  || os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
                  || os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
                  )
                  ) 
               {
                  ; // ignore
               } 
               else if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
               {
                  ok = true;
               }
            }

            if (ok)
            {
               switch (id)
               {
               case PB_FLAG_TRANS:     
                  node->SetUserPropInt("trans",      int(v.i ? 1 : 0)); 
                  node->SetUserPropInt("addblend",   0); 
                  node->SetUserPropInt("alphablend", 0); 
                  break;
               case PB_FLAG_HEAD:      node->SetUserPropInt("head",    int(v.i ? 1 : 0)); break;
               case PB_FLAG_GLOW:      node->SetUserPropInt("glow",    int(v.i ? 1 : 0)); break;
               case PB_FLAG_ENVMAP:    node->SetUserPropInt("envmap",  int(v.i ? 1 : 0)); break;
               case PB_FLAG_NOCAST:    node->SetUserPropInt("nocast",  int(v.i ? 1 : 0)); break;
               case PB_FLAG_PROJTEX:   node->SetUserPropInt("projtex", int(v.i ? 1 : 0)); break;
               }
            }
         }
      } break;

   case PB_SKEL:
      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 

            if (node->IsBoneShowing())
            {
               ;
            }
            else if (os.obj && os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID)
            {
               TSTR objClass;
               os.obj->GetClassName(objClass);

               if (  os.obj 
                  && (  os.obj->ClassID() == BONE_OBJ_CLASSID 
                  || os.obj->ClassID() == Class_ID(BONE_CLASS_ID,0)
                  || os.obj->ClassID() == Class_ID(0x00009125,0) /* Biped Twist Helpers */
                  )
                  ) 
               {
                  ; // ignore
               } 
               else if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
               {
                  if (NULL != GetSkin(node))
                  {
                     node->SetUserPropString("skelfile", v.s);
                  }
               }
            }
         }
      } break;

   case PB_HAIR:
   case PB_HELM:       
      for (int i=0; i<mIP->GetSelNodeCount(); i++) 
      {
         INode* node = mIP->GetSelNode(i);
         if (node != NULL)
         {
            TimeValue t = 0;
            ObjectState os = node->EvalWorldState(t); 
            if (os.obj && os.obj->SuperClassID()==HELPER_CLASS_ID)
            {
               if (os.obj->ClassID() == Class_ID(POINTHELP_CLASS_ID,0))
               {
                  TSTR str;
                  node->GetUserPropString("type", str);
                  if ( (id == PB_HAIR && _tcsicmp(str, "hair") == 0) 
                     ||(id == PB_HELM && _tcsicmp(str, "helm") == 0) )
                  {
                     node->SetUserPropInt("flag", v.i);
                  }
               }
            }
         }
      }
      break;
   }
}
