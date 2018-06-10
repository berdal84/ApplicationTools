#include <apt/log.h>

#include <apt/String.h>

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vfprintf

using namespace apt;

static thread_local LogCallback* g_logCallback;

static void DispatchLogCallback(const char* _fmt, va_list _args, LogType _type)
{
	if (g_logCallback) {
		String<1024> msg;
		msg.setfv(_fmt, _args);
		g_logCallback((const char*)msg, _type);
	}
}

void apt::SetLogCallback(LogCallback* _callback)
{
	g_logCallback= _callback;
}

LogCallback* apt::GetLogCallback()
{
	return g_logCallback;
}

void apt::internal::Log(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	#if !(APT_LOG_CALLBACK_ONLY)
		APT_VERIFY((vfprintf(stdout, _fmt, args)) >= 0);
		APT_VERIFY((fprintf(stdout, "\n")) >= 0);
	#endif
	DispatchLogCallback(_fmt, args, LogType_Log);
	va_end(args);
}

void apt::internal::LogError(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	#if !(APT_LOG_CALLBACK_ONLY)
		APT_VERIFY((vfprintf(stderr, _fmt, args)) > 0);
		APT_VERIFY((fprintf(stderr, "\n")) > 0);
	#endif
	DispatchLogCallback(_fmt, args, LogType_Error);
	va_end(args);
}

void apt::internal::LogDebug(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	#if !(APT_LOG_CALLBACK_ONLY)
		APT_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
		APT_VERIFY((fprintf(stdout, "\n")) > 0);
	#endif
	DispatchLogCallback(_fmt, args, LogType_Debug);
	va_end(args);
}
