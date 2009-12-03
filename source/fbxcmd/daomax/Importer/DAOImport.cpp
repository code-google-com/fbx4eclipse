/**********************************************************************
 *<
	FILE: DAOImport.cpp

	DESCRIPTION:	Appwizard generated plugin

	HISTORY: 

 *>	Copyright (c) 2009, All Rights Reserved.
 **********************************************************************/
#include "DAOImport.h"
#include "FBXImport.h"
#include <custcont.h>
#include <shlwapi.h>

const Class_ID DAOImport_CLASS_ID(0x4BE2940B, 0x02E74FCB);
const Interface_ID DAOImport_INTERFACE_ID(0x44934AEE, 0xCFE045E5);

extern ClassDesc2* GetDAOImportDesc();
extern ParamMap2UserDlgProc* GetMDImportDlgProc();

static const char *FBXImport = "FBXImport";
static const char *FBXCommon = "FBXCommon";

class DAOImport : public SceneImport {
	public:
		int				ExtCount();					// Number of extensions supported
		const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		const TCHAR *	AuthorName();				// ASCII Author name
		const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		const TCHAR *	OtherMessage1();			// Other message #1
		const TCHAR *	OtherMessage2();			// Other message #2
		unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		void			   ShowAbout(HWND hWnd);		// Show DLL's "About..." box
		int				DoImport(const TCHAR *name,ImpInterface *i,Interface *gi, BOOL suppressPrompts=FALSE);	// Import file
		
		//Constructor/Destructor
		DAOImport();
		~DAOImport();		

      _tstring iniFileName;
      std::vector<_tstring> Exts;
};


