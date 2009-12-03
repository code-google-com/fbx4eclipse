/**********************************************************************
*<
FILE: NIUtils.cpp

DESCRIPTION:	MSHImporter Utilities

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "pch.h"
#include "niutils.h"
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <malloc.h>
#include <sstream>
#include <modstack.h>
#include <iparamb2.h>
#include <iskin.h>
#include "AppSettings.h"
#ifdef USE_BIPED
#  include <cs/BipedApi.h>
#  include <cs/OurExp.h> 
#endif
#include "maxscrpt\Strings.h"
#include "maxscrpt\Parser.h"

using namespace std;

NameValueCollection Environment;

// Macro to create a dynamically allocated strdup on the stack
#define STRDUPA(p) (_tcscpy((TCHAR*)alloca((_tcslen(p)+1)*sizeof(*p)),p))

static inline float TODEG(float x) { return x * 180.0f / PI; }
static inline float TORAD(float x) { return x * PI / 180.0f; }

inline Point3 TODEG(const Point3& p){
	return Point3(TODEG(p[0]), TODEG(p[1]), TODEG(p[2]));
}

inline Point3 TORAD(const Point3& p){
	return Point3(TORAD(p[0]), TORAD(p[1]), TORAD(p[2]));
}


// routine for parsing white space separated lines.  Handled like command line parameters w.r.t quotes.
static void parse_line (
                        const char *start,
                        char **argv,
                        char *args,
                        int *numargs,
                        int *numchars
                        )
{
   const char NULCHAR    = '\0';
   const char SPACECHAR  = ' ';
   const char TABCHAR    = '\t';
   const char RETURNCHAR = '\r';
   const char LINEFEEDCHAR = '\n';
   const char DQUOTECHAR = '\"';
   const char SLASHCHAR  = '\\';
   const char *p;
   int inquote;                    /* 1 = inside quotes */
   int copychar;                   /* 1 = copy char to *args */
   unsigned numslash;              /* num of backslashes seen */

   *numchars = 0;
   *numargs = 0;                   /* the program name at least */

   p = start;

   inquote = 0;

   /* loop on each argument */
   for(;;) 
   {
      if ( *p ) { while (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR) ++p; }

      if (*p == NULCHAR) break; /* end of args */

      /* scan an argument */
      if (argv)
         *argv++ = args;     /* store ptr to arg */
      ++*numargs;

      /* loop through scanning one argument */
      for (;;) 
      {
         copychar = 1;
         /* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
         2N+1 backslashes + " ==> N backslashes + literal "
         N backslashes ==> N backslashes */
         numslash = 0;
         while (*p == SLASHCHAR) 
         {
            /* count number of backslashes for use below */
            ++p;
            ++numslash;
         }
         if (*p == DQUOTECHAR) 
         {
            /* if 2N backslashes before, start/end quote, otherwise copy literally */
            if (numslash % 2 == 0) {
               if (inquote) {
                  if (p[1] == DQUOTECHAR)
                     p++;    /* Double quote inside quoted string */
                  else        /* skip first quote char and copy second */
                     copychar = 0;
               } else
                  copychar = 0;       /* don't copy quote */

               inquote = !inquote;
            }
            numslash /= 2;          /* divide numslash by two */
         }

         /* copy slashes */
         while (numslash--) {
            if (args)
               *args++ = SLASHCHAR;
            ++*numchars;
         }

         /* if at end of arg, break loop */
         if (*p == NULCHAR || (!inquote && (*p == SPACECHAR || *p == TABCHAR || *p == RETURNCHAR || *p == LINEFEEDCHAR)))
            break;

         /* copy character into argument */
         if (copychar) 
         {
            if (args)
               *args++ = *p;
            ++*numchars;
         }
         ++p;
      }

      /* null-terminate the argument */
      if (args)
         *args++ = NULCHAR;          /* terminate string */
      ++*numchars;
   }
   /* We put one last argument in -- a null ptr */
   if (argv)
      *argv++ = NULL;
   ++*numargs;
}

// Tokenize a string using strtok and return it as a stringlist
stringlist TokenizeCommandLine(LPCTSTR str, bool trim)
{
   stringlist values;
   int nargs = 0, nchars = 0;
   parse_line( str, NULL, NULL, &nargs, &nchars);
   char **largv = (char **)_alloca(nargs * sizeof(TCHAR *) + nchars * sizeof(TCHAR));
   parse_line( str, largv, ((TCHAR*)largv) + nargs * sizeof(TCHAR*), &nargs, &nchars);
   for (int i=0; i<nargs; ++i) {
      LPTSTR p = largv[i];
      if (p == NULL) continue;
      values.push_back(string((trim) ? Trim(p) : p));
   }
   return values;
}

