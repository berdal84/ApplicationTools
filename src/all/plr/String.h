////////////////////////////////////////////////////////////////////////////////
// Modified from https://github.com/ocornut/str
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy,
// distribute, and modify this file as you see fit.
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_String_h
#define plr_String_h

#include <plr/def.h>

#include <cstdarg> // va_list

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class StringBase
/// Base for string class with an optional local buffer. If/when the local 
/// buffer overflows it is replaced with a heap-allocated buffer. String may 
/// reference a literal or an external buffer (see setRef()) in which case it is 
/// non-owned, but performing any mutating operations on the string data will
/// cause the referenced buffer to be copied.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class StringBase
{
public:

	StringBase();
	StringBase(const char* _rhs);

	~StringBase();

	/// Copy string content from _src.
	void set(const char* _src);

	/// Write formatted string content.
	int setf(const char* _fmt, ...);
	int setfv(const char* _fmt, va_list _args);


	/// String length is not stored internally, hence length() is not constant
	/// time.
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
	StringBase(uint _localBufferSize);

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
};

} // namespace plr

#endif // plr_String_h
