////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/log.h>

#ifdef PLR_COMPILER_MSVC
	// disable warning about vsnprintf
	#pragma warning(disable: 4996)
#endif

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsprintf
#include <iostream>

static const int kLogBufferSize = 1024;

static void LogImpl(std::ostream &_stream, const char* _fmt, va_list& _args)
{
	char buf[kLogBufferSize];
	vsnprintf(buf, kLogBufferSize, _fmt, _args);
	std::cout << buf << std::endl;
}

void plr::internal::Log(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	LogImpl(std::cout, _fmt, args);
	va_end(args);
}

void plr::internal::LogError(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	LogImpl(std::cerr, _fmt, args);
	va_end(args);
}

void plr::internal::LogDebug(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	LogImpl(std::cout, _fmt, args);
	va_end(args);
}