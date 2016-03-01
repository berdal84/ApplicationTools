////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#ifndef plr_platform_h
#define plr_platform_h

#include <plr/plr_def.h>
#ifndef PLR_PLATFORM_WIN
	#error plr: PLR_PLATFORM_WIN was not defined, probably the build system was configured incorrectly
#endif

#ifdef _M_X64
	#define PLR_DCACHE_LINE_SIZE 64
#else
	#error plr: Architecture not defined
#endif

// ASSERT/VERIFY with platform-specific error string (use to wrap OS calls)
// \todo
#define PLR_PLATFORM_ASSERT(e) PLR_ASSERT(e)
#define PLR_PLATFORM_VERIFY(e) PLR_VERIFY(e)

#endif // plr_platform_h
