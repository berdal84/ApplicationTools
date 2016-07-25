////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/log.h>

#include <plr/String.h>

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vfprintf

using namespace plr;

static const int kLogMsgMax = 1024; // max size for message buffer in DispatchLogCallback()
static LogCallback* g_logCallback= 0;

static void DispatchLogCallback(const char* _fmt, va_list _args, LogType _type)
{
	if (g_logCallback) {
		String<1024> buf(_fmt, _args);
		g_logCallback(buf, _type);
	}
}

void plr::SetLogCallback(LogCallback* _callback)
{
	g_logCallback= _callback;
}

LogCallback* plr::GetLogCallback()
{
	return g_logCallback;
}

void plr::internal::Log(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	PLR_VERIFY((fprintf(stdout, "\n")) > 0);
	DispatchLogCallback(_fmt, args, LogType::kLog);
	va_end(args);
}

void plr::internal::LogError(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stderr, _fmt, args)) > 0);
	PLR_VERIFY((fprintf(stderr, "\n")) > 0);
	DispatchLogCallback(_fmt, args, LogType::kError);
	va_end(args);
}

void plr::internal::LogDebug(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	PLR_VERIFY((fprintf(stdout, "\n")) > 0);
	DispatchLogCallback(_fmt, args, LogType::kDebug);
	va_end(args);
}
