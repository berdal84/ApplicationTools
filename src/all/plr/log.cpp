////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/log.h>

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vfprintf

static const int kLogMsgMax = 1024; // max size for message buffer in DispatchLogCallback()
static plr::LogCallback* g_LogCallback = 0;

static void DispatchLogCallback(const char* _fmt, va_list _args, plr::LogType _type)
{
	if (g_LogCallback) {
		char buf[kLogMsgMax];
		PLR_VERIFY(vsnprintf(buf, kLogMsgMax, _fmt, _args) > 0);
		g_LogCallback(buf, _type);
	}
}

void plr::SetLogCallback(LogCallback* _callback)
{
	g_LogCallback = _callback;
}

plr::LogCallback* plr::GetLogCallback()
{
	return g_LogCallback;
}

void plr::internal::Log(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	DispatchLogCallback(_fmt, args, LogType::kLog);
	va_end(args);
}

void plr::internal::LogError(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stderr, _fmt, args)) > 0);
	DispatchLogCallback(_fmt, args, LogType::kError);
	va_end(args);
}

void plr::internal::LogDebug(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	DispatchLogCallback(_fmt, args, LogType::kDebug);
	va_end(args);
}
