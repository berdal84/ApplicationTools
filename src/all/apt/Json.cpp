#include <apt/Json.h>

#include <apt/log.h>
#include <apt/math.h>
#include <apt/FileSystem.h>
#include <apt/String.h>

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
		case rapidjson::kNullType:   return Json::kNull;
		case rapidjson::kObjectType: return Json::kObject;
		case rapidjson::kArrayType:  return Json::kArray;
		case rapidjson::kFalseType:
		case rapidjson::kTrueType:   return Json::kBool;
		case rapidjson::kNumberType: return Json::kNumber;
		case rapidjson::kStringType: return Json::kString;
		default: APT_ASSERT(false); break;
	};

	return Json::kValueTypeCount;
}

/*******************************************************************************

                                   Json

*******************************************************************************/

struct Json::Impl
{
	static const int kMaxStackDepth = 8;

	rapidjson::Document m_dom;

 // current value set after find()
	rapidjson::Value* m_value;

 // value stack for objects/arrays
	rapidjson::Value* m_stack[kMaxStackDepth];
	int m_iter[kMaxStackDepth];
	int m_stackTop;

	void push(rapidjson::Value* _val = 0)
	{
		APT_ASSERT(m_stackTop < kMaxStackDepth);
		APT_ASSERT(top() != _val); // probably a mistake, called push() twice?
		m_stack[++m_stackTop] = _val ? _val : m_value;
		m_iter[m_stackTop] = 0;
	}
	void pop()
	{
		--m_stackTop; 
	}
	rapidjson::Value* top() 
	{
		return m_stack[m_stackTop]; 
	}
	int& topIter()
	{
		return m_iter[m_stackTop];
	}
	

	Impl(): m_stackTop(-1) {}
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
	File f;
	if (Write(_json, f)) {
		return FileSystem::Write(f, _path, _rootHint);
	}
	return false;
}

Json::Json(const char* _path, FileSystem::RootType _rootHint)
	: m_impl(0)
{
	m_impl = new Impl;
	m_impl->m_dom.SetObject();
	m_impl->push(&m_impl->m_dom);

	if (_path) {
		Json::Read(*this, _path, _rootHint);
	}
}

Json::~Json()
{
	if (m_impl) {
		delete m_impl;
	}
}

bool Json::find(const char* _name)
{
	if (!m_impl->top()->IsObject()) {
		return false;
	}
	auto it = m_impl->top()->FindMember(_name);
	if (it != m_impl->top()->MemberEnd()) {
		m_impl->m_value = &it->value;
		return true;
	}
	return false;
}

bool Json::next()
{
	if (GetValueType(m_impl->top()->GetType()) == kArray) {
		auto it = m_impl->top()->Begin() + (m_impl->topIter()++);
		m_impl->m_value = it;
		return it != m_impl->top()->End();
	}

	if (GetValueType(m_impl->top()->GetType()) == kObject) {
		auto it = m_impl->top()->MemberBegin() + (m_impl->topIter()++);
		m_impl->m_value = &it->value;
		return it != m_impl->top()->MemberEnd();
	}

	APT_ASSERT(false); // not an object or an array
	return false;
}

Json::ValueType Json::getType() const
{
	return GetValueType(m_impl->m_value->GetType()); 
}

