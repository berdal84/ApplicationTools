////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_RingBuffer_h
#define plr_RingBuffer_h

#include <plr/def.h>
#include <plr/memory.h>

#include <cstring>

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class RingBuffer
/// New items are added to the back of the buffer via push_back(), overwriting
/// items at the front if size() == capacity().
/// Access via operator[] returns items between front() and back() in order. Use
/// data() to access the underying buffer.
/// \todo pop_back may be useful, but how to destruct the object?
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class RingBuffer
{
public:

	RingBuffer(uint _capacity = 2)
		: m_buffer(0)
		, m_front(0)
		, m_back(0)
		, m_capacity(0)
	{
		reserve(_capacity);
	}

	~RingBuffer()
	{
		free_aligned(m_buffer);
	}

	void reserve(uint _capacity)
	{
		tType* newBuffer = (tType*)realloc_aligned(m_buffer, _capacity * sizeof(tType), PLR_ALIGNOF(tType));

		tType* end = m_buffer + _capacity;
		if (m_front >= end) {
			m_front = end - 1;
		} else {
			m_front = newBuffer + (m_front - m_buffer);
		}
		if (m_back >= end) {
			m_back = m_buffer;
		} else {
			m_back = newBuffer + (m_back - m_buffer);
		}

		m_capacity = _capacity;
		m_buffer = newBuffer;
	}

	void push_back(const tType& _v)
	{
		*m_back = _v;
		incBack();
	}

	tType*       front()                   { return m_front; }
	tType*       back()                    { return m_back; }

	bool         empty() const             { return m_front == m_back; }
	uint         size() const              { return (m_back >= m_front) ? (m_back - m_front) : (m_back - m_buffer + (m_buffer + m_capacity) - m_front); }
	uint         capacity() const          { return m_capacity; }

	/// Access the storage buffer directly.
	tType*       data()                    { return m_buffer; }
	const tType* data() const              { return m_buffer; }

	/// Access elements between front() and back().
	tType&       operator[](uint _i)       { return get(_i); }
	const tType& operator[](uint _i) const { return get(_i); } 

private:
	tType* m_buffer;   //< Storage.
	tType* m_front;    //< Oldest valid element in the buffer.
	tType* m_back;     //< Next free element in the buffer.
	uint   m_capacity;


	void incBack()
	{
		if_unlikely (++m_back >= (m_buffer + m_capacity)) {
			m_back = m_buffer;
		}
		if_likely (m_back == m_front) {
			if_unlikely (++m_front >= (m_buffer + m_capacity)) {
				m_front = m_buffer;
			}
		}
	}

	tType& get(uint _i)
	{
		tType* ret = m_front + _i;
		tType* end = m_buffer + m_capacity;
		if (ret >= end) {
			_i -= end - m_front;
			ret = m_buffer + _i;
		}
		PLR_ASSERT(ret < end);
		return *ret;
	}

}; // class RingBuffer

} // namespace plr

#endif // plr_RingBuffer_h
