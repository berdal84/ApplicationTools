////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_def_h
#define plr_def_h

#include <cstddef> // size_t
#include <cstdint> // sized ints

// Compiler
#if defined(__GNUC__)
	#define PLR_COMPILER_GNU
#elif defined(_MSC_VER)
	#define PLR_COMPILER_MSVC
#else
	#error plr: Compiler not defined
#endif

// Platform 
#if defined(_WIN32) || defined(_WIN64)
	#define PLR_PLATFORM_WIN
#else
	#error plr: Platform not defined
#endif

// Architecture
#if defined(_M_X64) || defined(__x86_64)
	#define PLR_DCACHE_LINE_SIZE 64
#else
	#error plr: Architecture not defined
#endif



#if defined(PLR_COMPILER_GNU)
	#define PLR_ALIGN(a) __attribute__((aligned(a)))
#elif defined(PLR_COMPILER_MSVC)
	#define PLR_ALIGN(a) __declspec(align(a))
#else
	#define PLR_ALIGN(a) alignas(a)
#endif

#if defined(PLR_COMPILER_GNU)
	#define PLR_ALIGNOF(t) __alignof__(t)
#elif defined(PLR_COMPILER_MSVC)
	#define PLR_ALIGNOF(t) __alignof(t)
#else
	#define PLR_ALIGNOF(t) alignof(t)
#endif

#if defined(PLR_COMPILER_GNU)
	#define PLR_THREAD_LOCAL __thread
#elif defined(PLR_COMPILER_MSVC)
	#define PLR_THREAD_LOCAL __declspec(thread)
#else
	#define PLR_THREAD_LOCAL thread_local
#endif

#if defined(PLR_COMPILER_GNU)
	#define if_likely(e)   if ( __builtin_expect(!!(e), 1) )
	#define if_unlikely(e) if ( __builtin_expect(!!(e), 0) )
//#elif defined(PLR_COMPILER_MSVC)
  // not defined for MSVC
#else
	#define if_likely(e)   if(!!(e))
	#define if_unlikely(e) if(!!(e))
#endif

#ifndef __COUNTER__
// __COUNTER__ is non-standard, so replace with __LINE__ if it's unavailable
	#define __COUNTER__ __LINE__
#endif
#define PLR_TOKEN_CONCATENATE_(_t0, _t1) _t0 ## _t1
#define PLR_TOKEN_CONCATENATE(_t0, _t1)  PLR_TOKEN_CONCATENATE_(_t0, _t1)
#define PLR_UNIQUE_NAME(_base) PLR_TOKEN_CONCATENATE(_base, __COUNTER__)

#define PLR_ARRAY_COUNT(_arr) plr::internal::ArrayCount(_arr)

#define PLR_MIN(_a, _b) plr::internal::Min(_a, _b)
#define PLR_MAX(_a, _b) plr::internal::Max(_a, _b)
#define PLR_CLAMP(_x, _min, _max) PLR_MAX(PLR_MIN(_x, _max), _min)

namespace plr {

typedef std::int8_t     sint8;
typedef std::uint8_t    uint8;
typedef std::int16_t    sint16;
typedef std::uint16_t   uint16;
typedef std::int32_t    sint32;
typedef std::uint32_t   uint32;
typedef std::int64_t    sint64;
typedef std::uint64_t   uint64;

typedef std::ptrdiff_t  sint;
typedef std::size_t     uint;


enum class AssertBehavior
{
	kBreak, 
	kContinue
};

/// Typedef for assert callbacks. See DefaultAssertCallback for a description
/// of the function arguments.
/// \ingroup plr_core
typedef AssertBehavior (AssertCallback)(const char* _e, const char* _msg, const char* _file, int _line);

/// \return Current assert callback. Default is DefaultAssertCallback().
/// \note This may return 0, if 0 was previously passed to SetAssertCallback().
/// \ingroup plr_core
AssertCallback* GetAssertCallback();

/// Set the function to be called when asserts fail, which may be 0.
/// \ingroup plr_core
void SetAssertCallback(AssertCallback* _callback);

/// Default assert callback, print message via PLR_LOG_ERR().
/// \param e Stringified version of the assert expression.
/// \param msg Optional message (passed from PLR_ASSERT_MSG,PLR_VERIFY_MSG macros).
/// \param file, line Location of the assert.
/// \return Always AssertBehaviour::kBreak.
/// \ingroup plr_core
AssertBehavior DefaultAssertCallback(const char* _e, const char* _msg,  const char* _file,  int _line);

} // namespace plr


namespace plr { namespace internal {

AssertBehavior AssertAndCallback(const char* _e, const char* _file, int _line, const char* _msg, ...);
const char* StripPath(const char* _path);

template <typename tType, unsigned kCount>
inline unsigned ArrayCount(const tType (&)[kCount]) { return kCount; }

template <typename tType>
inline tType Min(tType a, tType b) { return a < b ? a : b; }
template <typename tType>
inline tType Max(tType a, tType b) { return a > b ? a : b; }

} } // namespace plr::internal

#define PLR_UNUSED(x) do { (void)sizeof(x); } while(0)
#ifdef PLR_DEBUG
	#ifdef PLR_COMPILER_MSVC
		#define PLR_BREAK() __debugbreak()
	#else
		#include <cstdlib>
		#define PLR_BREAK() abort()
	#endif

	#define PLR_ASSERT_MSG(e, msg, ...) \
		do { \
			if_unlikely (!(e)) { \
				if (plr::internal::AssertAndCallback(#e, __FILE__, __LINE__, msg, __VA_ARGS__) == plr::AssertBehavior::kBreak) \
				{ PLR_BREAK(); } \
			} \
		} while(0)

	#define PLR_ASSERT(e)                 PLR_ASSERT_MSG(e, 0, 0)
	#define PLR_VERIFY_MSG(e, msg, ...)   PLR_ASSERT_MSG(e, msg, __VA_ARGS__)
	#define PLR_VERIFY(e)                 PLR_VERIFY_MSG(e, 0, 0)

#else
	#define PLR_ASSERT_MSG(e, msg, ...)   do { PLR_UNUSED(e); PLR_UNUSED(msg); } while(0)
	#define PLR_ASSERT(e)                 do { PLR_UNUSED(e); } while(0)
	#define PLR_VERIFY_MSG(e, msg, ...)   do { (void)(e); PLR_UNUSED(msg); } while(0)
	#define PLR_VERIFY(e)                 (void)(e)

#endif // plr_core

#define PLR_STATIC_ASSERT(e) { (void)sizeof(char[(e) ? 1 : -1]); }

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class non_copyable
/// Mixin class, forces a derived class to be non-copyable.
/// \note The template parameter permits Empty Base Optimization (see
///   http://en.wikibooks.org/wiki/More_C++_Idioms/Non-copyable_Mixin).
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class non_copyable
{
protected:
	inline  non_copyable() {}
	inline ~non_copyable() {}
private:
	non_copyable(const non_copyable&);
	non_copyable& operator=(const non_copyable&);
};


////////////////////////////////////////////////////////////////////////////////
/// \class non_instantiable
/// Mixin class, forces a derived class to be non-instantiable. Note that by
/// definitionn a non-instantiable class is also non-copyable.
/// \note The template parameter permits Empty Base Optimization (see
///   http://en.wikibooks.org/wiki/More_C++_Idioms/Non-copyable_Mixin).
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class non_instantiable: private non_copyable<tType>
{
private:
	non_instantiable();
};

} // namespace plr

#endif // plr_def_h
