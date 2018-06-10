#pragma once

#include <apt/apt.h>
#include <apt/compress.h>
#include <apt/math.h>
#include <apt/types.h>
#include <apt/String.h>

namespace apt {

////////////////////////////////////////////////////////////////////////////////
// Serializer
// Base class for serialization handlers. 
////////////////////////////////////////////////////////////////////////////////
class Serializer
{
public:
	enum Mode
	{
		Mode_Read,
		Mode_Write
	};

	Mode         getMode() const                       { return m_mode;  }
	void         setMode(Mode _mode)                   { m_mode = _mode; }
	const char*  getError() const                      { return m_errStr.isEmpty() ? nullptr : (const char*)m_errStr; }
	void         setError(const char* _msg, ...);

	// Return false if _name is not found, or if the end of the current array is reached.
	virtual bool beginObject(const char* _name = nullptr) = 0;
	virtual void endObject() = 0;

	virtual bool beginArray(uint& _length_, const char* _name = nullptr) = 0;
	virtual void endArray() = 0;

	// Variant for fixed-sized arrays or cases where _length_ isn't required.
	bool         beginArray(const char* _name = nullptr)
	{ 
		uint n = 0;
		return beginArray(n, _name);
	}

	virtual bool value(bool&       _value_, const char* _name = nullptr) = 0;
	virtual bool value(sint8&      _value_, const char* _name = nullptr) = 0;
	virtual bool value(uint8&      _value_, const char* _name = nullptr) = 0;
	virtual bool value(sint16&     _value_, const char* _name = nullptr) = 0;
	virtual bool value(uint16&     _value_, const char* _name = nullptr) = 0;
	virtual bool value(sint32&     _value_, const char* _name = nullptr) = 0;
	virtual bool value(uint32&     _value_, const char* _name = nullptr) = 0;
	virtual bool value(sint64&     _value_, const char* _name = nullptr) = 0;
	virtual bool value(uint64&     _value_, const char* _name = nullptr) = 0;
	virtual bool value(float32&    _value_, const char* _name = nullptr) = 0;
	virtual bool value(float64&    _value_, const char* _name = nullptr) = 0;
	virtual bool value(StringBase& _value_, const char* _name = nullptr) = 0;
	
	// vec* and mat* variants are implemented in terms of beginArray/endArray and value(float&).
	bool         value(vec2& _value_, const char* _name = nullptr);
	bool         value(vec3& _value_, const char* _name = nullptr);
	bool         value(vec4& _value_, const char* _name = nullptr);
	bool         value(mat2& _value_, const char* _name = nullptr);
	bool         value(mat3& _value_, const char* _name = nullptr);
	bool         value(mat4& _value_, const char* _name = nullptr);

	// Helper to avoid explicit cast to StringBase&.
	template <uint kCapacity>
	bool         value(String<kCapacity>& _value_, const char* _name = nullptr)
	{
		return value((StringBase&)_value_, _name); 
	}

	// Directly serialize bytes of binary data with optional compression.
	// \note When reading, _data_ will be allocated by the function if null, and should subsequently be released via free().
	virtual bool binary(
		void*&           _data_, 
		uint&            _sizeBytes_, 
		const char*      _name = nullptr, 
		CompressionFlags _compressionFlags = CompressionFlags_None
		) = 0;

	// Return tType as a string.
	template <typename tType>
	static const char* ValueTypeToStr();

protected:
	Mode m_mode;

	Serializer(Mode _mode)
		: m_mode(_mode)
	{
	}
	virtual ~Serializer()
	{
	}

private:
	String<64> m_errStr;

}; // class Serializer

// Serialize* variants implicitly log an error if _serializer_.value() returns false.
bool Serialize(Serializer& _serializer_, bool&        _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, sint8&       _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, uint8&       _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, sint16&      _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, uint16&      _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, sint32&      _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, uint32&      _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, sint64&      _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, uint64&      _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, float32&     _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, float64&     _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, StringBase&  _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, vec2&        _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, vec3&        _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, vec4&        _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, mat2&        _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, mat3&        _value_, const char* _name = nullptr);
bool Serialize(Serializer& _serializer_, mat4&        _value_, const char* _name = nullptr);
template <uint kCapacity>
bool Serialize(Serializer& _serializer_, String<kCapacity>& _value_, const char* _name = nullptr)
{
	return Serialize(_serializer_, (StringBase&)_value_, _name); 
}

// Helper for the common case of serializing an enum to/from a const char*.
template <typename tType, int kCount>
bool SerializeEnum(Serializer& _serializer_, tType& _value_, const char* (&_strList)[kCount], const char* _name = nullptr)
{
	String<32> tmp;
	if (_serializer_.getMode() == Serializer::Mode_Write) {
		APT_ASSERT((int)_value_ < kCount);
		tmp = _strList[(int)_value_];
	}
	bool ret = Serialize(_serializer_, (StringBase&)tmp, _name);
	if (ret && _serializer_.getMode() == Serializer::Mode_Read) {
		int i = 0;
		for (; i < kCount; ++i) {
			if (tmp == _strList[i]) {
				_value_ = (tType)i;
				break;
			}
		}
		if (i == kCount) {
			_serializer_.setError("Error serializing enum; '%s' not valid", (const char*)tmp);
			ret = false;
		}
	}
	return ret;
}

} // namespace apt