class DAOImportClassDesc : public ClassDesc2 {
	public:
   DAOImportClassDesc();
	int 			   IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new DAOImport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_IMPORT_CLASS_ID; }
	Class_ID		   ClassID() { return DAOImport_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("TZ_FBX_Importer"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
};

//--- Parameter map/block descriptors -------------------------------


// Parameter and ParamBlock IDs

enum { box_params_panel, };

class INIPBAccessor : public PBAccessor
{ 
public:
   TSTR iniFileName;
   bool iniFileValid;

   TSTR ShortDesc;
   TSTR Col2Match;
   TSTR BoneMatch;
   TSTR FaceBoneMatch;

   INIPBAccessor() : iniFileValid(false)
   {
      Interface *gi = GetCOREInterface();
      TCHAR iniName[MAX_PATH];
      GetModuleFileName(NULL, iniName, _countof(iniName));
      if (LPTSTR fname = PathFindFileName(iniName))
         *fname = NULL;
      PathAddBackslash(iniName);
      PathAppend(iniName, "plugcfg");
      PathAppend(iniName, "DAOUtils.ini");
#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+
      if (gi && (_access(iniName, 0) == -1)) {
         LPCTSTR pluginDir = gi->GetDir(APP_PLUGCFG_DIR);
         PathCombine(iniName, pluginDir, "DAOUtils.ini");
      } 
#endif
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
      case IMP_PB_SHORTDESC:        v.s = ShortDesc = GetIniValue<TSTR>("System", "ShortDescription", v.s); break;
      case IMP_PB_FLIPUV:           v.i = GetIniValue<BOOL>(FBXImport, "FlipUVTextures", v.i); break;
      case IMP_PB_AUTOSMOOTH:       v.i = GetIniValue<BOOL>(FBXImport, "EnableAutoSmooth", v.i); break;
      case IMP_PB_AUTOSMOOTH_ANGLE: v.f = GetIniValue<float>(FBXImport, "AutoSmoothAngle", v.f); break;
      case IMP_PB_RENDERTEX:        v.i = GetIniValue<BOOL>(FBXImport, "ShowTextures", v.i); break;
      case IMP_PB_TRIM_WALKMESH:    v.i = GetIniValue<BOOL>(FBXCommon, "TrimWalkmesh", v.i); break;
      case IMP_PB_WALKMESH_LIMIT:   v.f = GetIniValue<float>(FBXCommon, "WalkmeshLimit", v.f); break;
      case IMP_PB_ENABLE_COLLISION: v.i = GetIniValue<BOOL>(FBXImport, "EnableCollision", v.i); break;
      case IMP_PB_ENABLE_WALKMESH:  v.i = GetIniValue<BOOL>(FBXImport, "EnableWalkmesh", v.i); break;
      case IMP_PB_ENABLE_HOOKS:     v.i = GetIniValue<BOOL>(FBXImport, "EnableHooks", v.i); break;
      case IMP_PB_ENABLE_HAIRHELM:  v.i = GetIniValue<BOOL>(FBXImport, "EnableHairOrHelm", v.i); break;
      case IMP_PB_ENABLE_BONES:     v.i = GetIniValue<BOOL>(FBXImport, "EnableBones", v.i); break;
      case IMP_PB_BONEMATCH:        v.s = BoneMatch = GetIniValue<TSTR>(FBXImport, "BoneMatch", v.s); break;
      case IMP_PB_WALK_OPACITY:     v.f = GetIniValue<float>(FBXImport, "WalkmeshOpacity", v.f); break;
      case IMP_PB_FACEBONEMATCH:    v.s = FaceBoneMatch = GetIniValue<TSTR>(FBXImport, "FaceBoneMatch", v.s); break;
	  case IMP_PB_ENABLE_SCALE:     v.i = GetIniValue<BOOL>(FBXImport, "EnableScale", v.i); break;
	  case IMP_PB_SCALE_FACTOR:     v.f = GetIniValue<float>(FBXImport, "ScaleFactor", v.f); break;
	  case IMP_PB_ENABLE_ANIMATION: v.i = GetIniValue<BOOL>(FBXImport, "EnableAnimation", v.i); break;
	  case IMP_PB_NORMALBUMP:       v.i = GetIniValue<BOOL>(FBXImport, "NormalBump", v.i); break;
	  case IMP_PB_ENABLE_WELD:      v.i = GetIniValue<BOOL>(FBXImport, "EnableWeld", v.i); break;
	  case IMP_PB_WELD_LIMIT:       v.f = GetIniValue<float>(FBXImport, "WeldLimit", v.f); break;
      }
   }

   // set from v
   void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t) 
   { 
      switch (id)
      {
      case IMP_PB_SHORTDESC:        SetIniValue<TCHAR*>("System", "ShortDescription", v.s); break;
      case IMP_PB_FLIPUV:           SetIniValue<BOOL>(FBXImport, "FlipUVTextures", v.i); break;
      case IMP_PB_AUTOSMOOTH:       SetIniValue<BOOL>(FBXImport, "EnableAutoSmooth", v.i); break;
      case IMP_PB_AUTOSMOOTH_ANGLE: SetIniValue<float>(FBXImport, "AutoSmoothAngle", v.f); break;
      case IMP_PB_RENDERTEX:        SetIniValue<BOOL>(FBXImport, "ShowTextures", v.i); break;
      case IMP_PB_TRIM_WALKMESH:    SetIniValue<BOOL>(FBXCommon, "TrimWalkmesh", v.i); break;
      case IMP_PB_WALKMESH_LIMIT:   SetIniValue<float>(FBXCommon, "WalkmeshLimit", v.f); break;
      case IMP_PB_ENABLE_COLLISION: SetIniValue<BOOL>(FBXImport, "EnableCollision", v.i); break;
      case IMP_PB_ENABLE_WALKMESH:  SetIniValue<BOOL>(FBXImport, "EnableWalkmesh", v.i); break;
      case IMP_PB_ENABLE_HOOKS:     SetIniValue<BOOL>(FBXImport, "EnableHooks", v.i); break;
      case IMP_PB_ENABLE_HAIRHELM:  SetIniValue<BOOL>(FBXImport, "EnableHairOrHelm", v.i); break;
      case IMP_PB_ENABLE_BONES:     SetIniValue<BOOL>(FBXImport, "EnableBones", v.i); break;
      case IMP_PB_BONEMATCH:        SetIniValue<TCHAR*>(FBXImport, "BoneMatch", v.s); break;
      case IMP_PB_WALK_OPACITY:     SetIniValue<float>(FBXImport, "WalkmeshOpacity", v.f); break;
      case IMP_PB_FACEBONEMATCH:    SetIniValue<TSTR>(FBXImport, "FaceBoneMatch", v.s); break;
	  case IMP_PB_ENABLE_SCALE:     SetIniValue<BOOL>(FBXImport, "EnableScale", v.i); break;
	  case IMP_PB_SCALE_FACTOR:     SetIniValue<float>(FBXImport, "ScaleFactor", v.f); break;
	  case IMP_PB_ENABLE_ANIMATION: SetIniValue<BOOL>(FBXImport, "EnableAnimation", v.i); break;
	  case IMP_PB_NORMALBUMP:       SetIniValue<BOOL>(FBXImport, "NormalBump", v.i); break;
	  case IMP_PB_ENABLE_WELD:      SetIniValue<BOOL>(FBXImport, "EnableWeld", v.i); break;
	  case IMP_PB_WELD_LIMIT:       SetIniValue<float>(FBXImport, "WeldLimit", v.f); break;
      }
   }
};

static INIPBAccessor INIAccessor;

