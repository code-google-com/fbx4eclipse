/**********************************************************************
*<
FILE: GFFField.h

DESCRIPTION:	GR2 File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#pragma once 

#include "GFFCommon.h"

#include <pshpack1.h>

namespace DAO {
namespace GFF {

class GFFStruct;
typedef ValueRef<GFFStruct> GFFStructRef;
typedef ValuePtr<GFFStruct> GFFStructPtr;

class GFFList;
typedef ValueRef<GFFList> GFFListRef;
typedef ValuePtr<GFFList> GFFListPtr;

///<summary>
/// Definition of a field.
///</summary>
struct GFFFieldDef
{
	GFFID Label;
	FieldDataType TypeID;
	FieldFlag Flags;
	unsigned long Index;
	BEGIN_INI_MAP(GFFFieldDef)
		ADDCLASS(Label)
		ADDCLASS(TypeID)
		ADDCLASS(Flags)
		ADDCLASS(Index)
	END_INI_MAP()
};
typedef ValueRef<GFFFieldDef> GFFFieldDefRef;
typedef ValuePtr<GFFFieldDef> GFFFieldDefPtr;


///<summary>
/// Field Implementation
///</summary>
class GFFField
{
private:
	GFFFieldDefRef define;
	GFFStructRef owner;
protected:
	GFFField();
	GFFField(GFFStructRef owner, GFFFieldDefRef define);
	~GFFField();
public:
	static GFFField* Construct(GFFStructRef owner, GFFFieldDefRef define);
	static GFFField* Default();
	virtual void destroy() = 0;
	virtual LPVOID pdata() = 0;

	virtual GFFFieldDefRef getDefinition();

	virtual bool isPointer() const;
	virtual bool isPrimitive() const;
	virtual bool isComplex() const;
	virtual bool isEmpty() const;
	virtual bool isList() const;
	virtual size_t elementsize() const;

	virtual FieldDataType getFieldType();

	virtual Text asText();
	virtual signed char asInt8();
	virtual unsigned char asUInt8();
	virtual signed short asInt16();
	virtual unsigned short asUInt16();
	virtual signed long asInt32();
	virtual unsigned long asUInt32();
	virtual signed __int64 asInt64();
	virtual unsigned __int64 asUInt64();
	virtual float asFloat32();
	virtual double asFloat64();
	virtual Vector3f asVector3f();
	virtual Vector4f asVector4f();
	virtual Quaternion asQuat();
	virtual Text asECString();
	virtual Color4 asColor4();
	virtual Matrix44 asMatrix4();
	virtual Text asTlkString();
	virtual GFFListRef asList();
	virtual GFFStructRef asStruct();

	virtual void assign(char rhs);
	virtual void assign(unsigned char rhs);
	virtual void assign(short rhs);
	virtual void assign(unsigned short rhs);
	virtual void assign(long rhs);
	virtual void assign(unsigned long rhs);
	virtual void assign(float rhs);
	virtual void assign(double rhs);
	virtual void assign(__int64 rhs);
	virtual void assign(unsigned __int64 rhs);
	virtual void assign(LPCSTR rhs);
	virtual void assign(LPCWSTR rhs);
	virtual void assign(LPVOID rhs, int n);
	virtual void assign(const Vector3f& rhs);
	virtual void assign(const Vector4f& rhs);
	virtual void assign(const Quaternion& rhs);
	virtual void assign(const Color4& rhs);
	virtual void assign(const Matrix44& rhs);

	virtual size_t CopyData(LPVOID dst, size_t size);
	virtual int CopyText(LPTSTR value, int len);

	virtual bool Recall(IDAOStream& in);
	virtual bool Store(IDAOStream& out) const;
	virtual void Dump(IDAODumpStream& out, LPCTSTR name);

};

//////////////////////////////////////////////////////////////////////////

template<>
struct VPTraits<GFFField>
{
	static  GFFField *  create()  { return GFFField::Default(); }
	static  GFFField *  clone( GFFField const * p )  { return NULL; }
	static  GFFField *  construct( GFFField const * p )  { return const_cast<GFFField*>(p); }
	static  GFFField *  release( GFFField * p )  { 
		if (p) p->destroy();
		return NULL; 
	}
};

typedef ValueRef<GFFField> GFFFieldRef;
typedef ValuePtr<GFFField> GFFFieldPtr;

} // namespace GFF
} // namespace DAO

ROOTVARTYPE(DAO::GFF::GFFFieldDef);

template<> bool ::Recall(IDAOStream& in, DAO::GFF::GFFField& val);
template<> bool ::Store(IDAOStream& out, DAO::GFF::GFFField const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, DAO::GFF::GFFField const & val);

#include <poppack.h>
