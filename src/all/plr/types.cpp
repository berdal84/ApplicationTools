////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/types.h>
#include <plr/def.h>

using namespace plr;

uint DataType::GetSizeBytes(DataType _type)
{
	#define CASE_ENUM(_type) case DataType:: ## k ## _type: return sizeof(DataType::Traits<DataType:: ## k ## _type>::Type)
	switch (_type) {
		CASE_ENUM(Sint8);
		CASE_ENUM(Sint16);
		CASE_ENUM(Sint32);
		CASE_ENUM(Sint64);
		CASE_ENUM(Uint8);
		CASE_ENUM(Uint16);
		CASE_ENUM(Uint32);
		CASE_ENUM(Uint64);
		//CASE_ENUM(Float16);
		CASE_ENUM(Float32);
		CASE_ENUM(Float64);
		CASE_ENUM(Sint8N);
		CASE_ENUM(Sint16N);
		CASE_ENUM(Sint32N);
		CASE_ENUM(Sint64N);
		CASE_ENUM(Uint8N);
		CASE_ENUM(Uint16N);
		CASE_ENUM(Uint32N);
		CASE_ENUM(Uint64N);
		default: PLR_ASSERT(false);
	};
	#undef CASE_ENUM
	return 0;
}

const char* DataType::GetStr(DataType _type)
{
	#define CASE_ENUM(_type) case DataType:: ## k ## _type: return #_type;
	switch (_type) {
		CASE_ENUM(Sint8);
		CASE_ENUM(Sint16);
		CASE_ENUM(Sint32);
		CASE_ENUM(Sint64);
		CASE_ENUM(Uint8);
		CASE_ENUM(Uint16);
		CASE_ENUM(Uint32);
		CASE_ENUM(Uint64);
		//CASE_ENUM(Float16);
		CASE_ENUM(Float32);
		CASE_ENUM(Float64);
		CASE_ENUM(Sint8N);
		CASE_ENUM(Sint16N);
		CASE_ENUM(Sint32N);
		CASE_ENUM(Sint64N);
		CASE_ENUM(Uint8N);
		CASE_ENUM(Uint16N);
		CASE_ENUM(Uint32N);
		CASE_ENUM(Uint64N);

		default: PLR_ASSERT(false);
	};
	#undef CASE_ENUM
	return "Invalid Type";
}

const plr::DataType::Traits<DataType::kFloat32>::Type plr::DataType::Traits<DataType::kFloat32>::Min = FLT_MIN;
const plr::DataType::Traits<DataType::kFloat32>::Type plr::DataType::Traits<DataType::kFloat32>::Max = FLT_MAX;

const plr::DataType::Traits<DataType::kFloat64>::Type plr::DataType::Traits<DataType::kFloat64>::Min = DBL_MIN;
const plr::DataType::Traits<DataType::kFloat64>::Type plr::DataType::Traits<DataType::kFloat64>::Max = DBL_MAX;