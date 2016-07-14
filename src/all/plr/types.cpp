////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/types.h>
#include <plr/def.h>

using namespace plr;

uint plr::DataTypeSize(const DataType _type)
{
	#define CASE_TYPE(_t) case _t: return sizeof(DataTypeT<_t>::Type)
	switch (_type) {
		CASE_TYPE(DataType::kSint8);
		CASE_TYPE(DataType::kSint16);
		CASE_TYPE(DataType::kSint32);
		CASE_TYPE(DataType::kSint64);

		CASE_TYPE(DataType::kUint8);
		CASE_TYPE(DataType::kUint16);
		CASE_TYPE(DataType::kUint32);
		CASE_TYPE(DataType::kUint64);

		//CASE_TYPE(DataType::kFloat16);
		CASE_TYPE(DataType::kFloat32);
		CASE_TYPE(DataType::kFloat64);

		default: PLR_ASSERT(false);
	};
	#undef CASE_TYPE
	return 0;
};
