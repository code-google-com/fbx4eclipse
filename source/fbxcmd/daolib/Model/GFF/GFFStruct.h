/**********************************************************************
*<
FILE: GFFFile.h

DESCRIPTION:	GR2 File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once 

#include "GFFCommon.h"
#include "GFFField.h"

#include <pshpack1.h>

namespace DAO {
namespace GFF {


struct GFFInfo;

///<summary>
/// Definition of a field.
///</summary>
struct GFFStructDef
{
	ShortString StructType;
	unsigned long FieldCount;
	unsigned long FieldOffset;
	unsigned long StructSize;

	DAOArray< GFFFieldDefPtr > Fields; // note: field stored/recalled by GFFInfo

	BEGIN_INI_MAP(GFFStructDef)
		ADDCLASS(StructType)
		ADDCLASS(FieldCount)
		ADDCLASS(FieldOffset)
		ADDCLASS(StructSize)
	END_INI_MAP()
};
typedef ValueRef<GFFStructDef> GFFStructDefRef;
typedef ValuePtr<GFFStructDef> GFFStructDefPtr;

///<summary>
/// Implementation of a struct
///</summary>
class GFFStruct
{
private:
	friend class GFFField;
	// Template definition
	const GFFInfo& _owner;
	const GFFStructDefRef define;
	// raw data pointer
	void *pdata; 
	typedef std::map<GFFID, GFFFieldPtr> GFFFieldMap;
	GFFFieldMap fields;
public:
	GFFStruct(const GFFInfo& owner, const GFFStructDef& define);
	~GFFStruct();

	const GFFInfo&  getInfo()						{ return _owner; }
	GFFStructDefRef getDefinition()	const			{ return define; }

	bool Recall(IDAOStream& in);
	bool Store(IDAOStream& out) const;
	void Dump(IDAODumpStream& out, LPCTSTR name) const;

	GFFFieldRef GetField(const GFFID& key) const {
		GFFFieldMap::const_iterator itr = fields.find(key);
		if (itr == fields.end())
			return GFFFieldRef();
		return GFFFieldRef((*itr).second);
	}

	GFFFieldRef operator[](const GFFID& key){ return GetField(key); }

	GFFFieldRef operator[](const GFFID& key) const { return GetField(key); }


};
typedef ValueRef<GFFStruct> GFFStructRef;
typedef ValuePtr<GFFStruct> GFFStructPtr;



template <class T> 
ValuePtr<T> StructPtrCast( const GFFStructRef& object ) {
	if ( !object.isNull() && object->getDefinition()->StructType.Compare( T::Type(), true ) == 0 )
		return ValuePtr<T>( new T(object) );
	return ValuePtr<T>(NULL);
}

} // namespace GFF
} // namespace DAO

ROOTVARTYPE(DAO::GFF::GFFStructDef);
ROOTVARTYPE(DAO::GFF::GFFStruct);




#include <poppack.h>