string JoinCommandLine(stringlist args)
{
   std::stringstream str;
   for (stringlist::iterator itr = args.begin(); itr != args.end(); ++itr) {
      if (itr != args.begin()) str << ' ';
      str << (*itr);
   }
   return str.str();
}

//! Renames Max Node if it exists
void RenameNode(Interface *gi, LPCTSTR SrcName, LPCTSTR DstName)
{
   INode *node = gi->GetINodeByName(SrcName);
   if (node != NULL) node->SetName(const_cast<LPTSTR>(DstName));
}

//Point3 TOEULER(const Matrix3 &m)
//{
//   Point3 rv(0.0f, 0.0f, 0.0f);
//   if ( m.GetRow(2)[0] < 1.0 )
//   {
//      if ( m.GetRow(2)[0] > - 1.0 )
//      {
//         rv[2] = atan2( - m.GetRow(1)[0], m.GetRow(0)[0] );
//         rv[1] = asin( m.GetRow(2)[0] );
//         rv[0] = atan2( - m.GetRow(2)[1], m.GetRow(2)[2] );
//      }
//      else
//      {
//         rv[2] = - atan2( - m.GetRow(1)[2], m.GetRow(1)[1] );
//         rv[1] = - PI / 2;
//         rv[0] = 0.0;
//      }
//   }
//   else
//   {
//      rv[2] = atan2( m.GetRow(1)[2], m.GetRow(1)[1] );
//      rv[1] = PI / 2;
//      rv[0] = 0.0;
//   }
//   return rv;
//}

inline Text TOSTRING(const Point3& p) { return FormatText("[%g,%g,%g]", p[0], p[1], p[2]);}
inline Text TOSTRING(float p) { return FormatText("%g", p); }
inline Text TOSTRING(const Quat& q) { Matrix3 m; q.MakeMatrix(m); return TOSTRING( m ); }

//inline TSTR TOSTRING(const Matrix3& m) {
//   return TOSTRING( TODEG( TOEULER(m) ) );
//}

void PosRotScaleNode(INode *n, Matrix3& m3, PosRotScale prs, TimeValue t)
{
   Point3 p = m3.GetTrans();
   Quat q = m3;
   Matrix3 stm = m3 * Inverse(m3);
   //float s = (sqrt(stm.GetRow(0)[0]) + sqrt(stm.GetRow(1)[1]) + sqrt(stm.GetRow(1)[1])) / 3.0f;
   Point3 s(stm.GetRow(0)[0], stm.GetRow(1)[1], stm.GetRow(2)[2]);
   PosRotScaleNode(n, p, q, s, prs, t);
}

// Set Position and Rotation on a standard controller will need to handle bipeds
//   Always in World Transform coordinates
void PosRotScaleNode(INode *n, Point3 p, Quat& q, float s, PosRotScale prs, TimeValue t)
{
   if (Control *c = n->GetTMController()) {
      if (prs & prsRot && q.w == FloatNegINF) prs = PosRotScale(prs & ~prsRot);
      if (prs & prsPos && p.x == FloatNegINF) prs = PosRotScale(prs & ~prsPos);
      if (prs & prsScale && s == FloatNegINF) prs = PosRotScale(prs & ~prsScale);
#ifdef USE_BIPED
      // Bipeds are special.  And will crash if you dont treat them with care
      if ( (c->ClassID() == BIPSLAVE_CONTROL_CLASS_ID) 
         ||(c->ClassID() == BIPBODY_CONTROL_CLASS_ID) 
         ||(c->ClassID() == FOOTPRINT_CLASS_ID))
      {
         ScaleValue sv(Point3(s,s,s));
         // Get the Biped Export Interface from the controller 
         //IBipedExport *BipIface = (IBipedExport *) c->GetInterface(I_BIPINTERFACE);
         IOurBipExport *BipIface = (IOurBipExport *) c->GetInterface(I_OURINTERFACE);
         if (prs & prsScale)
            BipIface->SetBipedScale(sv, t, n);
         if (prs & prsRot)
            BipIface->SetBipedRotation(q, t, n, 0/*???*/);
         if (prs & prsPos)
            BipIface->SetBipedPosition(p, t, n);
         return;
      }
#endif
      PosRotScaleNode(c, p, q, s, prs, t);

//#ifdef _DEBUG
//      static TSTR sEmpty = "<Empty>";
//      TSTR spos = (prs & prsPos) ? TOSTRING(p) : sEmpty;
//      TSTR srot = (prs & prsRot) ? TOSTRING(q) : sEmpty;
//      TSTR sscl = (prs & prsScale) ? TOSTRING(s) : sEmpty;
//      OutputDebugString(FormatText("Transform(%s, %s, %s, %s)\n", n->GetName(), spos.data(), srot.data(), sscl.data()));
//#endif
   }
}

