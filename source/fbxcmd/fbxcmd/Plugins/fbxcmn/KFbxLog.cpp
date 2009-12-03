#include "KFbxLog.h"
#include <malloc.h>
#include <list>
#include <crtdbg.h>

extern "C" int __stdcall IsDebuggerPresent( );

static std::list<ILogListenerA*> listenersA;
static std::list<ILogListenerW*> listenersW;

static bool logEnabled = true;
static KFbxLogLevel logLevel = LOG_NONE;

static void DispatchMessage(KFbxLogLevel level, const char *message, int len);
static void DispatchMessage(KFbxLogLevel level, const wchar_t *message, int len);

void DispatchMessage(KFbxLogLevel level, const char *message, int len)
{
	for (std::list<ILogListenerA*>::iterator itr = listenersA.begin(); itr != listenersA.end(); ++itr) {
		(*itr)->Message(level, message);
	}
	if (!listenersW.empty()) {
		wchar_t* pbuf = (wchar_t*)_alloca((len+1) * sizeof(wchar_t));
		size_t n;
		mbstowcs_s(&n, pbuf, len+1, message, len);
		pbuf[len-1] = 0;
		for (std::list<ILogListenerW*>::iterator itr = listenersW.begin(); itr != listenersW.end(); ++itr) {
			(*itr)->Message(level, pbuf);
		}
	}
}

void DispatchMessage(KFbxLogLevel level, const wchar_t *message, int len)
{
	for (std::list<ILogListenerW*>::iterator itr = listenersW.begin(); itr != listenersW.end(); ++itr) {
		(*itr)->Message(level, message);
	}
	if (!listenersA.empty()) {
		char* pbuf = (char*)_alloca((len+1) * sizeof(char));
		size_t n;
		wcstombs_s(&n, pbuf, len+1, message, len);
		pbuf[len-1] = 0;
		for (std::list<ILogListenerA*>::iterator itr = listenersA.begin(); itr != listenersA.end(); ++itr) {
			(*itr)->Message(level, pbuf);
		}
	}
}

void KFbxLog::EnableLogging( bool enable )
{
	logEnabled = enable;
}

void KFbxLog::SetLogLevel( KFbxLogLevel level )
{
	logLevel = level;
}

KFbxLogLevel KFbxLog::GetLogLevel()
{
	return logLevel;
}

void KFbxLog::AddListener( ILogListenerA* pListener )
{
	listenersA.push_back(pListener);
}

void KFbxLog::AddListener( ILogListenerW* pListener )
{
	listenersW.push_back(pListener);
}


void KFbxLog::ClearListeners()
{
	listenersA.clear();
	listenersW.clear();
}

bool KFbxLog::IsErrorEnabled()
{
	return logEnabled && logLevel <= LOG_ERROR && (!listenersA.empty() || !listenersW.empty());
}

bool KFbxLog::IsWarnEnabled()
{
	return logEnabled && logLevel <= LOG_WARN && (!listenersA.empty() || !listenersW.empty());
}

bool KFbxLog::IsInfoEnabled()
{
	return logEnabled && logLevel <= LOG_INFO && (!listenersA.empty() || !listenersW.empty());
}

bool KFbxLog::IsDebugEnabled()
{
	return logEnabled && logLevel <= LOG_DEBUG && (!listenersA.empty() || !listenersW.empty());
}

bool KFbxLog::IsVerboseEnabled()
{
	return logEnabled && logLevel <= LOG_VERBOSE && (!listenersA.empty() || !listenersW.empty());
}

bool KFbxLog::IsEnabled(KFbxLogLevel level)
{
	return logEnabled && logLevel <= level && (!listenersA.empty() || !listenersW.empty());
}

void KFbxLog::LogError( const char* format, ... )
{
	if (!IsErrorEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_ERROR, format, args);
	va_end(args);		
}

void KFbxLog::LogWarn( const char* format, ... )
{
	if (!IsWarnEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_WARN, format, args);
	va_end(args);	
}

void KFbxLog::LogInfo( const char* format, ... )
{
	if (!IsInfoEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_INFO, format, args);
	va_end(args);	
}

void KFbxLog::LogDebug( const char* format, ... )
{
	if (!IsDebugEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_DEBUG, format, args);
	va_end(args);	
}

void KFbxLog::LogVerbose( const char* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void KFbxLog::LogMsg( KFbxLogLevel level, const char* format, ... )
{
	if (!IsEnabled(level)) return;
	va_list args;
	va_start(args, format);
	LogMsgV(level, format, args);
	va_end(args);
}

void KFbxLog::LogMsgV( KFbxLogLevel level, const char* format, va_list args )
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

void KFbxLog::LogError( const wchar_t* format, ... )
{
	if (!IsErrorEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_ERROR, format, args);
	va_end(args);		
}

void KFbxLog::LogWarn( const wchar_t* format, ... )
{
	if (!IsWarnEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_WARN, format, args);
	va_end(args);	
}

void KFbxLog::LogInfo( const wchar_t* format, ... )
{
	if (!IsInfoEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_INFO, format, args);
	va_end(args);	
}

void KFbxLog::LogDebug( const wchar_t* format, ... )
{
	if (!IsDebugEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_DEBUG, format, args);
	va_end(args);	
}

void KFbxLog::LogVerbose( const wchar_t* format, ... )
{
	if (!IsVerboseEnabled()) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void KFbxLog::LogMsg( KFbxLogLevel level, const wchar_t* format, ... )
{
	if (!IsEnabled(level)) return;
	va_list args;
	va_start(args, format);
	LogMsgV(LOG_VERBOSE, format, args);
	va_end(args);
}

void KFbxLog::LogMsgV( KFbxLogLevel level, const wchar_t* format, va_list args )
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

void KFbxLog::LogMessage( KFbxLogLevel level, const char* msg )
{
	DispatchMessage(level, msg, strlen(msg));
}

void KFbxLog::LogMessage( KFbxLogLevel level, const wchar_t* msg )
{
	DispatchMessage(level, msg, wcslen(msg));
}

void KFbxLog::RemoveListener( ILogListenerA* pListener )
{
	listenersA.remove(pListener);
}

void KFbxLog::RemoveListener( ILogListenerW* pListener )
{
	listenersW.remove(pListener);
}

void KFbxLog::AssertFailed(  KFbxLogLevel level
					  , const char *pFileName
					  , const char *pFunctionName
					  , unsigned long pLineNumber
					  , const char *pMessage
					  , bool *pHideForEver)
{
	if ( KFbxLog::IsEnabled(level) )
	{
		if (pHideForEver) *pHideForEver = true;
		KFbxLog::LogMsg(level, "%s(%u): %s: %s", pFileName, pLineNumber, pFunctionName, pMessage);
		if (IsDebuggerPresent()) 
			_CrtDbgBreak();
	}
}