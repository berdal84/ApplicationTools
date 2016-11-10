#pragma once
#ifndef apt_types_h
#define apt_types_h

#include <cfloat>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <limits>

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4244) // possible loss of data
#endif

namespace apt {

typedef std::int8_t     sint8;
typedef std::uint8_t    uint8;
typedef std::int16_t    sint16;
typedef std::uint16_t   uint16;
typedef std::int32_t    sint32;
typedef std::uint32_t   uint32;
typedef std::int64_t    sint64;
typedef std::uint64_t   uint64;
typedef float           float32;
typedef double          float64;
typedef std::ptrdiff_t  sint;
typedef std::size_t     uint;

// float16 is used for storage only (half-precision floating point ops are not supported)
// \todo implement if this is required
struct float16 { uint16 m_val; };

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
	#define APT_DataType_decl(_macro) \
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
		#define APT_DataType_case_enum(_enum, _typename) \
			case _enum : return sizeof(_typename);
		switch (_type) {
			APT_DataType_decl(APT_DataType_case_enum)
			default: return 0;
		};
		#undef APT_DataType_case_enum
	}

	/// Basic traits.
	static bool IsNormalized(DataType _type) { return (_type >= kSint8N) && (_type <= kUint64N); }
	static bool IsFloat(DataType _type)      { return (_type >= kFloat16) && (_type <= kFloat64); }
	static bool IsInt(DataType _type)        { return (_type >= kSint8) && (_type <= kUint64N); }
	static bool IsSigned(DataType _type)     { return IsFloat(_type) || (_type % 2 != 0); }

	template <typename tType> struct ToEnum {};
		#define APT_DataType_ToEnum(_enum, _typename) \
			template<> struct ToEnum<_typename> { static const DataType::Enum Enum = _enum; };
		APT_DataType_decl(APT_DataType_ToEnum)
		#undef APT_DataType_ToEnum

	template <Enum kEnum> struct ToType {};
		#define APT_DataType_ToType(_enum, _typename) \
			template<> struct ToType<_enum> { typedef _typename Type; };
		APT_DataType_decl(APT_DataType_ToType)
		#undef APT_DataType_ToType
		template<> struct ToType<kInvalidType> { typedef sint8 Type; }; // required so that ToType<(DataType::Enum)(kSint8 - 1)> will compile

	/// Copy _count objects from _src to _dst converting from _srcType to _dstType.
	static void Convert(DataType _srcType, DataType _dstType, const void* _src, void* dst_, uint _count = 1);

	/// Convert _src to tDst.
	template <typename tSrc, typename tDst>
	static tDst Convert(tSrc _src);


private:
	Enum m_val;

	#undef APT_DataType_decl
}; // struct DataType

namespace Bitfield {
	/// Create a bit mask covering _count bits.
	template <typename tType>
	inline tType Mask(int _count) 
	{ 
		return (1 << _count) - 1; 
	}

	/// Create a bit mask covering _count bits starting at _offset.
	template <typename tType>
	inline tType Mask(int _offset, int _count) 
	{ 
		return ((1 << _count) - 1) << _offset; 
	}
	
	/// Insert _count least significant bits from _insert into _base at _offset.
	template <typename tType>
	inline tType Insert(tType _base, tType _insert, int _offset, int _count) 
	{ 
		tType mask = Mask<tType>(_count);
		return (_base & ~(mask << _offset)) | ((_insert & mask) << _offset);
	}

	/// Extract _count bits from _base starting at _offset into the _count least significant bits of the result.
	template <typename tType>
	inline tType Extract(tType _base, int _offset, int _count) 
	{ 
		tType mask = Mask<tType>(_count) << _offset;
		return (_base & mask) >> _offset;
	}
}

} // namespace apt

namespace std {

/// numeric_limits specializations for normalized int types.
template <> class numeric_limits<apt::sint8N>:  public std::numeric_limits<apt::sint8N::BaseType>  {};
template <> class numeric_limits<apt::uint8N>:  public std::numeric_limits<apt::uint8N::BaseType>  {};
template <> class numeric_limits<apt::sint16N>: public std::numeric_limits<apt::sint16N::BaseType> {};
template <> class numeric_limits<apt::uint16N>: public std::numeric_limits<apt::uint16N::BaseType> {};
template <> class numeric_limits<apt::sint32N>: public std::numeric_limits<apt::sint32N::BaseType> {};
template <> class numeric_limits<apt::uint32N>: public std::numeric_limits<apt::uint32N::BaseType> {};
template <> class numeric_limits<apt::sint64N>: public std::numeric_limits<apt::sint64N::BaseType> {};
template <> class numeric_limits<apt::uint64N>: public std::numeric_limits<apt::uint64N::BaseType> {};

} // namespace std

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif // apt_types_h
