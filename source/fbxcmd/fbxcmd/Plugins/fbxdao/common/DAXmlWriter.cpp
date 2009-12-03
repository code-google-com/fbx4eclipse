/**********************************************************************
*<
FILE: daxmlwriter.cpp

DESCRIPTION:	Miscellaneous Utilities

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include "stdafx.h"
#include "DAXmlWriter.h"

namespace {
	extern "C"
	{
#undef XMLPUBFUN
#undef XMLCALL
#undef LIBXML_DOTTED_VERSION
#undef LIBXML_VERSION
#undef LIBXML_VERSION_STRING
#undef LIBXML_TEST_VERSION
#include "win32config.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
typedef const ::xmlChar *LPCXCHAR;
	}
}

class DAOXmlWriter::Impl
{
public:
	fbxsdk_20102::xmlOutputBufferPtr buffer;
	xmlTextWriterPtr writer;
	DAOStreamPtr stream;
	int refcnt;

	Impl() : writer(NULL), buffer(NULL), refcnt(0) { Init(); }
	~Impl() { Destroy(); }

	operator xmlTextWriterPtr() { return writer; }

	void Init()
	{
		fbxsdk_20102::xmlBufferPtr bufferPtr = fbxsdk_20102::xmlBufferCreateSize(0x1000);
		buffer = xmlOutputBufferCreateBuffer(bufferPtr, NULL);
		buffer->context = this;
		buffer->writecallback = OutputWriteCallback;
		buffer->closecallback = OutputCloseCallback;
		writer = xmlNewTextWriter(buffer);
	}

	void Destroy()
	{
		if (writer) {
			xmlFreeTextWriter(writer); 
			writer = NULL;
		}
		if (buffer) {
			//fbxsdk_20102::xmlOutputBufferClose(buffer);
			buffer = NULL;
		}
	}

	static int OutputWriteCallback(void * context, const char * buffer, int len)
	{
		DAOXmlWriter::Impl* pimpl = static_cast<DAOXmlWriter::Impl*>(context);
		int retval = -1;
		if (pimpl && !pimpl->stream.isNull())
		{
			int written = pimpl->stream->Write(buffer, len, 1);
			retval = ( written == 1 ) ? len : -1;
		}
		return retval;
	}
	static int OutputCloseCallback(void * context) 
	{
		DAOXmlWriter::Impl* pimpl = static_cast<DAOXmlWriter::Impl*>(context);
		if (pimpl && !pimpl->stream.isNull()) {
			pimpl->stream->Close();
			return 0;
		}
		return -1;
	}
};

DAOXmlWriter::DAOXmlWriter() 
	: pimpl( new Impl() )
{

}

DAOXmlWriter::~DAOXmlWriter()
{
	delete pimpl;
}

int DAOXmlWriter::StartDocument( const char *version, const char *encoding, const char *standalone )
{
	return xmlTextWriterStartDocument(*pimpl, version, encoding, standalone );
}

int DAOXmlWriter::EndDocument()
{
	return xmlTextWriterEndDocument(*pimpl);
}

int DAOXmlWriter::StartComment()
{
	return xmlTextWriterStartComment(*pimpl);
}

int DAOXmlWriter::EndComment()
{
	return xmlTextWriterEndComment(*pimpl);
}

int DAOXmlWriter::WriteFormatComment( const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatComment(*pimpl, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatComment( const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatComment(*pimpl,  format, argptr );
}

int DAOXmlWriter::WriteComment( const char * content )
{
	return xmlTextWriterWriteComment(*pimpl,  (LPCXCHAR)content );
}

int DAOXmlWriter::StartElement( const char * name )
{
	return xmlTextWriterStartElement(*pimpl,  (LPCXCHAR)name );
}

int DAOXmlWriter::StartElementNS( const char *prefix, const char * name, const char * namespaceURI )
{
	return xmlTextWriterStartElementNS(*pimpl,  (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI );
}

int DAOXmlWriter::EndElement()
{
	return xmlTextWriterEndElement(*pimpl);
}

int DAOXmlWriter::FullEndElement()
{
	return xmlTextWriterFullEndElement(*pimpl);
}

int DAOXmlWriter::WriteFormatElement( const char * name, const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatElement(*pimpl, (LPCXCHAR)name, format, argptr );
	va_end(argptr);		
	return ret;
}

int DAOXmlWriter::WriteVFormatElement( const char * name, const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatElement(*pimpl, (LPCXCHAR)name, format, argptr );
}

int DAOXmlWriter::WriteElement( const char * name, const char * content )
{
	return xmlTextWriterWriteElement(*pimpl, (LPCXCHAR)name, (LPCXCHAR)content );
}

int DAOXmlWriter::WriteFormatElementNS( const char *prefix, const char * name, const char * namespaceURI, const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatElementNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatElementNS( const char *prefix, const char * name, const char * namespaceURI, const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatElementNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI, format, argptr );
}

int DAOXmlWriter::WriteElementNS( const char *prefix, const char * name, const char * namespaceURI, const char * content )
{
	return xmlTextWriterWriteElementNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI, (LPCXCHAR)content );
}

int DAOXmlWriter::WriteFormatRaw( const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatRaw( *pimpl, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatRaw( const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatRaw(*pimpl,  format, argptr );
}

int DAOXmlWriter::WriteRawLen( const char * content, int len )
{
	return xmlTextWriterWriteRawLen(*pimpl, (LPCXCHAR)content, len );
}

int DAOXmlWriter::WriteRaw( const char * content )
{
	return xmlTextWriterWriteRaw(*pimpl, (LPCXCHAR)content );
}

int DAOXmlWriter::WriteFormatString( const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatString(*pimpl, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatString( const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatString(*pimpl,  format, argptr );
}

int DAOXmlWriter::WriteString( const char * content )
{
	return xmlTextWriterWriteString(*pimpl, (LPCXCHAR)content );
}

int DAOXmlWriter::WriteBase64( const char *data, int start, int len )
{
	return xmlTextWriterWriteBase64(*pimpl,  data, start, len );
}

int DAOXmlWriter::WriteBinHex( const char *data, int start, int len )
{
	return xmlTextWriterWriteBinHex(*pimpl,  data, start, len );
}

int DAOXmlWriter::StartAttribute( const char * name )
{
	return xmlTextWriterStartAttribute(*pimpl, (LPCXCHAR)name );
}

int DAOXmlWriter::StartAttributeNS( const char *prefix, const char * name, const char * namespaceURI )
{
	return xmlTextWriterStartAttributeNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI );
}

int DAOXmlWriter::EndAttribute()
{
	return xmlTextWriterEndAttribute(*pimpl);
}

int DAOXmlWriter::WriteFormatAttribute( const char * name, const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatAttribute(*pimpl, (LPCXCHAR)name, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatAttribute( const char * name, const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatAttribute(*pimpl, (LPCXCHAR)name, format, argptr );
}

int DAOXmlWriter::WriteAttribute( const char * name, const char * content )
{
	return xmlTextWriterWriteAttribute(*pimpl, (LPCXCHAR)name, (LPCXCHAR)content );
}

int DAOXmlWriter::WriteFormatAttributeNS( const char *prefix, const char * name, const char * namespaceURI, const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatAttributeNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatAttributeNS( const char *prefix, const char * name, const char * namespaceURI, const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatAttributeNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI, format, argptr );
}

int DAOXmlWriter::WriteAttributeNS( const char *prefix, const char * name, const char * namespaceURI, const char * content )
{
	return xmlTextWriterWriteAttributeNS(*pimpl, (LPCXCHAR)prefix, (LPCXCHAR)name, (LPCXCHAR)namespaceURI, (LPCXCHAR)content );
}

int DAOXmlWriter::StartPI( const char * target )
{
	return xmlTextWriterStartPI(*pimpl, (LPCXCHAR)target );
}

int DAOXmlWriter::EndPI()
{
	return xmlTextWriterEndPI(*pimpl);
}

int DAOXmlWriter::WriteFormatPI( const char * target, const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatPI(*pimpl, (LPCXCHAR)target, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatPI( const char * target, const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatPI(*pimpl, (LPCXCHAR)target, format, argptr );
}

int DAOXmlWriter::WritePI( const char * target, const char * content )
{
	return xmlTextWriterWritePI(*pimpl, (LPCXCHAR)target, (LPCXCHAR)content );
}

int DAOXmlWriter::StartCDATA()
{
	return xmlTextWriterStartCDATA(*pimpl);
}

int DAOXmlWriter::EndCDATA()
{
	return xmlTextWriterEndCDATA(*pimpl);
}

int DAOXmlWriter::WriteFormatCDATA( const char *format, ... )
{
	va_list argptr;
	va_start(argptr, format);
	int ret = xmlTextWriterWriteVFormatCDATA(*pimpl, format, argptr );
	va_end(argptr);
	return ret;
}

int DAOXmlWriter::WriteVFormatCDATA( const char *format, va_list argptr )
{
	return xmlTextWriterWriteVFormatCDATA(*pimpl,  format, argptr );
}

int DAOXmlWriter::WriteCDATA( const char * content )
{
	return xmlTextWriterWriteCDATA(*pimpl, (LPCXCHAR)content );
}

int DAOXmlWriter::SetIndent( int indent )
{
	return xmlTextWriterSetIndent(*pimpl,  indent );
}

int DAOXmlWriter::SetIndentString( const char * str )
{
	return xmlTextWriterSetIndentString(*pimpl, (LPCXCHAR)str );
}

int DAOXmlWriter::Flush()
{
	return xmlTextWriterFlush(*pimpl);
}

DAOXmlWriterPtr DAOXmlWriter::Create( DAOStreamPtr stream )
{
	if ( DAOXmlWriter* ptr = new DAOXmlWriter() )
	{
		ptr->pimpl->stream = stream;
		ptr->AddRef();
		return DAOXmlWriterPtr (ptr);	
	}
	return DAOXmlWriterPtr(NULL);
}

int DAOXmlWriter::AddRef()
{ 
	if (pimpl->refcnt <= 0) 
		return 0; 
	return ++pimpl->refcnt; 
}

int DAOXmlWriter::Release()
{ 
	if (pimpl->refcnt <= 0) 
		return 0; 
	if (--pimpl->refcnt == 0) { 
		delete this; 
		return 0; 
	} 
	return pimpl->refcnt; 
}

DAOXmlWriter * VPTraits<DAOXmlWriter>::clone( DAOXmlWriter const * p )
{
	DAOXmlWriter *mp = const_cast<DAOXmlWriter*>(p); 
	if (mp) mp->AddRef(); 
	return mp;
}

DAOXmlWriter * VPTraits<DAOXmlWriter>::construct( DAOXmlWriter const * p )
{
	DAOXmlWriter *mp = const_cast<DAOXmlWriter*>(p); 
	if (mp) mp->AddRef(); 
	return mp;
}

DAOXmlWriter * VPTraits<DAOXmlWriter>::release( DAOXmlWriter * p )
{
	if (p && p->Release() == 0)
		return NULL;
	return p;
}

DAOXmlWriter * VRTraits<DAOXmlWriter>::clone( DAOXmlWriter const * p )
{
	DAOXmlWriter *mp = const_cast<DAOXmlWriter*>(p); 
	//if (mp) mp->AddRef(); 
	return mp;
}

DAOXmlWriter * VRTraits<DAOXmlWriter>::release( DAOXmlWriter * p )
{
	//if (p->Release() <= 0) return NULL;
	return p;
}