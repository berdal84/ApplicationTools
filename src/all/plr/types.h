////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_types_h
#define plr_types_h

#include <cstddef> // size_t
#include <cfloat>
#include <cstdint>

namespace plr {

typedef std::int8_t     sint8;
typedef std::int16_t    sint16;
typedef std::int32_t    sint32;
typedef std::int64_t    sint64;
typedef std::uint8_t    uint8;
typedef std::uint16_t   uint16;
typedef std::uint32_t   uint32;
typedef std::uint64_t   uint64;
typedef float           float32;
typedef double          float64;
typedef sint8           sint8N;
typedef sint16          sint16N;
typedef sint32          sint32N;
typedef sint64          sint64N;
typedef uint8           uint8N;
typedef uint16          uint16N;
typedef uint32          uint32N;
typedef uint64          uint64N;

typedef std::ptrdiff_t  sint;
typedef std::size_t     uint;

struct float16          { uint16 m_val; }; // \todo implement (use glm internally)

struct DataType
{
	enum Enum {
		kSint8,
		kSint16,
		kSint32,
		kSint64,		
		kUint8,
		kUint16,
		kUint32,
		kUint64,
		kFloat16,
		kFloat32,
		kFloat64,

	 // normalized integer types
		kSint8N,
		kSint16N,
		kSint32N,
		kSint64N,
		kUint8N,
		kUint16N,
		kUint32N,
		kUint64N,

		kInvalid
	};


	/// Implicit conversions to/from Enum (pass and store DataType, not DataType::Enum).
	DataType(Enum _enum = kInvalid): m_val(_enum)  {}
	operator Enum() const { return m_val; }

	/// Init from a value of tType.
	template <typename tType>
	DataType(tType _val, bool _normalized =  false);
		template<> DataType(sint8  _val, bool _normalized):  m_val(_normalized ? kSint8N  : kSint8)  {}
		template<> DataType(sint16 _val, bool _normalized):  m_val(_normalized ? kSint16N : kSint16) {}
		template<> DataType(sint32 _val, bool _normalized):  m_val(_normalized ? kSint32N : kSint32) {}
		template<> DataType(sint64 _val, bool _normalized):  m_val(_normalized ? kSint64N : kSint64) {}
		template<> DataType(uint8  _val, bool _normalized):  m_val(_normalized ? kUint8N  : kUint8)  {}
		template<> DataType(uint16 _val, bool _normalized):  m_val(_normalized ? kUint16N : kUint16) {}
		template<> DataType(uint32 _val, bool _normalized):  m_val(_normalized ? kUint32N : kUint32) {}
		template<> DataType(uint64 _val, bool _normalized):  m_val(_normalized ? kUint64N : kUint64) {}		
		template<> DataType(float16 _val, bool _normalized): m_val(kFloat16) {}
		template<> DataType(float32 _val, bool _normalized): m_val(kFloat32) {}
		template<> DataType(float64 _val, bool _normalized): m_val(kFloat64) {}

	/// Get type metadata
	static uint        GetSizeBytes(DataType _type);
	static const char* GetStr(DataType _type);
	static bool        IsNormalized(DataType _type)  { return (_type >= kSint8N) && (_type <= kUint64N); }
	static bool        IsFloat(DataType _type)       { return (_type >= kFloat16) && (_type <= kFloat64); }
	static bool        IsInteger(DataType _type)     { return !IsFloat(_type); }
	static bool        IsSigned(DataType _type)      { return IsFloat(_type) || ((_type >= kSint8) && (_type <= kSint64)) || ((_type >= kSint8N) && (_type <= kSint64N)); }
	

