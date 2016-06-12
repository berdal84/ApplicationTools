////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_math_h
#define plr_math_h

#ifdef PLR_DEBUG
	#define GLM_MESSAGES
#endif

// \hack glm typedefs sized ints with the same names as plr; to get around the
// ensuing conflict we #define them with glm_ prefixes, then #undef them again
// below
#define int8    glm_int8
#define int16   glm_int16
#define int32   glm_int32
#define int64   glm_int64
#define uint8   glm_uint8
#define uint16  glm_uint16
#define uint32  glm_uint32
#define uint64  glm_uint64
#define uint    glm_uint

#define GLM_FORCE_SIZE_FUNC
#define GLM_FORCE_NO_CTOR_INIT
#define GLM_FORCE_EXPLICIT_CTOR
#include "extern/glm/glm/glm.hpp"
#include "extern/glm/glm/gtc/matrix_access.hpp"
#include "extern/glm/glm/gtc/matrix_inverse.hpp"
#include "extern/glm/glm/gtc/matrix_transform.hpp"
#include "extern/glm/glm/gtc/quaternion.hpp"

#undef int8
#undef int16
#undef int32
#undef int64
#undef uint8
#undef uint16
#undef uint32
#undef uint64
#undef uint

#include <plr/def.h>

namespace plr {

	using namespace glm;
	
	/// Linear Congruential Generator, a cheap random generator
	/// This is basically a reimplementation of std::minstd_rand	(see http://www.cplusplus.com/reference/random/minstd_rand) without the millions lines of bullshit incurred by STL (STHELL)
	/// The code is basically return (_seed = (_multiplier * _seed + _increment) % _modulus)
	///
	/// All the functions below return a value from the half-close interval [_min,_max) (ie. _max is never returned).
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
	
		void seed(uint _seed)               { PLR_ASSERT(_seed > 0u); m_seed = _seed; }
		void discard(uint _count = 1u)      { while (_count > 0) { rand(); --_count; } }
	
		int rand()                          { return (int)urand(); }
		int rand(int _max)                  { return (int)urand() % _max; }
		int rand(int _min, int _max)        { return _min + (int)urand() % (_max - _min); }

		uint urand()                        { m_seed = (m_seed * kMultiplier + kIncrement) % kModulus; return m_seed; }
		uint urand(uint _max)               { return urand() % _max; }
		uint urand(uint _min, uint _max)    { return _min + urand() % (_max - _min); }

		float frand()                       { return (float)urand() / (float)0x7fffffffU; }
		float frand(float _max)             { return frand() * _max; }
		float frand(float _min, float _max) { return _min + frand() * (_max - _min); }
	};
}

#endif // frm_math_h
