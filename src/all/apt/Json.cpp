#include <apt/Json.h>

#include <apt/log.h>
#include <apt/math.h>
#include <apt/memory.h>
#include <apt/FileSystem.h>
#include <apt/String.h>
#include <apt/Time.h>

#include <EASTL/vector.h>

#include <cstring>

#define RAPIDJSON_ASSERT(x) APT_ASSERT(x)
#define RAPIDJSON_PARSE_DEFAULT_FLAGS (kParseFullPrecisionFlag | kParseCommentsFlag | kParseTrailingCommasFlag)
#include <rapidjson/error/en.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

using namespace apt;

static Json::ValueType GetValueType(rapidjson::Type _type)
{
	switch (_type) {
		case rapidjson::kNullType:   return Json::ValueType_Null;
		case rapidjson::kObjectType: return Json::ValueType_Object;
		case rapidjson::kArrayType:  return Json::ValueType_Array;
		case rapidjson::kFalseType:
		case rapidjson::kTrueType:   return Json::ValueType_Bool;
		case rapidjson::kNumberType: return Json::ValueType_Number;
		case rapidjson::kStringType: return Json::ValueType_String;
		default: APT_ASSERT(false); break;
	};

	return Json::ValueType_Count;
}

/*******************************************************************************

                                   Json

*******************************************************************************/

struct Json::Impl
{
	rapidjson::Document m_dom;

 // current value set after find()
	rapidjson::Value* m_value = nullptr;

 // value stack for objects/arrays
	eastl::vector<eastl::pair<rapidjson::Value*, int> > m_stack;

	void push(rapidjson::Value* _val = nullptr)
	{
		APT_ASSERT(m_stack.empty() || top() != _val); // probably a mistake, called push() twice?
		m_stack.push_back(eastl::make_pair(_val ? _val : m_value, 0));
	}
	void pop()
	{
		APT_ASSERT(!m_stack.empty());
		m_stack.pop_back();
	}
	rapidjson::Value* top() 
	{
		APT_ASSERT(!m_stack.empty());
		return m_stack.back().first;
	}
	int& topIter()
	{
		APT_ASSERT(!m_stack.empty());
		return m_stack.back().second;
	}

	// Get the current value, optionally access the element _i if an array.
	rapidjson::Value* get(int _i = -1) 
	{
		rapidjson::Value* ret = m_value;
		APT_ASSERT(ret);
		if (_i >= 0 && GetValueType(ret->GetType()) == ValueType_Array) {
			int n = (int)ret->GetArray().Size();
			APT_ASSERT_MSG(_i < n, "Array index out of bounds (%d/%d)", _i, n);
			ret = &ret->GetArray()[_i];
		}
		return ret;
	}
};


// PUBLIC

bool Json::Read(Json& json_, const File& _file)
{
	json_.m_impl->m_dom.Parse(_file.getData());
	if (json_.m_impl->m_dom.HasParseError()) {
		APT_LOG_ERR("Json error: %s\n\t'%s'", _file.getPath(), rapidjson::GetParseError_En(json_.m_impl->m_dom.GetParseError()));
		return false;
	}
	return true;
}

bool Json::Read(Json& json_, const char* _path, FileSystem::RootType _rootHint)
{
	APT_AUTOTIMER("Json::Read(%s)", _path);
	File f;
	if (!FileSystem::ReadIfExists(f, _path, _rootHint)) {
		return false;
	}
	return Read(json_, f);
}

bool Json::Write(const Json& _json, File& file_)
{
	rapidjson::StringBuffer buf;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(buf);
	wr.SetIndent('\t', 1);
	wr.SetFormatOptions(rapidjson::kFormatSingleLineArray);
	_json.m_impl->m_dom.Accept(wr);
	file_.setData(buf.GetString(), buf.GetSize());
	return true;
}