	template <DataType::Enum T> struct Traits {};
		template<> struct Traits<DataType::kSint8>    { typedef sint8   Type; static const Type Min = INT8_MIN;   static const Type Max = INT8_MAX;   };
		template<> struct Traits<DataType::kSint16>   { typedef sint16  Type; static const Type Min = INT16_MIN;  static const Type Max = INT16_MAX;  };
		template<> struct Traits<DataType::kSint32>   { typedef sint32  Type; static const Type Min = INT32_MIN;  static const Type Max = INT32_MAX;  };
		template<> struct Traits<DataType::kSint64>   { typedef sint64  Type; static const Type Min = INT64_MIN;  static const Type Max = INT64_MAX;  };
		template<> struct Traits<DataType::kUint8>    { typedef uint8   Type; static const Type Min = 0;          static const Type Max = UINT8_MAX;  };
		template<> struct Traits<DataType::kUint16>   { typedef uint16  Type; static const Type Min = 0;          static const Type Max = UINT16_MAX; };
		template<> struct Traits<DataType::kUint32>   { typedef uint32  Type; static const Type Min = 0;          static const Type Max = UINT32_MAX; };
		template<> struct Traits<DataType::kUint64>   { typedef uint64  Type; static const Type Min = 0;          static const Type Max = UINT64_MAX; };
		template<> struct Traits<DataType::kFloat16>  { typedef float16 Type; static const Type Min;              static const Type Max;              }; // \todo
		template<> struct Traits<DataType::kFloat32>  { typedef float32 Type; static const Type Min;              static const Type Max;              };
		template<> struct Traits<DataType::kFloat64>  { typedef float64 Type; static const Type Min;              static const Type Max;              };
		template<> struct Traits<DataType::kSint8N>   { typedef sint8   Type; static const Type Min = INT8_MIN;   static const Type Max = INT8_MAX;   };
		template<> struct Traits<DataType::kSint16N>  { typedef sint16  Type; static const Type Min = INT16_MIN;  static const Type Max = INT16_MAX;  };
		template<> struct Traits<DataType::kSint32N>  { typedef sint32  Type; static const Type Min = INT32_MIN;  static const Type Max = INT32_MAX;  };
		template<> struct Traits<DataType::kSint64N>  { typedef sint64  Type; static const Type Min = INT64_MIN;  static const Type Max = INT64_MAX;  };
		template<> struct Traits<DataType::kUint8N>   { typedef uint8   Type; static const Type Min = 0;          static const Type Max = UINT8_MAX;  };
		template<> struct Traits<DataType::kUint16N>  { typedef uint16  Type; static const Type Min = 0;          static const Type Max = UINT16_MAX; };
		template<> struct Traits<DataType::kUint32N>  { typedef uint32  Type; static const Type Min = 0;          static const Type Max = UINT32_MAX; };
		template<> struct Traits<DataType::kUint64N>  { typedef uint64  Type; static const Type Min = 0;          static const Type Max = UINT64_MAX; };

	/// Convert _src from type kSrc to kDst. 
	template <Enum kSrc, Enum kDst>
	static typename Traits<kDst>::Type Convert(typename Traits<kSrc>::Type _src);

private:
	Enum m_val;

}; // class DataType


template <DataType::Enum kSrc, DataType::Enum kDst>
static inline typename DataType::Traits<kDst>::Type DataType::Convert(typename Traits<kSrc>::Type _src)
{
	typedef typename Traits<kSrc>::Type tSrc;
	typedef typename Traits<kDst>::Type tDst;

	if (kSrc == kDst) {
		return (tDst)_src;
	}

	if (IsNormalized(kSrc)) {
	 // \todo define the normalization behavior: what about signed values?
		if (IsFloat(kDst)) {
			return (tDst)_src / (tDst)Traits<kSrc>::Max; // \todo only works if range is symmetrical around 0
		} else {
			return (tDst)_src * ((tDst)Traits<kDst>::Max / (tDst)Traits<kSrc>::Max); // \todo doesn't work converting to a lower precision
		}
	} else {
		return (tDst)_src;
	}
}

} // namespace plr

#endif // plr_types_h
