////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_types_h
#define plr_types_h

namespace plr {

enum class DataType
{
	kSint8,
	kSint32,
	kSint16,
	kUint8,
	kUint16,
	kUint32,
	kFloat16,
	kFloat32,

	kInvalid
};

/// Data type tags (convert DataType to an actual type).
template <DataType T> struct DataTypeT {};
	template<> struct DataTypeT<DataType::kSint8>    { typedef sint8  Type; };
	template<> struct DataTypeT<DataType::kSint16>   { typedef sint16 Type; };
	template<> struct DataTypeT<DataType::kSint32>   { typedef sint32 Type; };
	template<> struct DataTypeT<DataType::kUint8>    { typedef uint8  Type; };
	template<> struct DataTypeT<DataType::kUint16>   { typedef uint16 Type; };
	template<> struct DataTypeT<DataType::kUint32>   { typedef uint32 Type; };
	//template<> struct DataTypeT<DataType::kFloat16>  { typedef float16 Type; }; // \todo
	template<> struct DataTypeT<DataType::kFloat32>  { typedef float  Type; };

} // namespace plr

#endif // plr_types_h
