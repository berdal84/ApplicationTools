////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_StringHash_h
#define plr_StringHash_h

#include <plr/def.h>

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class StringHash
/// Fast, non-cryptographic hash generated from a character string.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class StringHash
{
public:
	typedef uint64 HashType;
	static const StringHash kInvalidHash;

	/// Default ctor, hash is invalid.
	StringHash(): m_hash(0)  {}

	/// Initialize from a null-terminated string.
	StringHash(const char* _str);

	/// Initialize from _len characters of _str.
	StringHash(const char* _str, uint _len);

	/// \return hash value, which may be kInvalidHash in the case of an uninitialized StringHash.
	HashType getHash() const { return m_hash; }

	operator HashType() const                      { return m_hash; }
	bool operator==(const StringHash& _rhs) const  { return m_hash == _rhs.m_hash; }
	bool operator!=(const StringHash& _rhs) const  { return m_hash != _rhs.m_hash; }
	bool operator> (const StringHash& _rhs) const  { return m_hash >  _rhs.m_hash; }
	bool operator>=(const StringHash& _rhs) const  { return m_hash >= _rhs.m_hash; }
	bool operator< (const StringHash& _rhs) const  { return m_hash <  _rhs.m_hash; }
	bool operator<=(const StringHash& _rhs) const  { return m_hash <= _rhs.m_hash; }

private:

	HashType m_hash;

}; // class StringHash

inline bool operator==(StringHash::HashType _lhs, const StringHash& _rhs) { return _lhs == _rhs.getHash(); }
inline bool operator!=(StringHash::HashType _lhs, const StringHash& _rhs) { return _lhs != _rhs.getHash(); }

} // namespace plr

#endif // plr_StringHash_h