static ParamBlockDesc2 param_blk ( 
   import_params, _T("parameters"),  0, NULL, P_CLASS_PARAMS | P_AUTO_UI | P_MULTIMAP,
   //rollout
   1,
   import_params,	IDD_FBX_PANEL,  IDS_PARAMS, 0, 0, NULL, 
   // params
   IMP_PB_SHORTDESC, _T("shortDescription"),  TYPE_STRING, 0, IDS_LIBDESCRIPTION,
      p_default,  "DAO FBX Importer",
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_FLIPUV, _T("flipUV"), TYPE_BOOL, 0,	IDS_FLIPUV,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_FLIP_UV, 
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_AUTOSMOOTH, _T("autoSmooth"), TYPE_BOOL, 0,	IDS_AUTOSMOOTH,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_AUTOSMOOTH, 
      p_accessor,	&INIAccessor,
      p_prompt,   IDS_AUTOSMOOTH_TOOLTIP,
      end,

   IMP_PB_AUTOSMOOTH_ANGLE, _T("autoSmoothAngle"), TYPE_FLOAT, 0,	IDS_AUTOSMOOTHANGLE,
      p_default,  30.0f,
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_RENDERTEX, _T("renderTextures"), TYPE_BOOL, 0,	IDS_RENDERTEX,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_SHOW_TEX, 
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_TRIM_WALKMESH, _T("trimWalkmesh"), TYPE_BOOL, 0,	IDS_TRIM_WALKMESH,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_LIMIT_WALKMESH, 
      p_accessor,	&INIAccessor,
      p_enable_ctrls, 2, IDC_ED_WALK_LIMIT, IDC_SP_WALK_LIMIT,
      end,

   IMP_PB_WALKMESH_LIMIT, _T("walkmeshLimit"), TYPE_FLOAT, 0, IDS_WALKMESH_LIMIT,
      p_default,  -5.0f,
      p_accessor,	&INIAccessor,
      p_range,    -1000000.0, 0.0,
      p_ui,       import_params,    TYPE_SPINNER, EDITTYPE_UNIVERSE,  IDC_ED_WALK_LIMIT, IDC_SP_WALK_LIMIT, SPIN_AUTOSCALE,
      end,

   IMP_PB_ENABLE_COLLISION, _T("enableCollision"), TYPE_BOOL, 0,	IDS_ENABLE_COLLISION,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_COLLISON, 
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_ENABLE_WALKMESH, _T("enableWalkmesh"), TYPE_BOOL, 0,	IDS_ENABLE_WALKMESH,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_WALKMESH, 
      p_accessor,	&INIAccessor,
      p_enable_ctrls, 2, IDC_ED_WALK_LIMIT, IDC_SP_WALK_LIMIT,
      end,

   IMP_PB_ENABLE_HOOKS, _T("enableHooks"), TYPE_BOOL, 0,	IDS_ENABLE_HOOKS,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_HOOK, 
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_ENABLE_HAIRHELM, _T("enableHairOrHelm"), TYPE_BOOL, 0,	IDS_ENABLE_HAIRHELM,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_HAIR_HELM, 
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_ENABLE_BONES, _T("enableBones"), TYPE_BOOL, 0,	IDS_ENABLE_BONES,
      p_default,  TRUE,
      p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_BONES, 
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_BONEMATCH, _T("boneMatch"),  TYPE_STRING, 0, IDS_BONEMATCH,
      p_default,  "Ribcage;ap_*;f_*",
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_WALK_OPACITY, _T("walkmeshOpacity"), TYPE_FLOAT, 0, IDS_WALKMESH_OPACITY,
      p_default,  0.5f,
      p_accessor,	&INIAccessor,
      p_range,    0.0, 1.0,
      end,

   IMP_PB_FACEBONEMATCH, _T("faceBoneMatch"),  TYPE_STRING, 0, IDS_FACEBONEMATCH,
      p_default,  "f_*",
      p_accessor,	&INIAccessor,
      end,

   IMP_PB_ENABLE_SCALE, _T("enableScale"), TYPE_BOOL, 0,	IDS_ENABLE_SCALE,
	  p_default,  TRUE,
	  p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_SCALE_FACTOR, 
	  p_accessor,	&INIAccessor,
	  p_enable_ctrls, 2, IMP_PB_SCALE_FACTOR, IMP_PB_SCALE_FACTOR,
	  end,

   IMP_PB_SCALE_FACTOR, _T("scaleFactor"), TYPE_FLOAT, 0, IDS_SCALE_FACTOR,
	  p_default,  1.0f,
	  p_accessor,	&INIAccessor,
	  p_range,    0.0, 100000.0,
	  p_ui,       import_params,    TYPE_SPINNER, EDITTYPE_UNIVERSE,  IDC_ED_SCALE_FACTOR, IDC_SP_SCALE_FACTOR, SPIN_AUTOSCALE,
	  end,

   IMP_PB_ENABLE_ANIMATION, _T("enableAnimation"), TYPE_BOOL, 0,	IDS_ENABLE_ANIMATION,
	  p_default,  FALSE,
	  //p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_ANIMATION, 
	  p_accessor,	&INIAccessor,
	  end,

   IMP_PB_NORMALBUMP, _T("normalBump"), TYPE_BOOL, 0,	IDS_NORMALBUMP,
	  p_default,  FALSE,
	  p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_NORMALBUMP, 
	  p_accessor,	&INIAccessor,
	  end,

   IMP_PB_ENABLE_WELD, _T("enableWeld"), TYPE_BOOL, 0,	IDS_ENABLE_WELD,
	  p_default,  TRUE,
	  p_ui, 		imp_fbx_params,    TYPE_CHECKBUTTON,    IDC_CHK_WELD, 
	  p_accessor,	&INIAccessor,
	  p_enable_ctrls, 2, IDC_ED_WELD_LIMIT, IDC_SP_WELD_LIMIT,
	  end,

   IMP_PB_WELD_LIMIT, _T("weldLimit"), TYPE_FLOAT, 0, IDS_WALKMESH_LIMIT,
	  p_default,  0.00001f,
	  p_accessor,	&INIAccessor,
	  p_range,    0.0, 10.0,
	  p_ui,       import_params,    TYPE_SPINNER, EDITTYPE_UNIVERSE,  IDC_ED_WELD_LIMIT, IDC_SP_WELD_LIMIT, 0.0001f,
	  end,

   end
   );

