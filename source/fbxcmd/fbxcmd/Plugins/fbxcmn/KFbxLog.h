#pragma once
#include <tchar.h>
#include <stdarg.h>

enum KFbxLogLevel
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
class ILogListenerA
{
public:
	virtual void Message( KFbxLogLevel level, const char* strMessage ) = NULL;
};

// WCHAR version
class ILogListenerW
{
public:
	virtual void Message( KFbxLogLevel level, const wchar_t* strMessage ) = NULL;
};

#ifdef _UNICODE
#define ILogListener ILogListenerW
#else
#define ILogListener ILogListenerA
#endif

class __declspec(dllexport) KFbxLog
{
	KFbxLog();
public:

	static void EnableLogging( bool enable );

	static void SetLogLevel( KFbxLogLevel uLevel );
	static KFbxLogLevel GetLogLevel();

	static void AddListener( ILogListenerA* pListener );
	static void AddListener( ILogListenerW* pListener );
	static void RemoveListener( ILogListenerA* pListener );
	static void RemoveListener( ILogListenerW* pListener );
	static void ClearListeners();

	static bool IsErrorEnabled();
	static bool IsWarnEnabled();
	static bool IsInfoEnabled();
	static bool IsDebugEnabled();
	static bool IsVerboseEnabled();

	static void LogError( const char* strFormat, ... );
	static void LogWarn( const char* strFormat, ... );
	static void LogInfo( const char* strFormat, ... );
	static void LogDebug( const char* strFormat, ... );
	static void LogVerbose( const char* strFormat, ... );
	static void LogMessage( KFbxLogLevel level, const char* msg );
	static void LogMsg( KFbxLogLevel level, const char* strFormat, ... );
	static void LogMsgV( KFbxLogLevel level, const char* strFormat, va_list argptr );

	static void LogError( const wchar_t* strFormat, ... );
	static void LogWarn( const wchar_t* strFormat, ... );
	static void LogInfo( const wchar_t* strFormat, ... );
	static void LogDebug( const wchar_t* strFormat, ... );
	static void LogVerbose( const wchar_t* strFormat, ... );
	static void LogMessage( KFbxLogLevel level, const wchar_t* msg );
	static void LogMsg( KFbxLogLevel level, const wchar_t* strFormat, ... );
	static void LogMsgV( KFbxLogLevel level, const wchar_t* strFormat, va_list argptr );
};


#define KFBXLOG_ERROR(x) { if (KFbxLog::IsErrorEnabled()) { KFbxLog::LogError(x); } }
#define KFBXLOG_WARN(x) { if (KFbxLog::IsWarnEnabled()) { KFbxLog::LogWarn(x); } }
#define KFBXLOG_INFO(x) { if (KFbxLog::IsInfoEnabled()) { KFbxLog::LogInfo(x); } }
#define KFBXLOG_DEBUG(x) { if (KFbxLog::IsDebugEnabled()) { KFbxLog::LogDebug(x); } }
#define KFBXLOG_VERBOSE(x) { if (KFbxLog::IsVerboseEnabled()) { KFbxLog::LogVerbose(x); } }

#define KFBXLOG_ASSERT(x) { if (!(x)) { KFbxLog::LogInfo("Assert: %s", ##x); } }
#define KFBXLOG_ASSERT_WARN(x) { if (!(x)) { KFbxLog::LogWarn("Assert: %s", ##x); } }
#define KFBXLOG_ASSERT_ERROR(x) { if (!(x)) { KFbxLog::LogError("Assert: %s", ##x); } }
#define KFBXLOG_ASSERT_DEBUG(x) { if (!(x)) { KFbxLog::LogDebug("Assert: %s", ##x); } }
#define KFBXLOG_ASSERT_VERBOSE(x) { if (!(x)) { KFbxLog::LogVerbose("Assert: %s", ##x); } }
