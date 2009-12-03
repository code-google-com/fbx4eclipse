/**********************************************************************
*<
FILE: MATFile.h

DESCRIPTION:	MAT File Format

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
#include "MATFile.h"
#include "cxmlparser.h"
#include "DataHeader.h"
#include "daolog.h"

using namespace std;
using namespace boost;
using namespace DAO;
using namespace DAO::MAT;

class MATParser
{
	Context context;
	Material *p;
public:
	void set_Object(Material*pObject){
		this->p = pObject;
	}
	void StartElementHandler(XML_Parser parser, const XML_Char *name, const XML_Char **atts);
	void EndElementHandler(XML_Parser parser, const XML_Char *name);
};


//////////////////////////////////////////////////////////////////////////

class MATFile::Impl
{
public:
	MaterialPtr root;
};

DAO::MAT::MATFile::MATFile()
{

}

DAO::MAT::MATFile::~MATFile()
{

}

void DAO::MAT::MATFile::open( const _tstring& filename )
{
	CXmlParser<MATParser> modelParser;
	modelParser.set_Object( this->pimpl->root.ToPointer() );
	modelParser.Reset();
	modelParser.ParseFile(filename.c_str());
}

void DAO::MAT::MATFile::open( IDAOStream& stream )
{
	CXmlParser<MATParser> modelParser;
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

void DAO::MAT::MATFile::save( const _tstring& filename )
{

}

void DAO::MAT::MATFile::save( IDAOStream& stream )
{

}

void DAO::MAT::MATFile::dump()
{

}

void DAO::MAT::MATFile::dump( const _tstring& filename )
{

}

DAO::MAT::MaterialRef DAO::MAT::MATFile::get_Object()
{
	return MaterialRef(this->pimpl->root);
}

DAO::MAT::MaterialRef DAO::MAT::MATFile::get_Object() const
{
	return MaterialRef(this->pimpl->root);
}

//////////////////////////////////////////////////////////////////////////

static bool ParseParameterSet( const XML_Char * name, const XML_Char ** atts, ParameterSet &set )
{
	if ( 0 == _tcscmp( name, _T("Parameter") ) )
	{
		const XML_Char *type = XML_LookupTextAttribute("Type", atts);
		const XML_Char *name = XML_LookupTextAttribute("Name", atts);
		int size = XML_LookupIntAttribute("Size", atts, 0);
		if (type == NULL || name == NULL)
			return false;

		stringstream sval(XML_LookupStringAttribute("Default", atts));

		if ( 0 == _tcscmp( type, _T("Float") ) )
		{
			float value;
			sval >> value;
			set[name] = value;
		}
		else if ( 0 == _tcscmp( type, _T("Vector2f") ) )
		{
			Vector2f value;
			sval >> value.x >> value.y ;
			set[name] = value;
		}
		else if ( 0 == _tcscmp( type, _T("Vector3f") ) )
		{
			Vector4f value;
			sval >> value.x >> value.y >> value.z;
			set[name] = value;
		}
		else if ( 0 == _tcscmp( type, _T("Vector4f") ) )
		{
			Vector4f value;
			sval >> value.x >> value.y >> value.z >> value.w;
			set[name] = value;
		}
		else if ( 0 == _tcscmp( type, _T("Matrix4x4f") ) )
		{
			Matrix44 m;
			sval >> m[0][0] >> m[0][1] >> m[0][2] >> m[0][3]
			>> m[1][0] >> m[1][1] >> m[1][2] >> m[1][3]
			>> m[2][0] >> m[2][1] >> m[2][2] >> m[2][3]
			>> m[3][0] >> m[3][1] >> m[3][2] >> m[3][3]
			;
			set[name] = m;
		}
		else if ( 0 == _tcscmp( type, _T("Vector4fArray") ) )
		{
			set[name] = Vector4fArray();
			Vector4fArray &m = any_cast<Vector4fArray &>(set[name]);
			m.resize(size);
			for (int i=0; i<size; ++i)
			{
				Vector4f& value = m[i];
				sval >> value.x >> value.y >> value.y >> value.w;
			}
		}
		else if ( 0 == _tcscmp( type, _T("Texture") ) )
		{
			set[name] = Texture();
			Texture &m = any_cast<Texture&>(set[name]);
			m.Name = XML_LookupStringAttribute("Name", atts);
			m.SamplerName = XML_LookupStringAttribute("SamplerName", atts);
			m.Default = XML_LookupStringAttribute("Default", atts);
		}
		else
		{
			DAOLog::Debug("Unknown parameter specified: '%s'", type);
		}
		return true;
	}
	return false;
}

struct PassContext : ParserContext<Pass>
{
	typedef ParserContext<Pass> base;
	PassContext( Pass* val ) : base(val) {}
	PassContext( Pass& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		Pass& val = this->ref();
		if ( _tcscmp(name, TEXT("Pass")) == 0 )
		{
			val.Name = XML_LookupStringAttribute("Name", atts);
			val.VertexShader = XML_LookupStringAttribute("VertexShader", atts);
			val.PixelShader = XML_LookupStringAttribute("PixelShader", atts);
			val.State = XML_LookupStringAttribute("State", atts);
			return true;
		} else if ( 0 == _tcscmp( name, _T("ParameterSetRef") ) ) {
			val.RefName = XML_LookupStringAttribute("Ref", atts);
		} else if ( 0 == _tcscmp( name, _T("InternalParameter") ) ) {
			return true;
		} else {
			Pass& val = this->ref();
			if ( ParseParameterSet(name, atts, val.Parameters) )
				return true;
		}
		return false;
	}
};

struct TechniqueContext : ParserContext<Technique>
{
	typedef ParserContext<Technique> base;
	TechniqueContext( Technique* val ) : base(val) {}
	TechniqueContext( Technique& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		Technique& val = this->ref();
		if ( 0 == _tcscmp( name, _T("Technique") ) ) {
			val.Name = XML_LookupStringAttribute("Name", atts);
			return true;
		} else if ( 0 == _tcscmp( name, _T("Pass") ) ) {
			context.contexts.push_front( new PassContext(val.Pass) );
		} else if ( 0 == _tcscmp( name, _T("LogicTest") ) ) {
			context.contexts.push_front( new PassContext(val.Pass) );
		}
		return false;
	}
};

struct LogicContext : ParserContext<Logic>
{
	typedef ParserContext<Logic> base;
	LogicContext( Logic* val ) : base(val) {}
	LogicContext( Logic& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		Logic& val = this->ref();
		if ( 0 == _tcscmp( name, _T("Logic") ) ) {
			val.Note = XML_LookupStringAttribute("Note", atts);
			return true;
		} else if ( 0 == _tcscmp( name, _T("Technique") ) ) {
			context.contexts.push_front( new TechniqueContext(val.Technique) );
		}
		return false;
	}
};


struct LogicRootContext : ParserContext<LogicRoot>
{
	typedef ParserContext<LogicRoot> base;
	LogicRootContext( LogicRoot* val ) : base(val) {}
	LogicRootContext( LogicRoot& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		LogicRoot& val = this->ref();
		if ( 0 == _tcscmp( name, _T("Logic") ) ) {
			if (val.Note.empty()) {
				val.Note = XML_LookupStringAttribute("Note", atts);
				return true;
			} else {
				LogicList::iterator pr = val.Logic.insert(val.Logic.end(), Logic() );
				if (pr != val.Logic.end()) {
					context.contexts.push_front( new LogicContext(*pr) );
				}
				return false;
			}
		}
		return false;
	}
};


struct SemanticContext : ParserContext<Semantic>
{
	typedef ParserContext<Semantic> base;
	SemanticContext( Semantic* val ) : base(val) {}
	SemanticContext( Semantic& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		Semantic& val = this->ref();
		if ( 0 == _tcscmp( name, _T("Semantic") ) ) {
			val.Name = XML_LookupStringAttribute("Name", atts);
			val.RenderClass = XML_LookupStringAttribute("RenderClass", atts);
			return true;
		} else if ( 0 == _tcscmp( name, _T("Logic") ) ) {
			context.contexts.push_front( new LogicRootContext(val.Logic) );
			return false;
		}
		return false;
	}
};

struct ParameterSetContext : ParserContext<ParameterSet>
{
	typedef ParserContext<ParameterSet> base;
	ParameterSetContext( ParameterSet* val ) : base(val) {}
	ParameterSetContext( ParameterSet& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		ParameterSet& set = this->ref();
		if ( 0 == _tcscmp( name, _T("ParameterSet") ) ) {
			return true;
		}else if ( 0 == _tcscmp( name, _T("InternalParameter") ) ) {
			return true;
		} else {
			if ( ParseParameterSet(name, atts, set) )
				return true;
		}
		return false;
	}
};

//////////////////////////////////////////////////////////////////////////

struct MaterialContext : ParserContext<Material>
{
	typedef ParserContext<Material> base;
	MaterialContext( Material* val ) : base(val) {}
	MaterialContext( Material& val ) : base(val) {}

	bool StartElementHandler(Context& context, const XML_Char *name, const XML_Char **atts) 
	{
		Material& val = this->ref();
		if ( _tcscmp(name, TEXT("Material")) == 0 ) {
			val.Name = XML_LookupStringAttribute("Name", atts);
			return true;
		} else if ( _tcscmp(name, TEXT("ParameterSet")) == 0 ) {
			ParameterSets& list = val.ParameterSets;
			_tstring id = XML_LookupStringAttribute("Name", atts);
			std::pair<ParameterSets::iterator, bool> pr = list.insert( ParameterSets::value_type(id, ParameterSet()) );
			if (pr.second) {
				ParameterSet& val = (*pr.first).second;
				context.contexts.push_front( new ParameterSetContext( val ) );
			}
		} else if ( _tcscmp(name, TEXT("Semantic")) == 0 ) {
			SemanticMap& list = val.Semantics;
			_tstring id = XML_LookupStringAttribute("Name", atts);
			std::pair<SemanticMap::iterator, bool> pr = list.insert( SemanticMap::value_type(id, Semantic()) );
			if (pr.second) {
				Semantic& val = (*pr.first).second;
				context.contexts.push_front( new SemanticContext( val ) );
			}
		}
		return false;
	}
};


//////////////////////////////////////////////////////////////////////////
void MATParser::StartElementHandler( XML_Parser parser, const XML_Char *name, const XML_Char **atts )
{
	context.parser = parser;
	size_t len = context.contexts.size();
	if ( _tcscmp(name, TEXT("Material")) == 0 ) {
		if (p) { context.contexts.push_front( new MaterialContext(p) ); }
	}
	bool handled = false;
	if (!context.contexts.empty())
	{
		handled = context.contexts.front()->StartElementHandler(context, name, atts);
		if (!handled && (len == context.contexts.size()))
			handled = true; // element handler was not added
	}
	if (!handled)
	{
		for (ContextList::iterator itr = context.contexts.begin()
			; itr != context.contexts.end()
			; ++itr
			)
		{
			if ( IParserContext* localcontext = (*itr) ) {
				if ( localcontext->StartElementHandler(context, name, atts) ) {
					handled = true;
					break;
				}
			}
		}
	}
	if (len == context.contexts.size())
	{
		// Add empty handler
		context.contexts.push_front( new ParserContext<void*>(NULL) );
	}
}

void MATParser::EndElementHandler( XML_Parser parser, const XML_Char *name )
{
	if (!context.contexts.empty()) {
		if ( IParserContext* localcontext = context.contexts.front() ) {
			localcontext->EndElementHandler(context, name);
			context.contexts.pop_front();
			delete localcontext;
		}
	}
}