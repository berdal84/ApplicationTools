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
/// buffer overflows it is replaced with a heap-allocated buffer. String may 
/// reference a literal or an external buffer (see setRef()) in which case it is 
/// non-owned, but performing any mutating operations on the string data will
/// cause the referenced buffer to be copied.
/// All `const char*` interfaces expect null-terminated strings.
/// \todo Implement setRef().
/// \todo Do we ever want to 'revert' storage back to the local buffer? Seems 
///   unlikely that this will ever happen, in which case m_localBufferSize can
///   be merged with m_capacity.
/// \todo Revisit copy/move ctors. Should be able to copy and move between 
///   strings of different capacity.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class StringBase
{
public:
	/// Copy _count characters from _src. If _count == 0, all of _src is
	/// copied.
	/// \return New length of the string, excluding the null terminator.
	uint set(const char* _src, uint _count = 0u);
	/// Set formatted content.
	/// \return New length of the string, excluding the null terminator.
	uint setf(const char* _fmt, ...);
	uint setfv(const char* _fmt, va_list _args);

	/// Append _count characters from _src. If _count == 0, all of _src is
	/// appended.
	/// \return New length of the string, excluding the null terminator.
	uint append(const char* _src, uint _count = 0u);
	/// Append formatted content.
	/// \return New length of the string, excluding the null terminator.
	uint appendf(const char* _fmt, ...);
	uint appendfv(const char* _fmt, va_list _args);

	/// String length, excluding the terminating null.
	/// \note String length is not stored internally, hence getLength() is *not*
	///    a constant time operation.
	uint getLength() const;

	void clear();

	bool isOwned() const            { return m_isOwned != 0u; }
	bool isLocal() const            { return m_buf == getLocalBuf() && m_localBufSize != 0u; }

	uint getCapacity() const        { return m_capacity; }


	bool operator==(const char* _rhs) const;
	operator const char*() const    { return m_buf; }
	operator const char*()          { return m_buf; }
	char operator[](int _i) const   { return m_buf[_i]; }
	char& operator[](int _i)        { return m_buf[_i]; }

protected:
	
	StringBase();
	StringBase(uint _localBufferSize);
	StringBase(StringBase&& _rhs);

	~StringBase();

	char* getLocalBuf()             { return (char*)this + sizeof(StringBase); }
	const char* getLocalBuf() const { return (char*)this + sizeof(StringBase); }

private:
	char* m_buf;               //< Ptr to local buffer, or heap allocated.
	uint  m_capacity     : 21; //< Max 2mb.
	uint  m_localBufSize : 10; //< Max 1023 bytes.
	uint  m_isOwned      : 1;  //< If m_buf is owned by this instance.

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
	String(): StringBase(kCapacity) {}
	String(const char* _rhs): StringBase(kCapacity) { set(_rhs); }
	String(const String<kCapacity>& _rhs): StringBase(kCapacity) { set(_rhs); }
	String(String<kCapacity>&& _rhs): StringBase(std::move(_rhs)) {}
};

} // namespace plr

#endif // plr_String_h
