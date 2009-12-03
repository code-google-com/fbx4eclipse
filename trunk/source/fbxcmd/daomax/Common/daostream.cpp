/**********************************************************************
*<
FILE: DAOFormat.h

DESCRIPTION:	DA:O File Format

HISTORY:
*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/
//#include <stdafx.h>
#include "DAOStream.h"
#include <stdio.h> 
#include <stdarg.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
void DAOStream::Open(const TCHAR *file, bool readonly)
{
	if (fh) Close();

	if (file == NULL && readonly)
	{
		fh = stdin;
	}
	else if (file == NULL && !readonly)
	{
		fh = stdout;
	}
	else
	{
		fh = _tfopen(file, readonly ? _T("rbR") : _T("wbR"));
		if (!fh) {
			throw runtime_error("Unable to open file");
		}
		setvbuf(fh, NULL, _IOFBF, 0x10000);
	}
}

void DAOStream::Close()
{
	if (fh) {
		fclose(fh);
		fh = NULL;
	}
}

FILE* DAOStream::Handle() { return fh; }

size_t DAOStream::Read(void *buf, size_t size, size_t count)
{
	size_t n = (size_t)fread(buf, size, count, fh);
	//int err = errno;
	//if (!(n == count && err == 0))
	//{
	//	//int t = Tell();
	//	//ASSERT(n == count && err == 0);
	//}
	return n;
}

size_t DAOStream::Write(const void *buf, size_t size, size_t count)
{
	return (size_t)fwrite(buf, size, count, fh);
}

int DAOStream::Seek(int whence, long offset)
{
	return fseek(fh, offset, whence);
}

int DAOStream::Tell() const
{
	return ftell(fh);
}

int DAOStream::TellEnd() const
{
	struct _stat data;
	memset(&data, 0, sizeof(data));
	_fstat(_fileno(fh), &data);
	return int(data.st_size);
}

bool DAOStream::Eof() const
{
	return (feof(fh) != 0);
}

DAOStreamPtr DAOStream::Create( const TCHAR *file /*= NULL*/, bool readonly /*= true*/ )
{
	 DAOStream *ptr = new DAOStream(file, readonly);
	++ptr->refcnt;
	return DAOStreamPtr(ptr);	
}

void DAODumpStream::Print(char const * buf)
{
	if (buf == NULL)
		Write("<Empty>", 7, 1);
	else
		Write(buf, strlen(buf), sizeof(char));
}

void DAODumpStream::Print(wchar_t const * buf)
{
	Write(buf, wcslen(buf), sizeof(wchar_t));
}

void DAODumpStream::PrintF(const char *format, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, format);
	int nChars = _vsnprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		Write(buffer, nChars, sizeof(char));
	} else {
		size_t Size = _vscprintf(format, args);
		char *buffer = (char*)malloc(Size+1);
		nChars = _vsnprintf(buffer, Size, format, args);
		Write(buffer, nChars, sizeof(char));
		free(buffer);
	}
	va_end(args);
}

void DAODumpStream::PrintF(const wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);
	//vfwprintf(fh, format, args);
#if 0
	wchar_t buffer[512];
	int nChars = _vsnwprintf(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		Write(buffer, nChars, sizeof(wchar_t));
	} else {
		size_t Size = _vscwprintf(format, args);
		wchar_t *buffer = (wchar_t*)malloc(Size+1);
		nChars = _vsnwprintf(buffer, Size, format, args);
		Write(buffer, nChars, sizeof(wchar_t));
		free(buffer);
	}
#endif
	va_end(args);
}

void DAODumpStream::NewLine()
{
	Write("\n", 1, 1);
}

void DAODumpStream::Indent()
{
	for (int i=0; i<IndentLevel; ++i)
		Write("  ", 2, 1);
}
int DAODumpStream::IncreaseIndent()
{
	return ++IndentLevel;
}
int DAODumpStream::DecreaseIndent()
{
	return --IndentLevel;
}

