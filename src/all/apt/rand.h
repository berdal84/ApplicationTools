#pragma once

#include <apt/apt.h>
#include <apt/math.h>

namespace apt {

////////////////////////////////////////////////////////////////////////////////
// PRNG_CMWC
// Uniform PRNG via 'complimentary multiply-with-carry' (George Marsaglia's 
// 'Mother of All PRNGs'). Adapted from Agner Fog's implementation found here:
// http://www.agner.org/random/. Use as template parameter to Rand (see below).
////////////////////////////////////////////////////////////////////////////////
class PRNG_CMWC
{
public:
	PRNG_CMWC(uint32 _seed = 1) { seed(_seed); }

	void   seed(uint32 _seed);
	uint32 raw();

private:
	uint32 m_state[5];
};

////////////////////////////////////////////////////////////////////////////////
// Rand
// Uniform random number API, templated by generator type. Typical usage:
//    Rand<> rnd;                     // instantiate with default PRNG
//    rnd.get<bool>();                // return true/false
//    rnd.get<float>();               // in [0,1]
//    rnd.get<int>(-10,10);           // in [-10,10]
//    rnd.get<float>(-10.0f, 10.0f);  // in [-10,10]
////////////////////////////////////////////////////////////////////////////////
template <typename PRNG = PRNG_CMWC>
class Rand
{
public:
	Rand(uint32 _seed = 1): m_prng(_seed)   {}

	void   seed(uint32 _seed)               { m_prng.seed(_seed); }
	uint32 raw()                            { return m_prng.raw(); }

	template <typename tType>
	tType get();
		template <> bool get() 
		{ 
			return (raw() >> 31) != 0;
		}
		template <> float32 get()
		{
			internal::iee754_f32 x;
			x.u = raw();
			x.u &= 0x007fffffu;
			x.u |= 0x3f800000u;
			return x.f - 1.0f;
		}

	template <typename tType>
	tType get(const tType& _min, const tType& _max);
		template <> sint32 get(const sint32& _min, const sint32& _max)
		{
			uint64 i = (uint64)raw() * (_max - _min + 1);
			uint32 j = (uint32)(i >> 32);
			return (sint32)j + _min;
		}
		template <> float32 get(const float32& _min, const float32& _max)
		{
			float32 f = get<float32>();
			return _min + f * (_max - _min);
		}

		template <> vec2 get(const vec2& _min, const vec2& _max)
		{
			return vec2(
				get(_min.x, _max.x),
				get(_min.y, _max.y)
				);
		}
		template <> vec3 get(const vec3& _min, const vec3& _max)
		{
			return vec3(
				get(_min.x, _max.x),
				get(_min.y, _max.y),
				get(_min.z, _max.z)
				);
		}
		template <> vec4 get(const vec4& _min, const vec4& _max)
		{
			return vec4(
				get(_min.x, _max.x),
				get(_min.y, _max.y),
				get(_min.z, _max.z),
				get(_min.w, _max.w)
				);
		}
private:
	PRNG m_prng;
};

} // namespace apt