bool Json::Write(const Json& _json, const char* _path, FileSystem::RootType _rootHint)
{
	APT_AUTOTIMER("Json::Write(%s)", _path);
	File f;
	if (Write(_json, f)) {
		return FileSystem::Write(f, _path, _rootHint);
	}
	return false;
}

Json::Json(const char* _path, FileSystem::RootType _rootHint)
	: m_impl(nullptr)
{
	m_impl = APT_NEW(Impl);
	m_impl->m_dom.SetObject();
	m_impl->push(&m_impl->m_dom);

	if (_path) {
		Json::Read(*this, _path, _rootHint);
	}
}

Json::~Json()
{
	if (m_impl) {
		APT_DELETE(m_impl);
	}
}

bool Json::find(const char* _name)
{
	rapidjson::Value* top = m_impl->top();

	if (!top->IsObject()) {
		return false;
	}
	auto it = top->FindMember(_name);
	if (it != top->MemberEnd()) {
		m_impl->m_value = &it->value;
		return true;
	}
	return false;
}

bool Json::next()
{
	rapidjson::Value* top = m_impl->top();

	if (GetValueType(top->GetType()) == ValueType_Array) {
		auto it = top->Begin() + (m_impl->topIter()++);
		m_impl->m_value = it;
		return it != top->End();
	} else if (GetValueType(top->GetType()) == ValueType_Object) {
		auto it = top->MemberBegin() + (m_impl->topIter()++);
		m_impl->m_value = &it->value;
		return it != top->MemberEnd();
	}
	APT_ASSERT(false); // not an object or an array
	return false;
}

Json::ValueType Json::getType() const
{
	return GetValueType(m_impl->m_value->GetType()); 
}

template <> bool Json::getValue<bool>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Bool, "Json::getValue: not a bool");
	return jsonValue->GetBool();
}

template <> sint64 Json::getValue<sint64>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetInt64();
}
template <> sint32 Json::getValue<sint32>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetInt();
}
template <> sint16 Json::getValue<sint16>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetInt();
}
template <> sint8 Json::getValue<sint8>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetInt();
}
template <> uint64 Json::getValue<uint64>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetUint64();
}
template <> uint32 Json::getValue<uint32>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetUint();
}
template <> uint16 Json::getValue<uint16>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetUint();
}
template <> uint8 Json::getValue<uint8>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetUint();
}
template <> float32 Json::getValue<float32>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetFloat();
}
template <> float64 Json::getValue<float64>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Number, "Json::getValue: not a number");
	return jsonValue->GetDouble();
}
template <> const char* Json::getValue<const char*>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_String, "Json::getValue: not a string");
	return jsonValue->GetString();
}
template <> vec2 Json::getValue<vec2>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Array, "Json::getValue: not an array");
	APT_ASSERT_MSG(jsonValue->Size() == 2, "Json::getValue: invalid vec2, size = %d", jsonValue->Size());
	auto& arr = jsonValue->GetArray();
	return vec2(arr[0].GetFloat(), arr[1].GetFloat());
}
template <> vec3 Json::getValue<vec3>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Array, "Json::getValue: not an array");
	APT_ASSERT_MSG(jsonValue->Size() == 3, "Json::getValue: invalid vec3, size = %d", jsonValue->Size());
	auto& arr = jsonValue->GetArray();
	return vec3(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat());
}
template <> vec4 Json::getValue<vec4>(int _i) const
{
	const rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Array, "Json::getValue: not an array");
	APT_ASSERT_MSG(jsonValue->Size() == 4, "Json::getValue: invalid vec4, size = %d", jsonValue->Size());
	auto& arr = jsonValue->GetArray();
	return vec4(arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat(), arr[3].GetFloat());
}
template <> mat2 Json::getValue<mat2>(int _i) const
{
	rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Array, "Json::getValue: not an array");
	APT_ASSERT_MSG(jsonValue->Size() == 2, "Json::getValue: invalid mat2, size = %d (should be 2* vec2)", jsonValue->Size());
	mat2 ret;
	m_impl->push();
	m_impl->m_value = jsonValue;
	for (int i = 0; i < 2; ++i) {
		ret[i] = getValue<vec2>(i);
	}
	m_impl->m_value = m_impl->top();
	m_impl->pop();
	return ret;
}
template <> mat3 Json::getValue<mat3>(int _i) const
{
	rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Array, "Json::getValue: not an array");
	APT_ASSERT_MSG(jsonValue->Size() == 3, "Json::getValue: invalid mat3, size = %d (should be 3* vec3)", jsonValue->Size());
	mat3 ret;
	m_impl->push();
	m_impl->m_value = jsonValue;
	for (int i = 0; i < 3; ++i) {
		ret[i] = getValue<vec3>(i);
	}
	m_impl->m_value = m_impl->top();
	m_impl->pop();
	return ret;
}
template <> mat4 Json::getValue<mat4>(int _i) const
{
	rapidjson::Value* jsonValue = m_impl->get(_i);
	APT_ASSERT_MSG(GetValueType(jsonValue->GetType()) == ValueType_Array, "Json::getValue: not an array");
	APT_ASSERT_MSG(jsonValue->Size() == 4, "Json::getValue: invalid mat4, size = %d (should be 4* vec4)", jsonValue->Size());
	mat4 ret;
	m_impl->push();
	m_impl->m_value = jsonValue;
	for (int i = 0; i < 4; ++i) {
		ret[i] = getValue<vec4>(i);
	}
	m_impl->m_value = m_impl->top();
	m_impl->pop();
	return ret;
}

