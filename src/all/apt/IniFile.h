#pragma once
#ifndef apt_IniFile_h
#define apt_IniFile_h

#include <apt/def.h>
#include <apt/String.h>

#include <vector>

namespace apt {

class File;

////////////////////////////////////////////////////////////////////////////////
/// \class IniFile
/// Read/write INI files; e.g.
/// \code
///  ; comment
///    [section] ; sections are optional
///    int    = 12
///    double = 12.0
///    string = "twelve"  ; strings are enclosed in double quotes
///    array  = 12, 13, 14
///    array2 =  ; all whitespace is ignored, including line breaks
///       "twelve",
///       "thirteen",
///       "fourteen"
/// \endcode
///
/// \todo String escape characters
/// \todo Line breaks within strings
/// \todo Implement save() + api for constructing an IniFile.
/// \ingroup APT_core
////////////////////////////////////////////////////////////////////////////////
class IniFile
{
public:

	enum ValueType
	{
		kBool, kInt, kDouble, kString
	};

	union Value
	{
		bool    m_bool;
		sint64  m_int;
		double  m_double;
		char*   m_string;
	};

	class Property
	{
		friend class IniFile;

		uint8   m_type;
		uint16  m_count;
		Value*  m_first;

		Property(ValueType _type, uint16 _count, Value* _first)
			: m_type((uint8)_type)
			, m_count(_count)
			, m_first(_first)
		{
		}

	public:
		Property()
			: m_type(0)
			, m_count(0)
			, m_first(0)
		{
		}
		
		bool isNull() const                        { return m_first == 0; }
		ValueType getType() const                  { return (ValueType)m_type; }
		uint16 getCount() const                    { return m_count; }
		bool asBool(uint16 _i = 0u) const          { APT_ASSERT((ValueType)m_type == kBool);   APT_ASSERT(_i < m_count); return m_first[_i].m_bool; }
		sint64 asInt(uint16 _i = 0u) const         { APT_ASSERT((ValueType)m_type == kInt);    APT_ASSERT(_i < m_count); return m_first[_i].m_int; }
		double asDouble(uint16 _i = 0u) const      { APT_ASSERT((ValueType)m_type == kDouble); APT_ASSERT(_i < m_count); return m_first[_i].m_double; }
		const char* asString(uint16 _i = 0u) const { APT_ASSERT((ValueType)m_type == kString); APT_ASSERT(_i < m_count); return m_first[_i].m_string; }	
	};
	
	/// Read from a file and parse. Invalidates any existing Property instances
	/// returned by getProperty();
	/// \return false if an error occurred.
	static bool Read(IniFile& iniFile_, const File& _file);
	static bool Read(IniFile& iniFile_, const char* _path);

	/// Write to a file.
	static bool Write(const IniFile& _iniFile, File& file_);
	static bool Write(const IniFile& _iniFile, const char* _path);

	IniFile();
	~IniFile();
	
	/// Retrieve a named property, optionally specifying the section to search.
	/// Note that the returned Property instance is only valid during the lifetime
	/// of the IniFile to which it belongs. Calling load() will invalidate all
	/// previously returned Property instances.
	/// \return Property instance. If no matching property was found, isNull() will
	///   return true.
	Property getProperty(const char* _name, const char* _section = 0) const;

	void pushSection(const char* _name);
	template <typename tType>
	void pushValueArray(const char* _name, const tType _value[], uint16 _count);
	void pushValueArray(const char* _name, const char* _value[], uint16 _count);
	template <typename tType>
	void pushValue(const char* _name, const tType& _value) { pushValueArray<tType>(_name, &_value, 1); }
	void pushValue(const char* _name, const char* _value)  { pushValueArray(_name, &_value, 1); }

private:
	typedef String<32> NameStr;

	struct Key
	{
		NameStr    m_name;
		ValueType  m_type;
		uint16     m_valueCount;
		uint16     m_valueOffset;
	};
	struct Section
	{
		NameStr    m_name;
		uint16     m_propertyCount;
		uint16     m_keyOffset;      
	};

	std::vector<Section> m_sections;
	std::vector<Key>     m_keys;
	std::vector<Value>   m_values;

	bool parse(const char* _str);

	const Section* findSection(const char* _name) const;
	const Key* findKey(const char* _name, const Section* _section) const;
	
}; // class IniFile

} // namespace apt

#endif // apt_IniFile_h
