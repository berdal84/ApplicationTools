////////////////////////////////////////////////////////////////////////////////
// Modified from https://github.com/ocornut/str
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/String.h>

#include <cstdarg> // va_list, va_start, va_end
#include <cstdio>  // vsnprintf
#include <cstdlib>
#include <cstring>

#ifndef va_copy
	#define va_copy(_dst, _src) (_dst = _src)
#endif

using namespace plr;

static uint kMaxCapacity        = 2097151; // 2^21 - 1
static uint kMaxLocalBufferSize = 1023; // 2^10 - 1
static char* g_emptyString      = (char*)"\0NULL";

// PUBLIC

StringBase::StringBase()
	: m_buf(g_emptyString)
	, m_capacity(0u)
	, m_localBufSize(0u)
	, m_isOwned(0u)
{
}

StringBase::~StringBase()
{
	if (m_isOwned && !isLocal()) {
		delete[] m_buf;
	}
}

uint StringBase::set(const char* _src)
{
	uint len = strlen(_src) + 1u;
	if (m_capacity < len) {
		alloc(len);
	}
	memcpy(m_buf, _src, len);
	return len - 1u;
}

uint StringBase::setf(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	uint ret = setfv(_fmt, args);
	va_end(args);
	return ret;
}

uint StringBase::setfv(const char* _fmt, va_list _args)
{
	va_list args;
	va_copy(args, _args);

#ifdef PLR_COMPILER_MSVC
 // vsnprintf returns -1 on overflow, forces use to always do 2 passes
	int len = vsnprintf(0, 0u, _fmt, args);
	PLR_ASSERT(len >= 0);
	len += 1;
	if (m_capacity < len) {
		alloc(len);
	}
	PLR_VERIFY(vsnprintf(m_buf, m_capacity, _fmt, args) >= 0);
#else
	int len = vsnprintf(m_isOwned ? m_buf : 0, m_capacity, _fmt, args);
	PLR_ASSERT(len >= 0);
	len += 1;
	if (m_capacity < len) {
		alloc(len);
		PLR_VERIFY(vsnprintf(m_buf, m_capacity, _fmt, args) >= 0);
	}
#endif
	
	return (uint)len - 1u;
}

uint StringBase::append(const char* _src)
{
	uint len = getLength();
	uint srclen = strlen(_src) + 1u;
	if (m_capacity < len + srclen) {
		realloc(len + srclen);
	}
	strcpy(m_buf + len, _src);
	return len + srclen - 1u;
}

uint StringBase::appendf(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	uint ret = appendfv(_fmt, args);
	va_end(args);
	return ret;
}

uint StringBase::appendfv(const char* _fmt, va_list _args)
{
	va_list args;
	va_copy(args, _args);

	uint len = getLength();
	int srclen = vsnprintf(0, 0u, _fmt, args);
	PLR_ASSERT(srclen > 0);
	srclen += 1;
	if (m_capacity < len + srclen) {
		realloc(len + srclen);
	}
	PLR_VERIFY(vsnprintf(m_buf + len, (uint)srclen, _fmt, args) >= 0);
	
	return (uint)srclen + len - 1u;;
}

uint StringBase::getLength() const
{
	return strlen(m_buf);
}

void StringBase::clear()
{
	if (m_isOwned && !isLocal()) {
		free(m_buf);
	}
	if (m_localBufSize > 0u) {
		m_buf = getLocalBuf();
		m_buf[0] = '\0';
		m_capacity = m_localBufSize;
		m_isOwned = 1u;

	} else {
		m_buf = g_emptyString;
		m_capacity = 0u;
		m_localBufSize =  0u;
	}
}

bool StringBase::operator==(const char* _rhs) const
{
	return strcmp(_rhs, m_buf) == 0;
}

// PROTECTED

StringBase::StringBase(uint _localBufferSize)
	: m_buf(getLocalBuf())
	, m_capacity(_localBufferSize)
	, m_localBufSize(_localBufferSize)
	, m_isOwned(1u)
{
	PLR_ASSERT(_localBufferSize < kMaxLocalBufferSize);
	m_buf[0] = '\0';
}

// PRIVATE

void StringBase::alloc(uint _capacity)
{
	PLR_ASSERT(_capacity <= kMaxCapacity);

	if (!isLocal() && m_isOwned) {
		free(m_buf);
	}
	m_buf = (char*)malloc(_capacity * sizeof(char));
	m_capacity = _capacity;
	m_isOwned = 1u;
}

void StringBase::realloc(uint _capacity)
{
	PLR_ASSERT(_capacity <= kMaxCapacity);

	if (_capacity < m_localBufSize) {
		if (!isLocal()) {
			strncpy(getLocalBuf(), m_buf, m_localBufSize);
		}
		m_buf = getLocalBuf();
		m_capacity = m_localBufSize;

	} else if (isLocal()) {
		m_buf = (char*)malloc(_capacity * sizeof(char));
		strncpy(m_buf, getLocalBuf(), _capacity);
		m_capacity = _capacity;
	
	} else {
		m_buf = (char*)::realloc(m_buf, _capacity * sizeof(char));
		m_capacity = _capacity;
	}

	m_isOwned = 1u;
}