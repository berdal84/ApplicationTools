////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/log.h>

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vfprintf

void plr::internal::Log(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	va_end(args);
}

void plr::internal::LogError(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stderr, _fmt, args)) > 0);
	va_end(args);
}

void plr::internal::LogDebug(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	PLR_VERIFY((vfprintf(stdout, _fmt, args)) > 0);
	va_end(args);
}
