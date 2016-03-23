////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/platform.h>

#include <plr/win.h>

const char* plr::GetPlatformErrorString(unsigned int _err)
{
	const unsigned kMessageMax = 1024;
	PLR_THREAD_LOCAL static char buf[kMessageMax];
	buf[0] = 0; // effectively clear the buffer
	PLR_VERIFY(
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			_err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)buf, 
			kMessageMax, 
			NULL
		) != 0
	);
	return buf;
}
