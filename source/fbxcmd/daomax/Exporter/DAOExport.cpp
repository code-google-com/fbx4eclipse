/**********************************************************************
 *<
	FILE: DAOExport.cpp

	DESCRIPTION:	Appwizard generated plugin

	HISTORY: 

 *>	Copyright (c) 2009, All Rights Reserved.
 **********************************************************************/
#include "DAOExport.h"
#include "MSHExport.h"
#include "MMHExport.h"
#include <shlwapi.h>

const Class_ID DAOExport_CLASS_ID(0x3D0B6A15, 0x29CC403C);
const Interface_ID DAOExport_INTERFACE_ID(0xFB962361, 0x0E9B4145);

extern ClassDesc2* GetDAOExportDesc();
extern ParamMap2UserDlgProc* GetMDExportDlgProc();

static const char *MSHExport = "MSHExport";
static const char *MSHCommon = "MSHCommon";


class DAOExport : public SceneExport {
	public:

		static HWND hParams;
		
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
      int				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);	// Export file
      BOOL			   SupportsOptions(int ext, DWORD options);
		
		//Constructor/Destructor
		DAOExport();
		~DAOExport();		

      _tstring iniFileName;
};

class DAOExportClassDesc : public ClassDesc2 {
	public:
   DAOExportClassDesc();
	int 			   IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new DAOExport(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return SCENE_EXPORT_CLASS_ID; }
	Class_ID		   ClassID() { return DAOExport_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("TZ_MSH_Exporter"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
};

//--- Parameter map/block descriptors -------------------------------


enum { box_params_panel, };

class ExpPBAccessor : public PBAccessor
{ 
public:
   TSTR iniFileName;
   bool iniFileValid;

   TSTR ShortDesc;
   TSTR Col2Match, Col3Match, WalkMatch, HookMatch, HairMatch, HelmMatch, ColsMatch;
   TSTR MaterialUI;

   ExpPBAccessor() : iniFileValid(false)
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
      case EXP_PB_SHORTDESC:     v.s = ShortDesc = GetIniValue<TSTR>("System", "ShortDescription", v.s); break;
      case EXP_PB_FLIPUV:        v.i = GetIniValue<BOOL>(MSHExport, "FlipUVTextures", v.i); break;
      case EXP_PB_TRIM_WALKMESH: v.i = GetIniValue<BOOL>(MSHCommon, "TrimWalkmesh", v.i); break;
      case EXP_PB_WALKMESH_LIMIT: v.f = GetIniValue<float>(MSHCommon, "WalkmeshLimit", v.f); break;
      case EXP_PB_COL2MATCH:  v.s = Col2Match = GetIniValue<TCHAR*>(MSHExport, "Col2Match", v.s); break;
      case EXP_PB_COL3MATCH:  v.s = Col3Match = GetIniValue<TCHAR*>(MSHExport, "Col3Match", v.s); break;
      case EXP_PB_WALKMATCH:  v.s = WalkMatch = GetIniValue<TCHAR*>(MSHExport, "WalkMatch", v.s); break;
      case EXP_PB_HOOKMATCH:  v.s = HookMatch = GetIniValue<TCHAR*>(MSHExport, "HookMatch", v.s); break;
      case EXP_PB_HAIRMATCH:  v.s = HairMatch = GetIniValue<TCHAR*>(MSHExport, "HairMatch", v.s); break;
      case EXP_PB_HELMMATCH:  v.s = HelmMatch = GetIniValue<TCHAR*>(MSHExport, "HelmMatch", v.s); break;
      case EXP_PB_COLSMATCH:  v.s = ColsMatch = GetIniValue<TCHAR*>(MSHExport, "ColsMatch", v.s); break;
      case EXP_PB_SCALE_FACTOR: v.f = GetIniValue<float>(MSHExport, "ScaleFactor", v.f); break;
      case EXP_PB_TANGENT_ALGORITHM: v.i = GetIniValue<int>(MSHExport, "TangentSpaceAlgorithm", v.i); break;
      case EXP_PB_ENABLE_SIMPLE_EXPORT: v.i = GetIniValue<int>(MSHExport, "EnableSimpleExport", v.i); break;
      case EXP_PB_DEFAULT_MATERIAL_FLAG: v.s = MaterialUI = GetIniValue<TSTR>(MSHExport, "DefaultMaterialUIFlag", v.s); break;
      case EXP_PB_EXPORTHIDDEN: v.i = GetIniValue<BOOL>(MSHExport, "ExportHidden", v.i); break;
      }
   }

