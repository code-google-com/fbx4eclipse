/**********************************************************************
*<
FILE: MATFile.h

DESCRIPTION:	MAT File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once

#include "MATCommon.h"
#include "MAT/Material.h"

namespace DAO {
namespace MAT {


class MATFile
{
	class Impl;
	ValuePtr<Impl> pimpl;
public:
	MATFile();
	~MATFile();

	virtual void open(const _tstring& filename);
	virtual void open(IDAOStream& stream);

	virtual void save(const _tstring& filename);
	virtual void save(IDAOStream& stream);

	virtual void dump();
	virtual void dump(const _tstring& filename);

	MaterialRef get_Object() ;
	MaterialRef get_Object() const;

};
}
}