////////////////////////////////////////////////////////////////////////////////
// Modified from https://github.com/ocornut/str
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_String_h
#define plr_String_h

#include <plr/def.h>

#include <cstdarg> // va_list
#include <utility> // std::move

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class StringBase
/// Base for string class with an optional local buffer. If/when the local 
/// buffer overflows it is replaced with a heap-allocated buffer. Once the 
/// buffer is heap-allocated it never returns to using the local buffer.
/// All `const char*` interfaces expect null-terminated strings.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class StringBase
{
public:
	/// Copy _count characters from _src. If _count == 0, all of _src is copied
	/// If the end of _src is found  before _count characters have been copied,
	/// the result is padded with zeros until _count characters have been written
	/// (like strncpy). Unlike strncpy, an implicit null char is appended to the
	/// end of the result.
	/// \return New length of the string, excluding the null terminator.
	uint set(const char* _src, uint _count = 0u);
	/// Set formatted content.
	/// \return New length of the string, excluding the null terminator.
	uint setf(const char* _fmt, ...);
	uint setfv(const char* _fmt, va_list _args);

	/// Append _count characters from _src. If _count == 0, all of _src is
	/// appended. If the end of _src is found  before _count characters have been 
	/// appended, the result is padded with zeros until _count characters have been 
	/// written (like strncpy). Unlike strncpy, an implicit null char is appended 
	/// to the end of the result.
	/// \return New length of the string, excluding the null terminator.
	uint append(const char* _src, uint _count = 0u);
	/// Append formatted content.
	/// \return New length of the string, excluding the null terminator.
	uint appendf(const char* _fmt, ...);
	uint appendfv(const char* _fmt, va_list _args);

	/// String length, excluding the terminating null.
	/// \note String length is not stored internally, hence getLength() is **not**
	///    a constant time operation.
	uint getLength() const;

	void clear()                    { m_buf[0] = '\0'; }

	bool isEmpty() const            { return m_buf[0] == '\0'; }
	bool isLocal() const            { return m_buf == getLocalBuf(); }
	uint getCapacity() const        { return m_capacity; }


	bool operator==(const char* _rhs) const;
	operator const char*() const    { return m_buf; }
	operator const char*()          { return m_buf; }
	char operator[](int _i) const   { return m_buf[_i]; }
	char& operator[](int _i)        { return m_buf[_i]; }

protected:
	
	/// String always heap-allocated.
	StringBase();
	/// String has a local buffer of _localBufferSize chars.
	StringBase(uint _localBufferSize);
	/// Move ctor. If _rhs is local it *must* have the same capacity as this 
	/// (because we don't store the local buffer size). 
	StringBase(StringBase&& _rhs);

	~StringBase();

	char* getLocalBuf()             { return (char*)this + sizeof(StringBase); }
	const char* getLocalBuf() const { return (char*)this + sizeof(StringBase); }

private:
	char* m_buf;       //< Ptr to local buffer, or heap allocated.
	uint  m_capacity;  //< Local buffer/allocated size.

	/// Resize m_buf to _capacity, discard contents.
	void alloc(uint _capacity);

	/// Resize m_buf to _capaicty, maintain contents.
	void realloc(uint _capacity);

}; // class String


template <uint kCapacity>
class String: public StringBase
{
	char m_localBuf[kCapacity];

public:
	String():                              StringBase(kCapacity)       {}
	String(const String<kCapacity>& _rhs): StringBase(kCapacity)       { set(_rhs); }
	String<kCapacity>& operator=(const String<kCapacity>& _rhs)        { set(_rhs); return *this; }
	String(String<kCapacity>&& _rhs):      StringBase(std::move(_rhs)) {}
	String(const char* _fmt, ...):         StringBase(kCapacity)
	{
		va_list args;
		va_start(args, _fmt);
		setfv(_fmt, args);
		va_end(args);
	}
};

template <>
class String<0>: public StringBase
{
public:
	String():                      StringBase()                {}
	String(const String<0>& _rhs): StringBase()                { set(_rhs); }
	String<0>& operator=(const String<0>& _rhs)                { set(_rhs); return *this; }
	String(String<0>&& _rhs):      StringBase(std::move(_rhs)) {}
	String(const char* _fmt, ...): StringBase()
	{
		va_list args;
		va_start(args, _fmt);
		setfv(_fmt, args);
		va_end(args);
	}
};

} // namespace plr

#endif // plr_String_h