// bug in pb desc? forces us to declare class desc after param block
static DAOImportClassDesc DAOImportDesc;
ClassDesc2* GetDAOImportDesc() { return &DAOImportDesc; }

DAOImportClassDesc::DAOImportClassDesc() {
   param_blk.SetClassDesc(this);
   //MakeAutoParamBlocks(this);
}

#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+

class DAOImportFunctionPublish : public FPStaticInterface {
public:
   DECLARE_DESCRIPTOR(DAOImportFunctionPublish);
public:
   FPStatus _dispatch_fn(FunctionID fid, TimeValue t, FPValue& result, FPParams* p)
   {
      FPStatus status = FPS_OK;
      if (fid < IMP_PB_MAX_BLOCKS)
      {
         ParamType2 type = param_blk.class_params->GetParameterType(ParamID(fid));
         PB2Value val =  param_blk.class_params->GetPB2Value(ParamID(fid));
         switch (type)
         {
         case TYPE_FLOAT: result.LoadPtr(type, val.f); break;
         case TYPE_INT: result.LoadPtr(type, val.i); break;
         case TYPE_RGBA: result.LoadPtr(type, val.i); break;
         case TYPE_POINT3: result.LoadPtr(type, val.p); break;
         case TYPE_BOOL: result.LoadPtr(type, val.i); break;
         case TYPE_STRING: result.LoadPtr(type, val.s); break;
         case TYPE_INODE: result.LoadPtr(type, val.p); break;
         }
      }
      else if (fid > IMP_PB_FN_SET_OFFSET && fid <= IMP_PB_FN_SET_OFFSET+IMP_PB_MAX_BLOCKS)
      {
         ParamID pid = ParamID(fid-IMP_PB_FN_SET_OFFSET);
         ParamType2 type = param_blk.class_params->GetParameterType(pid);
         FPValue& v = p->params[0];
         switch (v.type)
         {
         case TYPE_FLOAT: param_blk.class_params->SetValue(pid, 0, v.f); break;
         case TYPE_INT: param_blk.class_params->SetValue(pid, 0, v.i); break;
         case TYPE_RGBA: param_blk.class_params->SetValue(pid, 0, v.i); break;
         case TYPE_POINT3: param_blk.class_params->SetValue(pid, 0, *v.p); break;
         case TYPE_BOOL: param_blk.class_params->SetValue(pid, 0, v.i); break;
         case TYPE_STRING: param_blk.class_params->SetValue(pid, 0, v.s); break;
         case TYPE_INODE: param_blk.class_params->SetValue(pid, 0, v.n); break;
         }
      }
      else
      {
         status = FPS_NO_SUCH_FUNCTION;
      }
      return status;
   }
   virtual FPInterfaceDesc* GetDesc();
};

