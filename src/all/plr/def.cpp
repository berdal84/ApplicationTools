#include <plr/def.h>
#include <plr/log.h>

#ifdef PLR_COMPILER_MSVC
	// disable warning about vsnprintf
	#pragma warning(disable: 4996)
#endif
#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf

static const int kAssertMsgMax = 256; // max size for message buffer in assert()
static plr::AssertCallback* g_AssertCallback = &plr::DefaultAssertCallback;

void plr::SetAssertCallback(AssertCallback* _callback) 
{
	g_AssertCallback = _callback;
}

plr::AssertCallback* plr::GetAssertCallback() {
	return g_AssertCallback;
}

plr::AssertBehavior plr::DefaultAssertCallback(const char* _e, const char* _msg, const char* _file, int _line) 
{
	PLR_LOG_ERR("PLR_ASSERT (%s, line %d)\n\t'%s' %s", _file, _line, _e ? _e : "", _msg ? _msg : "");
	return AssertBehavior::kBreak;
}

plr::AssertBehavior plr::internal::AssertAndCallback(const char* _e, const char* _file, int _line, const char* _msg, ...) 
{
	char buf[kAssertMsgMax];
	if (_msg != 0) {
		va_list args;
		va_start(args, _msg);
		PLR_VERIFY(vsnprintf(buf, kAssertMsgMax, _msg, args) > 0);
		va_end(args);
	} else {
		buf[0] = '\0';
	}

	if (g_AssertCallback != 0) {
		return g_AssertCallback(_e, buf, plr::internal::StripPath(_file), _line);
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
