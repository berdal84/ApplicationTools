////////////////////////////////////////////////////////////////////////////////
// FNV-1a Hash functions adapted from http://www.isthe.com/chongo/tech/comp/fnv
////////////////////////////////////////////////////////////////////////////////
#include <plr/Hash.h>

#include <plr/def.h>

static const plr::uint32 kFnv1aBase32  = 0x811C9DC5u;
static const plr::uint32 kFnv1aPrime32 = 0x01000193u;
static const plr::uint64 kFnv1aBase64  = 0xCBF29CE484222325ull;
static const plr::uint64 kFnv1aPrime64 = 0x100000001B3ull;

plr::uint16 plr::internal::Hash16(const uint8* _buf, uint _bufSize)
{
	PLR_ASSERT(_buf);
 // xor-folded 32-bit Hash
	uint32 ret = internal::Hash32(_buf, _bufSize);
	return (ret >> 16) ^ (((uint32)1u << 16) - 1);
}

plr::uint32 plr::internal::Hash32(const uint8* _buf, uint _bufSize)
{
	PLR_ASSERT(_buf);
	uint32 ret = kFnv1aBase32;
	const uint8* lim = _buf + _bufSize;
	while (_buf < lim) {
		ret ^= (uint32)*_buf++;
		ret *= kFnv1aPrime32;
	}
	return ret;
}

plr::uint64 plr::internal::Hash64(const uint8* _buf, uint _bufSize)
{
	PLR_ASSERT(_buf);
	uint64 ret = kFnv1aBase64;
	const uint8* lim = _buf + _bufSize;
	while (_buf < lim) {
		ret ^= (uint64)*_buf++;
		ret *= kFnv1aPrime64;
	}
	return ret;
}

plr::uint16 plr::internal::HashString16(const char* _str)
{
	PLR_ASSERT(_str);
 // xor-folded 32-bit Hash
	uint32 ret = internal::HashString32(_str);
	return (ret >> 16) ^ (((uint32)1u << 16) - 1);
}

plr::uint32 plr::internal::HashString32(const char* _str)
{
	PLR_ASSERT(_str);
	uint32 ret = kFnv1aBase32;
	while (*_str) {
		ret ^= (uint32)*_str++;
		ret *= kFnv1aPrime32;
	}
	return ret;
}

plr::uint64 plr::internal::HashString64(const char* _str)
{
	PLR_ASSERT(_str);
	uint64 ret = kFnv1aBase64;
	while (*_str) {
		ret ^= (uint64)*_str++;
		ret *= kFnv1aPrime64;
	}
	return ret;
}