   // set from v
   void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t) 
   { 
      switch (id)
      {
      case EXP_PB_SHORTDESC:  SetIniValue<TCHAR*>("System", "ShortDescription", v.s); break;
      case EXP_PB_FLIPUV:     SetIniValue<BOOL>(MSHExport, "FlipUVTextures", v.i); break;
      case EXP_PB_TRIM_WALKMESH:  SetIniValue<BOOL>(MSHCommon, "TrimWalkmesh", v.i); break;
      case EXP_PB_WALKMESH_LIMIT: SetIniValue<float>(MSHCommon, "WalkmeshLimit", v.f); break;
      case EXP_PB_COL2MATCH:  SetIniValue<TCHAR*>(MSHExport, "Col2Match", v.s); break;
      case EXP_PB_COL3MATCH:  SetIniValue<TCHAR*>(MSHExport, "Col3Match", v.s); break;
      case EXP_PB_WALKMATCH:  SetIniValue<TCHAR*>(MSHExport, "WalkMatch", v.s); break;
      case EXP_PB_HOOKMATCH:  SetIniValue<TCHAR*>(MSHExport, "HookMatch", v.s); break;
      case EXP_PB_HAIRMATCH:  SetIniValue<TCHAR*>(MSHExport, "HairMatch", v.s); break;
      case EXP_PB_HELMMATCH:  SetIniValue<TCHAR*>(MSHExport, "HelmMatch", v.s); break;
      case EXP_PB_COLSMATCH:  SetIniValue<TCHAR*>(MSHExport, "ColsMatch", v.s); break;
      case EXP_PB_SCALE_FACTOR: SetIniValue<float>(MSHExport, "ScaleFactor", v.f); break;
      case EXP_PB_TANGENT_ALGORITHM: SetIniValue<int>(MSHExport, "TangentSpaceAlgorithm", v.i); break;
      case EXP_PB_ENABLE_SIMPLE_EXPORT: SetIniValue<int>(MSHExport, "EnableSimpleExport", v.i); break;
      case EXP_PB_DEFAULT_MATERIAL_FLAG: SetIniValue<TCHAR*>(MSHExport, "DefaultMaterialUIFlag", v.s); break;
      case EXP_PB_EXPORTHIDDEN: SetIniValue<BOOL>(MSHExport, "ExportHidden", v.i); break;
      }
   }
};

static ExpPBAccessor ExpINIAccessor;

