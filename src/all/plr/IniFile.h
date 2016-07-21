////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_IniFile_h
#define plr_IniFile_h

#include <plr/def.h>
#include <plr/StringHash.h>

#include <vector>

namespace plr {

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
/// \todo String escape characters
/// \todo Line breaks within strings
/// \todo Implement save() + api for constructing an IniFile.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class IniFile
{
public:

	enum class ValueType
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

		uint   m_type;
		uint   m_count;
		Value* m_first;

		Property(ValueType _type, uint16 _count, Value* _first)
			: m_type((uint8)_type), m_count(_count), m_first(_first)
		{
		}

	public:
		bool isNull() const                     { return m_first == 0; }
		ValueType getType() const               { return (ValueType)m_type; }
		uint getCount() const                   { return m_count; }
		bool asBool(uint i = 0u) const          { PLR_ASSERT(i < m_count); return m_first[i].m_bool; }
		sint64 asInt(uint i = 0u) const         { PLR_ASSERT(i < m_count); return m_first[i].m_int; }
		double asDouble(uint i = 0u) const      { PLR_ASSERT(i < m_count); return m_first[i].m_double; }
		const char* asString(uint i = 0u) const { PLR_ASSERT(i < m_count); return m_first[i].m_string; }	
	};
	
	/// Read from a file and parse. Invalidates any existing Property instances
	/// returned by getProperty();
	/// \return false if an error occurred.
	static bool Read(IniFile* iniFile_, const File* _file);
	static bool Read(IniFile* iniFile_, const char* _path);

	IniFile() {}
	~IniFile();
	
	/// Retrieve a named property, optionally specifying the section to search.
	/// Note that the returned Property instance is only valid during the lifetime
	/// of the IniFile to which it belongs. Calling load() will invalidate all
	/// Property instances previously returned by these methods.
	/// \return Property instance. If no matching property was found, isNull() will
	///   return true.
	Property getProperty(const char* _key, const char* _section = 0);
	Property getProperty(StringHash _key, StringHash _section = kDefaultSection);

private:
	struct Key
	{
		StringHash m_key;
		ValueType  m_type;
		uint       m_count;
		uint       m_valueOffset;
	};
	struct Section
	{
		StringHash m_name;
		uint       m_count;
		uint       m_keyOffset;
	};

	std::vector<Section> m_sections;
	std::vector<Key>     m_keys;
	std::vector<Value>   m_values;

	bool parse(const char* _str);

}; // class IniFile

} // namespace plr

#endif // plr_IniFile_h