void PosRotScaleNode(INode *n, Point3 p, Quat& q, Point3 s, PosRotScale prs, TimeValue t)
{
	if (Control *c = n->GetTMController()) {
		PosRotScaleNode(c, p, q, s, prs, t);
	}
}

void PosRotScaleNode(Control *c, Point3 p, Quat& q, float s, PosRotScale prs, TimeValue t)
{
	PosRotScaleNode(c, p, q, Point3(s,s,s), prs, t);
}

void PosRotScaleNode(Control *c, Point3 p, Quat& q, Point3 s, PosRotScale prs, TimeValue t)
{
	if (c) {
		if (prs & prsRot && q.w == FloatNegINF) prs = PosRotScale(prs & ~prsRot);
		if (prs & prsPos && p.x == FloatNegINF) prs = PosRotScale(prs & ~prsPos);
		if (prs & prsScale && s[0] == FloatNegINF) prs = PosRotScale(prs & ~prsScale);

		ScaleValue sv(s);
		if (prs & prsScale)
			if (Control *sclCtrl = c->GetScaleController())
				sclCtrl->SetValue(t, &sv, 1, CTRL_ABSOLUTE);
		if (prs & prsRot)
			if (Control *rotCtrl = c->GetRotationController())
				rotCtrl->SetValue(t, &q, 1, CTRL_ABSOLUTE);
		if (prs & prsPos)
			if (Control *posCtrl = c->GetPositionController())
				posCtrl->SetValue(t, &p, 1, CTRL_ABSOLUTE);
	}
}

// Recursively search through directories applying a filter on what to return
template <typename FileMatch>
void BuildFileNameMap(NameValueCollection & collection, const TCHAR *root, const TCHAR *path, FileMatch pred)
{
   TCHAR buffer[MAX_PATH], buffer2[MAX_PATH], search[MAX_PATH];
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;
   ZeroMemory(&FindFileData, sizeof(FindFileData));
   if (path == NULL || path[0] == 0)
      return;
   PathCanonicalize(search, path);
   PathAddBackslash(search);
   _tcscat(search, "*");

   hFind = FindFirstFile(search, &FindFileData);
   if (hFind != INVALID_HANDLE_VALUE) {
      stringlist list;
      for (BOOL ok = TRUE ; ok ; ok = FindNextFile(hFind, &FindFileData)) {
         if (FindFileData.cFileName[0] == '.' || (FindFileData.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM)))
            continue;
         if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            PathCombine(buffer, path, FindFileData.cFileName);
            PathAddBackslash(buffer);
            list.push_back(buffer);
         } else {
            if (pred(FindFileData.cFileName)) {
               if (collection.find(FindFileData.cFileName) == collection.end()) {
                  PathCombine(buffer, path, FindFileData.cFileName);
                  GetLongPathName(buffer, buffer, MAX_PATH);
                  PathRemoveExtension(FindFileData.cFileName);
                  PathRelativePathTo(buffer2, root, FILE_ATTRIBUTE_DIRECTORY, buffer, FILE_ATTRIBUTE_NORMAL);
                  TCHAR *p = buffer2; while (*p == '\\') ++p;
                  collection.insert(KeyValuePair(FindFileData.cFileName, p));					
               }
            }
         }
      }
      FindClose(hFind);
      for (stringlist::iterator itr = list.begin(), end = list.end(); itr != end; ++itr) {
         BuildFileNameMap(collection, root, (*itr).c_str(), pred);
      }
   }
}
// Implementation for BuildFileNameMap which will search for a specific set of extensions
struct ExtensionMatch : public std::unary_function<LPCTSTR, bool>
{
   stringlist extns;
   ExtensionMatch(string extnlist) {
      extns = TokenizeString(extnlist.c_str(), ";");
   }
   ExtensionMatch(const stringlist& extnlist) : extns(extnlist) {
   }
   bool operator()(LPCTSTR name) const {
      LPCSTR ext = PathFindExtension(name);
      for (stringlist::const_iterator itr = extns.begin(), end = extns.end(); itr != end; ++itr) {
         if (0 == _tcsicmp(ext, (*itr).c_str()))
            return true;
      }
      return false;      
   }
};

