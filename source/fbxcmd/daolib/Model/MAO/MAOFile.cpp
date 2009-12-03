/**********************************************************************
*<
FILE: MAOFile.h

DESCRIPTION:	MAO File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include <windows.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdexcept>
#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <typeinfo.h>

#include "expat.h"
#include "MAOFile.h"
#include "cxmlparser.h"
#include "MAO/MaterialObject.h"
#include "DataHeader.h"
#include "MATFormat.h"
#include "daolog.h"


using namespace std;
using namespace boost;
using namespace DAO;
using namespace DAO::MAO;

class MAOParser
{
	MaterialObject *p;
public:
	void set_Object(MaterialObject *pObject){
		this->p = pObject;
	}
	void StartElementHandler(XML_Parser parser, const XML_Char *name, const XML_Char **atts);
	void EndElementHandler(XML_Parser parser, const XML_Char *name);
};

typedef struct TypeLookup {
	const type_info& type;
	const char *name;
} TypeLookup;

const TypeLookup TypeLookupFields[] =
{
	{typeid(float)			,_T("Float")},
	{typeid(double)			,_T("Double")},
	{typeid(Vector2f)		,_T("Vector2f")},
	{typeid(Vector3f)		,_T("Vector3f")},
	{typeid(Vector4f) 		,_T("Vector4f")},
	{typeid(Quaternion)		,_T("Quaternion")},
	{typeid(Vector2fArray)	,_T("Vector2fArray")},
	{typeid(Vector3fArray)	,_T("Vector3fArray")},
	{typeid(Vector4fArray)	,_T("Vector4fArray")},
	{typeid(Matrix44)		,_T("Matrix4x4f")},
	{typeid(void)			,NULL},
};

//////////////////////////////////////////////////////////////////////////

class MAOFile::Impl
{
public:
	ValuePtr<MaterialObject> root;
};

DAO::MAO::MAOFile::MAOFile()
{

}

DAO::MAO::MAOFile::~MAOFile()
{

}

void DAO::MAO::MAOFile::open( const _tstring& filename )
{
	CXmlParser<MAOParser> modelParser;
	modelParser.set_Object( this->pimpl->root.ToPointer() );
	modelParser.Reset();
	modelParser.ParseFile(filename.c_str());
}

void DAO::MAO::MAOFile::open( IDAOStream& stream )
{
	CXmlParser<MAOParser> modelParser;
	modelParser.set_Object( this->pimpl->root.ToPointer() );

	XML_Status status = XML_STATUS_ERROR;
	char buf[0x1000];
	status = XML_STATUS_OK;
	for( size_t nRead = stream.Read(buf, 1, sizeof(buf))
		; nRead != 0 && status == XML_STATUS_OK
		; nRead = stream.Read(buf, 1, sizeof(buf))
		)
	{
		int isFinal = (nRead != sizeof(buf));
		status = modelParser.ParseString(buf, nRead, isFinal);
	}

}

void DAO::MAO::MAOFile::save( const _tstring& filename )
{

}

void DAO::MAO::MAOFile::save( IDAOStream& stream )
{

}

void DAO::MAO::MAOFile::dump()
{

}

void DAO::MAO::MAOFile::dump( const _tstring& filename )
{

}

DAO::MAO::MaterialObjectRef DAO::MAO::MAOFile::get_Object()
{
	return MaterialObjectRef(this->pimpl->root);
}

DAO::MAO::MaterialObjectRef DAO::MAO::MAOFile::get_Object() const
{
	return MaterialObjectRef(this->pimpl->root);
}

//////////////////////////////////////////////////////////////////////////
void MAOParser::StartElementHandler( XML_Parser parser, const XML_Char *name, const XML_Char **atts )
{
	try
	{
		if ( 0 == _tcscmp( name, _T("MaterialObject") ) )
		{
			p->Name = XML_LookupStringAttribute("Name", atts);
		}
		else if ( 0 == _tcscmp( name, _T("Material") ) )
		{
			p->Material = XML_LookupStringAttribute("Name", atts);
		}
		else if ( 0 == _tcscmp( name, _T("DefaultSemantic") ) )
		{
			p->DefaultSemantic = XML_LookupStringAttribute("Name", atts);
		}
		else if ( 0 == _tcscmp( name, _T("Texture") ) )
		{
			Texture tex;
			_tstring name = XML_LookupStringAttribute("Name", atts);
			tex.resName = XML_LookupStringAttribute("ResName", atts);
			p->ParameterOverrides[name] = tex;
		}
		else if ( 0 == _tcscmp( name, _T("Vector4f") ) )
		{
			Vector4f value;
			_tstring name = XML_LookupStringAttribute("Name", atts);
			//const XML_CHAR * tval = XML_LookupStringAttribute("value", atts);
			//sscanf(tval, "%f %f %f %f", &value.x, &value.y, &value.z, &value.z);
			//p->ParameterOverrides[name] = tval;
			stringstream sval(XML_LookupStringAttribute("value", atts));
			sval >> value.x >> value.y >> value.y >> value.w;
			p->ParameterOverrides[name] = value;
		}
		else if ( 0 == _tcscmp( name, _T("Float") ) )
		{
			float value;
			_tstring name = XML_LookupStringAttribute("Name", atts);
			
			stringstream sval(XML_LookupStringAttribute("value", atts));
			sval >> value;
			p->ParameterOverrides[name] = value;
		}
		else if ( 0 == _tcscmp( name, _T("Vector2f") ) )
		{
			Vector2f value;
			_tstring name = XML_LookupStringAttribute("Name", atts);
			stringstream sval(XML_LookupStringAttribute("value", atts));
			sval >> value.x >> value.y ;
			p->ParameterOverrides[name] = value;
		}
		else if ( 0 == _tcscmp( name, _T("Vector3f") ) )
		{
			Vector4f value;
			_tstring name = XML_LookupStringAttribute("Name", atts);
			stringstream sval(XML_LookupStringAttribute("value", atts));
			sval >> value.x >> value.y >> value.y;
			p->ParameterOverrides[name] = value;
		}
		else if ( 0 == _tcscmp( name, _T("Matrix4x4f") ) )
		{
			Matrix44 m;
			_tstring name = XML_LookupStringAttribute("Name", atts);
			stringstream sval(XML_LookupStringAttribute("value", atts));
			sval >> m[0][0] >> m[0][1] >> m[0][2] >> m[0][3]
				 >> m[1][0] >> m[1][1] >> m[1][2] >> m[1][3]
				 >> m[2][0] >> m[2][1] >> m[2][2] >> m[2][3]
				 >> m[3][0] >> m[3][1] >> m[3][2] >> m[3][3]
				;
			p->ParameterOverrides[name] = m;
		}
		else if ( 0 == _tcscmp( name, _T("Vector4fArray") ) )
		{
			_tstring name = XML_LookupStringAttribute("Name", atts);
			p->ParameterOverrides[name] = Vector4fArray();
			Vector4fArray &m = any_cast<Vector4fArray &>(p->ParameterOverrides[name]);

			stringstream szval(XML_LookupStringAttribute("Size", atts));
			int size = 0;
			szval >> size;
			m.resize(size);

			stringstream sval(XML_LookupStringAttribute("value", atts));
			for (int i=0; i<size; ++i)
			{
				Vector4f& value = m[i];
				sval >> value.x >> value.y >> value.y >> value.w;
			}
		}
		else
		{
			DAOLog::Debug("Unknown Property Type: %s", name);
		}

	}
	catch (...)
	{
	}
}

void MAOParser::EndElementHandler( XML_Parser parser, const XML_Char *name )
{

}