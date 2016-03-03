////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_hash_h
#define plr_hash_h

#include <plr/def.h>

namespace plr { namespace internal {

/// \return 16-bit hash of _bufSize bytes from buf.
/// \internal \ingroup plr_core
uint16 Hash16(const uint8* _buf, uint _bufSize);

/// \return 32-bit hash of _bufSize bytes from _buf.
/// \internal \ingroup plr_core
uint32 Hash32(const uint8* _buf, uint _bufSize);

/// \return 64-bit hash of _bufSize bytes from _buf.
/// \internal \ingroup plr_core
uint64 Hash64(const uint8* _buf, uint _bufSize);

/// \return 16-bit hash of null-terminated string.
/// \internal \ingroup plr_core
uint16 HashString16(const char* _str);

/// \return 32-bit hash of null-terminated string.
/// \internal \ingroup plr_core
uint32 HashString32(const char* _str);

/// \return 64-bit hash of null-terminated string.
/// \internal \ingroup plr_core
uint64 HashString64(const char* _str);

} } // namespace plr::internal


namespace plr {

/// \return hash of _bufSize bytes from _buf.
/// \tparam tType type of the returned hash (only 16-, 32- and 64-bit unsigned
///    integers are supported).
/// \ingroup plr_core
template <typename tType>
tType Hash(const void* _buf, uint _bufSize);
	template <>
	inline uint16 Hash<uint16>(const void* _buf, uint _bufSize)
	{
		return internal::Hash16((const uint8*)_buf, _bufSize);
	}
	template <>
	inline uint32 Hash<uint32>(const void* _buf, uint _bufSize)
	{
		return internal::Hash32((const uint8*)_buf, _bufSize);
	}
	template <>
	inline uint64 Hash<uint64>(const void* _buf, uint _bufSize)
	{
		return internal::Hash64((const uint8*)_buf, _bufSize);
	}

/// \return hash of of null-terminated string.
/// \tparam tType type of the returned hash (only 16-, 32- and 64-bit unsigned
///    integers are supported).
/// \ingroup plr_core
template <typename tType>
tType HashString(const char* _str);
	template <>
	inline uint16 HashString<uint16>(const char* _str)
	{
		return internal::HashString16(_str);
	}
	template <>
	inline uint32 HashString<uint32>(const char* _str)
	{
		return internal::HashString32(_str);
	}
	template <>
	inline uint64 HashString<uint64>(const char* _str)
	{
		return internal::HashString64(_str);
	}

} // namespace plr

#endif // plr_hash_h
