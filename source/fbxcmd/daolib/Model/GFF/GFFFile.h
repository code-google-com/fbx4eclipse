/**********************************************************************
*<
FILE: GFFFile.h

DESCRIPTION:	GR2 File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include "GFFCommon.h"
#include "GFFStruct.h"
#include "GFFField.h"

#include <pshpack1.h>

namespace DAO {
namespace GFF {

//////////////////////////////////////////////////////////////////////////

struct GFFHeader
{
	ShortString GFFMagicNumber;
	ShortString GFFVersion;
	ShortString TargetPlatform;
	ShortString FileType;
	ShortString FileVersion;
	unsigned long StructCount;
	unsigned long DataOffset;

BEGIN_INI_MAP(GFFHeader)
	ADDITEM(GFFMagicNumber,  ShortString("GFF "))
	ADDITEM(GFFVersion,		 ShortString("V4.0"))
	ADDITEM(TargetPlatform,  ShortString("PC  "))
	ADDCLASS(FileType)
	ADDCLASS(FileVersion)
	ADDCLASS(StructCount)
	ADDCLASS(DataOffset)
END_INI_MAP()
};

struct GFFInfo
{
	GFFHeader Header;

	DAOArray< GFFStructDefPtr > Structs;

	BEGIN_INI_MAP(GFFInfo)
		ADDCLASS(Header)
		ADDARRAY(Structs, Header.StructCount)
	END_INI_MAP()
};

//////////////////////////////////////////////////////////////////

class GFFFile
{
	GFFStructPtr rootStruct;
	_tstring filename;
	GFFInfo fileInfo;
public:
	GFFFile();
	~GFFFile();

	virtual void open(const _tstring& filename);
	virtual void open(IDAOStream& stream);

	virtual void save(const _tstring& filename);
	virtual void save(IDAOStream& stream);

	virtual void dump();
	virtual void dump(const _tstring& filename);

	const _tstring& get_filename() const { return filename; }
	void set_filename(const _tstring& value) { filename = value; }

	GFFStructRef get_RootStruct() { return GFFStructRef(rootStruct); }
	GFFStructRef get_RootStruct() const { return GFFStructRef(rootStruct); }

	const GFFInfo & get_FileInfo() const { return fileInfo; }

};


} // namespace GFF
} // namespace DAO

ROOTVARTYPE(DAO::GFF::GFFHeader);
ROOTVARTYPE(DAO::GFF::GFFInfo);

#include <poppack.h>