static DAOImportFunctionPublish theDAOImportFunctionPublish(
   DAOImport_INTERFACE_ID, _T("DAOImportProp"), -1/*IDS_OPS*/, 0, FP_CORE,
   properties,
      IMP_PB_SHORTDESC, IMP_PB_SHORTDESC+IMP_PB_FN_SET_OFFSET, _T("shortDescription"), 0, TYPE_STRING,
      IMP_PB_FLIPUV, IMP_PB_FLIPUV+IMP_PB_FN_SET_OFFSET,       _T("flipUV"), 0, TYPE_BOOL,
      IMP_PB_AUTOSMOOTH, IMP_PB_AUTOSMOOTH+IMP_PB_FN_SET_OFFSET,   _T("autoSmooth"), 0, TYPE_BOOL,
      IMP_PB_AUTOSMOOTH_ANGLE, IMP_PB_AUTOSMOOTH_ANGLE+IMP_PB_FN_SET_OFFSET,   _T("autoSmoothAngle"), 0, TYPE_FLOAT,
      IMP_PB_RENDERTEX, IMP_PB_RENDERTEX+IMP_PB_FN_SET_OFFSET,   _T("renderTextures"), 0, TYPE_BOOL,
      IMP_PB_TRIM_WALKMESH, IMP_PB_TRIM_WALKMESH+IMP_PB_FN_SET_OFFSET,   _T("trimWalkmesh"), 0, TYPE_BOOL,
      IMP_PB_WALKMESH_LIMIT, IMP_PB_WALKMESH_LIMIT+IMP_PB_FN_SET_OFFSET,   _T("walkmeshLimit"), 0, TYPE_FLOAT,

      IMP_PB_ENABLE_COLLISION, IMP_PB_ENABLE_COLLISION+IMP_PB_FN_SET_OFFSET,   _T("enableCollision"), 0, TYPE_BOOL,
      IMP_PB_ENABLE_WALKMESH, IMP_PB_ENABLE_WALKMESH+IMP_PB_FN_SET_OFFSET,   _T("enableWalkmesh"), 0, TYPE_BOOL,
      IMP_PB_ENABLE_HOOKS, IMP_PB_ENABLE_HOOKS+IMP_PB_FN_SET_OFFSET,   _T("enableHooks"), 0, TYPE_BOOL,
      IMP_PB_ENABLE_HAIRHELM, IMP_PB_ENABLE_HAIRHELM+IMP_PB_FN_SET_OFFSET,   _T("enableHairOrHelm"), 0, TYPE_BOOL,
      IMP_PB_ENABLE_BONES, IMP_PB_ENABLE_BONES+IMP_PB_FN_SET_OFFSET,   _T("enableBones"), 0, TYPE_BOOL,
      IMP_PB_BONEMATCH, IMP_PB_BONEMATCH+IMP_PB_FN_SET_OFFSET,   _T("boneMatch"), 0, TYPE_STRING,
      IMP_PB_WALK_OPACITY, IMP_PB_WALK_OPACITY+IMP_PB_FN_SET_OFFSET,   _T("walkmeshOpacity"), 0, TYPE_FLOAT,
      IMP_PB_FACEBONEMATCH, IMP_PB_FACEBONEMATCH+IMP_PB_FN_SET_OFFSET,   _T("faceBoneMatch"), 0, TYPE_STRING,
	  IMP_PB_ENABLE_SCALE, IMP_PB_ENABLE_SCALE+IMP_PB_FN_SET_OFFSET,   _T("enableScale"), 0, TYPE_BOOL,
	  IMP_PB_SCALE_FACTOR, IMP_PB_SCALE_FACTOR+IMP_PB_FN_SET_OFFSET,   _T("scaleFactor"), 0, TYPE_FLOAT,
	  IMP_PB_ENABLE_ANIMATION, IMP_PB_ENABLE_ANIMATION+IMP_PB_FN_SET_OFFSET,   _T("enableAnimation"), 0, TYPE_BOOL,
	  IMP_PB_NORMALBUMP, IMP_PB_NORMALBUMP+IMP_PB_FN_SET_OFFSET,   _T("normalBump"), 0, TYPE_BOOL,
	  IMP_PB_ENABLE_WELD, IMP_PB_ENABLE_WELD+IMP_PB_FN_SET_OFFSET,   _T("enableWeld"), 0, TYPE_BOOL,
	  IMP_PB_WELD_LIMIT, IMP_PB_WELD_LIMIT+IMP_PB_FN_SET_OFFSET,   _T("weldLimit"), 0, TYPE_FLOAT,
   end
);

FPInterfaceDesc* DAOImportFunctionPublish::GetDesc()
{
   return &theDAOImportFunctionPublish;
}

#endif


