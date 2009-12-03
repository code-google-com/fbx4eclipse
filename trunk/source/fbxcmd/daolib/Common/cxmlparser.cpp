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

_tstring XML_LookupStringAttribute(const XML_Char *attrName, const XML_Char **atts)
{
	if ( const XML_Char* val = XML_LookupTextAttribute(attrName, atts) ) {
		return _tstring(val);
	}
	return _tstring();
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

//////////////////////////////////////////////////////////////////////////


IParserContext* Context::FirstContextOfType( const type_info& type )
{
	for (ContextList::iterator itr = contexts.begin (), end = contexts.end(); itr != end; ++itr)
	{
		if ( (*itr)->type() == type )
			return (*itr);
	}
	return NULL;
}
