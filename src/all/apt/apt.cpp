#include <apt/apt.h>

#include <apt/log.h>
#include <apt/String.h>

#include <cstdarg> // va_list, va_start, va_end

static thread_local apt::AssertCallback* g_AssertCallback = &apt::DefaultAssertCallback;

void apt::SetAssertCallback(AssertCallback* _callback) 
{
	g_AssertCallback = _callback;
}

apt::AssertCallback* apt::GetAssertCallback() {
	return g_AssertCallback;
}

apt::AssertBehavior apt::DefaultAssertCallback(const char* _expr, const char* _msg, const char* _file, int _line) 
{
	APT_LOG_ERR("APT_ASSERT (%s, line %d)\n\t'%s' %s", _file, _line, _expr ? _expr : "", _msg ? _msg : "");
	return AssertBehavior_Break;
}

apt::AssertBehavior apt::internal::AssertAndCallback(const char* _expr, const char* _file, int _line, const char* _msg, ...) 
{
	thread_local String<1024> msg = "";

	if (_msg != 0) {
		va_list args;
		va_start(args, _msg);
		msg.setfv(_msg, args);
		va_end(args);
	}

	if (g_AssertCallback) {
		return g_AssertCallback(_expr, (const char*)msg, apt::internal::StripPath(_file), _line);
	}
	return AssertBehavior_Break;
}

const char* apt::internal::StripPath(const char* _path) 
{
	int i = 0, last = 0;
	while (_path[i] != 0) {
		if (_path[i] == '\\' || _path[i] == '/')
			last = i + 1;
		++i;
	}
	return &_path[last];
}