bool Json::enterObject()
{
	if (getType() == ValueType_Object) {
		m_impl->push();
		return true;
	}
	APT_ASSERT(false); // not an object
	return false;
}

void Json::leaveObject()
{
	APT_ASSERT(GetValueType(m_impl->top()->GetType()) == ValueType_Object);
	m_impl->m_value = m_impl->top();
	m_impl->pop();
}

bool Json::enterArray()
{
	if (getType() == ValueType_Array) {
		m_impl->push();
		return true;
	}
	APT_ASSERT(false); // not an array
	return false;
}

void Json::leaveArray()
{
	APT_ASSERT(GetValueType(m_impl->top()->GetType()) == ValueType_Array);
	m_impl->m_value = m_impl->top();
	m_impl->pop();
}

int Json::getArrayLength() const
{
	if (GetValueType(m_impl->top()->GetType()) == ValueType_Array) {
		return (int)m_impl->top()->GetArray().Size();
	}
	return -1;
}

void Json::beginObject(const char* _name)
{
	if (_name && find(_name)) {
	  // object already existed, check the type
		APT_ASSERT(GetValueType(m_impl->m_value->GetType()) == ValueType_Object);
	} else {
		if (GetValueType(m_impl->top()->GetType()) == ValueType_Array) {
			if (_name) {
				APT_LOG("Json warning: calling beginObject() in an array, name '%s' will be ignored", _name);
			}
			m_impl->top()->PushBack(
				rapidjson::Value(rapidjson::kObjectType).Move(), 
				m_impl->m_dom.GetAllocator()
				);
			m_impl->m_value = m_impl->top()->End() - 1;

		} else {
			m_impl->top()->AddMember(
				rapidjson::StringRef(_name),
				rapidjson::Value(rapidjson::kObjectType).Move(), 
				m_impl->m_dom.GetAllocator()
				);
			m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
		}
	}
	APT_VERIFY(enterObject());
}

