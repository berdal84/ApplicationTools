#pragma once
#ifndef apt_math_h
#define apt_math_h

#ifdef APT_DEBUG
	//#define GLM_MESSAGES
#endif

// \hack glm typedefs sized ints with the same names as apt; to get around the
// ensuing conflict we #define them with glm_ prefixes, then #undef them again
// below
#define aligned glm_hack_aligned
#define int8    glm_hack_int8
#define int16   glm_hack_int16
#define int32   glm_hack_int32
#define int64   glm_hack_int64
#define uint8   glm_hack_uint8
#define uint16  glm_hack_uint16
#define uint32  glm_hack_uint32
#define uint64  glm_hack_uint64
#define uint    glm_hack_uint
#define float16 glm_hack_float16
#define float32 glm_hack_float32
#define float64 glm_hack_float64

// \hack \todo need to disable SSE intrinsics because the glm types are not always 
// correctly aligned (they should be, did the hack above break this?)
#define GLM_FORCE_PURE

#define GLM_FORCE_SIZE_FUNC
#define GLM_FORCE_NO_CTOR_INIT
#define GLM_FORCE_EXPLICIT_CTOR
#include "extern/glm/glm/glm.hpp"
#include "extern/glm/glm/gtc/matrix_access.hpp"
#include "extern/glm/glm/gtc/matrix_inverse.hpp"
#include "extern/glm/glm/gtc/matrix_transform.hpp"
#include "extern/glm/glm/gtc/quaternion.hpp"

#define GLM_GTX_norm
#include "extern/glm/glm/gtx/norm.hpp"

#undef aligned
#undef int8
#undef int16
#undef int32
#undef int64
#undef uint8
#undef uint16
#undef uint32
#undef uint64
#undef uint
#undef float16
#undef float32
#undef float64

#include <apt/def.h>

namespace apt {

	using namespace glm;
	
	class LCG
	{
		static const uint kMultiplier  = 48271u;
		static const uint kIncrement   = 0u;
		static const uint kModulus     = 2147483647u;
		static const uint kDefaultSeed = 1u;
		
		uint m_seed;
		
	public:
		LCG(uint _seed = kDefaultSeed)
			: m_seed(_seed) 
		{
		}
	
		void seed(uint _seed)               { APT_ASSERT(_seed > 0u); m_seed = _seed; }
		void discard(uint _count = 1u)      { while (_count > 0) { rand(); --_count; } }

		int rand()                          { return (int)urand(); }
		int rand(int _max)                  { return (int)urand() % _max; }
		int rand(int _min, int _max)        { return _min + (int)urand() % (_max - _min); }

		uint urand()                        { m_seed = (m_seed * kMultiplier + kIncrement) % kModulus; return m_seed; }
		uint urand(uint _max)               { return urand() % _max; }
		uint urand(uint _min, uint _max)    { return _min + urand() % (_max - _min); }

		float frand()                       { return (float)urand() / (float)0x7fffffffu; }
		float frand(float _max)             { return frand() * _max; }
		float frand(float _min, float _max) { return _min + frand() * (_max - _min); }
	};
}

#endif // frm_math_h
