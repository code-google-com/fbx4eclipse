/**********************************************************************
*<
FILE: DAOLog.h

DESCRIPTION:	DAO File Formats

HISTORY:
*>	Copyright (c) 2009, All Rights Reserved.
**********************************************************************/

#pragma once

#include <tchar.h>
#include <stdarg.h>

namespace DAO {

enum DAOLogLevel
{
	LOG_ALL = 0,
	LOG_VERBOSE,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARN,
	LOG_ERROR,
	LOG_NONE,
};

// CHAR version
class IDAOLogListenerA
{
public:
	virtual void Message( DAOLogLevel level, const char* strMessage ) = NULL;
};

// WCHAR version
class IDAOLogListenerW
{
public:
	virtual void Message( DAOLogLevel level, const wchar_t* strMessage ) = NULL;
};

#ifdef _UNICODE
#define IDAOLogListener IDAOLogListenerW
#else
#define IDAOLogListener IDAOLogListenerA
#endif

class __declspec(dllexport) DAOLog
{
	DAOLog();
public:

	static void EnableLogging( bool enable );

	static void SetLogLevel( DAOLogLevel uLevel );
	static DAOLogLevel GetLogLevel();

	static void AddListener( IDAOLogListenerA* pListener );
	static void AddListener( IDAOLogListenerW* pListener );
	static void RemoveListener( IDAOLogListenerA* pListener );
	static void RemoveListener( IDAOLogListenerW* pListener );
	static void ClearListeners();

	static bool IsErrorEnabled();
	static bool IsWarnEnabled();
	static bool IsInfoEnabled();
	static bool IsDebugEnabled();
	static bool IsVerboseEnabled();

	static void Error( const char* strFormat, ... );
	static void Warn( const char* strFormat, ... );
	static void Info( const char* strFormat, ... );
	static void Debug( const char* strFormat, ... );
	static void Verbose( const char* strFormat, ... );
	static void Msg( DAOLogLevel level, const char* strFormat, ... );
	static void MsgV( DAOLogLevel level, const char* strFormat, va_list argptr );

	static void Error( const wchar_t* strFormat, ... );
	static void Warn( const wchar_t* strFormat, ... );
	static void Info( const wchar_t* strFormat, ... );
	static void Debug( const wchar_t* strFormat, ... );
	static void Verbose( const wchar_t* strFormat, ... );
	static void Msg( DAOLogLevel level, const wchar_t* strFormat, ... );
	static void MsgV( DAOLogLevel level, const wchar_t* strFormat, va_list argptr );
};


#define DAOLOG_ERROR(x) { if (DAOLog::IsErrorEnabled()) { DAOLog::Error(x); } }
#define DAOLOG_WARN(x) { if (DAOLog::IsWarnEnabled()) { DAOLog::Warn(x); } }
#define DAOLOG_INFO(x) { if (DAOLog::IsInfoEnabled()) { DAOLog::Info(x); } }
#define DAOLOG_DEBUG(x) { if (DAOLog::IsDebugEnabled()) { DAOLog::Debug(x); } }
#define DAOLOG_VERBOSE(x) { if (DAOLog::IsVerboseEnabled()) { DAOLog::Verbose(x); } }

#define DAOLOG_ASSERT(x) { if ( if (!(x) && DAOLog::IsInfoEnabled()) { DAOLog::Info(##x); } }
#define DAOLOG_ASSERT_WARN(x) { if (!(x) && DAOLog::IsWarnEnabled()) { DAOLog::Warn(##x); } }
#define DAOLOG_ASSERT_ERROR(x) { if (!(x) && DAOLog::IsErrorEnabled()) { DAOLog::Error(##x); } }
#define DAOLOG_ASSERT_DEBUG(x) { if (!(x) && DAOLog::IsDebugEnabled()) { DAOLog::Debug(##x); } }
#define DAOLOG_ASSERT_VERBOSE(x) { if (!(x) && DAOLog::IsVerboseEnabled()) { DAOLog::Verbose(##x); } }

} // namespace DAO