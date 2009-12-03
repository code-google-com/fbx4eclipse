#include "stdafx.h"
#include <Shlwapi.h>
#include "CXmlParser.h"

#pragma comment(lib, "shlwapi.lib")

const XML_Char* XML_LookupTextAttribute(const XML_Char *attrName, const XML_Char **atts)
{
   for (const XML_Char **att = atts; att[0] != NULL; att += 2)
   {
      if (_tcscmp(att[0], attrName) == 0)
         return att[1];
   }
   return NULL;
}

int XML_LookupIntAttribute(const XML_Char *attrName, const XML_Char **atts, int defaultValue)
{
   for (const XML_Char **att = atts; att[0] != NULL; att += 2)
   {
      if (_tcscmp(att[0], attrName) == 0)
         return _ttoi(att[1]);
   }
   return defaultValue;
}

float XML_LookupFloatAttribute(const XML_Char *attrName, const XML_Char **atts, float defaultValue)
{
   for (const XML_Char **att = atts; att[0] != NULL; att += 2)
   {
      if (_tcscmp(att[0], attrName) == 0)
      {
         return (float)strtod(att[1], NULL);
      }
   }
   return defaultValue;
}

int XML_LookupEnumAttribute(const XML_Char *attrName, const XML_Char **atts, const EnumLookupType* table, int default)
{
   if ( const XML_Char* s = XML_LookupTextAttribute(attrName, atts))
   {
      return StringToEnum(s, table);   
   }
   return default;
}


//////////////////////////////////////////////////////////////////////////

// Enumeration Support
Text EnumToString(int value, const EnumLookupType *table) {
   for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
      if (itr->value == value) return Text(itr->name);
   }
   return FormatText("%x", value);
}

int StringToEnum(LPCTSTR value, const EnumLookupType *table) {
   Text tstr = value;
   Trim(tstr);
   if (tstr.isNull()) return 0;

   for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
      if (0 == _tcsicmp(value, itr->name)) return itr->value;
   }
   char *end = NULL;
   return (int)strtol(tstr, &end, 0);
}

Text FlagsToString(int value, const EnumLookupType *table) {
   Text sstr;
   for (const EnumLookupType *itr = table; itr->name != NULL; ++itr) {
      if (itr->value && (itr->value & value) == itr->value) {
         if (!sstr.isNull()) sstr += " | ";
         sstr += itr->name;
         value ^= itr->value;
      }
   }
   if (value == 0 && sstr.isNull()) {
      return EnumToString(value, table);
   }
   if (value != 0) {
      if (!sstr.isNull()) sstr += "|";
      sstr += EnumToString(value, table);
   }
   return sstr;
}

int StringToFlags(LPCTSTR value, const EnumLookupType *table) {
   int retval = 0;
   Text tstr = value;
   LPCTSTR start = tstr.data();
   LPCTSTR end = tstr.data() + tstr.Length();
   while(start < end) {
      LPCTSTR bar = _tcschr(start, '|');
      int len = (bar != NULL) ?  bar-start : end-start;
      Text subval = tstr.Substr(start-tstr.data(), len);
      retval |= StringToEnum(subval, table);
      start += (len + 1);
   }
   return retval;
}

XML_Status XML_ParseFile(XML_Parser parser, const char *fileName )
{
   XML_Status status = XML_STATUS_ERROR;
   char buf[0x1000];
   FILE* hFile = fopen(fileName, "rb");
   if ( hFile == NULL )
   {
      char path[MAX_PATH];
      GetModuleFileName(NULL, path, sizeof(path));
      PathRemoveFileSpec(path);
      PathAddBackslash(path);
      PathAppend( path, PathFindFileName(fileName) );
      hFile = fopen(path, "rb");
   }
   if ( hFile != NULL )
   {
      status = XML_STATUS_OK;
      for( size_t nRead = fread(buf, 1, sizeof(buf), hFile)
         ; nRead != 0 && status == XML_STATUS_OK
         ; nRead = fread(buf, 1, sizeof(buf), hFile)
         )
      {
         int isFinal = (nRead != sizeof(buf));
         status = XML_Parse(parser, buf, nRead, isFinal);
      }
      fclose(hFile);
   }
   return status;
}
