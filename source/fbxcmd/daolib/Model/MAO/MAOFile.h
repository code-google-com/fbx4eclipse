/**********************************************************************
*<
FILE: MAOFile.h

DESCRIPTION:	MAO File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "MAOCommon.h"
#include "MAO/MaterialObject.h"

namespace DAO {
namespace MAO {


class MAOFile
{
	class Impl;
	ValuePtr<Impl> pimpl;
public:
	MAOFile();
	~MAOFile();

	virtual void open(const _tstring& filename);
	virtual void open(IDAOStream& stream);

	virtual void save(const _tstring& filename);
	virtual void save(IDAOStream& stream);

	virtual void dump();
	virtual void dump(const _tstring& filename);

	MaterialObjectRef get_Object() ;
	MaterialObjectRef get_Object() const;

};
}
}