template <> bool Json::getValue<bool>() const
{
	APT_ASSERT_MSG(getType() == kBool, "Json::getValue: value was not a boolean");
	return m_impl->m_value->GetBool();
}
template <> int Json::getValue<int>() const
{
	APT_ASSERT_MSG(getType() == kNumber, "Json::getValue: value was not a number");
	return m_impl->m_value->GetInt();
}
template <> unsigned int Json::getValue<unsigned int>() const
{
	APT_ASSERT_MSG(getType() == kNumber, "Json::getValue: value was not a number");
	return m_impl->m_value->GetUint();
}
template <> uint64 Json::getValue<uint64>() const
{
	APT_ASSERT_MSG(getType() == kNumber, "Json::getValue: value was not a number");
	return m_impl->m_value->GetUint64();
}
template <> float Json::getValue<float>() const
{
	APT_ASSERT_MSG(getType() == kNumber, "Json::getValue: value was not a number");
	return m_impl->m_value->GetFloat();
}
template <> double Json::getValue<double>() const
{
	APT_ASSERT_MSG(getType() == kNumber, "Json::getValue: value was not a number");
	return m_impl->m_value->GetDouble();
}
template <> const char* Json::getValue<const char*>() const
{
	APT_ASSERT_MSG(getType() == kString, "Json::getValue: value was not a string");
	return m_impl->m_value->GetString();
}
template <> vec2 Json::getValue<vec2>() const
{
	vec2 ret;
	Json* json = const_cast<Json*>(this);
	if (json->enterArray()) {
		APT_ASSERT_MSG(m_impl->top()->Size() == 2, "Json::getValue: invalid vec2, size = %d", m_impl->top()->Size());
		int i = 0;
		while (json->next()) {
			ret[i++] = getValue<float>();
		}
		json->leaveArray();
	}
	return ret;
}
template <> vec3 Json::getValue<vec3>() const
{
	vec3 ret;
	Json* json = const_cast<Json*>(this);
	if (json->enterArray()) {
		APT_ASSERT_MSG(m_impl->top()->Size() == 3, "Json::getValue: invalid vec3, size = %d", m_impl->top()->Size());
		int i = 0;
		while (json->next()) {
			ret[i++] = getValue<float>();
		}
		json->leaveArray();
	}
	return ret;
}
template <> vec4 Json::getValue<vec4>() const
{
	vec4 ret;
	Json* json = const_cast<Json*>(this);
	if (json->enterArray()) {
		APT_ASSERT_MSG(m_impl->top()->Size() == 4, "Json::getValue: invalid vec4, size = %d", m_impl->top()->Size());
		int i = 0;
		while (json->next()) {
			ret[i++] = getValue<float>();
		}
		json->leaveArray();
	}
	return ret;
}


bool Json::enterObject()
{
	if (getType() == kObject) {
		m_impl->push();
		return true;
	}
	APT_ASSERT(false); // not an object
	return false;
}

void Json::leaveObject()
{
	APT_ASSERT(GetValueType(m_impl->top()->GetType()) == kObject);
	m_impl->m_value = m_impl->top();
	m_impl->pop();
}

bool Json::enterArray()
{
	if (getType() == kArray) {
		m_impl->push();
		return true;
	}
	APT_ASSERT(false); // not an array
	return false;
}

void Json::leaveArray()
{
	APT_ASSERT(GetValueType(m_impl->top()->GetType()) == kArray);
	m_impl->m_value = m_impl->top();
	m_impl->pop();
}

int Json::getArrayLength() const
{
	if (GetValueType(m_impl->top()->GetType()) == kArray) {
		return (int)m_impl->top()->GetArray().Size();
	}
	return -1;
}

