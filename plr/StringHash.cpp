////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/StringHash.h>

#include <plr/hash.h>

using namespace plr;

const StringHash StringHash::kInvalidHash;

StringHash::StringHash(const char* _str)
	: m_hash(0)
{
	m_hash = HashString<HashType>(_str);
}

StringHash::StringHash(const char* _str, uint _len)
	: m_hash(0)
{
	m_hash = Hash<HashType>(_str, _len);
}
