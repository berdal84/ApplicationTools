////////////////////////////////////////////////////////////////////////////////
// Modified from https://github.com/ocornut/str
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy,
// distribute, and modify this file as you see fit.
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

StringBase::StringBase(const char* _rhs)
	: m_buf(g_emptyString)
	, m_capacity(0u)
	, m_localBufSize(0u)
	, m_isOwned(0u)
{
	set(_rhs);
}

StringBase::~StringBase()
{
	if (m_isOwned && !isLocal()) {
		delete[] m_buf;
	}
}

void StringBase::set(const char* _src)
{
	uint ln = strlen(_src) + 1u;
	if (m_capacity < ln) {
		alloc(ln);
	}
	memcpy(m_buf, _src, ln);
	m_isOwned = 1u;
}

int StringBase::setf(const char* _fmt, ...)
{
	va_list args;
	va_start(args, _fmt);
	int ret = setfv(_fmt, args);
	va_end(args);
	return ret;
}

int StringBase::setfv(const char* _fmt, va_list _args)
{
	va_list valist;
	va_copy(valist, _args);

	int ln = vsnprintf(m_isOwned ? m_buf : 0, m_isOwned ? m_capacity : 0u, _fmt, valist) + 1u; // +1 for null terminator
	if (m_capacity < ln) {
		alloc(ln);
		ln = vsnprintf(m_buf, m_capacity, _fmt, valist);
	}

	m_isOwned = 1;
	return ln;
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

	if (_capacity <= m_capacity) {
		return;
	}

	if (m_isOwned && !isLocal()) {
		free(m_buf);
	}

	if (_capacity < m_localBufSize) {
		m_buf = getLocalBuf();
		m_capacity = m_localBufSize;

	} else {
		m_buf = (char*)malloc(_capacity * sizeof(char));
		m_capacity = _capacity;
	}
}

void StringBase::realloc(uint _capacity)
{
	PLR_ASSERT(_capacity <= kMaxCapacity);

	if (_capacity <= m_capacity) {
		return;
	}

	if (_capacity < m_localBufSize) {
		m_buf = getLocalBuf();
		m_capacity = m_localBufSize;

	} else {
		m_buf = (char*)::realloc(m_buf, _capacity * sizeof(char));
		m_capacity = _capacity;
	}
}