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
/// \todo pop_back may be useful, but how to destruct the object?
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class RingBuffer:
{
public:

	RingBuffer(uint _capacity = 2)
		: m_buffer(0)
		, m_capcity(0)
		, m_head(0)
		, m_next(0)
	{
		setCapacity(_capacity);
	}

	~RingBuffer()
	{
		free_aligned(m_buffer);
	}

	void setCapacity(uint _capacity)
	{
		tType* buf = malloc_aligned(_capacity, PLR_ALIGNOF(tType));
		if (m_buffer) {
			memcpy(buf, m_buffer, PLR_MIN(_capacity, m_capacity) * sizeof(tType));
		}
		free_aligned(m_buffer);
		m_buffer = buf;
		m_capcity = _capacity;
	}

	void push_back(const tType& _v)
	{
		m_buffer[m_back] = _v;
		incBack();
	}

	uint getCapacity() const { return m_capacity; }

private:
	tType* m_buffer;
	uint   m_capcity;
	uint   m_front;    //< Oldest valid element in the buffer.
	uint   m_back;     //< Next free element in the buffer.


	void incBack()
	{
		if_unlikely (++m_back >= m_capacity) {
			m_back = 0;
		}
		if_likely (m_back == m_front) {
			if_unlikely (++m_front >= m_capacity) {
				m_front = 0;
			}
		}
	}

}; // class RingBuffer

} // namespace plr

#endif // plr_RingBuffer_h
