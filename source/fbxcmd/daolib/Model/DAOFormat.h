/**********************************************************************
*<
FILE: DAOFormat.h

DESCRIPTION:	DAO File Formats

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once
#include "DataHeader.h"
#include "../common/valueptr.h"
#include "../common/daostream.h"
#include <vector>
#include <iostream>
#include <pshpack1.h>
#include <string>
#include <list>
#include <sys/stat.h>
#include <sys/types.h>
#include "text.h"
typedef unsigned short	ushort;
typedef void*			VoidPtr;
typedef unsigned int	uint;
typedef const unsigned char * LPCBYTE;


#pragma region DAOArray
/////////////////////////////////////
// NWArray
template <typename T>
class DAOArray : public std::vector<T>
{
public:
	DAOArray() { }
	DAOArray(int length) : std::vector<T>(length) {}
	DAOArray(int capacity, int length) : std::vector<T>(capacity, length) { }
	DAOArray(const DAOArray& other) : std::vector<T>( other ) {}

	//! Default Destructor
	~DAOArray() {}

	T& appendNew()
	{
		int index = size();
		push_back( T() );
		return back();
	}

	void append(T& val) {
		push_back( val );
	}

	int size() const { return int(std::vector<T>::size()); }
	int count() const { return size(); }
	int length() const { return size(); }
};

template <typename T>
void DeleteContents(DAOArray<T>& array) {
	for (int i=0, n=array.size(); i<n; ++i)
		delete array[i];
	array.resize(0);
}
#pragma endregion

#pragma region Recall|Store|Dump|SizeOf
//////////////////////////////////////////////////////////////////////////

template<typename T>
bool Recall(IDAOStream& in, T& val) {
	return (in.Read((char*)&val, sizeof(T)) == 1);
}

template<class T, uint size, int padding>
bool Recall(IDAOStream& in, FixedString<T, size, padding>& val) {
	return (in.Read(val, size) == 1);
}

template<typename T>
bool Recall(IDAOStream& in, DAOArray<T>& val) 
{
	bool ok = true;
	for (int i=0, n=val.count(); i<n && ok; ++i) {
		ok = Recall(in, val[i]);
	}
	return ok;
}

template<typename T>
bool Recall(IDAOStream& in, ValuePtr<T>& val);

template<typename T>
bool Recall(IDAOStream& in, ValueRef<T>& val);

template<typename T>
bool Store(IDAOStream& out, T const & val) {
	return (out.Write((const char*)&val, sizeof(T)) == 1);
}

template<class T, uint size, int padding>
bool Store(IDAOStream& out, FixedString<T, size, padding> const & val) {
	return (out.Write(val, size) == 1);
}

template<typename T>
bool Store(IDAOStream& out, DAOArray<T> const & val) 
{
	bool ok = true;
	for (int i=0, n=val.count(); i<n && ok; ++i) {
		ok = ::Store(out, val[i]);
	}
	return ok;
}


template<typename T>
bool Store(IDAOStream& out, ValuePtr<T>& val);


template<typename T>
bool Store(IDAOStream& out, ValueRef<T>& val);

template<typename T>
void Dump(IDAODumpStream& out, LPCTSTR name, T const & val)
{
	_tstringstream sstr;
	sstr << val;
	out.Indent();
	out.PrintF(_T("%-30s\t%s\n"), name, sstr.str().c_str());
}

template<typename T>
void DumpArray(IDAODumpStream& out, LPCTSTR name, T* const val, uint size) 
{
	out.Indent();
	out.PrintF("%-30s\t(size=%d)\n", name, size);
	out.IncreaseIndent();
	uint len = 0;
	TCHAR buffer[100];
	buffer[0] = 0;
	for (uint i=0, n=size; i<n; ++i) {
		_stprintf(buffer, _T("[%d]"), i);
		Dump(out, buffer, val[i]);
	}
	out.DecreaseIndent();
}

template<typename T> 
void Dump(IDAODumpStream& out, LPCTSTR name, const DAOArray<T> & val)
{
	if (val.size() == 0)
		out.Indent(), out.PrintF("%-30s\t(size=%d)\n", name, val.size());
	else
		DumpArray(out, name, &val[0], val.count());
}

template<typename T>
void Dump(IDAODumpStream& out, LPCTSTR name, const ValuePtr<T>& val);

template<typename T>
void Dump(IDAODumpStream& out, LPCTSTR name, const ValueRef<T>& val);

#pragma region array
template<int size, typename T>
bool Recall(IDAOStream& in, array<size, T>& val) 
{
	bool ok = true;
	for (int i=0; i<size && ok; ++i) {
		ok = Recall(in, val[i]);
	}
	return ok;
}

template<int size, typename T>
bool Store(IDAOStream& out, array<size, T> const & val) 
{
	bool ok = true;
	for (int i=0; i<size && ok; ++i) {
		ok = Store(out, val[i]);
	}
	return ok;
}

template<int size, typename T> 
void Dump(IDAODumpStream& out, LPCTSTR name, array<size, T> const & val) 
{
	if (size == 0)
		out.Indent(), out.PrintF("%-30s\t(size=%d)\n", name, size);
	else
		DumpArray(out, name, &val[0], size);
}

#pragma endregion

template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, char const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, unsigned char const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, float const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, Vector2f const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, Vector3f const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, Vector4f const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, Quaternion const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, ColorRef const & val);
template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, Color4 const & val);

//template<> bool ::Recall(IDAOStream& in, TSTR& val);
//template<> bool ::Store(IDAOStream& out, TSTR const & val);
//template<> void ::Dump(IDAODumpStream& out, LPCTSTR name, TSTR const & val);

#pragma endregion 

#pragma region SizeOf

template<typename T>
unsigned int SizeOf(IDAOStream& in, T const & val) {
	return sizeof(T);
}

template<class T, uint size, int padding>
unsigned int SizeOf(IDAOStream& in, FixedString<T, size, padding> const & val) {
	return size;
}

template<typename T>
unsigned int SizeOf(IDAOStream& in, DAOArray<T> const & val) 
{
	uint len = 0;
	for (int i=0, n=val.count(); i<n; ++i) {
		len += SizeOf(in, val[i]);
	}
	return len;
}
#if 0
template<> inline 
unsigned int SizeOf(IDAOStream& in, TSTR const& val) {
	return sizeof(int) + val.Length();
}
#endif

template<typename T>
unsigned int SizeOf(IDAOStream& in, ValuePtr<T> const & val);

template<typename T>
unsigned int SizeOf(IDAOStream& in, ValueRef<T> const & val);

#pragma endregion 

#pragma region Resize

template<typename T>
inline void Resize(IDAOStream& in, T & val, uint cnt) {}

template<typename T>
inline void Resize(IDAOStream& in, DAOArray<T> & val, uint cnt) {
	val.resize(cnt);
}

template<> inline void Resize<char>(IDAOStream& in, char & val, uint cnt) { val = cnt; }
template<> inline void Resize<short>(IDAOStream& in, short & val, uint cnt) { val = cnt; }
template<> inline void Resize<int>(IDAOStream& in, int & val, uint cnt) { val = cnt; }
template<> inline void Resize<unsigned char>(IDAOStream& in, unsigned char & val, uint cnt) { val = cnt; }
template<> inline void Resize<unsigned short>(IDAOStream& in, unsigned short & val, uint cnt) { val = cnt; }
template<> inline void Resize<unsigned int>(IDAOStream& in, unsigned int & val, uint cnt) { val = cnt; }
template<> inline void Resize<unsigned long>(IDAOStream& in, unsigned long & val, uint cnt) { val = cnt; }

#pragma endregion 

#pragma region ToSize

template<typename T> uint ToSize(T const & val) { return 0; }
template<> inline uint ToSize<char>(char const & val) { return val; }
template<> inline uint ToSize<short>(short const & val) { return val; }
template<> inline uint ToSize<int>(int const & val) { return val; }
template<> inline uint ToSize<unsigned char>(unsigned char const & val) { return val; }
template<> inline uint ToSize<unsigned short>(unsigned short const & val) { return val; }
template<> inline uint ToSize<unsigned int>(unsigned int const & val) { return val; }
template<> inline uint ToSize<unsigned long>(unsigned long const & val) { return val; }

template<typename T>
uint ToSize(DAOArray<T> const & val) {
	return val.size();
}

#pragma endregion 


#pragma region TYPE

struct TYPE
{
	virtual bool recall(IDAOStream& in, LPVOID val) = 0;
	virtual bool store(IDAOStream& out, LPCVOID val) = 0;
	virtual void dump(IDAODumpStream& out, LPCTSTR name, LPCVOID val) = 0;
	virtual unsigned int SizeOf(IDAOStream& in, LPCVOID val) = 0;
	virtual void resize(IDAOStream& in, LPVOID val, uint cnt) = 0;
	virtual uint toSize(LPCVOID val) = 0;
};

template<typename U>
struct VTYPE : public TYPE
{
	bool recall(IDAOStream& in, LPVOID val)				{ return ::Recall(in, *((U*)val)); }
	bool store(IDAOStream& out, LPCVOID val)				{ return ::Store(out, *((U const *)val)); }
	void dump(IDAODumpStream& out, LPCTSTR name, LPCVOID val){ ::Dump(out, name, *((U const *)val)); }
	unsigned int SizeOf(IDAOStream& str, LPCVOID val)			{ return ::SizeOf(str, *((U const *)val)); }
	void resize(IDAOStream& in, LPVOID val, uint cnt)	{ ::Resize(in, *((U *)val), cnt); }
	uint toSize(LPCVOID val)							{ return ::ToSize(*((U const *)val)); }
};

struct VARIABLE
{
	VARIABLE() : ShortName(NULL), MemberAddr(0), Description(NULL), DefaultValue(NULL), ValueSize(0), MemberType(NULL) {}

	template<typename U>
	VARIABLE(LPCTSTR sName, U const & member, U const & default, LPCTSTR desc) 
		: ShortName(sName), Description(desc)
		, MemberAddr((LPCBYTE)&member - (LPCBYTE)0), MemberType(new VTYPE<U>()) 
		, SizeAddr(0), SizeType(NULL)
	{
		SetDefault(default);
	}

	template<typename U>
	VARIABLE(LPCTSTR sName, U const & member, LPCTSTR desc) 
		: ShortName(sName), Description(desc)
		, MemberAddr((LPCBYTE)&member - (LPCBYTE)0), MemberType(new VTYPE<U>()) 
		, SizeAddr(0), SizeType(NULL)
	{}

	template<typename T, typename U>
	VARIABLE(LPCTSTR sName, DAOArray<U> const & member, T const & size, LPCTSTR desc) 
		: ShortName(sName), Description(desc)
		, MemberAddr((LPCBYTE)&member - (LPCBYTE)0), MemberType(new VTYPE<DAOArray<U> >()) 
		, SizeAddr((LPCBYTE)&size - (LPCBYTE)0), SizeType(new VTYPE<T>())
	{}

	~VARIABLE() {
		if (ValueSize && DefaultValue) {
			delete DefaultValue; DefaultValue = NULL; ValueSize = 0;
		}
		if (MemberType) {
			delete MemberType; MemberType = NULL;
		}
	}

	template<typename U> void SetDefault(U default) { ValueSize = sizeof(U); DefaultValue = new U(default); }
	template <typename U> const U& GetDefault() const { return *(U*)DefaultValue; }

	LPCTSTR ShortName;
	intptr_t MemberAddr;
	TYPE *MemberType;
	LPCTSTR Description;
	LPVOID DefaultValue;
	DWORD ValueSize;
	DWORD SizeAddr;
	TYPE *SizeType;
};

#define INITVARTYPE(T) \
	T() { ::Init(this->GetInfDefmap(), (LPVOID)this); } \

#define BEGIN_INI_MAP_NOINIT(T) \
	LPCTSTR GetTypeName() const { return _T(#T); } \
	void *GetMember(const VARIABLE* memptr) const { return (void*)(((char*)this) + memptr->MemberAddr); } \
	const VARIABLE* GetInfDefmap() const { return InternalGetInfDefmap(); } \
	static VARIABLE* InternalGetInfDefmap() { \
	const T* pThis = 0; static VARIABLE map[] = { \

#define BEGIN_INI_MAP(T) \
	INITVARTYPE(T) \
	BEGIN_INI_MAP_NOINIT(T) \

#define BEGIN_BASE_MAP(T) \
	T() { ::Init(this->GetInfDefmap(), (LPVOID)this); } \
	virtual LPCTSTR GetTypeName() const { return _T(#T); } \
	virtual void *GetMember(const VARIABLE* memptr) const { return (void*)(((char*)this) + memptr->MemberAddr); } \
	virtual const VARIABLE* GetInfDefmap() const { return InternalGetInfDefmap(); } \
	static VARIABLE* InternalGetInfDefmap() { \
	const T* pThis = 0; static VARIABLE map[] = { \

#define END_INI_MAP() \
	VARIABLE() };\
	return map;} \

#define ADDITEM(member, defval) \
	VARIABLE(_T(#member), pThis->member, defval, _T(#member)), \

#define ADDITEMEX(sName, member, defval, desc) \
	VARIABLE(sName, pThis->member, defval, desc), \

#define ADDCLASS(member) \
	VARIABLE(_T(#member), pThis->member, _T(#member)), \

#define ADDARRAY(member, elementsize) \
	VARIABLE(_T(#member), pThis->member, pThis->elementsize, _T(#member)), \

#define ROOTVARTYPE(T) \
	template<> bool ::Recall<T>(IDAOStream& in, T& val); \
	template<> bool ::Store<T>(IDAOStream& out, T const & val); \
	template<> void ::Dump<T>(IDAODumpStream& out,  LPCTSTR name, T const & val); \
	template<> unsigned int ::SizeOf<T>(IDAOStream& in, T const & val); \

#define DEFVARTYPE(N, T) \
	namespace N { struct T; } \
	template<> bool ::Recall<N::T>(IDAOStream& in, N::T& val); \
	template<> bool ::Store<N::T>(IDAOStream& out, N::T const & val); \
	template<> void ::Dump<N::T>(IDAODumpStream& out,  LPCTSTR name, N::T const & val); \
	template<> unsigned int ::SizeOf<N::T>(IDAOStream& in, N::T const & val); \

#define IMPLVARTYPE(T) \
	template<> bool ::Recall<T>(IDAOStream& in, T& val) { \
	return Recall(val.GetInfDefmap(), in, static_cast<LPVOID>(&val)); \
	} \
	template<> bool ::Store<T>(IDAOStream& out, T const & val) {\
	return Store(val.GetInfDefmap(), out, static_cast<LPCVOID>(&val)); \
	} \
	template<> void ::Dump<T>(IDAODumpStream& out,  LPCTSTR name, T const & val) { \
	Dump(val.GetInfDefmap(), val.GetTypeName(), out, name, static_cast<LPCVOID>(&val)); \
	} \
	template<> unsigned int ::SizeOf<T>(IDAOStream& in, T const & val) { \
	return SizeOf(val.GetInfDefmap(), in, static_cast<LPCVOID>(&val)); \
	} \

//Find:  [a-zA-Z_0-9 \<\>\*]+:b+{:i}:b*;.*
//Repl:	 ADDCLASS(\1)

extern void Init(const VARIABLE* defmap, LPVOID val);
extern bool Recall(const VARIABLE* defmap, IDAOStream& in, LPVOID val);
extern bool Store(const VARIABLE* defmap, IDAOStream& out, LPCVOID val);
extern void Dump(const VARIABLE* defmap, LPCTSTR type, IDAODumpStream& out,  LPCTSTR name, LPCVOID val); 
extern unsigned int SizeOf(const VARIABLE* defmap, IDAOStream& in, LPCVOID val); 

#pragma endregion

#include <poppack.h>

