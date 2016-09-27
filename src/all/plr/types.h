////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_types_h
#define plr_types_h

#include <cfloat>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <limits>

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4244) // possible loss of data
#endif

namespace plr {

typedef std::int8_t     sint8;
typedef std::uint8_t    uint8;
typedef std::int16_t    sint16;
typedef std::uint16_t   uint16;
typedef std::int32_t    sint32;
typedef std::uint32_t   uint32;
typedef std::int64_t    sint64;
typedef std::uint64_t   uint64;
struct                  float16 { uint16 m_val; }; // \todo implement (use glm internally)
typedef float           float32;
typedef double          float64;
typedef std::ptrdiff_t  sint;
typedef std::size_t     uint;

namespace internal {
	template <typename tType>
	class normalized_int
	{
		tType m_val;
	public:
		typedef tType BaseType;

		normalized_int() {}		
		template <typename tSrc>
		normalized_int(tSrc _src): m_val(_src) {}
		operator tType&() { return m_val; }

	}; // class normalized_int

	template <typename tSrc, typename tDst>
	tDst normalized_int_convert(tSrc _src);

} // namespace internal

typedef internal::normalized_int<sint8>  sint8N;
typedef internal::normalized_int<uint8>  uint8N;
typedef internal::normalized_int<sint16> sint16N;
typedef internal::normalized_int<uint16> uint16N;
typedef internal::normalized_int<sint32> sint32N;
typedef internal::normalized_int<uint32> uint32N;
typedef internal::normalized_int<sint64> sint64N;
typedef internal::normalized_int<uint64> uint64N;

struct DataType
{
	enum Enum {
		kInvalidType,

		kSint8,
		kUint8,
		kSint16,
		kUint16,
		kSint32,
		kUint32,
		kSint64,		
		kUint64,
		
		kSint8N,
		kUint8N,
		kSint16N,
		kUint16N,
		kSint32N,
		kUint32N,
		kSint64N,
		kUint64N,

		kFloat16,
		kFloat32,
		kFloat64,

	};

	// helper macro; instantiate _macro for all enum-type pairs
	#define plr_DataType_decl(_macro) \
		_macro(kSint8,   sint8)   \
		_macro(kUint8,   uint8)   \
		_macro(kSint16,  sint16)  \
		_macro(kUint16,  uint16)  \
		_macro(kSint32,  sint32)  \
		_macro(kUint32,  uint32)  \
		_macro(kSint64,  sint64)  \
		_macro(kUint64,  uint64)  \
		_macro(kSint8N,  sint8N)  \
		_macro(kUint8N,  uint8N)  \
		_macro(kSint16N, sint16N) \
		_macro(kUint16N, uint16N) \
		_macro(kSint32N, sint32N) \
		_macro(kUint32N, uint32N) \
		_macro(kSint64N, sint64N) \
		_macro(kUint64N, uint64N) \
		_macro(kFloat16, float16) \
		_macro(kFloat32, float32) \
		_macro(kFloat64, float64)

	/// Implicit conversions to/from Enum (pass and store DataType, not DataType::Enum).
	DataType(Enum _enum = kInvalidType): m_val(_enum)  {}
	operator Enum() const { return m_val; }

	template <typename tType>
	DataType(tType _val): m_val((Enum)_val) {}

	/// \return Size in bytes of the type corresponding to _type.
	static uint GetSizeBytes(DataType _type)
	{
		#define plr_DataType_case_enum(_enum, _typename) \
			case _enum : return sizeof(_typename);
		switch (_type) {
			plr_DataType_decl(plr_DataType_case_enum)
			default: return 0;
		};
		#undef plr_DataType_case_enum
	}

	/// Basic traits.
	static bool IsNormalized(DataType _type) { return (_type >= kSint8N) && (_type <= kUint64N); }
	static bool IsFloat(DataType _type)      { return (_type >= kFloat16) && (_type <= kFloat64); }
	static bool IsInt(DataType _type)        { return (_type >= kSint8) && (_type <= kUint64N); }
	static bool IsSigned(DataType _type)     { return IsFloat(_type) || (_type % 2 != 0); }

	template <typename tType> struct ToEnum {};
		#define plr_DataType_ToEnum(_enum, _typename) \
			template<> struct ToEnum<_typename> { static const DataType::Enum Enum = _enum; };
		plr_DataType_decl(plr_DataType_ToEnum)
		#undef plr_DataType_ToEnum

	template <Enum kEnum> struct ToType {};
		#define plr_DataType_ToType(_enum, _typename) \
			template<> struct ToType<_enum> { typedef _typename Type; };
		plr_DataType_decl(plr_DataType_ToType)
		#undef plr_DataType_ToType
		template<> struct ToType<kInvalidType> { typedef sint8 Type; }; // required so that ToType<(DataType::Enum)(kSint8 - 1)> will compile

	/// Copy data from _src to _dst converting from _srcType to _dstType.
	static void Convert(DataType _srcType, DataType _dstType, const void* _src, void* dst_);

	/// Convert _src to tDst.
	template <typename tSrc, typename tDst>
	static tDst Convert(tSrc _src);
	
private:
	Enum m_val;

	#undef plr_DataType_decl
}; // struct DataType

template <typename tSrc>
struct numeric_limits {};
	template <> struct numeric_limits<sint8N>:  public std::numeric_limits<sint8N::BaseType>  {};
	template <> struct numeric_limits<uint8N>:  public std::numeric_limits<uint8N::BaseType>  {};
	template <> struct numeric_limits<sint16N>: public std::numeric_limits<sint16N::BaseType> {};
	template <> struct numeric_limits<uint16N>: public std::numeric_limits<uint16N::BaseType> {};
	template <> struct numeric_limits<sint32N>: public std::numeric_limits<sint32N::BaseType> {};
	template <> struct numeric_limits<uint32N>: public std::numeric_limits<uint32N::BaseType> {};
	template <> struct numeric_limits<sint64N>: public std::numeric_limits<sint64N::BaseType> {};
	template <> struct numeric_limits<uint64N>: public std::numeric_limits<uint64N::BaseType> {};


namespace Bitfield {
	template <typename tType>
	inline tType BitMask(int _count) 
	{ 
		return (1 << _count) - 1; 
	}
	
	template <typename tType>
	inline tType Insert(tType _base, tType _insert, int _offset, int _count) 
	{ 
		tType mask = BitMask<tType>(_count);
		return (_base & ~(mask << _offset)) | ((_insert & mask) << _offset);
	}

	template <typename tType>
	inline tType Extract(tType _base, int _offset, int _count) 
	{ 
		tType mask = BitMask<tType>(_count) << _offset;
		return (_base & mask) >> _offset;
	}
}

} // namespace plr

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif // plr_types_h