void Json::beginArray(const char* _name)
{
	if (_name && find(_name)) {
	  // object already existed, check the type
		APT_ASSERT(GetValueType(m_impl->m_value->GetType()) == ValueType_Array);
	} else {
		if (GetValueType(m_impl->top()->GetType()) == ValueType_Array) {
			if (_name) {
				APT_LOG("Json warning: calling beginArray() in an array, name '%s' will be ignored", _name);
			}
			m_impl->top()->PushBack(
				rapidjson::Value(rapidjson::kArrayType).Move(), 
				m_impl->m_dom.GetAllocator()
				);
			m_impl->m_value = m_impl->top()->End() - 1;

		} else {
			m_impl->top()->AddMember(
				rapidjson::StringRef(_name),
				rapidjson::Value(rapidjson::kArrayType).Move(), 
				m_impl->m_dom.GetAllocator()
				);
			m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
		}
	}
	APT_VERIFY(enterArray());
}


template <> void Json::setValue<bool>(const char* _name, bool _val)
{
	if (find(_name)) {
		m_impl->m_value->SetBool(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<bool>(int _i, bool _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetBool(_val);
	} else {
		m_impl->m_value->SetBool(_val);
	}
}
template <> void Json::setValue<sint32>(const char* _name, sint32 _val)
{
	if (find(_name)) {
		m_impl->m_value->SetInt(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<sint32>(int _i, sint32 _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetInt(_val);
	} else {
		m_impl->m_value->SetInt(_val);
	}
}
template <> void Json::setValue<sint64>(const char* _name, sint64 _val)
{
	if (find(_name)) {
		m_impl->m_value->SetInt64(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<sint64>(int _i, sint64 _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetInt64(_val);
	} else {
		m_impl->m_value->SetInt64(_val);
	}
}
template <> void Json::setValue<sint16>(const char* _name, sint16 _val)
{
	setValue<sint32>(_name, (sint32)_val);
}
template <> void Json::setValue<sint16>(int _i, sint16 _val)
{
	setValue<sint32>(_i, (sint32)_val);
}
template <> void Json::setValue<sint8>(const char* _name, sint8 _val)
{
	setValue<sint32>(_name, (sint32)_val);
}
template <> void Json::setValue<sint8>(int _i, sint8 _val)
{
	setValue<sint32>(_i, (sint32)_val);
}
template <> void Json::setValue<uint64>(const char* _name, uint64 _val)
{
	if (find(_name)) {
		m_impl->m_value->SetUint64(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<uint64>(int _i, uint64 _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetUint64(_val);
	} else {
		m_impl->m_value->SetUint64(_val);
	}
}
template <> void Json::setValue<uint32>(const char* _name, uint32 _val)
{
	if (find(_name)) {
		m_impl->m_value->SetUint(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<uint32>(int _i, uint32 _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetUint(_val);
	} else {
		m_impl->m_value->SetUint(_val);
	}
}
template <> void Json::setValue<uint16>(const char* _name, uint16 _val)
{
	setValue<uint32>(_name, (uint16)_val);
}
template <> void Json::setValue<uint16>(int _i, uint16 _val)
{
	setValue<uint32>(_i, (uint16)_val);
}
template <> void Json::setValue<uint8>(const char* _name, uint8 _val)
{
	setValue<uint32>(_name, (uint32)_val);
}
template <> void Json::setValue<uint8>(int _i, uint8 _val)
{
	setValue<uint32>(_i, (uint32)_val);
}
template <> void Json::setValue<float32>(const char* _name, float32 _val)
{
	if (find(_name)) {
		m_impl->m_value->SetFloat(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<float32>(int _i, float32 _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetFloat(_val);
	} else {
		m_impl->m_value->SetFloat(_val);
	}
}
template <> void Json::setValue<float64>(const char* _name, float64 _val)
{
	if (find(_name)) {
		m_impl->m_value->SetDouble(_val);
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value(_val).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<float64>(int _i, float64 _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetDouble(_val);
	} else {
		m_impl->m_value->SetDouble(_val);
	}
}
template <> void Json::setValue<const char*>(const char* _name, const char* _val)
{
	if (find(_name)) {
		m_impl->m_value->SetString(_val, m_impl->m_dom.GetAllocator());
	} else {
		m_impl->top()->AddMember(
			rapidjson::StringRef(_name),
			rapidjson::Value().SetString(_val, m_impl->m_dom.GetAllocator()).Move(), 
			m_impl->m_dom.GetAllocator()
			);
		m_impl->m_value = &(m_impl->top()->MemberEnd() - 1)->value;
	}
}
template <> void Json::setValue<const char*>(int _i, const char* _val)
{
	rapidjson::Value* top = m_impl->top();
	if (_i >= 0 && GetValueType(top->GetType()) == ValueType_Array) {
		m_impl->m_value->GetArray()[_i].SetString(_val, m_impl->m_dom.GetAllocator());
	} else {
		m_impl->m_value->SetString(_val, m_impl->m_dom.GetAllocator());
	}
}

template <> void Json::pushValue<bool>(bool _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}

template <> void Json::pushValue<sint64>(sint64 _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}
template <> void Json::pushValue<sint32>(sint32 _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}
template <> void Json::pushValue<sint16>(sint16 _val)
{
	pushValue<sint32>((sint32)_val);
}
template <> void Json::pushValue<sint8>(sint8 _val)
{
	pushValue<sint32>((sint32)_val);
}
template <> void Json::pushValue<uint64>(uint64 _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}
template <> void Json::pushValue<uint32>(uint32 _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}
template <> void Json::pushValue<uint16>(uint16 _val)
{
	pushValue((uint32)_val);
}
template <> void Json::pushValue<uint8>(uint8 _val)
{
	pushValue((uint32)_val);
}
template <> void Json::pushValue<float32>(float32 _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}
template <> void Json::pushValue<float64>(float64 _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}
template <> void Json::pushValue<const char*>(const char* _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value().SetString(_val, m_impl->m_dom.GetAllocator()).Move(),
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}


template <> void Json::setValue<vec2>(const char* _name, vec2 _val)
{
	beginArray(_name);
		pushValue(_val.x);
		pushValue(_val.y);
	leaveArray();
}
template <> void Json::setValue<vec3>(const char* _name, vec3 _val)
{
	beginArray(_name);
		pushValue(_val.x);
		pushValue(_val.y);		
		pushValue(_val.z);
	leaveArray();
}
template <> void Json::setValue<vec4>(const char* _name, vec4 _val)
{
	beginArray(_name);
		pushValue(_val.x);
		pushValue(_val.y);		
		pushValue(_val.z);				
		pushValue(_val.w);
	leaveArray();
}
template <> void Json::pushValue<vec2>(vec2 _val)
{
	beginArray();
		pushValue(_val.x);
		pushValue(_val.y);
	leaveArray();
}
template <> void Json::pushValue<vec3>(vec3 _val)
{
	beginArray();
		pushValue(_val.x);
		pushValue(_val.y);		
		pushValue(_val.z);
	leaveArray();
}
template <> void Json::pushValue<vec4>(vec4 _val)
{
	beginArray();
		pushValue(_val.x);
		pushValue(_val.y);		
		pushValue(_val.z);				
		pushValue(_val.w);
	leaveArray();
}

template <> void Json::setValue<mat2>(const char* _name, mat2 _val)
{
	beginArray(_name);
		for (int i = 0; i < 2; ++i) {
			pushValue<vec2>(_val[i]);
		}
	leaveArray();
}
template <> void Json::setValue<mat3>(const char* _name, mat3 _val)
{
	beginArray(_name);
		for (int i = 0; i < 3; ++i) {
			pushValue<vec3>(_val[i]);
		}
	leaveArray();
}
template <> void Json::setValue<mat4>(const char* _name, mat4 _val)
{
	beginArray(_name);
		for (int i = 0; i < 4; ++i) {
			pushValue<vec4>(_val[i]);
		}
	leaveArray();
}
template <> void Json::pushValue<mat2>(mat2 _val)
{
	beginArray();
		for (int i = 0; i < 2; ++i) {
			pushValue<vec2>(_val[i]);
		}
	leaveArray();
}
template <> void Json::pushValue<mat3>(mat3 _val)
{
	beginArray();
		for (int i = 0; i < 3; ++i) {
			pushValue<vec3>(_val[i]);
		}
	leaveArray();
}
template <> void Json::pushValue<mat4>(mat4 _val)
{
	beginArray();
		for (int i = 0; i < 4; ++i) {
			pushValue<vec4>(_val[i]);
		}
	leaveArray();
}

/*******************************************************************************

                              SerializerJson

*******************************************************************************/

// PUBLIC

SerializerJson::SerializerJson(Json& _json_, Mode _mode)
	: Serializer(_mode) 
	, m_json(&_json_)
{
}

bool SerializerJson::beginObject(const char* _name)
{
	if (getMode() == Mode_Read) {
		if (m_json->getArrayLength() >= 0) { // inside array
			if (!m_json->next()) {
				return false;
			}
		} else {
			APT_ASSERT(_name);
			if (!m_json->find(_name)) {
				setError("SerializerJson::beginObject(); '%s' not found", _name);
				return false;
			}
		}
		if (m_json->getType() == Json::ValueType_Object) {
			m_json->enterObject();
			return true;
		} else {
			setError("SerializerJson::beginObject(); '%s' not an object", _name ? _name : "");
			return false;
		}
	} else {
		m_json->beginObject(_name);
		return true;
	}
}
void SerializerJson::endObject()
{
	if (m_mode == Mode_Read) {
		m_json->leaveObject();
	} else {
		m_json->endObject();
	}
}

bool SerializerJson::beginArray(uint& _length_, const char* _name)
{
	if (m_mode == Mode_Read) {
		if (m_json->getArrayLength() >= 0) { // inside array
			if (!m_json->next()) {
				return false;
			}
		} else {
			APT_ASSERT(_name);
			if (!m_json->find(_name)) {
				setError("SerializerJson::beginArray(); '%s' not found", _name);
				return false;
			}
		}
		if (m_json->getType() == Json::ValueType_Array) {
			m_json->enterArray();
			_length_ = (uint)m_json->getArrayLength();
			return true;
		} else {
			setError("SerializerJson::beginArray(); '%s' not an array", _name ? _name : "");
			return false;
		}
	} else {
		m_json->beginArray(_name);
		return true;
	}
}
void SerializerJson::endArray()
{
	if (m_mode == Mode_Read) {
		m_json->leaveArray();
	} else {
		m_json->endArray();
	}
}

template <typename tType>
static bool ValueImpl(SerializerJson& _serializer_, tType& _value_, const char* _name)
{
	Json* json = _serializer_.getJson();
	if (!_name && json->getArrayLength() == -1) {
		_serializer_.setError("Error serializing %s; name must be specified if not in an array", Serializer::ValueTypeToStr<tType>());
		return false;
	}
	if (_serializer_.getMode() == SerializerJson::Mode_Read) {
		if (_name) {
			if (!json->find(_name)) {
				_serializer_.setError("Error serializing %s; '%s' not found", Serializer::ValueTypeToStr<tType>(), _name);
				return false;
			}
		} else {
			if (!json->next()) {
				return false;
			}
		}
		_value_ = json->getValue<tType>();
		return true;

	} else {
		if (_name) {
			json->setValue<tType>(_name, _value_);
		} else {
			json->pushValue<tType>(_value_);
		}
		return true; 
	}
}

bool SerializerJson::value(bool&    _value_, const char* _name) { return ValueImpl<bool>   (*this, _value_, _name); }
bool SerializerJson::value(sint8&   _value_, const char* _name) { return ValueImpl<sint8>  (*this, _value_, _name); }
bool SerializerJson::value(uint8&   _value_, const char* _name) { return ValueImpl<uint8>  (*this, _value_, _name); }
bool SerializerJson::value(sint16&  _value_, const char* _name) { return ValueImpl<sint16> (*this, _value_, _name); }
bool SerializerJson::value(uint16&  _value_, const char* _name) { return ValueImpl<uint16> (*this, _value_, _name); }
bool SerializerJson::value(sint32&  _value_, const char* _name) { return ValueImpl<sint32> (*this, _value_, _name); }
bool SerializerJson::value(uint32&  _value_, const char* _name) { return ValueImpl<uint32> (*this, _value_, _name); }
bool SerializerJson::value(sint64&  _value_, const char* _name) { return ValueImpl<sint64> (*this, _value_, _name); }
bool SerializerJson::value(uint64&  _value_, const char* _name) { return ValueImpl<uint64> (*this, _value_, _name); }
bool SerializerJson::value(float32& _value_, const char* _name) { return ValueImpl<float32>(*this, _value_, _name); }
bool SerializerJson::value(float64& _value_, const char* _name) { return ValueImpl<float64>(*this, _value_, _name); }

bool SerializerJson::value(StringBase& _value_, const char* _name) 
{ 
	if (!_name && m_json->getArrayLength() == -1) {
		setError("Error serializing StringBase; name must be specified if not in an array");
		return false;
	}
	if (getMode() == SerializerJson::Mode_Read) {
		if (_name) {
			if (!m_json->find(_name)) {
				setError("Error serializing StringBase; '%s' not found", _name);
				return false;
			}
		} else {
			if (!m_json->next()) {
				return false;
			}
		}

		if (m_json->getType() == Json::ValueType_String) {
			_value_.set(m_json->getValue<const char*>());
			return true;
		} else {
			setError("Error serializing StringBase; '%s' not a string", _name ? _name : "");
			return false;
		}

	} else {
		if (_name) {
			m_json->setValue<const char*>(_name, (const char*)_value_);
		} else {
			m_json->pushValue<const char*>((const char*)_value_);
		}
		return true; 
	}
}


// Base64 encode/decode of binary data, adapted from https://github.com/adamvr/arduino-base64
static const char kBase64Alphabet[] = 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/"
		;
static inline void Base64A3ToA4(const unsigned char* _a3, unsigned char* a4_) 
{
	a4_[0] = (_a3[0] & 0xfc) >> 2;
	a4_[1] = ((_a3[0] & 0x03) << 4) + ((_a3[1] & 0xf0) >> 4);
	a4_[2] = ((_a3[1] & 0x0f) << 2) + ((_a3[2] & 0xc0) >> 6);
	a4_[3] = (_a3[2] & 0x3f);
}
static inline void Base64A4ToA3(const unsigned char* _a4, unsigned char* a3_) {
	a3_[0] = (_a4[0] << 2) + ((_a4[1] & 0x30) >> 4);
	a3_[1] = ((_a4[1] & 0xf) << 4) + ((_a4[2] & 0x3c) >> 2);
	a3_[2] = ((_a4[2] & 0x3) << 6) + _a4[3];
}
static inline unsigned char Base64Index(char _c)
{
	if (_c >= 'A' && _c <= 'Z') return _c - 'A';
	if (_c >= 'a' && _c <= 'z') return _c - 71;
	if (_c >= '0' && _c <= '9') return _c + 4;
	if (_c == '+') return 62;
	if (_c == '/') return 63;
	return -1;
}
static void Base64Encode(const char* _in, uint _inSizeBytes, char* out_, uint outSizeBytes_)
{
	uint i = 0;
	uint j = 0;
	uint k = 0;
	unsigned char a3[3];
	unsigned char a4[4];
	while (_inSizeBytes--) {
		a3[i++] = *(_in++);
		if (i == 3) {
			Base64A3ToA4(a3, a4);
			for (i = 0; i < 4; i++) {
				out_[k++] = kBase64Alphabet[a4[i]];
			}
			i = 0;
		}
	}
	if (i) {
		for (j = i; j < 3; j++) {
			a3[j] = '\0';
		}
		Base64A3ToA4(a3, a4);
		for (j = 0; j < i + 1; j++) {
			out_[k++] = kBase64Alphabet[a4[j]];
		}
		while ((i++ < 3)) {
			out_[k++] = '=';
		}
	}
	out_[k] = '\0';
	APT_ASSERT(outSizeBytes_ == k); // overflow
}
static void Base64Decode(const char* _in, uint _inSizeBytes, char* out_, uint outSizeBytes_) {
	uint i = 0;
	uint j = 0;
	uint k = 0;
	unsigned char a3[3];
	unsigned char a4[4];
	while (_inSizeBytes--) {
		if (*_in == '=') {
			break;
		}
		a4[i++] = *(_in++);
		if (i == 4) {
			for (i = 0; i < 4; i++) {
				a4[i] = Base64Index(a4[i]);
			}
			Base64A4ToA3(a4, a3);
			for (i = 0; i < 3; i++) {
				out_[k++] = a3[i];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 4; j++) {
			a4[j] = '\0';
		}
		for (j = 0; j < 4; j++) {
			a4[j] = Base64Index(a4[j]);
		}
		Base64A4ToA3(a4, a3);
		for (j = 0; j < i - 1; j++) {
			out_[k++] = a3[j];
		}
	}
	APT_ASSERT(outSizeBytes_ == k); // overflow
}
static uint Base64EncSizeBytes(uint _sizeBytes) 
{
	uint n = _sizeBytes;
	return (n + 2 - ((n + 2) % 3)) / 3 * 4;
}
static uint Base64DecSizeBytes(char* _buf, uint _sizeBytes) 
{
	uint padCount = 0;
	for (uint i = _sizeBytes - 1; _buf[i] == '='; i--) {
		padCount++;
	}
	return ((6 * _sizeBytes) / 8) - padCount;
}

bool SerializerJson::binary(void*& _data_, uint& _sizeBytes_, const char* _name, CompressionFlags _compressionFlags)
{
	if (getMode() == Mode_Write) {
		APT_ASSERT(_data_);
		char* data = (char*)_data_;
		uint sizeBytes = _sizeBytes_;
		if (_compressionFlags != CompressionFlags_None) {
			data = nullptr;
			Compress(_data_, _sizeBytes_, (void*&)data, sizeBytes, _compressionFlags);
		}
		String<0> str;
		str.setLength(Base64EncSizeBytes(sizeBytes) + 1);
		str[0] = _compressionFlags == CompressionFlags_None ? '0' : '1'; // prepend 0, or 1 if compression
		Base64Encode(data, sizeBytes, (char*)str + 1, str.getLength() - 1);
		if (_compressionFlags != CompressionFlags_None) {
			free(data);
		}
		value((StringBase&)str, _name);

	} else {
		String<0> str;
		value((StringBase&)str, _name);
		bool compressed = str[0] == '1' ? true : false;
		uint binSizeBytes = Base64DecSizeBytes((char*)str + 1, str.getLength() - 1);
		char* bin = (char*)APT_MALLOC(binSizeBytes);
		Base64Decode((char*)str + 1, str.getLength() - 1, bin, binSizeBytes);

		char* ret = bin;
		uint retSizeBytes = binSizeBytes;
		if (compressed) {
			ret = nullptr; // Decompress to allocates the final buffer
			Decompress(bin, binSizeBytes, (void*&)ret, retSizeBytes);
		}
		if (_data_) {
			if (retSizeBytes != _sizeBytes_) {
				setError("Error serializing %s, buffer size was %llu (expected %llu)", _sizeBytes_, retSizeBytes);
				if (compressed) {
					APT_FREE(ret);
				}
				return false;
			}
			memcpy(_data_, ret, retSizeBytes);
			if (compressed) {
				APT_FREE(ret);
			}
		} else {
			_data_ = ret;
			_sizeBytes_ = retSizeBytes;
		}
	}
	return true;
}
