/**********************************************************************
*<
FILE: DAOLog.h

DESCRIPTION:	DA:O File Format

HISTORY:
*>	Copyright (c) 2007, All Rights Reserved.
**********************************************************************/
#include <stdafx.h>
#include "DAOLog.h"

#include <malloc.h>
#include <list>
using namespace DAO;

static std::list<IDAOLogListenerA*> listenersA;
static std::list<IDAOLogListenerW*> listenersW;

static bool logEnabled = true;
static DAOLogLevel logLevel = LOG_NONE;

static void DispatchMessage(DAOLogLevel level, const char *message, int len);
static void DispatchMessage(DAOLogLevel level, const wchar_t *message, int len);

void DispatchMessage(DAOLogLevel level, const char *message, int len)
{
	for (std::list<IDAOLogListenerA*>::iterator itr = listenersA.begin(); itr != listenersA.end(); ++itr) {
		(*itr)->Message(level, message);
	}
	if (!listenersW.empty()) {
		wchar_t* pbuf = (wchar_t*)_alloca((len+1) * sizeof(wchar_t));
		size_t n;
		mbstowcs_s(&n, pbuf, len+1, message, len);
		pbuf[len-1] = 0;
		for (std::list<IDAOLogListenerW*>::iterator itr = listenersW.begin(); itr != listenersW.end(); ++itr) {
			(*itr)->Message(level, pbuf);
		}
	}
}

void DispatchMessage(DAOLogLevel level, const wchar_t *message, int len)
{
	for (std::list<IDAOLogListenerW*>::iterator itr = listenersW.begin(); itr != listenersW.end(); ++itr) {
		(*itr)->Message(level, message);
	}
	if (!listenersA.empty()) {
		char* pbuf = (char*)_alloca((len+1) * sizeof(char));
		size_t n;
		wcstombs_s(&n, pbuf, len+1, message, len);
		pbuf[len-1] = 0;
		for (std::list<IDAOLogListenerA*>::iterator itr = listenersA.begin(); itr != listenersA.end(); ++itr) {
			(*itr)->Message(level, pbuf);
		}
	}
}

void DAOLog::EnableLogging( bool enable )
{
	logEnabled = enable;
}

void DAOLog::SetLogLevel( DAOLogLevel level )
{
	logLevel = level;
}

DAOLogLevel DAOLog::GetLogLevel()
{
	return logLevel;
}

void DAOLog::AddListener( IDAOLogListenerA* pListener )
{
	listenersA.push_back(pListener);
}

void DAOLog::AddListener( IDAOLogListenerW* pListener )
{
	listenersW.push_back(pListener);
}


void DAOLog::ClearListeners()
{
	listenersA.clear();
	listenersW.clear();
}

bool DAOLog::IsErrorEnabled()
{
	return logEnabled && logLevel <= LOG_ERROR && (!listenersA.empty() || !listenersW.empty());
}

bool DAOLog::IsWarnEnabled()
{
	return logEnabled && logLevel <= LOG_WARN && (!listenersA.empty() || !listenersW.empty());
}

bool DAOLog::IsInfoEnabled()
{
	return logEnabled && logLevel <= LOG_INFO && (!listenersA.empty() || !listenersW.empty());
}

bool DAOLog::IsDebugEnabled()
{
	return logEnabled && logLevel <= LOG_DEBUG && (!listenersA.empty() || !listenersW.empty());
}

bool DAOLog::IsVerboseEnabled()
{
	return logEnabled && logLevel <= LOG_VERBOSE && (!listenersA.empty() || !listenersW.empty());
}

void DAOLog::Error( const char* format, ... )
{
	if (!IsErrorEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_ERROR, format, args);
	va_end(args);		
}

void DAOLog::Warn( const char* format, ... )
{
	if (!IsWarnEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_WARN, format, args);
	va_end(args);	
}

void DAOLog::Info( const char* format, ... )
{
	if (!IsInfoEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_INFO, format, args);
	va_end(args);	
}

void DAOLog::Debug( const char* format, ... )
{
	if (!IsDebugEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_DEBUG, format, args);
	va_end(args);	
}

void DAOLog::Verbose( const char* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void DAOLog::Msg( DAOLogLevel level, const char* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void DAOLog::MsgV( DAOLogLevel level, const char* format, va_list args )
{
	char buffer[512];
	int nChars = _vsnprintf_s(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		DispatchMessage(level, buffer, nChars);
	} else {
		size_t Size = _vscprintf(format, args);
		char* pbuf = (char*)_alloca(Size * sizeof(char));
		nChars = _vsnprintf_s(pbuf, Size, Size, format, args);
		DispatchMessage(level, pbuf, nChars);
	}
}

void DAOLog::Error( const wchar_t* format, ... )
{
	if (!IsErrorEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_ERROR, format, args);
	va_end(args);		
}

void DAOLog::Warn( const wchar_t* format, ... )
{
	if (!IsWarnEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_WARN, format, args);
	va_end(args);	
}

void DAOLog::Info( const wchar_t* format, ... )
{
	if (!IsInfoEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_INFO, format, args);
	va_end(args);	
}

void DAOLog::Debug( const wchar_t* format, ... )
{
	if (!IsDebugEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_DEBUG, format, args);
	va_end(args);	
}

void DAOLog::Verbose( const wchar_t* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void DAOLog::Msg( DAOLogLevel level, const wchar_t* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	MsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void DAOLog::MsgV( DAOLogLevel level, const wchar_t* format, va_list args )
{
	wchar_t buffer[512];
	int nChars = _vsnwprintf_s(buffer, _countof(buffer), format, args);
	if (nChars != -1) {
		DispatchMessage(level, buffer, nChars);
	} else {
		size_t Size = _vscwprintf(format, args);
		wchar_t* pbuf = (wchar_t*)_alloca((Size+1) * sizeof(wchar_t));
		nChars = _vsnwprintf_s(pbuf, Size, Size, format, args);
		DispatchMessage(level, pbuf, nChars);
	}
}

void DAOLog::RemoveListener( IDAOLogListenerA* pListener )
{
	listenersA.remove(pListener);
}

void DAOLog::RemoveListener( IDAOLogListenerW* pListener )
{
	listenersW.remove(pListener);
}