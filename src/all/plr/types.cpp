////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/types.h>
#include <plr/def.h>

using namespace plr;

const DataType::Traits<float32>::Type DataType::Traits<float32>::Min = FLT_MIN;
const DataType::Traits<float32>::Type DataType::Traits<float32>::Max = FLT_MAX;
const DataType::Traits<float64>::Type DataType::Traits<float64>::Min = DBL_MIN;
const DataType::Traits<float64>::Type DataType::Traits<float64>::Max = DBL_MAX;
