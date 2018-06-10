#pragma once

// Global options

//#define APT_ENABLE_ASSERT              1   // Enable asserts. If APT_DEBUG this is enabled by default.
//#define APT_ENABLE_STRICT_ASSERT       1   // Enable 'strict' asserts.
//#define APT_LOG_CALLBACK_ONLY          1   // By default, log messages are written to stdout/stderr prior to the log callback dispatch. Disable this behavior.

#if defined(APT_DEBUG)
	#ifndef APT_ENABLE_ASSERT
		#define APT_ENABLE_ASSERT 1
	#endif
#endif

// Compiler
#if defined(__GNUC__)
	#define APT_COMPILER_GNU 1
#elif defined(_MSC_VER)
	#define APT_COMPILER_MSVC 1
#else
	#error apt: Compiler not defined
#endif

// Platform 
#if defined(_WIN32) || defined(_WIN64)
	#define APT_PLATFORM_WIN 1
#elif defined(__linux__)
	#define APT_PLATFORM_LINUX 1
#else
	#error apt: Platform not defined
#endif

// Architecture
#if defined(_M_X64) || defined(__x86_64)
	#define APT_DCACHE_LINE_SIZE 64
#else
	#error apt: Architecture not defined
#endif
