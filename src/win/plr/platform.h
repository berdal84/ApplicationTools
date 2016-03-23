////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_platform_h
#define plr_platform_h

#include <plr/def.h>
#ifndef PLR_PLATFORM_WIN
	#error plr: PLR_PLATFORM_WIN was not defined, probably the build system was configured incorrectly
#endif

// ASSERT/VERIFY with platform-specific error string (use to wrap OS calls)
#define PLR_PLATFORM_ASSERT(e) PLR_ASSERT_MSG(e, plr::GetPlatformErrorString(GetLastError()))
#define PLR_PLATFORM_VERIFY(e) PLR_VERIFY_MSG(e, plr::GetPlatformErrorString(GetLastError()))

namespace plr {

/// Format a system error code as a string.
/// \note This returns a ptr to an internal static char buffer. The buffer is
///	   thread-local but calling code **must** copy the returned string immediately
///	   and not store the returned ptr.
/// \ingroup plr_core
const char* GetPlatformErrorString(unsigned int _err);

} // namespace plr

#endif // plr_platform_h