#pragma region ConsoleLogger
class ConsoleLogger : public ILogListener
{
public:
	void Message( KFbxLogLevel level, const TCHAR* strMessage )
	{
		switch (level)
		{
		case LOG_ERROR:   OutputDebugString(FormatText( "ERROR:   %s\n", strMessage ) ); break;
		case LOG_WARN:    OutputDebugString(FormatText( "WARN:    %s\n", strMessage ) ); break;
		case LOG_INFO:    OutputDebugString(FormatText( "INFO:    %s\n", strMessage ) ); break;
		case LOG_DEBUG:   OutputDebugString(FormatText( "DEBUG:   %s\n", strMessage ) ); break;
		case LOG_VERBOSE: OutputDebugString(FormatText( "VERBOSE: %s\n", strMessage ) ); break;
		}
	}
};
#pragma endregion

extern "C" int GetFbxReaderFormats(std::vector<_tstring> &exts);

//--- DAOImport -------------------------------------------------------
DAOImport::DAOImport()
{
	GetFbxReaderFormats(Exts);
}

DAOImport::~DAOImport() 
{

}

int DAOImport::ExtCount()
{
	return int(Exts.size());
}

const TCHAR *DAOImport::Ext(int n)
{
   if (n<0 || n >= int(Exts.size()))
      return _T("");	
   return Exts[n].c_str();
}

const TCHAR *DAOImport::LongDesc()
{
	return _T("Dragon Age: Origins FBX Importer");
}
	
const TCHAR *DAOImport::ShortDesc() 
{			
   return "DAO Importer";
}

const TCHAR *DAOImport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("tazpn");
}

const TCHAR *DAOImport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T(
      "Copyright (c) 2009\n" 
      "All rights reserved."
      );
}

const TCHAR *DAOImport::OtherMessage1() 
{		
	return _T(
      "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
      "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
      "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS\n"
      "FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE\n"
      "COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,\n"
      "INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,\n"
      "BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
      "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER\n"
      "CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT\n"
      "LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN\n"
      "ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE\n"
      "POSSIBILITY OF SUCH DAMAGE.\n"
      );
}

const TCHAR *DAOImport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int DAOImport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 1 * 100;
}

void DAOImport::ShowAbout(HWND hWnd)
{			
	// Optional
}


int DAOImport::DoImport(const TCHAR *filename,ImpInterface *i, Interface *gi, BOOL suppressPrompts)
{
	bool ok = true;
	try 
	{
		HoldSuspend myHold(TRUE);

		ReadEnvironment("Environment", INIAccessor.iniFileName, Environment);

		LPCTSTR ext = PathFindExtension(filename);
		FBXImporter importer(filename, i, gi, suppressPrompts);
		if (!importer.isValid())
			return FALSE;

#if VERSION_3DSMAX < ((5000<<16)+(15<<8)+0) // Version 4.2 and lower
		suppressPrompts = TRUE;
#endif

		if (!suppressPrompts)
		{
			ok = CreateModalParamMap2(
				0, 
				param_blk.class_params,
				0,
				hInstance,
				MAKEINTRESOURCE(IDD_FBX_PANEL),
				GetActiveWindow(),
				GetMDImportDlgProc()
				) ? true : false;
		}
		if (ok)
		{
			ok = importer.DoImport();
		}
	}
	//catch (RuntimeError &e)
	//{
	//   //StringStream s; e.sprin1(&s);
	//   //MessageBox(NULL, s.to_string(), "Import Error", MB_OK);
	//}
	catch (std::exception &e)
	{
		MessageBox(NULL, e.what(), "Import Error", MB_OK);
	}
	catch (...)
	{
		MessageBox(NULL, "Unknown error.", "Import Error", MB_OK);
	}
	return TRUE;
}

class FBXImportDlgProc : public ParamMap2UserDlgProc {
public:
   FBXImportDlgProc() {}
   INT_PTR DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
   void DeleteThis() { }

   //--- ParamDlgProc --------------------------------
   void TurnSpinner(HWND hWnd,int SpinNum,BOOL ison)
   {	
      ISpinnerControl *spin2 = GetISpinner(GetDlgItem(hWnd,SpinNum));
      if (ison) spin2->Enable();else spin2->Disable();
      ReleaseISpinner(spin2);
   };
   void Update(TimeValue t);

   HWND hDlg;
   ISpinnerControl *walkLimit;
   ISpinnerControl *scaleFactor;
   ISpinnerControl *weldLimit;
};
extern ParamMap2UserDlgProc* GetMDImportDlgProc()
{
   static FBXImportDlgProc theFBXImportDlgProc;
   return &theFBXImportDlgProc;
}