static ParamBlockDesc2 param_blk ( 
   Export_params, _T("parameters"),  0, NULL, P_CLASS_PARAMS | P_AUTO_UI | P_MULTIMAP,
   //rollout
   1,
   Export_params,	IDD_MSH_PANEL,  IDS_PARAMS, 0, 0, NULL, 
   // params
   EXP_PB_SHORTDESC, _T("shortDescription"),  TYPE_STRING, 0, IDS_LIBDESCRIPTION,
      p_default,  "DAO MSH Exporter",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_FLIPUV, _T("flipUV"), TYPE_BOOL, 0,	IDS_FLIPUV,
      p_default,  TRUE,
      p_ui, 		export_msh_params,    TYPE_CHECKBUTTON,    IDC_CHK_FLIP_UV, 
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_TRIM_WALKMESH, _T("trimWalkmesh"), TYPE_BOOL, 0,	IDS_TRIM_WALKMESH,
      p_default,  TRUE,
      p_ui, 		export_msh_params,    TYPE_CHECKBUTTON,    IDC_CHK_LIMIT_WALKMESH, 
      p_accessor,	&ExpINIAccessor,
      p_enable_ctrls, 1, EXP_PB_WALKMESH_LIMIT,
      end,

   EXP_PB_WALKMESH_LIMIT, _T("walkmeshLimit"), TYPE_FLOAT, 0, IDS_WALKMESH_LIMIT,
      p_default,  -50.0f,
      p_accessor,	&ExpINIAccessor,
      p_range,    -1000000.0, 0.0,
      p_ui,       export_msh_params,    TYPE_SPINNER, EDITTYPE_UNIVERSE,  IDC_ED_WALK_LIMIT, IDC_SP_WALK_LIMIT, SPIN_AUTOSCALE,
      end,

   EXP_PB_COL2MATCH, _T("col2Match"),  TYPE_STRING, 0, IDS_COL2MATCH,
      p_default,  "*_C2",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_COL3MATCH, _T("col3Match"),  TYPE_STRING, 0, IDS_COL3MATCH,
      p_default,  "*_C3",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_WALKMATCH, _T("walkMatch"),  TYPE_STRING, 0, IDS_WALKMATCH,
      p_default,  "*_W",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_HOOKMATCH, _T("hookMatch"),  TYPE_STRING, 0, IDS_HOOKMATCH,
      p_default,  "HP_*",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_HAIRMATCH, _T("hairMatch"),  TYPE_STRING, 0, IDS_HAIRMATCH,
      p_default,  "*_Hair*",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_HELMMATCH, _T("helmMatch"),  TYPE_STRING, 0, IDS_HELMMATCH,
      p_default,  "*_Helm*",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_SCALE_FACTOR, _T("scaleFactor"), TYPE_FLOAT, 0,	IDS_SCALE_FACTOR,
      p_default,  1.0f,
      p_accessor,	&ExpINIAccessor,
      p_range,    0.0, 1000.0,
      p_ui,       export_msh_params,    TYPE_SPINNER, EDITTYPE_UNIVERSE,  IDC_ED_SCALE_FACTOR, IDC_SP_SCALE_FACTOR, SPIN_AUTOSCALE,
      end,

   EXP_PB_TANGENT_ALGORITHM, _T("tangentSpace"), TYPE_INT, 0,	IDS_TANGENT_SPACE,
      p_default,  0,
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_ENABLE_SIMPLE_EXPORT, _T("enableSimpleExport"), TYPE_INT, 0,	IDS_DEBUGEXPORT,
      p_default,  0,
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_DEFAULT_MATERIAL_FLAG, _T("defaultMaterialFlag"),  TYPE_STRING, 0, IDS_DEFAULTMATERIALUIFLAG,
      p_default,  "None",
      p_accessor,	&ExpINIAccessor,
      end,

   EXP_PB_COLSMATCH, _T("colsMatch"),  TYPE_STRING, 0, IDS_COLSMATCH,
      p_default,  "COLS*",
      p_accessor,	&ExpINIAccessor,
      end,


   EXP_PB_EXPORTHIDDEN, _T("exportHidden"), TYPE_BOOL, 0,	IDS_EXPORTHIDDEN,
      p_default,  TRUE,
      p_ui, 		export_msh_params,    TYPE_CHECKBUTTON,    IDC_CHK_EXPORTHIDDEN, 
      p_accessor,	&ExpINIAccessor,
      end,


   end
    );

// bug in pb desc? forces us to declare class desc after param block
static DAOExportClassDesc DAOExportDesc;
ClassDesc2* GetDAOExportDesc() { return &DAOExportDesc; }

DAOExportClassDesc::DAOExportClassDesc() {
   param_blk.SetClassDesc(this);
   //MakeAutoParamBlocks(this);
}


#if VERSION_3DSMAX >= ((5000<<16)+(15<<8)+0) // Version 5+

