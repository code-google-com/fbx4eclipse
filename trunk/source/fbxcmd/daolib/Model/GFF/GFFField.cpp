/**********************************************************************
*<
FILE: GFFField.h

DESCRIPTION:	GFF File Format

HISTORY:

*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "GFFField.h"
#include "GFFStruct.h"
#include "GFFFile.h"
#include "GFFList.h"

using namespace std;
using namespace DAO;
using namespace DAO::GFF;

IMPLVARTYPE(DAO::GFF::GFFFieldDef);

const LPVOID NegativePointer = (LPVOID)0xFFFFFFFF;
const LPVOID NullPointer = (LPVOID)NULL;

GFFField::GFFField(GFFStructRef owner, GFFFieldDefRef define)
{
	this->owner = owner;
	this->define = define;
}
GFFField::GFFField() {}
GFFField::~GFFField() {}

GFFFieldDefRef GFFField::getDefinition(){ return define; }
bool GFFField::isPointer() const		{ return (this->define->Flags & FF_REFERENCE) == FF_REFERENCE; }
bool GFFField::isPrimitive() const		{ return false; }
bool GFFField::isComplex() const		{ return true; }
bool GFFField::isList() const			{ return false; }
bool GFFField::isEmpty() const			{ return false; }
FieldDataType GFFField::getFieldType()	{ return this->define->TypeID; }
size_t GFFField::elementsize() const	{ return 0; }

Text GFFField::asText()
{
	Text value;
	switch(this->define->TypeID)
	{
	case DT_INT8:       value.Format("%d", (int)asInt8()); break;
	case DT_UINT8:      value.Format("%u", (unsigned int)asUInt8()); break;
	case DT_INT16:      value.Format("%d", (int)asInt16()); break;
	case DT_UINT16:     value.Format("%u", (unsigned int)asUInt16()); break;
	case DT_INT32:      value.Format("%d", (int)asInt32()); break;
	case DT_UINT32:     value.Format("%u", (unsigned int)asUInt32()); break;
	case DT_INT64:      value.Format("%I64d", asInt64()); break;
	case DT_UINT64:     value.Format("%I64u", asUInt64()); break;
	case DT_FLOAT32:    value.Format("%g", (double)asFloat32()); break;
	case DT_FLOAT64:    value.Format("%g", asFloat64()); break;

	case DT_VECTOR3F:  	
		{ 
			Vector3f v = asVector3f(); 
			value.Format("%g,%g,%g", (double)v.x, (double)v.y, (double)v.z);
		} break;
	case DT_VECTOR4F:  	
		{ 
			Vector4f v = asVector4f(); 
			value.Format("%g,%g,%g,%g", (double)v.x, (double)v.y, (double)v.z, (double)v.w);
		} break;
	case DT_QUATERNIONF:
		{ 
			Quaternion v = asQuat(); 
			value.Format("%g,%g,%g,%g", (double)v.x, (double)v.y, (double)v.z, (double)v.w);
		} break;
	case DT_COLOR4F:  	
		{ 
			Color4 v = asColor4(); 
			value.Format("%g,%g,%g", (double)v.r, (double)v.g, (double)v.b);
		} break;
	case DT_MATRIX4X4F: 
		{ 
			Matrix44 v = asMatrix4(); 
			for ( int i=0; i<4; ++i) {
				Vector4f& r = v.GetRow(i); 
				value.AppendFormat("[%g,%g,%g,%g]", (double)r.x, (double)r.y, (double)r.z, (double)r.w);
			}
		} break;

	case DT_GENERIC: 
		if (!isList())
		{
			static char *hex = "0123456789ABCDEF";
			int len = elementsize();
			LPVOID p = pdata();
			value.reserve(len * 3);
			for (int i=0; i<len; ++i) {
				byte b = ((byte*)p)[i]; 
				value.AppendFormat("%c%c ", hex[ b >> 4 ], hex[ b & 0xF ]);
			}
		} break;
	}
	return value;
}

int GFFField::CopyText(LPTSTR value, int len)
{
	if (value != NULL)
	{
		Text v = asText(); 
		return _sntprintf(value, len, "%s", v.c_str());
	}
	return 0;
}


signed char GFFField::asInt8()			{ return 0; }
unsigned char GFFField::asUInt8()		{ return 0; }
signed short GFFField::asInt16()		{ return 0; }
unsigned short GFFField::asUInt16()		{ return 0; }
signed long GFFField::asInt32()			{ return 0; }
unsigned long GFFField::asUInt32()		{ return 0; }
signed __int64 GFFField::asInt64()		{ return 0; }
unsigned __int64 GFFField::asUInt64()	{ return 0; }
float GFFField::asFloat32()				{ return 0; }
double GFFField::asFloat64()			{ return 0; }
Vector3f GFFField::asVector3f()			{ return Vector3f(); }
Vector4f GFFField::asVector4f()			{ return Vector4f(); }
Quaternion GFFField::asQuat()			{ return Quaternion(); }
Text GFFField::asECString()				{ return Text(); }
Color4 GFFField::asColor4()				{ return Color4(); }
Matrix44 GFFField::asMatrix4()			{ return Matrix44(); }
Text GFFField::asTlkString()			{ return Text(); }
GFFListRef GFFField::asList()			{ return GFFListRef(); }
GFFStructRef GFFField::asStruct()		{ return GFFStructRef(); }

size_t GFFField::CopyData(LPVOID dst, size_t dlen) { 
	if (!isList() && !isComplex()) {
		size_t len = min( elementsize(), dlen );
		memcpy(dst, pdata(), len);
		return len; 
	}
	return 0;
}

void GFFField::assign(char rhs){}
void GFFField::assign(unsigned char rhs){}
void GFFField::assign(short rhs){}
void GFFField::assign(unsigned short rhs){}
void GFFField::assign(long rhs){}
void GFFField::assign(unsigned long rhs){}
void GFFField::assign(float rhs){}
void GFFField::assign(double rhs){}
void GFFField::assign(__int64 rhs){}
void GFFField::assign(unsigned __int64 rhs){}
void GFFField::assign(LPCSTR rhs){}
void GFFField::assign(LPCWSTR rhs){}
void GFFField::assign(const Vector3f& rhs){}
void GFFField::assign(const Vector4f& rhs){}
void GFFField::assign(const Quaternion& rhs){}
void GFFField::assign(const Color4& rhs){}
void GFFField::assign(const Matrix44& rhs){}

void GFFField::assign(LPVOID rhs, int n) {
	if (!isComplex()){
		size_t len = min( elementsize(), (size_t)n );
		memcpy(pdata(), rhs, len);
	}
}

bool GFFField::Recall(IDAOStream& in)
{
	return ( in.Read( pdata(), elementsize(), 1) == 1 );
}

bool GFFField::Store(IDAOStream& out) const
{
	GFFField *pThis = const_cast<GFFField*>(this);
	return ( out.Write( pThis->pdata(), pThis->elementsize(), 1) == 1 );
}

void GFFField::Dump(IDAODumpStream& out, LPCTSTR name) 
{
	Text text = asText();
	out.Indent();
	out.PrintF("%-30s\t%s\n", name, text.data());
}

template<> bool ::Recall(IDAOStream& in, GFFField& val)
{
	return val.Recall(in);
}
template<> bool ::Store(IDAOStream& out, GFFField const & val)
{
	GFFField &rThis = const_cast<GFFField&>(val);
	return rThis.Store(out);
}
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, GFFField const & val)
{
	GFFField &rThis = const_cast<GFFField&>(val);
	rThis.Dump(out, name);
}


//////////////////////////////////////////////////////////////////////////
// Wraps and exiting field pointer to implement reference behavior 
class GFFPointerField : public GFFField
{
	GFFFieldPtr impl;
	bool empty;
private:
	GFFPointerField();
protected:
	friend GFFField;
	GFFPointerField(GFFField* value) : impl(value), empty(false) {}
	~GFFPointerField() {}
public:
	virtual void destroy() { impl->destroy(); }
	virtual LPVOID pdata() { return impl->pdata(); }
	void create() {  empty = false; }

	virtual GFFFieldDefRef getDefinition()		{ return impl->getDefinition(); }
	virtual bool isPointer() const				{ return true; }
	virtual bool isPrimitive() const			{ return impl->isPrimitive(); }
	virtual bool isComplex() const				{ return impl->isComplex(); }
	virtual bool isEmpty() const				{ return empty ? empty : impl->isEmpty(); }
	virtual bool isList() const					{ return impl->isList(); }
	virtual size_t elementsize() const			{ return impl->elementsize(); }
	virtual FieldDataType getFieldType()		{ return impl->getFieldType(); }
	virtual Text asText()						{ return impl->asText(); }
	virtual signed char asInt8()				{ return impl->asInt8(); }
	virtual unsigned char asUInt8()			{ return impl->asUInt8(); }
	virtual signed short asInt16()				{ return impl->asInt16(); }
	virtual unsigned short asUInt16()			{ return impl->asUInt16(); }
	virtual signed long asInt32()				{ return impl->asInt32(); }
	virtual unsigned long asUInt32()			{ return impl->asUInt32(); }
	virtual signed __int64 asInt64()			{ return impl->asInt64(); }
	virtual unsigned __int64 asUInt64()			{ return impl->asUInt64(); }
	virtual float asFloat32()					{ return impl->asFloat32(); }
	virtual double asFloat64()					{ return impl->asFloat64(); }
	virtual Vector3f asVector3f()					{ return impl->asVector3f(); }
	virtual Vector4f asVector4f()					{ return impl->asVector4f(); }
	virtual Quaternion asQuat()						{ return impl->asQuat(); }
	virtual Text asECString()					{ return impl->asECString(); }
	virtual Color4 asColor4()					{ return impl->asColor4(); }
	virtual Matrix44 asMatrix4()					{ return impl->asMatrix4(); }
	virtual Text asTlkString()					{ return impl->asTlkString(); }
	virtual GFFListRef asList()					{ return impl->asList(); }
	virtual GFFStructRef asStruct()				{ return impl->asStruct(); }

	virtual void assign(char rhs)				{ create(); impl->assign(rhs); }
	virtual void assign(unsigned char rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(short rhs)				{ create(); impl->assign(rhs); }
	virtual void assign(unsigned short rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(long rhs)				{ create(); impl->assign(rhs); }
	virtual void assign(unsigned long rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(float rhs)				{ create(); impl->assign(rhs); }
	virtual void assign(double rhs)				{ create(); impl->assign(rhs); }
	virtual void assign(__int64 rhs)			{ create(); impl->assign(rhs); }
	virtual void assign(unsigned __int64 rhs)	{ create(); impl->assign(rhs); }
	virtual void assign(LPCSTR rhs)				{ create(); impl->assign(rhs); }
	virtual void assign(LPCWSTR rhs)			{ create(); impl->assign(rhs); }
	virtual void assign(LPVOID rhs, int n)		{ create(); impl->assign(rhs, n); }
	virtual void assign(const Vector3f& rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(const Vector4f& rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(const Quaternion& rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(const Color4& rhs)		{ create(); impl->assign(rhs); }
	virtual void assign(const Matrix44& rhs)		{ create(); impl->assign(rhs); }
												  
	virtual size_t CopyData(LPVOID dst, size_t size) { return impl->CopyData(dst, size); }
	virtual int CopyText(LPTSTR value, int len){ return impl->CopyText(value, len); }

	virtual bool Recall(IDAOStream& in)
	{
		long data;
		bool ok = (in.Read(&data, sizeof(long)) == 1);
		if (!ok || data == 0 || data == 0xFFFFFFFF) {
			empty = true;
			return true;
		}
		empty = false;
		int pos = in.Tell();
		in.Seek(SEEK_SET, data);
		impl->Recall(in);
		in.Seek(SEEK_SET, pos);
			return ok;
	}
	virtual bool Store(IDAOStream& out) const
	{
		bool ok = true;
		long pos = out.Tell();
		if (isEmpty())
		{
			ok = (out.Write(&NegativePointer, sizeof(NegativePointer), 1) == 1);
		}
		else
		{
			long data = out.TellEnd();
			out.Seek(SEEK_SET, data);
			long elemsize = elementsize();
			ok = impl->Store(out);
			long fillOffset = out.Tell();
			long fillerSize = AlignWord(fillOffset) - fillOffset;
			if (fillerSize) {
				ok = (out.Write(&NegativePointer, fillerSize, 1) == 1);
			}
			out.Seek(SEEK_SET, pos);
		}
		return ok;
	}
};

//////////////////////////////////////////////////////////////////////////

template <class T>
class GFFPrimitiveField : public GFFField
{
	T value;
protected:
	friend GFFField;
	GFFPrimitiveField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define), value(0) {}
public:
	virtual void destroy() { delete this; }

	size_t elementsize() const	{ return sizeof(T); }

	bool isPrimitive()	{ return true; }
	bool isComplex()	{ return false; }
	void* pdata()		{ return &value; }

	signed char asInt8()		{ return (signed char)(*(T*)pdata()); }
	unsigned char asUInt8()	{ return (unsigned char)(*(T*)pdata()); }
	signed short asInt16()		{ return (signed short)(*(T*)pdata()); }
	unsigned short asUInt16()	{ return (unsigned short)(*(T*)pdata()); }
	signed long asInt32()		{ return (signed long)(*(T*)pdata()); }
	unsigned long asUInt32()	{ return (unsigned long)(*(T*)pdata()); }
	signed __int64 asInt64()	{ return (signed __int64)(*(T*)pdata()); }
	unsigned __int64 asUInt64()	{ return (unsigned __int64)(*(T*)pdata()); }
	float asFloat32()			{ return (float)(*(T*)pdata()); }
	double asFloat64()			{ return (double)(*(T*)pdata()); }

	void assign(char rhs)				{ *(T*)pdata() = (T)rhs; }
	void assign(unsigned char rhs)		{ *(T*)pdata() = (T)rhs; }
	void assign(short rhs)				{ *(T*)pdata() = (T)rhs; }
	void assign(unsigned short rhs)		{ *(T*)pdata() = (T)rhs; }
	void assign(long rhs)				{ *(T*)pdata() = (T)rhs; }
	void assign(unsigned long rhs)		{ *(T*)pdata() = (T)rhs; }
	void assign(float rhs)				{ *(T*)pdata() = (T)rhs; }
	void assign(double rhs)				{ *(T*)pdata() = (T)rhs; }
	void assign(__int64 rhs)			{ *(T*)pdata() = (T)rhs; }
	void assign(unsigned __int64 rhs)	{ *(T*)pdata() = (T)rhs; }


	bool Recall(IDAOStream& in)			{ return ::Recall(in, value); }
	bool Store(IDAOStream& out)			{ return ::Store(out, value); }
	void Dump(IDAODumpStream& out, LPCTSTR name) { ::Dump(out, name, value); }
};

//////////////////////////////////////////////////////////////////////////

class GFFGenericField : public GFFField
{
	LPVOID data;
	int len;
protected:
	friend GFFField;
	GFFGenericField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define), data(NULL), len(0) {}
public:
	virtual void destroy()		{ delete this; }
	virtual void *pdata()		{ return data; }
	size_t elementsize() const	{ return len; }
	bool isComplex() const		{ return true; }
	bool isEmpty() const		{ return (len == 0); }

};

//////////////////////////////////////////////////////////////////////////

class GFFVector3fField : public GFFField
{
	Vector3f value;
protected:
	friend GFFField;
	GFFVector3fField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define) {}
public:
	virtual void destroy()				{ delete this; }
	void *pdata()						{ return &value; }
	bool isComplex() const				{ return false; }
	virtual size_t elementsize() const	{ return sizeof(Vector3f); }
	Vector3f asVector3f()					{ return *(Vector3f*)pdata(); }
	void assign(const Vector3f& rhs)		{ *(Vector3f*)pdata() = rhs; }

	//void Dump(IDAODumpStream& out, LPCTSTR name) { ::Dump(out, name, value); }
};

//////////////////////////////////////////////////////////////////////////

class GFFVector4fField : public GFFField
{
	Vector4f value;
protected:
	friend GFFField;
	GFFVector4fField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define) {}
public:
	virtual void destroy()				{ delete this; }
	void *pdata()						{ return &value; }
	bool isComplex() const				{ return false; }
	virtual size_t elementsize() const	{ return sizeof(Vector4f); }
	Vector4f asVector4f()					{ return *(Vector4f*)pdata(); }
	void assign(const Vector4f& rhs)		{ *(Vector4f*)pdata() = rhs; }

	//void Dump(IDAODumpStream& out, LPCTSTR name) { ::Dump(out, name, value); }
};

//////////////////////////////////////////////////////////////////////////

class GFFQuatField : public GFFField
{
	Quaternion value;
protected:
	friend GFFField;
	GFFQuatField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define) {}
public:
	virtual void destroy()				{ delete this; }
	void *pdata()						{ return &value; }
	bool isComplex() const				{ return false; }
	virtual size_t elementsize() const	{ return sizeof(Quaternion); }
	Quaternion asQuat()						{ return *(Quaternion*)pdata(); }
	void assign(const Quaternion& rhs)		{ *(Quaternion*)pdata() = rhs; }

	//void Dump(IDAODumpStream& out, LPCTSTR name) { ::Dump(out, name, value); }
};

//////////////////////////////////////////////////////////////////////////

class GFFColorField : public GFFField
{
	Color4 value;
protected:
	friend GFFField;
	GFFColorField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define) {}
public:
	virtual void destroy()				{ delete this; }
	void *pdata()						{ return &value; }
	bool isComplex() const				{ return false; }
	virtual size_t elementsize() const	{ return sizeof(Color4); }
	Color4 asColor4()					{ return *(Color4*)pdata(); }
	void assign(const Color4& rhs)		{ *(Color4*)pdata() = rhs; }

	//void Dump(IDAODumpStream& out, LPCTSTR name) { ::Dump(out, name, value); }
};

//////////////////////////////////////////////////////////////////////////

class GFFMatrix4Field : public GFFField
{
	Matrix44 value;
protected:
	friend GFFField;
	GFFMatrix4Field(GFFStructRef owner, GFFFieldDefRef define)
		: GFFField(owner, define) {}
public:
	virtual void destroy()				{ delete this; }
	void *pdata()						{ return &value; }
	bool isComplex() const				{ return false; }
	virtual size_t elementsize() const	{ return sizeof(Matrix44); }
	Matrix44 asMatrix4()					{ return *(Matrix44*)pdata(); }
	void assign(const Matrix44& rhs)		{ *(Matrix44*)pdata() = rhs; }

	//void Dump(IDAODumpStream& out, LPCTSTR name) { ::Dump(out, name, value); }
};

//////////////////////////////////////////////////////////////////////////

class GFFTextField : public GFFField
{
	WText value;
protected:
	friend GFFField;
	GFFTextField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define) 
	{}
public:
	virtual void destroy() { delete this; }
	void *pdata() { return &value; }

	bool isComplex() const	{ return true; }

	virtual size_t elementsize() const { 
		size_t len = value.Length();
		return (len ? len + 1 : 0) * sizeof(wchar_t); 
	}

	Text asText()					{ return Text( value.data() ); }
	Text asECString()				{ return Text( value.data() ); }
	Text asTlkString()				{ return Text( value.data() ); }
	void assign(const Text& rhs)	{ value = rhs; }

	virtual size_t CopyData(LPVOID dst, size_t dlen)
	{
		size_t len = min( dlen, elementsize() );
		memcpy(dst, value.data(), len);
		return len;
	}

	virtual int CopyText(LPTSTR dst, int len)
	{
		size_t l = min( (size_t)len , value.Length() + 1 );
#ifdef _UNICODE
		wcscpy(dst, value.data(), l);
#else
		wcstombs(dst, value.data(), l);
#endif
		return l;
	}

	bool Recall(IDAOStream& in)
	{
		long data;
		bool ok = (in.Read(&data, sizeof(long)) == 1);
		if (!ok || data == 0 || data == 0xFFFFFFFF)
			return true;
		int pos = in.Tell();
		in.Seek(SEEK_SET, data);
		int len;
		ok = ::Recall(in, len);
		if (len > 0)
		{
			value.Resize(len);
			ok = (in.Read(value.data(), sizeof(wchar_t), len) == len);
		}
		in.Seek(SEEK_SET, pos);
		return ok;
	}

	bool Store(IDAOStream& out)
	{
		bool ok = true;
		if (isEmpty())
		{
			ok = (out.Write(&NegativePointer, sizeof(NegativePointer), 1) == 1);
		}
		else
		{
			long data = out.TellEnd();
			out.Seek(SEEK_SET, data);
			::Store(out, data);

			long pos = out.Tell();
			long len = value.Length();
			::Store(out, len);
			ok = (out.Write(value.data(), sizeof(wchar_t), len) == len);
			long fillOffset = out.Tell();
			long fillerSize = AlignWord(fillOffset) - fillOffset;
			if (fillerSize) {
				ok = ( out.Write(&NegativePointer, fillerSize, 1) == 1 );
			}
			out.Seek(SEEK_SET, pos);
		}
		return ok;
	}
};

//////////////////////////////////////////////////////////////////////////

class GFFListField : public GFFField
{
	GFFListPtr list;
protected:
	friend GFFField;
	GFFListField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define), list( GFFList::Construct(owner, define) ) {}

public:
	virtual void destroy() { delete this; }
	virtual LPVOID pdata() { return NULL; }

	bool isComplex() const	{ return true; }
	bool isList() const		{ return true; }

	virtual size_t elementsize() const { 
		throw new runtime_error("Dont call this method.");
		return sizeof(long);
	}

	Text asText()		{ return Text(); }
	GFFListRef asList()	{ return GFFListRef(list); }

	virtual size_t CopyData(LPVOID dst, size_t dlen)
	{
		return 0;
	}

	virtual int CopyText(LPTSTR dst, int len)
	{
		return 0;
	}


	bool Recall(IDAOStream& in)
	{
		long data;
		bool ok = (in.Read(&data, sizeof(long)) == 1);
		if (!ok || data == 0 || data == 0xFFFFFFFF)
			return true;
		int pos = in.Tell();
		in.Seek(SEEK_SET, data);
		ok = ::Recall(in, list);
		in.Seek(SEEK_SET, pos);
		return ok;
	}

	bool Store(IDAOStream& out)
	{
		bool ok = true;
		long pos = out.Tell();
		if (isEmpty())
		{
			ok = (out.Write(&NegativePointer, sizeof(NegativePointer), 1) == 1);
		}
		else
		{
			long data = out.TellEnd();
			out.Seek(SEEK_SET, data);
			long elemsize = elementsize();
			ok = ::Store(out, list);
			long fillOffset = out.Tell();
			long fillerSize = AlignWord(fillOffset) - fillOffset;
			if (fillerSize) {
				ok = ( out.Write(&NegativePointer, fillerSize, 1) == 1);
			}
			out.Seek(SEEK_SET, pos);
		}
		return ok;
	}

	void Dump(IDAODumpStream& out, LPCTSTR name) 
	{
		::Dump(out, name, list);
	}
};


//////////////////////////////////////////////////////////////////////////

class GFFStructField : public GFFField
{
	GFFStructPtr ptr;
protected:
	friend GFFField;
	GFFStructField(GFFStructRef owner, GFFFieldDefRef define) 
		: GFFField(owner, define)
		, ptr( new GFFStruct(owner->getInfo(), *owner->getInfo().Structs[(short)define->TypeID]) ) {}

public:
	virtual void destroy()								{ delete this; }
	virtual LPVOID pdata()								{ return NULL; }
	bool isComplex() const								{ return true; }
	bool isList() const									{ return false; }
	virtual size_t elementsize() const 					{ return ptr->getDefinition()->StructSize; }
	Text asText()										{ return Text(); }
	GFFStructRef asStruct()								{ return GFFStructRef(ptr); }
	virtual size_t CopyData(LPVOID dst, size_t dlen)	{ return 0; }
	virtual int CopyText(LPTSTR dst, int len)			{ return 0; }

	bool Recall(IDAOStream& in)							{ return ::Recall(in, ptr); }
	bool Store(IDAOStream& out)							{ return ::Store(out, ptr); }
	void Dump(IDAODumpStream& out, LPCTSTR name)		{ ::Dump(out, name, ptr); }
};

//////////////////////////////////////////////////////////////////////////

class GFFDefaultField : public GFFField
{
	GFFFieldDef define;
public:
	GFFDefaultField() { memset(&define, 0, sizeof(define)); define.TypeID = DT_GENERIC; }
	GFFFieldDefRef getDefinition()				{ return GFFFieldDefRef(&define); }
	void destroy()								{}
	LPVOID pdata()								{ return NULL; }
	bool isComplex() const						{ return false; }
	bool isPointer() const						{ return false; }
	size_t CopyData(LPVOID dst, size_t dlen)	{ return 0; }
	FieldDataType getFieldType()				{ return DT_GENERIC; }
	Text asText()								{ return Text(); }
	int CopyText(LPTSTR value, int len)			{ return 0; }
	void assign(LPVOID rhs, int n)				{}
	bool Recall(IDAOStream& in)					{ return true; }
	bool Store(IDAOStream& out) const			{ return true; }
	void Dump(IDAODumpStream& out, LPCTSTR name){}
};
//////////////////////////////////////////////////////////////////////////

GFFField* GFFField::Construct(GFFStructRef owner, GFFFieldDefRef define)
{
	FieldFlag flags = define->Flags;
	if (flags == FF_REFERENCE)
	{
		define->Flags = (FieldFlag)(flags & ~FF_REFERENCE);
		GFFField* ptr = new GFFPointerField( Construct(owner, define) );
		define->Flags = flags;
		return ptr;
	}
	if (flags == 0)
	{
		switch(define->TypeID)
		{
		case DT_GENERIC: 	return new GFFGenericField(owner, define);
		case DT_UINT8:  	return new GFFPrimitiveField<unsigned char>(owner, define);
		case DT_INT8:  		return new GFFPrimitiveField<signed char>(owner, define);
		case DT_UINT16:  	return new GFFPrimitiveField<unsigned short>(owner, define);
		case DT_INT16:  	return new GFFPrimitiveField<signed short>(owner, define);
		case DT_UINT32:  	return new GFFPrimitiveField<unsigned long>(owner, define);
		case DT_INT32:  	return new GFFPrimitiveField<signed long>(owner, define);
		case DT_UINT64:  	return new GFFPrimitiveField<unsigned __int64>(owner, define);
		case DT_INT64:  	return new GFFPrimitiveField<signed __int64>(owner, define);
		case DT_FLOAT32:  	return new GFFPrimitiveField<float>(owner, define);
		case DT_FLOAT64:  	return new GFFPrimitiveField<double>(owner, define);

		case DT_VECTOR3F:  	return new GFFVector3fField(owner, define);
		case DT_VECTOR4F:  	return new GFFVector4fField(owner, define);
		case DT_QUATERNIONF:return new GFFQuatField(owner, define);
		case DT_ECSTRING:  	return new GFFTextField(owner, define);
		case DT_COLOR4F:  	return new GFFColorField(owner, define);
		case DT_MATRIX4X4F: return new GFFMatrix4Field(owner, define);
		case DT_TLKSTRING:  return new GFFGenericField(owner, define);
		}
	}
	else if ( (flags & FF_LIST) == FF_LIST )
	{
		return new GFFListField( owner, define );
	}
	else if (flags == FF_STRUCT)
	{
		return new GFFStructField(owner, define);
	}
	return NULL;
}

GFFField* GFFField::Default()
{
	static GFFDefaultField defaultField;
	return &defaultField;
}