int DAODumpStream::get_Indent()
{
	return IndentLevel;
}

void DAODumpStream::Open(const TCHAR *file, bool readonly) { impl.Open(file, readonly); }
void DAODumpStream::Close() { impl.Close(); }
size_t DAODumpStream::Read(void *buf, size_t size, size_t count ){ return impl.Read(buf, size, count); }
size_t DAODumpStream::Write(void const * buf, size_t size, size_t count ) { return impl.Write(buf, size, count); }
int DAODumpStream::Seek(int whence, long offset) { return impl.Seek(whence, offset); }
int DAODumpStream::Tell() const { return impl.Tell(); }
int DAODumpStream::TellEnd() const { return impl.TellEnd(); }
bool DAODumpStream::Eof() const { return impl.Eof(); }
void *DAODumpStream::get_pdata() { return impl.get_pdata(); }
void DAODumpStream::set_pdata(const void* value) { impl.set_pdata(value); }

//////////////////////////////////////////////////////////////////////////

DAOMemStream::DAOMemStream(bool readonly) 
	: data(NULL), myptr(NULL), n(0), o(0)
{
	this->readonly = readonly;
}

DAOMemStream::DAOMemStream( const void* p, int len ) 
	: data(NULL), myptr(NULL), n(0), o(0)
{
	Open(p, len);
}

DAOMemStream::~DAOMemStream()
{
	Close();
}

bool DAOMemStream::Open( const void* p, int len )
{
	this->data = const_cast<void*>(p);
	this->o = 0;
	this->n = len;
	this->readonly = true;
	return true;
}

bool DAOMemStream::Open()
{
	Close();
	this->o = NULL;
	this->readonly = false;
	return true;
}

void DAOMemStream::Close()
{
	if (data && !readonly) 
		free(data);
	data = NULL;
	n = o = 0;
	readonly = true;
}

size_t DAOMemStream::Read( void *buf, size_t size, size_t count /*= 1*/ )
{
	size_t len = size * count;
	if (n >= o + len) {
		if (buf != NULL){
			memcpy(buf, (char*)data + o, len);
			return count;
		}
	}
	return 0;
}

size_t DAOMemStream::Write( void const * buf, size_t size, size_t count /*= 1*/ )
{
	if (readonly) return 0;
	size_t len = size*count;
	Reserve(len + o);
	memcpy((char*)data + o, buf, len);
	o += len;
	return count;
}

int DAOMemStream::Seek( int whence, long offset )
{
	switch (whence)
	{
	case SEEK_SET: o = offset; break;
	case SEEK_END: o = n + offset; break;
	case SEEK_CUR: o = o + offset; break;
	}
	return o = std::max<size_t>( std::min<size_t>( o, n ), 0 );
}

int DAOMemStream::Tell() const { return o; }
int DAOMemStream::TellEnd() const { return n; }
bool DAOMemStream::Eof() const { return o == n; }

void DAOMemStream::Reserve( size_t len )
{
	if (len >= n)
	{
		size_t a = AlignWord(n * 3 / 2), b = AlignWord(len+1);
		n = max(a, b);
		data = realloc(data, n);
	}
}
void* DAOMemStream::dataptr() const{
	return (char*)data + o;
}

DAOStreamPtr DAOMemStream::Create( bool readonly /*= false*/ )
{
	DAOMemStream *ptr = new DAOMemStream(readonly);
	++ptr->refcnt;
	return DAOStreamPtr(ptr);	
}

DAOStreamPtr DAOMemStream::Create( const void* data, int len )
{
	DAOMemStream *ptr = new DAOMemStream(data, len);
	++ptr->refcnt;
	return DAOStreamPtr(ptr);	
}

DAOStreamPtr DAOOffsetStream::Create( DAOStreamPtr& stream, int offset /*= -1*/, int size /*= -1*/ )
{
	DAOOffsetStream *ptr = new DAOOffsetStream(stream, offset, size);
	++ptr->refcnt;
	return DAOStreamPtr(ptr);	
}