class DAOExportFunctionPublish : public FPStaticInterface {
public:
   DECLARE_DESCRIPTOR(DAOExportFunctionPublish);
public:
   FPStatus _dispatch_fn(FunctionID fid, TimeValue t, FPValue& result, FPParams* p)
   {
      FPStatus status = FPS_OK;
      if (fid < EXP_PB_MAX_BLOCKS)
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
      else if (fid > EXP_PB_FN_SET_OFFSET && fid <= EXP_PB_FN_SET_OFFSET+EXP_PB_MAX_BLOCKS)
      {
         ParamID pid = ParamID(fid-EXP_PB_FN_SET_OFFSET);
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

static DAOExportFunctionPublish theDAOExportFunctionPublish(
   DAOExport_INTERFACE_ID, _T("DAOExportProp"), -1/*IDS_OPS*/, 0, FP_CORE,
   properties,
      EXP_PB_SHORTDESC, EXP_PB_SHORTDESC+EXP_PB_FN_SET_OFFSET, _T("shortDescription"), 0, TYPE_STRING,
      EXP_PB_FLIPUV, EXP_PB_FLIPUV+EXP_PB_FN_SET_OFFSET,       _T("flipUV"), 0, TYPE_BOOL,
      EXP_PB_TRIM_WALKMESH, EXP_PB_TRIM_WALKMESH+EXP_PB_FN_SET_OFFSET,   _T("trimWalkmesh"), 0, TYPE_BOOL,
      EXP_PB_WALKMESH_LIMIT, EXP_PB_WALKMESH_LIMIT+EXP_PB_FN_SET_OFFSET,   _T("walkmeshLimit"), 0, TYPE_FLOAT,
      EXP_PB_COL2MATCH, EXP_PB_COL2MATCH+EXP_PB_FN_SET_OFFSET,   _T("col2Match"), 0, TYPE_STRING,
      EXP_PB_COL3MATCH, EXP_PB_COL3MATCH+EXP_PB_FN_SET_OFFSET,   _T("col3Match"), 0, TYPE_STRING,
      EXP_PB_WALKMATCH, EXP_PB_WALKMATCH+EXP_PB_FN_SET_OFFSET,   _T("walkMatch"), 0, TYPE_STRING,
      EXP_PB_HOOKMATCH, EXP_PB_HOOKMATCH+EXP_PB_FN_SET_OFFSET,   _T("hookMatch"), 0, TYPE_STRING,
      EXP_PB_HAIRMATCH, EXP_PB_HAIRMATCH+EXP_PB_FN_SET_OFFSET,   _T("hairMatch"), 0, TYPE_STRING,
      EXP_PB_HELMMATCH, EXP_PB_HELMMATCH+EXP_PB_FN_SET_OFFSET,   _T("helmMatch"), 0, TYPE_STRING,
      EXP_PB_SCALE_FACTOR, EXP_PB_SCALE_FACTOR+EXP_PB_FN_SET_OFFSET,   _T("scaleFactor"), 0, TYPE_FLOAT,
      EXP_PB_TANGENT_ALGORITHM, EXP_PB_TANGENT_ALGORITHM+EXP_PB_FN_SET_OFFSET,   _T("tangentSpace"), 0, TYPE_INT,
      EXP_PB_ENABLE_SIMPLE_EXPORT, EXP_PB_ENABLE_SIMPLE_EXPORT+EXP_PB_FN_SET_OFFSET,   _T("enableSimpleExport"), 0, TYPE_INT,
      EXP_PB_DEFAULT_MATERIAL_FLAG, EXP_PB_DEFAULT_MATERIAL_FLAG+EXP_PB_FN_SET_OFFSET,   _T("defaultMaterialFlag"), 0, TYPE_STRING,
      EXP_PB_COLSMATCH, EXP_PB_COLSMATCH+EXP_PB_FN_SET_OFFSET,   _T("colsMatch"), 0, TYPE_STRING,
      EXP_PB_EXPORTHIDDEN, EXP_PB_EXPORTHIDDEN+EXP_PB_FN_SET_OFFSET,       _T("exportHidden"), 0, TYPE_BOOL,
   end
);

FPInterfaceDesc* DAOExportFunctionPublish::GetDesc()
{
   return &theDAOExportFunctionPublish;
}
#endif

//--- DAOExport -------------------------------------------------------
DAOExport::DAOExport()
{
}

DAOExport::~DAOExport() 
{

}

int DAOExport::ExtCount()
{
	return 2;
}

const TCHAR *DAOExport::Ext(int n)
{		
   switch (n)
   {
   case 0: return _T("MSH");
   case 1: return _T("MMH");
   }
   return _T("");	
}

BOOL DAOExport::SupportsOptions(int ext, DWORD options)
{
   switch (ext)
   {
   case 0: 
      return (options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
   }
   return FALSE;
}


const TCHAR *DAOExport::LongDesc()
{
	//TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
	return _T("Neverwinter Nights 2 DAO Exporter");
}
	
const TCHAR *DAOExport::ShortDesc() 
{			
	//TODO: Return short ASCII description (i.e. "Targa")
	return param_blk.GetStr(EXP_PB_SHORTDESC);
}

const TCHAR *DAOExport::AuthorName()
{			
	//TODO: Return ASCII Author name
	return _T("tazpn");
}

const TCHAR *DAOExport::CopyrightMessage() 
{	
	// Return ASCII Copyright message
	return _T(
      "Copyright (c) 2009\n" 
      "All rights reserved."
      );
}

const TCHAR *DAOExport::OtherMessage1() 
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

const TCHAR *DAOExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int DAOExport::Version()
{				
	//TODO: Return Version number * 100 (i.e. v3.01 = 301)
	return 1 * 100;
}

void DAOExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

static DWORD WINAPI dummyProgress(LPVOID arg) {
   return(0);
}

int DAOExport::DoExport(const TCHAR *filename, ExpInterface *i,Interface *gi, BOOL suppressPrompts, DWORD options)
{
   bool ok = true;

   gi->ProgressStart(FormatText("Exporting '%s'...", PathFindFileName(filename)), TRUE, dummyProgress, 0);
   gi->PushPrompt(FormatText("Exporting '%s' as DAO ...", filename));

   try 
   {
      HoldSuspend myHold(TRUE);

      LPCTSTR ext = PathFindExtension(filename);
      if (_tcsicmp(ext, ".MSH") == 0)
      {
         MSHExporter Exporter(filename, i, gi, suppressPrompts, options);
         if (!Exporter.isValid())
            return FALSE;
         // Disable dialog for now
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
               MAKEINTRESOURCE(IDD_MSH_PANEL),
               GetActiveWindow(),
               GetMDExportDlgProc()
               );
         }
         if (ok)
         {
            ok = Exporter.DoExport();
         }
      }
	  else if (_tcsicmp(ext, ".MMH") == 0)
	  {
		  MMHExporter Exporter(filename, i, gi, suppressPrompts, options);
		  if (!Exporter.isValid())
			  return FALSE;
		  ok = Exporter.DoExport();
	  }
   }
   //catch (RuntimeError &e)
   //{
   //   StringStream s; e.sprin1(&s);
   //   MessageBox(NULL, s.to_string(), "Export Error", MB_OK);
   //}
   catch (std::exception &e)
   {
      if (!suppressPrompts)
         MessageBox(NULL, e.what(), "Export Error", MB_OK);
   }
   catch (...)
   {
      if (!suppressPrompts)
         MessageBox(NULL, "Unknown error.", "Export Error", MB_OK);
   }
   gi->PopPrompt();
   gi->ProgressEnd();
   return TRUE;
}

class MSHExportDlgProc : public ParamMap2UserDlgProc {
public:
   MSHExportDlgProc() {}
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
};
extern ParamMap2UserDlgProc* GetMDExportDlgProc()
{
   static MSHExportDlgProc theMSHExportDlgProc;
   return &theMSHExportDlgProc;
}

INT_PTR MSHExportDlgProc::DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
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

         CheckDlgButton(hWnd, IDC_CHK_FLIP_UV, map->GetParamBlock()->GetPB2Value(EXP_PB_FLIPUV).i);
         CheckDlgButton(hWnd, IDC_CHK_EXPORTHIDDEN, map->GetParamBlock()->GetPB2Value(EXP_PB_EXPORTHIDDEN).i);

         CheckDlgButton(hWnd, IDC_CHK_LIMIT_WALKMESH, map->GetParamBlock()->GetPB2Value(EXP_PB_TRIM_WALKMESH).i);
         EnableWindow(GetDlgItem(hWnd, IDC_ED_WALK_LIMIT), TRUE);
         walkLimit = GetISpinner( GetDlgItem(hWnd, IDC_SP_WALK_LIMIT) );
         walkLimit->SetLimits(-1000000.0f, 0.0f, TRUE);
         walkLimit->SetAutoScale(TRUE);
         walkLimit->SetResetValue( -5.0f );
         walkLimit->LinkToEdit( GetDlgItem(hWnd, IDC_ED_WALK_LIMIT), EDITTYPE_POS_FLOAT );
         walkLimit->SetValue(map->GetParamBlock()->GetPB2Value(EXP_PB_WALKMESH_LIMIT).f, 0);

         scaleFactor = GetISpinner( GetDlgItem(hWnd, IDC_SP_SCALE_FACTOR) );
         scaleFactor->SetLimits(0.0f, 1000000.0f, TRUE);
         scaleFactor->SetAutoScale(TRUE);
         scaleFactor->SetResetValue( 100.0f );
         scaleFactor->LinkToEdit( GetDlgItem(hWnd, IDC_ED_SCALE_FACTOR), EDITTYPE_POS_FLOAT );
         scaleFactor->SetValue(map->GetParamBlock()->GetPB2Value(EXP_PB_SCALE_FACTOR).f, 0);

         int idx = map->GetParamBlock()->GetPB2Value(EXP_PB_TANGENT_ALGORITHM).i;
         SendDlgItemMessage(hWnd, IDC_CBO_TANGENTSPACE, CB_ADDSTRING, 0, LPARAM("Obsidian"));
         SendDlgItemMessage(hWnd, IDC_CBO_TANGENTSPACE, CB_ADDSTRING, 1, LPARAM("m4444x"));
         SendDlgItemMessage(hWnd, IDC_CBO_TANGENTSPACE, CB_ADDSTRING, 1, LPARAM("Zylch000"));
         SendDlgItemMessage(hWnd, IDC_CBO_TANGENTSPACE, CB_SETCURSEL, WPARAM(idx), LPARAM(0));
         
         Update(t);
      }
      return TRUE;

