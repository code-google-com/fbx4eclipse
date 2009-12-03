/**********************************************************************
*<
FILE: DAOStream.h

DESCRIPTION:	DAO File Formats

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include <vector>
#include <iostream>
#include <pshpack1.h>
#include <string>
#include <list>
#include <tchar.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "valueptr.h"

#pragma region DAOStream
class IDAOStream;

template<> struct VPTraits<IDAOStream>;
template<> struct VRTraits<IDAOStream>;

class IDAOStream
{
protected:
	friend struct VPTraits<IDAOStream>;
	friend struct VRTraits<IDAOStream>;
	virtual int AddRef() = 0;
	virtual int Release() = 0;
public:
	virtual void Close()= 0;

	virtual size_t Read(void *buf, size_t size, size_t count = 1) = 0;
	virtual size_t Write(void const * buf, size_t size, size_t count = 1) = 0;
	virtual int Seek(int whence, long offset) = 0;
	virtual int Tell() = 0;
	virtual int TellEnd() = 0;

	virtual bool Eof() const = 0;

	virtual void *get_pdata() = 0;
	virtual void set_pdata(const void* value) = 0;
};
typedef ValuePtr<IDAOStream> DAOStreamPtr;
typedef ValueRef<IDAOStream> DAOStreamRef;

template<>
struct VPTraits<IDAOStream>
{
	static  IDAOStream *  create()  { return NULL; }
	static  IDAOStream *  clone( IDAOStream const * p )  { 
		IDAOStream *mp = const_cast<IDAOStream*>(p); 
		if (mp) mp->AddRef(); 
		return mp; 
	}
	static  IDAOStream *  release( IDAOStream * p )  { 
		if (p && p->Release() == 0)
			return NULL;
		return p; 
	}
};  // VPTraits

template<>
struct VRTraits<IDAOStream>
{
	static  IDAOStream *  create()  { return NULL; }
	static  IDAOStream *  clone( IDAOStream const * p )  { 
		IDAOStream *mp = const_cast<IDAOStream*>(p); 
		//if (mp) mp->AddRef(); 
		return mp; 
	}
	static  IDAOStream *  release( IDAOStream * p )  { 
		//if (p->Release() <= 0) return NULL;
		return p; 
	}
};  // VPTraits


class DAOStream : public IDAOStream
{
	virtual int AddRef() { return ++refcnt; }
	virtual int Release() { if (refcnt <= 0) return 0; if (--refcnt == 0) { delete this; return 0; } return refcnt; }
public:
	DAOStream() : fh(NULL), refcnt(0) {}

	DAOStream(const TCHAR *file, bool readonly) : fh(NULL) { 
		Open(file, readonly); 
	}
	~DAOStream() { 
		Close(); 
	}

	static DAOStreamPtr Create(const TCHAR *file = NULL, bool readonly = true);	

	virtual FILE* Handle();
	virtual void Open(const TCHAR *file, bool readonly);
	virtual void Close();

	virtual size_t Read(void *buf, size_t size, size_t count = 1);
	virtual size_t Write(void const * buf, size_t size, size_t count = 1);
	virtual int Seek(int whence, long offset);
	virtual int Tell();
	virtual int TellEnd();

	virtual bool Eof() const;

	virtual void *&pdata() { return data; }
	virtual void *get_pdata() { return data; }
	virtual void set_pdata(const void* value) { data = const_cast<void*>(value); }
protected:
	void* data;
	FILE* fh;
	int refcnt;
};


class DAOMemStream : public IDAOStream
{
protected:
	virtual int AddRef() { return ++refcnt; }
	virtual int Release() { if (refcnt < 0) return 0; if (--refcnt == 0) { delete this; return 0; } return refcnt; }
public:
	DAOMemStream(bool readonly = false);

	DAOMemStream(const void* data, int len);
	~DAOMemStream();

	static DAOStreamPtr Create(bool readonly = false);	
	static DAOStreamPtr Create(const void* data, int len);	

	virtual bool Open();
	virtual bool Open(const void* data, int len);
	virtual void Close();

	virtual size_t Read(void *buf, size_t size, size_t count = 1);
	virtual size_t Write(void const * buf, size_t size, size_t count = 1);
	virtual int Seek(int whence, long offset);
	virtual int Tell();
	virtual int TellEnd();
	virtual bool Eof() const;

	virtual void Reserve( size_t len );

	virtual void* dataptr() const;

	virtual void *&pdata() { return myptr; }
	virtual void *get_pdata() { return myptr; }
	virtual void set_pdata(const void* value) { myptr = const_cast<void*>(value); }
protected:
	void* data;
	size_t n, o;
	bool readonly;
	void* myptr;
	int refcnt;
};


class DAOOffsetStream : public IDAOStream
{
	DAOStreamPtr impl;
	int _offset;
	int _size;
	int refcnt;
protected:
	virtual int AddRef() { if (refcnt <= 0) return 0; return ++refcnt; }
	virtual int Release() { if (refcnt <= 0) return 0; if (--refcnt == 0) { delete this; return 0; } return refcnt; }


	DAOOffsetStream(DAOStreamPtr& stream, int offset, int size) 
		: impl(stream), _offset(offset), _size(size), refcnt(0)
	{
		impl->Seek(SEEK_SET, offset);
	}
public:
	DAOOffsetStream(IDAOStream& stream) 
		: impl(&stream), _offset(impl->Tell()), refcnt(0)
	{
		_size = impl->TellEnd() - _offset;
	}
	DAOOffsetStream(IDAOStream& stream, int offset) 
		: impl(&stream), _offset(offset), refcnt(0)
	{
		impl->Seek(SEEK_SET, offset);
		_size = impl->TellEnd() - _offset;
	}
	DAOOffsetStream(IDAOStream& stream, int offset, int size) 
		: impl(&stream), _offset(offset), _size(size), refcnt(0)
	{
		impl->Seek(SEEK_SET, offset);
	}

	static DAOStreamPtr Create(DAOStreamPtr stream, int offset = -1, int size = -1);	


	virtual void Close() { }

	virtual size_t Read(void *buf, size_t size, size_t count = 1){
		return impl->Read(buf, size, count);
	}
	virtual size_t Write(void const * buf, size_t size, size_t count = 1) {
		return impl->Write(buf, size, count);
	}
	virtual int Seek(int whence, long offset) {
		if (whence == SEEK_SET) {
			return impl->Seek(whence, _offset + offset) - _offset;
		} else if (whence == SEEK_END) {
			return impl->Seek(whence, _offset + _size + offset) - _offset;
		} else {
			return impl->Seek(whence, offset ) - _offset;
		}		
	}
	virtual int Tell()			{ return impl->Tell() - _offset; }
	virtual int TellEnd()		{ return impl->TellEnd() - _offset; }
	virtual bool Eof() const	{ return impl->Tell() >= _size; }
	virtual void *get_pdata()	{ return impl->get_pdata(); }
	virtual void set_pdata(const void* value) { impl->set_pdata(value); }
};


struct IDAODumpStream : public IDAOStream
{
	virtual void Print(char const * buf) = 0;
	virtual void Print(wchar_t const * buf) = 0;
	virtual void PrintF(const char *format, ...) = 0;
	virtual void PrintF(const wchar_t *format, ...) = 0;
	virtual void NewLine() = 0;
	virtual void Indent() = 0;
	virtual int IncreaseIndent() = 0;
	virtual int DecreaseIndent() = 0;
	virtual int get_Indent() = 0;
};

class DAODumpStream : public IDAODumpStream
{
	DAOStream impl;
	int IndentLevel;
	int refcnt;
protected:
	virtual int AddRef() { return ++refcnt; }
	virtual int Release() { if (refcnt <= 0) return 0; if (--refcnt == 0) { delete this; return 0; } return refcnt; }
public:
	DAODumpStream() : impl(), IndentLevel(0), refcnt(0) {}

	DAODumpStream(const TCHAR *file, bool readonly) 
		: impl(file, readonly), IndentLevel(0), refcnt(0) {}

	~DAODumpStream() { 
	}

	void Print(char const * buf);
	void Print(wchar_t const * buf);
	void PrintF(const char *format, ...);
	void PrintF(const wchar_t *format, ...);
	void NewLine();
	void Indent();
	int IncreaseIndent();
	int DecreaseIndent();
	int get_Indent();

	virtual void Open(const TCHAR *file, bool readonly);
	virtual void Close();
	virtual size_t Read(void *buf, size_t size, size_t count = 1);
	virtual size_t Write(void const * buf, size_t size, size_t count = 1);
	virtual int Seek(int whence, long offset);
	virtual int Tell();
	virtual int TellEnd();
	virtual bool Eof() const;
	virtual void *get_pdata();
	virtual void set_pdata(const void* value);
};
#pragma endregion
