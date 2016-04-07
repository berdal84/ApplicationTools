////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/def.h>
#include <plr/log.h>

#ifdef PLR_COMPILER_MSVC
	// disable warning about vsnprintf
	#pragma warning(disable: 4996)
#endif
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

static const int kAssertMsgMax = 256; // max size for message buffer in assert()
static PLR_THREAD_LOCAL plr::AssertCallback* pfAssertCallback = &plr::DefaultAssertCallback;

void plr::SetAssertCallback(AssertCallback* pfCallback) 
{
	pfAssertCallback = pfCallback;
}

plr::AssertCallback* plr::GetAssertCallback() {
	return pfAssertCallback;
}

plr::AssertBehavior plr::DefaultAssertCallback(const char* e, const char* msg, const char* file, int line) 
{
	PLR_LOG_ERR("PLR_ASSERT (%s, line %d)\n\t'%s' %s", file, line, e ? e : "", msg ? msg : "");
	return AssertBehavior::kBreak;
}

plr::AssertBehavior plr::internal::AssertAndCallback(const char* e, const char* file, int line, const char* msg, ...) 
{
	char buf[kAssertMsgMax];
	if (msg != 0) {
		va_list args;
		va_start(args, msg);
		vsnprintf(buf, kAssertMsgMax, msg, args);
		va_end(args);
	} else {
		buf[0] = '\0';
	}

	if (pfAssertCallback != 0) {
		return pfAssertCallback(e, buf, plr::internal::StripPath(file), line);
	}
	return AssertBehavior::kBreak;
}

const char* plr::internal::StripPath(const char* _path) 
{
	int i = 0, last = 0;
	while (_path[i] != 0) {
		if (_path[i] == '\\' || _path[i] == '/')
			last = i + 1;
		++i;
	}
	return &_path[last];
}
