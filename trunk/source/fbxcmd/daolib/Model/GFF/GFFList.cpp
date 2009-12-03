/**********************************************************************
*<
FILE: GFFList.h

DESCRIPTION:	GFF File Format

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFFList.h"
#include "GFFStruct.h"
#include "GFFFile.h"
#include "GFFField.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;

//////////////////////////////////////////////////////////////////////////

GFFList::GFFList(GFFStructRef owner, GFFFieldDefRef define)
{
	this->owner = owner;
	this->define = define;
	data = NULL;
	len = 0;
}

GFFList::~GFFList()
{
	if (data) free(data);
}

LPCVOID GFFList::pdata() const					{ return data; }
size_t GFFList::size() const					{ return count() * elementsize(); }

size_t GFFList::count() const					{ return len; }
void GFFList::resize(size_t len)				{ this->len = len; data = realloc(data, elementsize()*len); }

Text GFFList::asText( int idx )					{ return Text(); }
signed char GFFList::asInt8( int idx )			{ return 0; }
unsigned char GFFList::asUInt8( int idx )		{ return 0; }
signed short GFFList::asInt16( int idx )		{ return 0; }
unsigned short GFFList::asUInt16( int idx )		{ return 0; }
signed long GFFList::asInt32( int idx )			{ return 0; }
unsigned long GFFList::asUInt32( int idx )		{ return 0; }
signed __int64 GFFList::asInt64( int idx )		{ return 0; }
unsigned __int64 GFFList::asUInt64( int idx )	{ return 0; }
float GFFList::asFloat32( int idx )				{ return 0; }
double GFFList::asFloat64( int idx )			{ return 0; }
Vector3f GFFList::asVector3f( int idx )				{ return Vector3f(); }
Vector4f GFFList::asVector4f( int idx )				{ return Vector4f(); }
Quaternion GFFList::asQuat( int idx )					{ return Quaternion(); }
Text GFFList::asECString( int idx )				{ return Text(); }
Color4 GFFList::asColor4( int idx )				{ return Color4(); }
Matrix44 GFFList::asMatrix4( int idx )			{ return Matrix44(); }
Text GFFList::asTlkString( int idx )			{ return Text(); }
GFFStructRef GFFList::asStruct( int idx )		{ return GFFStructRef(NULL); }
GFFFieldRef GFFList::asField(int idx)			{ return GFFFieldRef(NULL); }

void GFFList::assign(int idx, char rhs)				{}
void GFFList::assign(int idx, unsigned char rhs)	{}
void GFFList::assign(int idx, short rhs)			{}
void GFFList::assign(int idx, unsigned short rhs)	{}
void GFFList::assign(int idx, int rhs)				{}
void GFFList::assign(int idx, unsigned int rhs)		{}
void GFFList::assign(int idx, float rhs)			{}
void GFFList::assign(int idx, double rhs)			{}
void GFFList::assign(int idx, __int64 rhs)			{}
void GFFList::assign(int idx, unsigned __int64 rhs)	{}
void GFFList::assign(int idx, LPCSTR rhs)			{}
void GFFList::assign(int idx, LPCWSTR rhs)			{}
void GFFList::assign(int idx, LPVOID rhs, int n )	{}
void GFFList::assign(int idx, const Vector3f& rhs)	{}
void GFFList::assign(int idx, const Vector4f& rhs)	{}
void GFFList::assign(int idx, const Quaternion& rhs)		{}
void GFFList::assign(int idx, const Color4& rhs)		{}
void GFFList::assign(int idx, const Matrix44& rhs)	{}

size_t GFFList::CopyData( LPVOID dst, size_t size, int idx ) { return 0; }
int GFFList::CopyText( LPTSTR value, int len, int idx )		 { return 0; }

bool GFFList::Recall( IDAOStream& in )						{ return false; }
bool GFFList::Store( IDAOStream& in ) const					{ return false; }

void GFFList::Dump(IDAODumpStream& out, LPCTSTR name) const {
	out.Indent();
	out.PrintF("%-30s\t(size=%d)\n", name, count());
	out.IncreaseIndent();
	uint len = 0;
	TCHAR buffer[100];
	buffer[0] = 0;
	for (size_t i=0; i<count(); ++i) {
		GFFList* pThis = const_cast<GFFList*>(this);
		_stprintf(buffer, _T("[%d]"), i);
		Text txt = pThis->asText(i);
		::Dump(out, buffer, txt);
	}
	out.DecreaseIndent();
}



template <> void Dump(struct IDAODumpStream &out, LPCSTR name, class GFFList const &val)
{
	val.Dump(out, name);
}

//////////////////////////////////////////////////////////////////////////


template <class T>
class GFFPrimitiveList : public GFFList
{
protected:
	T* ptr(int idx) const { return (idx >= 0 && idx < len) ? ((T*)data) + idx : NULL; }
	T get_value(int idx) const { T* p = ptr(idx); return p ? *p : 0; }
	void set_value(int idx, T value)  { T* p = ptr(idx); if (p) *p = value; }
protected:	
	friend GFFList;
	GFFPrimitiveList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	virtual void destroy() { delete this; }

	size_t elementsize() const					{ return sizeof(T); }
	bool isPrimitive()							{ return true; }
	bool isComplex()							{ return false; }

	Text asText( int idx ) { 
		_tstringstream sstr;
		sstr << get_value(idx);
		return sstr.str().c_str();
	}

	signed char asInt8( int idx )				{ return (signed char)get_value(idx); }
	unsigned char asUInt8( int idx )			{ return (unsigned char)get_value(idx); }
	signed short asInt16( int idx )				{ return (signed short)get_value(idx); }
	unsigned short asUInt16( int idx )			{ return (unsigned short)get_value(idx); }
	signed long asInt32( int idx )				{ return (signed long)get_value(idx); }
	unsigned long asUInt32( int idx )			{ return (unsigned long)get_value(idx); }
	signed __int64 asInt64( int idx )			{ return (signed __int64)get_value(idx); }
	unsigned __int64 asUInt64( int idx )		{ return (unsigned __int64)get_value(idx); }
	float asFloat32( int idx )					{ return (float)get_value(idx); }
	double asFloat64( int idx )					{ return (double)get_value(idx); }

	void assign(int idx, char rhs)				{ set_value(idx, (T)rhs); }
	void assign(int idx, unsigned char rhs)		{ set_value(idx, (T)rhs); }
	void assign(int idx, short rhs)				{ set_value(idx, (T)rhs); }
	void assign(int idx, unsigned short rhs)	{ set_value(idx, (T)rhs); }
	void assign(int idx, int rhs)				{ set_value(idx, (T)rhs); }
	void assign(int idx, unsigned int rhs)		{ set_value(idx, (T)rhs); }
	void assign(int idx, float rhs)				{ set_value(idx, (T)rhs); }
	void assign(int idx, double rhs)			{ set_value(idx, (T)rhs); }
	void assign(int idx, __int64 rhs)			{ set_value(idx, (T)rhs); }
	void assign(int idx, unsigned __int64 rhs)	{ set_value(idx, (T)rhs); }

	virtual bool Recall(IDAOStream& in) {
		bool ok = ::Recall(in, len);
		resize(len);
		for (int i=0; i<len && ok; ++i) {
			ok = ::Recall(in, *ptr(i));
		}
		return ok;
	}

	virtual bool Store(IDAOStream& out) const {
		bool ok = ::Store(out, len);
		for (int i=0; i<len && ok; ++i) {
			GFFPrimitiveList<T>* pThis = const_cast< GFFPrimitiveList<T> *>(this);
			ok = ::Store<T>(out, *pThis->ptr(i));
		}
		return ok;
	}

	virtual void Dump(IDAODumpStream& out, LPCTSTR name) const {
		out.Indent();
		out.PrintF("%-30s\t(size=%d)\n", name, count());
		out.IncreaseIndent();
		TCHAR buffer[100]; buffer[0] = 0;
		size_t sz = min(10, count());
		for (size_t i=0; i<sz; ++i) {
			GFFPrimitiveList<T>* pThis = const_cast< GFFPrimitiveList<T> *>(this);
			_stprintf(buffer, _T("[%d]"), i);
			::Dump<T>(out, buffer, *pThis->ptr(i));
		}
		out.DecreaseIndent();
	}

};

class GFFBinaryList : public GFFPrimitiveList<unsigned char>
{
protected:	
	friend GFFList;
	GFFBinaryList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFPrimitiveList<unsigned char>(owner, define) {}
public:
	virtual void destroy() { delete this; }

	virtual void Dump(IDAODumpStream& out, LPCTSTR name) const {
		out.Indent();
		static char *hex = "0123456789ABCDEF";
		Text value;
		value.reserve(len * 3);
		int l = min(20, len);
		for (int i=0; i<l; ++i) {
			byte b = get_value(i); 
			value.AppendFormat("%c%c ", hex[ b >> 4 ], hex[ b & 0xF ]);
		}
		if (l < len) value.append("...");
		out.PrintF("%-30s\t[%6d] %s\n", name, len, value.data());
	}
};

//////////////////////////////////////////////////////////////////////////

class GFFVector3fList : public GFFList
{
	typedef Vector3f T ;
	T* ptr(int idx) { return (idx >= 0 && idx < len) ? ((T*)data) + idx : NULL; }
	void set_value(int idx, const T& value)  { T* p = ptr(idx); if (p) *p = value; }
protected:
	friend GFFList;
	GFFVector3fList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	bool isComplex()							{ return false; }
	virtual size_t elementsize() const			{ return sizeof(T); }
	Vector3f asVector3f(int idx)				{ T* p = ptr(idx); return p ? *p : T(); }
	void assign(int idx, const Vector3f& rhs)	{ set_value(idx, rhs); }
};

//////////////////////////////////////////////////////////////////////////

class GFFVector4fList : public GFFList
{
	typedef Vector4f T;
	T* ptr(int idx) { return (idx >= 0 && idx < len) ? ((T*)data) + idx : NULL; }
	void set_value(int idx, const T& value)  { T* p = ptr(idx); if (p) *p = value; }
protected:
	friend GFFList;
	GFFVector4fList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	bool isComplex()							{ return false; }
	virtual size_t elementsize() const			{ return sizeof(T); }
	Vector4f asVector4f(int idx)				{ T* p = ptr(idx); return p ? *p : T(); }
	void assign(int idx, const Vector4f& rhs)	{ set_value(idx, rhs); }
};

//////////////////////////////////////////////////////////////////////////

class GFFColor4List : public GFFList
{
	typedef Color4 T;
	T* ptr(int idx) { return (idx >= 0 && idx < len) ? ((T*)data) + idx : NULL; }
	void set_value(int idx, const T& value)  { T* p = ptr(idx); if (p) *p = value; }
protected:
	friend GFFList;
	GFFColor4List(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	bool isComplex()						{ return false; }
	virtual size_t elementsize() const		{ return sizeof(T); }
	Color4 asColor4(int idx)				{ T* p = ptr(idx); return p ? *p : T(); }
	void assign(int idx, const Color4& rhs)	{ set_value(idx, rhs); }
};

//////////////////////////////////////////////////////////////////////////

class GFFMatrix4List : public GFFList
{
	typedef Matrix44 T;
	T* ptr(int idx) { return (idx >= 0 && idx < len) ? ((T*)data) + idx : NULL; }
	void set_value(int idx, const T& value)  { T* p = ptr(idx); if (p) *p = value; }
protected:
	friend GFFList;
	GFFMatrix4List(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	bool isComplex()						{ return false; }
	virtual size_t elementsize() const		{ return sizeof(T); }
	Matrix44 asMatrix4(int idx)				{ T* p = ptr(idx); return p ? *p : T(); }
	void assign(int idx, const Matrix44& rhs)	{ set_value(idx, rhs); }
};

//////////////////////////////////////////////////////////////////////////

class GFFQuatList : public GFFList
{
	typedef Quaternion T;
	T* ptr(int idx) { return (idx >= 0 && idx < len) ? ((T*)data) + idx : NULL; }
	void set_value(int idx, const T& value)  { T* p = ptr(idx); if (p) *p = value; }
protected:
	friend GFFList;
	GFFQuatList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	bool isComplex()						{ return false; }
	virtual size_t elementsize() const		{ return sizeof(T); }
	Quaternion asQuat(int idx)					{ T* p = ptr(idx); return p ? *p : T(); }
	void assign(int idx, const Quaternion& rhs)	{ set_value(idx, rhs); }
};


//////////////////////////////////////////////////////////////////////////

class GFFTextList : public GFFList
{
	typedef Text T;
protected:
	friend GFFList;
	GFFTextList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	bool isComplex()						{ return true; }
	virtual size_t elementsize() const		{ return sizeof(long); }
	Text asText(int idx)					{ return T(); }
	void assign(int idx, const Text& rhs)	{ }
};


//////////////////////////////////////////////////////////////////////////

class GFFStructList : public GFFList
{
	DAOArray< GFFStructPtr > list;

protected:
	friend GFFList;
	GFFStructList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	GFFStructDefRef getDefinition()			{ return GFFStructDefRef( owner->getInfo().Structs[ (short)define->TypeID ] ); }

	LPCVOID pdata() const					{ return &list[0]; }
	size_t count() const					{ return list.size(); }
	void resize(size_t len)					{ list.resize(len, GFFStructPtr(NULL)); }

	bool isComplex()						{ return true; }
	virtual size_t elementsize() const		{ return sizeof(LPVOID); }
	GFFStructRef asStruct(int idx)			{ return GFFStructRef(list[idx]); }

	virtual bool Recall(IDAOStream& in) {
		long len = 0;
		bool ok = ::Recall(in, len);
		resize(len);
		for (int i=0; i<len && ok; ++i) {
			GFFStructPtr& ptr = list[i];
			GFFInfo& info = const_cast<GFFInfo&>(owner->getInfo());
			GFFStructPtr(new GFFStruct(info, *getDefinition())).swap(ptr);

			if (define->Flags & FF_REFERENCE)
			{
				long data;
				bool ok = (in.Read(&data, sizeof(long)) == 1);
				if (!ok || data == 0 || data == 0xFFFFFFFF) {
					continue;
				}
				int pos = in.Tell();
				in.Seek(SEEK_SET, data);
				ok = ::Recall(in, ptr);
				in.Seek(SEEK_SET, pos);
			}
			else
			{
				ok = ::Recall(in, ptr);
			}
		}
		return ok;
	}

	virtual bool Store(IDAOStream& out) const {
		long len = list.count();
		bool ok = ::Store(out, len);
		for (int i=0; i<len && ok; ++i) {
			ok = ::Store(out, list[i]);
		}
		return ok;
	}


	virtual void Dump(IDAODumpStream& out, LPCTSTR name) const {
		::Dump(out, name, list);
	}
};


//////////////////////////////////////////////////////////////////////////
struct GFFGenericDef : GFFFieldDef
{
	//GFFGenericDef() : Label(GFF_INVALIDENTRY), TypeID(DT_GENERIC), Flags(0), Index(0) {}

	BEGIN_INI_MAP(GFFGenericDef)
		ADDCLASS(TypeID)
		ADDCLASS(Flags)
		ADDCLASS(Index)
	END_INI_MAP()
};
IMPLVARTYPE(GFFGenericDef);


class GFFGenericList : public GFFList
{
	DAOArray< GFFGenericDef > defines;
	DAOArray< GFFFieldPtr > list;
	typedef GFFList base;
protected:
	friend GFFList;
	GFFGenericList(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFList(owner, define) {}
public:
	virtual void destroy() { delete this; }

	LPCVOID pdata() const					{ return NULL; }
	size_t elementsize() const				{ return sizeof(short) + sizeof(short) + sizeof(long); }
	bool isPrimitive()						{ return false; }
	bool isComplex()						{ return true; }

	GFFFieldRef asField(int idx)			{ return GFFFieldRef(list[idx]); }

	void resize(size_t len){ 
		defines.resize(len);
		list.resize(len, GFFFieldPtr(NULL)); 
	}

	virtual bool Recall(IDAOStream& in) {
		bool ok = ::Recall(in, len);
		resize(len);
		for (int i=0; i<len && ok; ++i) {
			ok = ::Recall(in, defines[i]);
			defines[i].Label = GFF_INVALIDENTRY;
		}
		size_t pos = in.Tell();
		for (int i=0; i<len && ok; ++i) {
			GFFFieldDefRef fref ( &defines[i] );
			fref->Label = GFF_INVALIDENTRY;
			GFFFieldPtr fptr(GFFField::Construct(owner, fref));
			//DAOArray<GFFFieldPtr>::iterator itr = list.insert(list.end(), GFFFieldPtr());
			list[i].swap(fptr);

			in.Seek(SEEK_SET, fref->Index);
			ok = list[i]->Recall(in);
		}
		in.Seek(SEEK_SET, pos);
		return ok;
	}

	virtual bool Store(IDAOStream& out) const {
		bool ok = ::Store(out, len);
		for (int i=0; i<len && ok; ++i) {
			ok = ::Store(out, defines[i]);
		}
		return ok;
	}
	virtual void Dump(IDAODumpStream& out, LPCTSTR name) const {
		out.Indent();
		out.PrintF(_T("%-30s\n"), name);
		out.IncreaseIndent();
		::Dump(out, "Define", defines);
		::Dump(out, "Fields", list);
		out.DecreaseIndent();
	}
};

//////////////////////////////////////////////////////////////////////////

template<>
bool Recall(IDAOStream& in, DAO::GFF::GFFList& val)
{
	return val.Recall(in);
}

template<>
bool Store(IDAOStream& out, DAO::GFF::GFFList const & val)
{
	return val.Store(out);
}

GFFList* GFFList::Construct(GFFStructRef owner, GFFFieldDefRef define)
{
	FieldFlag flags = define->Flags;
	flags = (FieldFlag)(flags & ~FF_LIST);
	flags = (FieldFlag)(flags & ~FF_REFERENCE);

	if (flags == 0)
	{
		switch(define->TypeID)
		{
		case DT_GENERIC: 	return new GFFGenericList(owner, define);
		case DT_UINT8:  	return new GFFBinaryList(owner, define);
		case DT_INT8:  		return new GFFPrimitiveList<signed char>(owner, define);
		case DT_UINT16:  	return new GFFPrimitiveList<unsigned short>(owner, define);
		case DT_INT16:  	return new GFFPrimitiveList<signed short>(owner, define);
		case DT_UINT32:  	return new GFFPrimitiveList<unsigned long>(owner, define);
		case DT_INT32:  	return new GFFPrimitiveList<signed long>(owner, define);
		case DT_UINT64:  	return new GFFPrimitiveList<unsigned __int64>(owner, define);
		case DT_INT64:  	return new GFFPrimitiveList<signed __int64>(owner, define);
		case DT_FLOAT32:  	return new GFFPrimitiveList<float>(owner, define);
		case DT_FLOAT64:  	return new GFFPrimitiveList<double>(owner, define);

		case DT_VECTOR3F:  	return new GFFVector3fList(owner, define);
		case DT_VECTOR4F:  	return new GFFVector4fList(owner, define);
		case DT_QUATERNIONF:return new GFFQuatList(owner, define);
		case DT_ECSTRING:  	return new GFFTextList(owner, define);
		case DT_COLOR4F:  	return new GFFColor4List(owner, define);
		case DT_MATRIX4X4F: return new GFFMatrix4List(owner, define);
		case DT_TLKSTRING:  return new GFFGenericList(owner, define);
		}
	}
	else if (flags & FF_STRUCT)
	{
		return new GFFStructList(owner, define);
	}
	return NULL;
}