// Run through the search paths and add them to the image collection
void FindImages(NameValueCollection& images, const string& rootPath, const stringlist& searchpaths, const stringlist& extensions)
{
   ExtensionMatch ddsMatch(extensions);
   for (stringlist::const_iterator itr = searchpaths.begin(), end = searchpaths.end(); itr != end; ++itr) {
      if (PathIsRelative((*itr).c_str()))
      {
         TCHAR texPath[MAX_PATH];
         PathCombine(texPath, rootPath.c_str(), (*itr).c_str());
         PathAddBackslash(texPath);
         BuildFileNameMap(images, rootPath.c_str(), texPath, ddsMatch);
      }
      else
      {
         BuildFileNameMap(images, rootPath.c_str(), (*itr).c_str(), ddsMatch);
      }
   }
}


// Debugger Trace Window Utilities
Text PrintMatrix3(Matrix3& m)
{
   Point3 pt = m.GetTrans();
   float y,p,r;
   m.GetYawPitchRoll(&y,&p,&r);
   return FormatText("Matrix3: [%g,%g,%g] <%g,%g,%g>\n"
      ,pt.x, pt.y, pt.z
      ,TODEG(y), TODEG(p), TODEG(r)
      );
}

void DumpMatrix3(Matrix3& m)
{
   OutputDebugString(PrintMatrix3(m));
}

INode* FindINode(Interface *i, const string& name)
{
   return i->GetINodeByName(name.c_str());
}

Matrix3 GetNodeLocalTM(INode *n, TimeValue t)
{
   Matrix3 m3 = n->GetNodeTM(t);
   Matrix3 m3p = n->GetParentTM(t);
   m3p.Invert();
   return m3 * m3p;
}

// Locate a TriObject in an Object if it exists
TriObject* GetTriObject(Object *o)
{
   if (o && o->CanConvertToType(triObjectClassID))
      return (TriObject *)o->ConvertToType(0, triObjectClassID);
   while (o->SuperClassID() == GEN_DERIVOB_CLASS_ID && o)
   {
      IDerivedObject* dobj = (IDerivedObject *)(o);
      o = dobj->GetObjRef();
      if (o && o->CanConvertToType(triObjectClassID))
         return (TriObject *)o->ConvertToType(0, triObjectClassID);
   }
   return NULL;
}

// Get or Create the Skin Modifier
Modifier *GetOrCreateSkin(INode *node)
{
   Modifier *skinMod = GetSkin(node);
   if (skinMod)
      return skinMod;   
   
   IDerivedObject *dobj = CreateDerivedObject(node->GetObjectRef());
   //create a skin modifier and add it
   skinMod = (Modifier*) CreateInstance(OSM_CLASS_ID, SKIN_CLASSID);
   dobj->SetAFlag(A_LOCK_TARGET);
   dobj->AddModifier(skinMod);
   dobj->ClearAFlag(A_LOCK_TARGET);
   node->SetObjectRef(dobj);
   return skinMod;
}

Modifier *GetSkin(INode *node)
{
   Object* pObj = node->GetObjectRef();
   if (!pObj) return NULL;
   while (pObj->SuperClassID() == GEN_DERIVOB_CLASS_ID)
   {
      IDerivedObject* pDerObj = (IDerivedObject *)(pObj);
      int Idx = 0;
      while (Idx < pDerObj->NumModifiers())
      {
         // Get the modifier. 
         Modifier* mod = pDerObj->GetModifier(Idx);
         if (mod->ClassID() == SKIN_CLASSID)
         {
            // is this the correct Physique Modifier based on index?
            return mod;
         }
         Idx++;
      }
      pObj = pDerObj->GetObjRef();
   }
   return NULL;
}


