#include <apt/StringHash.h>

#include <apt/hash.h>

using namespace apt;

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
