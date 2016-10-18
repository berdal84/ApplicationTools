#include <apt/log.h>

#include <apt/String.h>

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vfprintf

using namespace apt;

static const int kLogMsgMax = 1024; // max size for message buffer in DispatchLogCallback()
static LogCallback* g_logCallback= 0;

static void DispatchLogCallback(const char* _fmt, va_list _args, LogType _type)
{
	if (g_logCallback) {
		String<1024> buf;
		buf.setfv(_fmt, _args);
		g_logCallback(buf, _type);
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
	APT_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	APT_VERIFY((fprintf(stdout, "\n")) > 0);
	DispatchLogCallback(_fmt, args, LogType::kLog);
	va_end(args);
}

void apt::internal::LogError(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	APT_VERIFY((vfprintf(stderr, _fmt, args)) > 0);
	APT_VERIFY((fprintf(stderr, "\n")) > 0);
	DispatchLogCallback(_fmt, args, LogType::kError);
	va_end(args);
}

void apt::internal::LogDebug(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	APT_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	APT_VERIFY((fprintf(stdout, "\n")) > 0);
	DispatchLogCallback(_fmt, args, LogType::kDebug);
	va_end(args);
}
