////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_log_h
#define plr_log_h

#include <plr/def.h>

#define PLR_LOG(fmt, ...)         do { plr::internal::Log(fmt ## "\n", __VA_ARGS__); } while (0)
#define PLR_LOG_ERR(fmt, ...)     do { plr::internal::LogError(fmt ## "\n", __VA_ARGS__); } while (0)
#ifdef PLR_DEBUG
	#define PLR_LOG_DBG(fmt, ...) do { plr::internal::LogDebug(fmt ## "\n", __VA_ARGS__); } while (0)
#else
	#define PLR_LOG_DBG(fmt, ...) do { PLR_UNUSED(fmt); } while(0)
#endif

namespace plr { namespace internal {

void Log     (const char* _fmt, ...);
void LogError(const char* _fmt, ...);
void LogDebug(const char* _fmt, ...);

} } // namespace plr::internal

#endif // plr_log_h
