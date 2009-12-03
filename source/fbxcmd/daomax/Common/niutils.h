/**********************************************************************
*<
FILE: NIUtils.h

DESCRIPTION:	Miscellaneous Utilities

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#ifndef _WINDOWS_
#  include <windows.h>
#endif
#include <tchar.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <set>

// Max Headers
#include <Max.h>
#include <strclass.h>
#include <color.h>

#include "utility.h"

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof((x)[0]))
#endif

const unsigned int IntegerInf = 0x7f7fffff;
const unsigned int IntegerNegInf = 0xff7fffff;
const float FloatINF = *(float*)&IntegerInf;
const float FloatNegINF = *(float*)&IntegerNegInf;

#if 0
using std::string;


// Case insensitive string equivalence but numbers are sorted together
struct NumericStringEquivalence
{
   bool operator()(const TCHAR* s1, const TCHAR* s2) const
   { return numstrcmp(s1, s2) < 0; }

   bool operator()(const std::string& s1, const TCHAR* s2) const
   { return numstrcmp(s1.c_str(), s2) < 0; }

   bool operator()(const TCHAR* s1, const std::string& s2) const
   { return numstrcmp(s1, s2.c_str()) < 0; }

   bool operator()(const std::string& s1, const std::string& s2) const
   { return numstrcmp(s1.c_str(), s2.c_str()) < 0; }

   static int numstrcmp(const TCHAR *str1, const TCHAR *str2)
   {
      TCHAR *p1, *p2;
      int c1, c2, lcmp;
      for(;;)
      {
         c1 = tolower(*str1), c2 = tolower(*str2);
         if ( c1 == 0 || c2 == 0 )
            break;
         else if (isdigit(c1) && isdigit(c2))
         {			
            lcmp = strtol(str1, &p1, 10) - strtol(str2, &p2, 10);
            if ( lcmp == 0 )
               lcmp = (p2 - str2) - (p1 - str1);
            if ( lcmp != 0 )
               return (lcmp > 0 ? 1 : -1);
            str1 = p1, str2 = p2;
         }
         else
         {
            lcmp = (c1 - c2);
            if (lcmp != 0)
               return (lcmp > 0 ? 1 : -1);
            ++str1, ++str2;
         }
      }
      lcmp = (c1 - c2);
      return ( lcmp < 0 ) ? -1 : (lcmp > 0 ? 1 : 0);
   }
};

// Common collections that I use
typedef std::map<std::string, std::string, ltstr> NameValueCollection;
typedef std::pair<std::string, std::string> KeyValuePair;
typedef std::vector<std::string> stringlist;
typedef std::set<std::string, ltstr> stringset;

extern int wildcmp(const TCHAR *wild, const TCHAR *string);
extern int wildcmpi(const TCHAR *wild, const TCHAR *string);

inline bool strmatch(const string& lhs, const std::string& rhs) {
   return (0 == _tcsicmp(lhs.c_str(), rhs.c_str()));
}
inline bool strmatch(const TCHAR* lhs, const std::string& rhs) {
   return (0 == _tcsicmp(lhs, rhs.c_str()));
}
inline bool strmatch(const string& lhs, const TCHAR* rhs) {
   return (0 == _tcsicmp(lhs.c_str(), rhs));
}
inline bool strmatch(const TCHAR* lhs, const TCHAR* rhs) {
   return (0 == _tcsicmp(lhs, rhs));
}

bool wildmatch(const TCHAR* match, const TCHAR* value);
bool wildmatch(const string& match, const std::string& value);
bool wildmatch(const stringlist& matches, const TCHAR* value);
bool wildmatch(const stringlist& matches, const std::string& value);
#endif

// Generic IniFile reading routine
template<typename T>
inline T GetIniValue(LPCTSTR Section, LPCTSTR Setting, T Default, LPCTSTR iniFileName){
   T v = Default;
   TCHAR buffer[1024];
   std::stringstream sstr;
   sstr << Default;
   buffer[0] = 0;
   if (0 < GetPrivateProfileString(Section, Setting, sstr.str().c_str(), buffer, sizeof(buffer), iniFileName)){
      std::stringstream sstr(buffer);
      sstr >> v;
      return v;
   }
   return Default;
}

// Specific override for int values
template<>
inline int GetIniValue<int>(LPCTSTR Section, LPCTSTR Setting, int Default, LPCTSTR iniFileName){
   return GetPrivateProfileInt(Section, Setting, Default, iniFileName);
}

// Specific override for int values
template<>
inline bool GetIniValue<bool>(LPCTSTR Section, LPCTSTR Setting, bool Default, LPCTSTR iniFileName){
   return GetPrivateProfileInt(Section, Setting, Default, iniFileName) ? true : false;
}

// Specific override for string values
template<>
inline std::string GetIniValue<std::string>(LPCTSTR Section, LPCTSTR Setting, std::string Default, LPCTSTR iniFileName){
   TCHAR buffer[1024];
   buffer[0] = 0;
   if (0 < GetPrivateProfileString(Section, Setting, Default.c_str(), buffer, sizeof(buffer), iniFileName)){
      return std::string(buffer);
   }
   return Default;
}

// Specific override for TSTR values
template<>
inline TSTR GetIniValue<TSTR>(LPCTSTR Section, LPCTSTR Setting, TSTR Default, LPCTSTR iniFileName){
   TCHAR buffer[1024];
   buffer[0] = 0;
   if (0 < GetPrivateProfileString(Section, Setting, Default.data(), buffer, sizeof(buffer), iniFileName)){
      return TSTR(buffer);
   }
   return Default;
}

// Generic IniFile reading routine
template<typename T>
inline void SetIniValue(LPCTSTR Section, LPCTSTR Setting, T value, LPCTSTR iniFileName){
   std::stringstream sstr;
   sstr << value;
   WritePrivateProfileString(Section, Setting, sstr.str().c_str(), iniFileName);
}

// Specific override for string values
template<>
inline void SetIniValue<std::string>(LPCTSTR Section, LPCTSTR Setting, std::string value, LPCTSTR iniFileName){
   WritePrivateProfileString(Section, Setting, value.c_str(), iniFileName);
}

// Specific override for TSTR values
template<>
inline void SetIniValue<TSTR>(LPCTSTR Section, LPCTSTR Setting, TSTR value, LPCTSTR iniFileName){
   WritePrivateProfileString(Section, Setting, value.data(), iniFileName);
}

template<>
inline void SetIniValue<LPTSTR>(LPCTSTR Section, LPCTSTR Setting, LPTSTR value, LPCTSTR iniFileName){
   WritePrivateProfileString(Section, Setting, value, iniFileName);
}

template<>
inline void SetIniValue<LPCTSTR>(LPCTSTR Section, LPCTSTR Setting, LPCTSTR value, LPCTSTR iniFileName){
   WritePrivateProfileString(Section, Setting, value, iniFileName);
}

//extern TSTR FormatTStr(const TCHAR* format,...);
extern std::string FormatString(const TCHAR* format,...);

extern stringlist TokenizeCommandLine(LPCTSTR str, bool trim);
extern _tstring JoinCommandLine(stringlist args);

extern _tstring GetIndirectValue(LPCSTR path);
extern NameValueCollection ReadIniSection(LPCTSTR Section, LPCTSTR iniFileName);
extern _tstring ExpandQualifiers(const _tstring& src, const NameValueCollection& map);
extern _tstring ExpandEnvironment(const _tstring& src);
extern void ReadEnvironment(LPCTSTR Section, LPCTSTR iniFileName, NameValueCollection& Environment);

extern void FindImages(NameValueCollection& images, const _tstring& rootPath, const stringlist& searchpaths, const stringlist& extensions);

extern void RenameNode(Interface *gi, LPCTSTR SrcName, LPCTSTR DstName);

enum PosRotScale
{
   prsPos = 0x1,
   prsRot = 0x2,
   prsScale = 0x4,
   prsDefault = prsPos | prsRot | prsScale,
};
extern void PosRotScaleNode(INode *n, Point3 p, Quat& q, float s, PosRotScale prs = prsDefault, TimeValue t = 0);
extern void PosRotScaleNode(Control *c, Point3 p, Quat& q, float s, PosRotScale prs = prsDefault, TimeValue t = 0);
extern void PosRotScaleNode(INode *n, Point3 p, Quat& q, Point3 s, PosRotScale prs = prsDefault, TimeValue t = 0);
extern void PosRotScaleNode(Control *c, Point3 p, Quat& q, Point3 s, PosRotScale prs = prsDefault, TimeValue t = 0);

extern void PosRotScaleNode(INode *n, Matrix3& m3, PosRotScale prs = prsDefault, TimeValue t = 0);
extern void PosRotScaleNode(Control *c, Matrix3& m3, PosRotScale prs = prsDefault, TimeValue t = 0);

extern Matrix3 GetNodeLocalTM(INode *n, TimeValue t = 0);

extern INode* FindINode(Interface *i, const _tstring& name);

// Simple conversion helpers
Text PrintMatrix3(Matrix3& m);

extern Modifier *GetOrCreateSkin(INode *node);
extern Modifier *GetSkin(INode *node);
extern TriObject* GetTriObject(Object *o);

extern Text GetFileVersion(const char *fileName);
extern unsigned int ParseVersionString(_tstring version);
extern _tstring PrintVersionString(unsigned int version);

extern void ExpandFileList( stringlist& list );

extern NameValueCollection Environment;

template<typename T>
inline T GetSetting(std::string setting){
   T v;
   NameValueCollection::iterator itr = Environment.find(setting);
   if (itr != Environment.end()){
      stringstream sstr((*itr).second);
      sstr >> v;
   }
   return v;
}
template<>
inline std::string GetSetting(std::string setting){
   NameValueCollection::iterator itr = Environment.find(setting);
   if (itr != Environment.end())
      return (*itr).second;
   return std::string();
}

template<typename T>
inline T GetSetting(std::string setting, T Default){
   NameValueCollection::iterator itr = Environment.find(setting);
   if (itr != Environment.end()){
      T v;
      stringstream sstr((*itr).second);
      sstr >> v;
      return v;
   }
   return Default;
}
template<>
inline std::string GetSetting(std::string setting, std::string Default){
   NameValueCollection::iterator itr = Environment.find(setting);
   if (itr != Environment.end())
      return (*itr).second;
   return Default;
}
