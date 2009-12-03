#pragma once

#define XML_STATIC
#include <expat.h>
#include <list>
#include "text.h"

/////////////////////////////////////////////////////////////////////////
// Helper Declarations
struct EnumLookupType;
const XML_Char* XML_LookupTextAttribute(const XML_Char *attrName, const XML_Char **atts);
_tstring XML_LookupStringAttribute(const XML_Char *attrName, const XML_Char **atts);
int XML_LookupIntAttribute(const XML_Char *attrName, const XML_Char **atts, int defaultValue = 0);
float XML_LookupFloatAttribute(const XML_Char *attrName, const XML_Char **atts, float defaultValue = 0.0);
int XML_LookupEnumAttribute(const XML_Char *attrName, const XML_Char **atts, const EnumLookupType* table, int default = 0);
XML_Status XML_ParseFile(XML_Parser parser, const char *fileName );


//////////////////////////////////////////////////////////////////////////
// CXmlParser

template <typename ClassName>
class CXmlParser : public ClassName
{
   typedef ClassName BaseClass;
   typedef CXmlParser<ClassName> ThisClass;
public:
   CXmlParser() : parser(NULL) {}
   ~CXmlParser()
   {
      if (parser != NULL)
      {
         XML_ParserFree(parser);
         parser = NULL;
      }
   }

   void StartElementHandler(XML_Parser parser, const XML_Char *name, const XML_Char **atts) {}

   void EndElementHandler(XML_Parser parser, const XML_Char *name) {}

   XML_Status ParseFile(const char *fileName)
   {
      Reset();
      return XML_ParseFile(parser, fileName);
   }

   XML_Status ParseString(const char *s, int length = -1, int isFinal = 1)
   {
      if (length == -1)
         length = strlen(s);
      if (parser == NULL)
         Reset();
      XML_Status status = XML_Parse(parser, s, length, isFinal);
      if (isFinal) Reset();
      return status;
   }

   void Reset()
   {
      if (parser == NULL)
         parser = XML_ParserCreate(NULL);
      else
         XML_ParserReset(parser, NULL);

      BaseClass* pThis = static_cast<BaseClass*>(this);

      XML_UseParserAsHandlerArg(parser);
      XML_SetUserData(parser, this);

      __if_exists(BaseClass::ElementDeclHandler) {
         XML_SetElementDeclHandler(parser, ThisClass::XML_ElementDeclHandler);
      }
      __if_exists(BaseClass::AttlistDeclHandler) {
         XML_SetAttlistDeclHandler(parser, ThisClass::XML_AttlistDeclHandler);
      }
      __if_exists(BaseClass::StartElementHandler) {
         XML_SetStartElementHandler(parser, ThisClass::XML_StartElementHandler);
      }
      __if_exists(BaseClass::EndElementHandler) {
         XML_SetEndElementHandler(parser, ThisClass::XML_EndElementHandler);
      }
   }


private:
   XML_Parser parser;

   static void (XMLCALL XML_ElementDeclHandler) (void *userData, const XML_Char *name, XML_Content *model)
   {
      XML_Parser parser = static_cast<XML_Parser>(userData);
      if ( BaseClass* pThis = static_cast<ThisClass*>(XML_GetUserData(parser)) )
         pThis->ElementDeclHandler(parser, name, model);
   }

   static void (XMLCALL XML_AttlistDeclHandler) (
      void            *userData,
      const XML_Char  *elname,
      const XML_Char  *attname,
      const XML_Char  *att_type,
      const XML_Char  *dflt,
      int              isrequired)
   {
      XML_Parser parser = static_cast<XML_Parser>(userData);
      if ( BaseClass* pThis = static_cast<ThisClass*>(XML_GetUserData(parser)) )
         pThis->AttlistDeclHandler(parser, elname, attname, att_type, dflt, isrequired);
   }

   static void (XMLCALL XML_XmlDeclHandler) (void           *userData,
      const XML_Char *version,
      const XML_Char *encoding,
      int             standalone)
   {
      XML_Parser parser = static_cast<XML_Parser>(userData);
      if ( BaseClass* pThis = static_cast<ThisClass*>(XML_GetUserData(parser)) )
         pThis->XmlDeclHandler(parser, version, encoding, standalone);
   }

   static void (XMLCALL XML_StartElementHandler) (void *userData,
      const XML_Char *name,
      const XML_Char **atts)
   {
      XML_Parser parser = static_cast<XML_Parser>(userData);
      if ( BaseClass* pThis = static_cast<ThisClass*>(XML_GetUserData(parser)) )
         pThis->StartElementHandler(parser, name, atts);
   }

   static void (XMLCALL XML_EndElementHandler) (void *userData,
      const XML_Char *name)
   {
      XML_Parser parser = static_cast<XML_Parser>(userData);
      if ( BaseClass* pThis = static_cast<ThisClass*>(XML_GetUserData(parser)) )
         pThis->EndElementHandler(parser, name);
   }


   /* s is not 0 terminated. */
   static void (XMLCALL XML_CharacterDataHandler) (void *userData,
      const XML_Char *s,
      int len);

   /* target and data are 0 terminated */
   static void (XMLCALL XML_ProcessingInstructionHandler) (
      void *userData,
      const XML_Char *target,
      const XML_Char *data);

   /* data is 0 terminated */
   static void (XMLCALL XML_CommentHandler) (void *userData,
      const XML_Char *data);

   static void (XMLCALL XML_StartCdataSectionHandler) (void *userData);
   static void (XMLCALL XML_EndCdataSectionHandler) (void *userData);

};

//////////////////////////////////////////////////////////////////////////

struct IParserContext;
typedef std::list<IParserContext*> ContextList;

struct Context
{
	XML_Parser parser;
	ContextList contexts;
	IParserContext* FirstContextOfType( const type_info& type );
};

struct __declspec(novtable) IParserContext
{
	virtual ~IParserContext() {}
	virtual const type_info& type() const = 0;
	virtual void* valueptr() = 0;

	virtual bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) = 0;
	virtual bool EndElementHandler(Context& context, const XML_Char *name) = 0;
};

template <typename ClassType>
struct ParserContext : public IParserContext 
{
	typedef typename ClassType ContextType;

	ParserContext( ClassType& val ) : valref(&val) {}
	ParserContext( ClassType* val ) : valref(val) {}
	const type_info& type() const { return typeid(ContextType); }
	void* valueptr() { return valref; }
	ClassType* ptr() { return (ClassType*)valref; }
	ClassType& ref() { return *(ClassType*)valref; }

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) {
		__if_exists(ContextType::StartElement) { return valref->StartElement(name, atts); }
		return false;
	}

	bool EndElementHandler(Context& context, const XML_Char *name) {
		__if_exists(ContextType::EndElement) { return valref->EndElement(name); }
		return false;
	}

	ContextType* valref;
};