INT_PTR FBXImportDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
   switch(msg) 
   {
   case WM_INITDIALOG:
      {
         hDlg = hWnd;
         // Append file version to dialog
         TSTR version = GetFileVersion(NULL);
         if (!version.isNull()) {
            char buffer[256];
            GetWindowText(hWnd, buffer, _countof(buffer));
            _tcscat(buffer, TEXT(" "));
            _tcscat(buffer, version);
            SetWindowText(hWnd, buffer);
         }
         CenterWindow(hWnd,GetParent(hWnd));

         CheckDlgButton(hWnd, IDC_CHK_FLIP_UV, map->GetParamBlock()->GetPB2Value(IMP_PB_FLIPUV).i);
         CheckDlgButton(hWnd, IDC_CHK_AUTOSMOOTH, map->GetParamBlock()->GetPB2Value(IMP_PB_AUTOSMOOTH).i);
         CheckDlgButton(hWnd, IDC_CHK_SHOW_TEX, map->GetParamBlock()->GetPB2Value(IMP_PB_RENDERTEX).i);
         CheckDlgButton(hWnd, IDC_CHK_COLLISON, map->GetParamBlock()->GetPB2Value(IMP_PB_ENABLE_COLLISION).i);
         CheckDlgButton(hWnd, IDC_CHK_WALKMESH, map->GetParamBlock()->GetPB2Value(IMP_PB_ENABLE_WALKMESH).i);
         CheckDlgButton(hWnd, IDC_CHK_HOOK, map->GetParamBlock()->GetPB2Value(IMP_PB_ENABLE_HOOKS).i);
         CheckDlgButton(hWnd, IDC_CHK_HAIR_HELM, map->GetParamBlock()->GetPB2Value(IMP_PB_ENABLE_HAIRHELM).i);
         CheckDlgButton(hWnd, IDC_CHK_BONES, map->GetParamBlock()->GetPB2Value(IMP_PB_ENABLE_BONES).i);
		 CheckDlgButton(hWnd, IDC_CHK_NORMALBUMP, map->GetParamBlock()->GetPB2Value(IMP_PB_NORMALBUMP).i);
		 //CheckDlgButton(hWnd, IDC_CHK_ANIMATION, map->GetParamBlock()->GetPB2Value(IMP_PB_ENABLE_ANIMATION).i);

         CheckDlgButton(hWnd, IDC_CHK_LIMIT_WALKMESH, map->GetParamBlock()->GetPB2Value(IMP_PB_TRIM_WALKMESH).i);

         EnableWindow(GetDlgItem(hWnd, IDC_ED_WALK_LIMIT), TRUE);
         walkLimit = GetISpinner( GetDlgItem(hWnd, IDC_SP_WALK_LIMIT) );
         walkLimit->SetLimits(-1000000.0f, 0.0f, TRUE);
         walkLimit->SetAutoScale(TRUE);
         walkLimit->SetResetValue( -5.0f );
         walkLimit->LinkToEdit( GetDlgItem(hWnd, IDC_ED_WALK_LIMIT), EDITTYPE_POS_FLOAT );
         walkLimit->SetValue(map->GetParamBlock()->GetPB2Value(IMP_PB_WALKMESH_LIMIT).f, 0);

		 CheckDlgButton(hWnd, IDC_CHK_SCALE_FACTOR, map->GetParamBlock()->GetPB2Value(IMP_PB_SCALE_FACTOR).i);

		 EnableWindow(GetDlgItem(hWnd, IDC_ED_SCALE_FACTOR), TRUE);
		 scaleFactor = GetISpinner( GetDlgItem(hWnd, IDC_SP_SCALE_FACTOR) );
		 scaleFactor->SetLimits(0.0f, 100000.0f, TRUE);
		 scaleFactor->SetAutoScale(TRUE);
		 scaleFactor->SetResetValue( 1.0f );
		 scaleFactor->LinkToEdit( GetDlgItem(hWnd, IDC_ED_SCALE_FACTOR), EDITTYPE_POS_FLOAT );
		 scaleFactor->SetValue(map->GetParamBlock()->GetPB2Value(IMP_PB_SCALE_FACTOR).f, 0);

		 CheckDlgButton(hWnd, IDC_CHK_WELD, map->GetParamBlock()->GetPB2Value(IMP_PB_WELD_LIMIT).i);

		 EnableWindow(GetDlgItem(hWnd, IDC_ED_WELD_LIMIT), TRUE);
		 weldLimit = GetISpinner( GetDlgItem(hWnd, IDC_SP_WELD_LIMIT) );
		 weldLimit->SetAutoScale(FALSE);
		 weldLimit->SetLimits(0.00001f, 1.0f, FALSE);
		 weldLimit->SetScale(0.001f);
		 weldLimit->SetResetValue( 0.001f );
		 weldLimit->LinkToEdit( GetDlgItem(hWnd, IDC_ED_WELD_LIMIT), EDITTYPE_POS_FLOAT );
		 weldLimit->SetValue(map->GetParamBlock()->GetPB2Value(IMP_PB_WELD_LIMIT).f, 0);
		 

         Update(t);
      }
      return TRUE;

   case WM_NCDESTROY:
      if (walkLimit) 
      {
         ReleaseISpinner(walkLimit);
         walkLimit = NULL;
      }
	  if (scaleFactor)
	  {
		  ReleaseISpinner(scaleFactor);
		  scaleFactor = NULL;
	  }
	  if (weldLimit)
	  {
		  ReleaseISpinner(weldLimit);
		  weldLimit = NULL;
	  }
      break;

   case WM_COMMAND : 
      {
         if (HIWORD(wParam) == BN_CLICKED)
         {
            switch (LOWORD(wParam))
            {
            case IDOK:
               map->GetParamBlock()->SetValue(IMP_PB_FLIPUV, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_FLIP_UV));
               map->GetParamBlock()->SetValue(IMP_PB_AUTOSMOOTH, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_AUTOSMOOTH));
               map->GetParamBlock()->SetValue(IMP_PB_RENDERTEX, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_SHOW_TEX));
               map->GetParamBlock()->SetValue(IMP_PB_ENABLE_COLLISION, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_COLLISON));
               map->GetParamBlock()->SetValue(IMP_PB_ENABLE_WALKMESH, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_WALKMESH));
               map->GetParamBlock()->SetValue(IMP_PB_ENABLE_HOOKS, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_HOOK));
               map->GetParamBlock()->SetValue(IMP_PB_ENABLE_HAIRHELM, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_HAIR_HELM));
               map->GetParamBlock()->SetValue(IMP_PB_ENABLE_BONES, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_BONES));
               map->GetParamBlock()->SetValue(IMP_PB_TRIM_WALKMESH, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_LIMIT_WALKMESH));
			   //map->GetParamBlock()->SetValue(IMP_PB_ENABLE_ANIMATION, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_ANIMATION));
			   map->GetParamBlock()->SetValue(IMP_PB_ENABLE_SCALE, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_SCALE_FACTOR));
			   map->GetParamBlock()->SetValue(IMP_PB_NORMALBUMP, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_NORMALBUMP));
			   map->GetParamBlock()->SetValue(IMP_PB_ENABLE_WELD, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_WELD));

               if (walkLimit) map->GetParamBlock()->SetValue(IMP_PB_WALKMESH_LIMIT, t, walkLimit->GetFVal());
			   if (scaleFactor) map->GetParamBlock()->SetValue(IMP_PB_SCALE_FACTOR, t, scaleFactor->GetFVal());
			   if (weldLimit) map->GetParamBlock()->SetValue(IMP_PB_WELD_LIMIT, t, weldLimit->GetFVal());
               break;
            }

            Update(t);
         }
      }
      break;
   }
   return FALSE;
}

