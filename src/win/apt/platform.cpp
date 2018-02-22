#include <apt/platform.h>

#include <apt/win.h>

const char* apt::GetPlatformErrorString(uint64 _err)
{
	const int kMessageMax = 1024;
	thread_local static char buf[kMessageMax];
	buf[0] = '\0';
	APT_VERIFY(
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			(DWORD)_err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)buf, 
			kMessageMax, 
			NULL
		) != 0
	);
	return buf;
}
