////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_types_h
#define plr_types_h

#include <cstddef> // size_t
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

typedef std::ptrdiff_t  sint;
typedef std::size_t     uint;

enum class DataType
{
	kSint8,
	kSint32,
	kSint16,
	kSint64,
	
	kUint8,
	kUint16,
	kUint32,
	kUint64,

	kFloat16,
	kFloat32,
	kFloat64,

	kInvalid
};

/// Data type tags (convert DataType to an actual type).
template <DataType T> struct DataTypeT {};
	template<> struct DataTypeT<DataType::kSint8>    { typedef sint8   Type; };
	template<> struct DataTypeT<DataType::kSint16>   { typedef sint16  Type; };
	template<> struct DataTypeT<DataType::kSint32>   { typedef sint32  Type; };
	template<> struct DataTypeT<DataType::kSint64>   { typedef sint64  Type; };
	template<> struct DataTypeT<DataType::kUint8>    { typedef uint8   Type; };
	template<> struct DataTypeT<DataType::kUint16>   { typedef uint16  Type; };
	template<> struct DataTypeT<DataType::kUint32>   { typedef uint32  Type; };
	template<> struct DataTypeT<DataType::kUint64>   { typedef uint64  Type; };
	//template<> struct DataTypeT<DataType::kFloat16>  { typedef float16 Type; }; // todo
	template<> struct DataTypeT<DataType::kFloat32>  { typedef float32 Type; };
	template<> struct DataTypeT<DataType::kFloat64>  { typedef float64 Type; };

/// \return Size (bytes) of DataType _type.
uint DataTypeSize(const DataType _type);

} // namespace plr

#endif // plr_types_h