void Json::beginObject(const char* _name)
{
	if (_name && find(_name)) {
	  // object already existed, check the type
		APT_ASSERT(GetValueType(m_impl->m_value->GetType()) == kObject);
		return;
	} else {
		if (GetValueType(m_impl->top()->GetType()) == kArray) {
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
		APT_ASSERT(GetValueType(m_impl->m_value->GetType()) == kArray);
		return;
	} else {
		if (GetValueType(m_impl->top()->GetType()) == kArray) {
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

template <> void Json::setValue<int>(const char* _name, int _val)
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

template <> void Json::setValue<unsigned int>(const char* _name, unsigned int _val)
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

template <> void Json::setValue<float>(const char* _name, float _val)
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

template <> void Json::setValue<double>(const char* _name, double _val)
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

template <> void Json::pushValue<bool>(bool _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}

template <> void Json::pushValue<int>(int _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}

template <> void Json::pushValue<unsigned int>(unsigned int _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}

template <> void Json::pushValue<float>(float _val)
{
	m_impl->top()->PushBack(
		rapidjson::Value(_val).Move(), 
		m_impl->m_dom.GetAllocator()
		);
	m_impl->m_value = m_impl->top()->End() - 1;
}

template <> void Json::pushValue<double>(double _val)
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


/*******************************************************************************

                              JsonSerializer

*******************************************************************************/

JsonSerializer::JsonSerializer(Json* _json_, Mode _mode)
	: m_json(_json_)
	, m_mode(_mode)
{
}

bool JsonSerializer::beginObject(const char* _name)
{
	if (m_mode == kRead) {
		if (insideArray()) {
			if (!m_json->next()) {
				return false;
			}
		} else {
			APT_ASSERT(_name);
			if (!m_json->find(_name)) {
				return false;
			}
		}
		if (m_json->getType() == Json::kObject) { // \todo assert if it's not an object?
			m_json->enterObject();
			return true;
		}
	} else {
		m_json->beginObject(_name);
		return true;
	}
	return false;
}
void JsonSerializer::endObject()
{
	if (m_mode == kRead) {
		m_json->leaveObject();
	} else {
		m_json->endObject();
	}
}

bool JsonSerializer::beginArray(const char* _name)
{
	if (m_mode == kRead) {
		if (insideArray()) {
			if (!m_json->next()) {
				return false;
			}
		} else {
			APT_ASSERT(_name);
			if (!m_json->find(_name)) {
				return false;
			}
			if (m_json->getType() == Json::kArray) { // \todo assert if it's not an array?
				m_json->enterArray();
				return true;
			}
		}
	} else {
		m_json->beginArray(_name);
		return true;
	}
	return false;
}
void JsonSerializer::endArray()
{
	if (m_mode == kRead) {
		m_json->leaveArray();
	} else {
		m_json->endArray();
	}
}

#define DEFINE_value(_type) \
	template <> bool JsonSerializer::value<_type>(const char* _name, _type& _value_) { \
		APT_ASSERT_MSG(!insideArray(), "JsonSerializer::value: _name variant called inside an array"); \
		if (m_mode == kRead) { \
			if (m_json->find(_name)) { \
				_value_ = m_json->getValue<_type>(); \
				return true; \
			} \
		} else { \
			m_json->setValue<_type>(_name, _value_); \
			return true; \
		} \
		return false; \
	} \
	template <> bool JsonSerializer::value<_type>(_type& _value_) { \
		APT_ASSERT_MSG(insideArray(), "JsonSerializer::value: array variant called outside an array"); \
		if (m_mode == kRead) { \
			if (!m_json->next()) { \
				return false; \
			} \
			_value_ = m_json->getValue<_type>(); \
		} else { \
			m_json->pushValue<_type>(_value_); \
		} \
		return true; \
	}

DEFINE_value(bool)
DEFINE_value(int)
DEFINE_value(unsigned int)
DEFINE_value(float)
DEFINE_value(double)
DEFINE_value(vec2)
DEFINE_value(vec3)
DEFINE_value(vec4)

#undef DEFINE_value

template <> bool JsonSerializer::value<StringBase>(const char* _name, StringBase& _value_)
{
	APT_ASSERT(!insideArray());
	if (m_mode == kRead) {
		int ln = string(_name, 0);
		_value_.setCapacity(ln + 1);
	}
	return string(_name, _value_) != 0;
}

template <> bool JsonSerializer::value<StringBase>(StringBase& _value_)
{
	APT_ASSERT(insideArray());
	if (m_mode == kRead) {
		int ln = string(0);
		if (ln == 0) {
			return false;
		}
		_value_.setCapacity(ln + 1);
	}
	return string(_value_) != 0;
}

int JsonSerializer::string(const char* _name, char* _string_)
{
	APT_ASSERT(!insideArray()); 
	if (m_mode == kRead) {
		if (m_json->find(_name)) {
			if (_string_) { 
			 // valid ptr, copy string to buffer
				const char* str = m_json->getValue<const char*>();
				strcpy(_string_, str);
				return (int)strlen(str);
			} else {
				return (int)strlen(m_json->getValue<const char*>());
			}
		}
	} else {
		APT_ASSERT(_string_);
		m_json->setValue(_name, (const char*)_string_);
		return (int)strlen(_string_);
	}
	return 0;
}

int JsonSerializer::string(char* _value_)
{
	APT_ASSERT(insideArray()); 
	if (m_mode == kRead) {
		if (_value_) {
		 // valid ptr, copy string to buffer
			const char* str = m_json->getValue<const char*>();
			strcpy(_value_, str);
			return (int)strlen(str);
		} else {
			if (!m_json->next()) {
				return 0;
			}
			return (int)strlen(m_json->getValue<const char*>());
		}
	} else {
		APT_ASSERT(_value_);
		m_json->pushValue((const char*)_value_);
		return (int)strlen(_value_);
	}
	return 0;
}

// PRIVATE

bool JsonSerializer::insideArray()
{
	rapidjson::Value* top = m_json->m_impl->top();
	if (top) {
		return GetValueType(top->GetType()) == Json::kArray;
	} else {
		return false;
	}
}