   case WM_NCDESTROY:
      if (walkLimit) {
         ReleaseISpinner(walkLimit);
         walkLimit = NULL;
      }
      if (scaleFactor) {
         ReleaseISpinner(scaleFactor);
         scaleFactor = NULL;
      }
      break;

   case WM_COMMAND : 
      {
         if (HIWORD(wParam) == BN_CLICKED)
         {
            switch (LOWORD(wParam))
            {
            case IDOK:
               map->GetParamBlock()->SetValue(EXP_PB_FLIPUV, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_FLIP_UV));
               map->GetParamBlock()->SetValue(EXP_PB_EXPORTHIDDEN, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_EXPORTHIDDEN));
               map->GetParamBlock()->SetValue(EXP_PB_TRIM_WALKMESH, t, (BOOL)IsDlgButtonChecked(hWnd, IDC_CHK_LIMIT_WALKMESH));
               if (walkLimit) map->GetParamBlock()->SetValue(EXP_PB_WALKMESH_LIMIT, t, walkLimit->GetFVal());
               if (scaleFactor) map->GetParamBlock()->SetValue(EXP_PB_SCALE_FACTOR, t, scaleFactor->GetFVal());

               int idx = SendDlgItemMessage(hWnd, IDC_CBO_TANGENTSPACE, CB_GETCURSEL, 0, 0);
               if (idx < 0) idx = 0;
               map->GetParamBlock()->SetValue(EXP_PB_TANGENT_ALGORITHM, t, idx);
               break;
            }
            //switch (LOWORD(wParam))
            //{
            //case IDC_LBL_LINK:
            //   ShellExecute(hWnd, "open", imp->webSite, NULL, NULL, SW_SHOWNORMAL);
            //   break;

            //case IDC_LBL_WIKI:
            //   ShellExecute(hWnd, "open", imp->wikiSite, NULL, NULL, SW_SHOWNORMAL);
            //   break;
            //}
            Update(t);
         }
      }
      break;
   }
   return FALSE;
}

void MSHExportDlgProc::Update(TimeValue t)
{
   BOOL enabled;

   // Handle Walk mesh
   enabled = IsDlgButtonChecked(hDlg, IDC_CHK_LIMIT_WALKMESH);
   EnableWindow(GetDlgItem(hDlg, IDC_ED_WALK_LIMIT), enabled);
   EnableWindow(GetDlgItem(hDlg, IDC_SP_WALK_LIMIT), enabled);
}