void FBXImportDlgProc::Update(TimeValue t)
{
   BOOL enabled;

   // Handle Walk mesh
   enabled = IsDlgButtonChecked(hDlg, IDC_CHK_WALKMESH);
   EnableWindow(GetDlgItem(hDlg, IDC_CHK_LIMIT_WALKMESH), enabled);
   if (enabled) enabled = IsDlgButtonChecked(hDlg, IDC_CHK_LIMIT_WALKMESH);
   EnableWindow(GetDlgItem(hDlg, IDC_ED_WALK_LIMIT), enabled);
   EnableWindow(GetDlgItem(hDlg, IDC_SP_WALK_LIMIT), enabled);

   enabled = IsDlgButtonChecked(hDlg, IDC_CHK_SCALE_FACTOR);
   EnableWindow(GetDlgItem(hDlg, IDC_ED_SCALE_FACTOR), enabled);
   EnableWindow(GetDlgItem(hDlg, IDC_SP_SCALE_FACTOR), enabled);

   enabled = IsDlgButtonChecked(hDlg, IDC_CHK_BONES);
   EnableWindow(GetDlgItem(hDlg, IDC_STC_SKELETON), enabled);

   enabled = IsDlgButtonChecked(hDlg, IDC_CHK_WELD);
   EnableWindow(GetDlgItem(hDlg, IDC_ED_WELD_LIMIT), enabled);
   EnableWindow(GetDlgItem(hDlg, IDC_SP_WELD_LIMIT), enabled);


   //EnableWindow(GetDlgItem(hDlg, IDC_CHK_ANIMATION), enabled);
   
}