Text GetFileVersion(const char *fileName)
{
   Text retval;
   char fileVersion[MAX_PATH];
   if (fileName == NULL)
   {
      GetModuleFileName(hInstance, fileVersion, MAX_PATH);
      fileName = fileVersion;
   }
   HMODULE ver = GetModuleHandle("version.dll");
   if (!ver) ver = LoadLibrary("version.dll");
   if (ver != NULL)
   {
      DWORD (APIENTRY *GetFileVersionInfoSize)(LPCTSTR, LPDWORD) = NULL;
      BOOL (APIENTRY *GetFileVersionInfo)(LPCTSTR, DWORD, DWORD, LPVOID) = NULL;
      BOOL (APIENTRY *VerQueryValue)(const LPVOID, LPTSTR, LPVOID *, PUINT) = NULL;
      *(FARPROC*)&GetFileVersionInfoSize = GetProcAddress(ver, "GetFileVersionInfoSizeA");
      *(FARPROC*)&GetFileVersionInfo = GetProcAddress(ver, "GetFileVersionInfoA");
      *(FARPROC*)&VerQueryValue = GetProcAddress(ver, "VerQueryValueA");
      if (GetFileVersionInfoSize && GetFileVersionInfo && VerQueryValue)
      {
         DWORD vLen = 0;
         DWORD vSize = GetFileVersionInfoSize(fileName,&vLen);
         if (vSize) 
         {
            LPVOID versionInfo = malloc(vSize+1);
            if (GetFileVersionInfo(fileName,vLen,vSize,versionInfo))
            {            
               LPVOID version=NULL;
               if (VerQueryValue(versionInfo,"\\VarFileInfo\\Translation",&version,(UINT *)&vLen) && vLen==4) 
               {
                  DWORD langD = *(DWORD*)version;
                  sprintf(fileVersion, "\\StringFileInfo\\%02X%02X%02X%02X\\ProductVersion",
                     (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, (langD & 0xff0000)>>16);            
               }
               else 
               {
                  sprintf(fileVersion, "\\StringFileInfo\\%04X04B0\\ProductVersion", GetUserDefaultLangID());
               }
               LPCTSTR value = NULL;
               if (VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&vLen))
                  value = LPCTSTR(version);
               else if (VerQueryValue(versionInfo,"\\StringFileInfo\\040904B0\\ProductVersion",&version,(UINT *)&vLen))
                  value = LPCTSTR(version);
               if (value != NULL)
               {
                  stringlist val = TokenizeString(value, ",", true);
                  if (val.size() >= 4){
                     retval = FormatText("%s.%s.%s.%s", val[0].c_str(), val[1].c_str(), val[2].c_str(), val[3].c_str());
                  }
               }
               free(versionInfo);
            }
         }
      }
   }
   return retval;
}

static Value* LocalExecuteScript(CharStream* source, bool *res) {

   *res = true;

   init_thread_locals();
   push_alloc_frame();
   three_typed_value_locals(Parser* parser, Value* code, Value* result);
   CharStream* out = thread_local(current_stdout);
   vl.parser = new Parser (out);

   try	{

      source->flush_whitespace();
      while (!source->at_eos()) {
         vl.code		= vl.parser->compile(source);
         vl.result	= vl.code->eval()->get_heap_ptr();
         source->flush_whitespace();
      }
      source->close();

   } catch (...) {
      *res = false;
   }

   if (vl.result == NULL)
      vl.result = &ok;

   pop_alloc_frame();
   return_value(vl.result);
}

// CallMaxscript
// Send the string to maxscript 
//
void CallMaxscript(const TCHAR *s)
{
   static bool script_initialized = false;
   if (!script_initialized) {
      init_MAXScript();
      script_initialized = TRUE;
   }
   init_thread_locals();

   push_alloc_frame();
   two_typed_value_locals(StringStream* ss, Value* result);

   vl.ss = new StringStream ( const_cast<TCHAR*>(s) );
   bool res = false;
   try	{
      vl.result = LocalExecuteScript(vl.ss,&res);
   } catch (...) {
      res = false;
   }
   thread_local(current_result) = vl.result;
   thread_local(current_locals_frame) = vl.link;
   pop_alloc_frame();
}

unsigned int ParseVersionString(string version) {

   unsigned int outver = 0;

   string::size_type start = 0, len, end;
   for( int offset = 0; offset < 3 && start < version.length(); ++offset ) {
      end = version.find_first_of( ".", start );

      if ( end == string.npos ) {
         len = end;
      } else {
         len = end-start;
      }

      int num = 0;
      stringstream sstr( version.substr(start, len) );
      sstr >> num;
      if ( num > 0xFF ) {
         return 0;
      }
      outver |= ( num << (offset * 8) );
      if ( len == string::npos ) {
         break;
      }
      start = start + len + 1;
   }

   if ( outver == 0 ) {
      return 0;
   } else {
      return outver;
   }
}

string PrintVersionString(unsigned int version) {
   return FormatString("%d.%d.%d.%d", version&0xFF, version>>8&0xFF, version>>16&0xFF, version>>24&0xFF);
}
