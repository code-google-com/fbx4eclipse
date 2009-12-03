/**********************************************************************
*<
FILE: daxmlwriter.h

DESCRIPTION:	Miscellaneous Utilities

HISTORY: 

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "valueptr.h"

class DAOXmlWriter;

template<>
struct VPTraits<DAOXmlWriter>
{
	static  DAOXmlWriter *  create()  { return NULL; }
	static  DAOXmlWriter *  clone( DAOXmlWriter const * p );
	static  DAOXmlWriter *  construct( DAOXmlWriter const * p );
	static  DAOXmlWriter *  release( DAOXmlWriter * p );
};  // VPTraits

template<>
struct VRTraits<DAOXmlWriter>
{
	static  DAOXmlWriter *  create()  { return NULL; }
	static  DAOXmlWriter *  clone( DAOXmlWriter const * p );
	static  DAOXmlWriter *  release( DAOXmlWriter * p );
};  // VPTraits

typedef ValuePtr<DAOXmlWriter> DAOXmlWriterPtr;

class DAOXmlWriter
{
	class Impl;
	Impl* pimpl;

protected:
	DAOXmlWriter();

public:
	~DAOXmlWriter();

	int AddRef();
	int Release();
/*
 * Functions
 */
	static DAOXmlWriterPtr Create(DAOStreamPtr stream);

/*
 * Document
 */
    int StartDocument( const char *version, const char *encoding, const char *standalone);
    int EndDocument();

/*
 * Comments
 */
    int StartComment( );
    int EndComment( );
    int WriteFormatComment( const char *format, ...);
    int WriteVFormatComment( const char *format, va_list argptr);
    int WriteComment( const char * content);

/*
 * Elements
 */
    int StartElement( const char * name);
    int StartElementNS( const char *prefix, const char * name, const char * namespaceURI);
    int EndElement( );
    int FullEndElement( );

/*
 * Elements conveniency functions
 */
    int WriteFormatElement( const char * name, const char *format, ...);
    int WriteVFormatElement( const char * name, const char *format, va_list argptr);
    int WriteElement( const char * name, const char * content);
    int WriteFormatElementNS( const char * prefix, const char * name, const char * namespaceURI, const char *format, ...);
    int WriteVFormatElementNS( const char * prefix, const char * name, const char * namespaceURI, const char *format, va_list argptr);
    int WriteElementNS( const char * prefix, const char * name, const char * namespaceURI, const char * content);
/*
 * Text
 */
    int WriteFormatRaw( const char *format, ...);
    int WriteVFormatRaw( const char *format, va_list argptr);
    int WriteRawLen( const char * content, int len);
    int WriteRaw( const char * content);
    int WriteFormatString( const char *format, ...);
    int WriteVFormatString( const char *format, va_list argptr);
    int WriteString( const char * content);
	int WriteBase64( const char *data, int start, int len);
    int WriteBinHex( const char *data, int start, int len);

/*
 * Attributes
 */
    int StartAttribute( const char * name);
    int StartAttributeNS( const char * prefix, const char * name, const char * namespaceURI);
    int EndAttribute( );

/*
 * Attributes conveniency functions
 */
    int WriteFormatAttribute( const char * name, const char *format, ...);
    int WriteVFormatAttribute( const char * name, const char *format, va_list argptr);
    int WriteAttribute( const char * name, const char * content);
    int WriteFormatAttributeNS( const char * prefix, const char * name, const char * namespaceURI, const char *format, ...);
    int WriteVFormatAttributeNS( const char * prefix, const char * name, const char * namespaceURI, const char *format, va_list argptr);
    int WriteAttributeNS( const char * prefix, const char * name, const char * namespaceURI, const char * content);

/*
 * PI's
 */
    int StartPI( const char * target);
    int EndPI( );

/*
 * PI conveniency functions
 */
    int WriteFormatPI( const char * target, const char *format, ...);
    int WriteVFormatPI( const char * target, const char *format, va_list argptr);
    int WritePI(  const char * target, const char * content);

/*
 * CDATA
 */
    int StartCDATA( );
    int EndCDATA( );

/*
 * CDATA conveniency functions
 */
    int WriteFormatCDATA( const char *format, ...);
    int WriteVFormatCDATA( const char *format, va_list argptr);
    int WriteCDATA( const char * content);

/*
 * Indentation
 */
    int SetIndent(  int indent);
    int SetIndentString( const char * str);

/*
 * misc
 */
    int Flush( );
};