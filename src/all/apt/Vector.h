#pragma once
#ifndef apt_Vector_h
#define apt_Vector_h

#include <apt/def.h>
#include <apt/memory.h>

#include <new>     // placement new
#include <utility> // std::move

namespace apt {

////////////////////////////////////////////////////////////////////////////////
/// \class VectorBase
/// Base for container with an optional local buffer. If/when the local buffer 
/// overflows it is replaced with a heap-allocated buffer. Once the buffer is 
/// heap-allocated it never returns to using the local buffer (the ptr is not
/// stored).
/// \todo Avoid storing m_localBuffer? It should always be 
///    this + sizeof(VectorBase) rounded up to the nearest alignof(tType)
////////////////////////////////////////////////////////////////////////////////
template <typename tType>
class VectorBase
{
public:

	typedef tType value_type;
	typedef uint  size_type;
	
	typedef tType*        iterator;
	typedef const tType*  const_iterator;

	/// Add a new element at the end of the container. v is copied (or moved) to the new element.
	void push_back(const tType& _v);
	void push_back(tType&& _v_);

	/// Remove the last element from the end of the container. The element's dtor is called.
	void pop_back();

	/// Reserve enough space in the container to hold at least n elements.
	void reserve(uint _n);

	/// Remove all elements from the container. Element dtors are called.
	void clear();
	
	/// Resize the container so that it contains n elements. If n is smaller
	/// than the current size, elements are removed from the container (and
	/// destructed). If n is greater than the current size, additional
	/// elements are added and default constructed.
	void resize(uint _n);

	/// Resize the container so that it contains n elements. If n is smaller
	/// than the current size, elements are removed from the container (and
	/// destructed). If n is greater than the current size, additional 
	/// elements are added and copy constructed from v.
	void resize(uint _n, const tType& _v);

	uint           size() const              { return m_size; }
	uint           capacity() const          { return m_capacity; }
	
	bool           empty() const             { return m_size == 0; }
	bool           full() const              { return m_size == capacity(); }
	
	tType&         front()                   { return (*this)[0]; }
	const tType&   front() const             { return (*this)[0]; }
	
	tType&         back()                    { return (*this)[size() - 1]; }
	const tType&   back() const              { return (*this)[size() - 1]; }

	iterator       begin()                   { return m_buf; }
	const_iterator begin() const             { return m_buf; }
	iterator       end()                     { return m_buf + m_size * sizeof(tType); }
	const_iterator end() const               { return m_buf + m_size * sizeof(tType); }

	tType&         operator[](uint _i)       { APT_ASSERT(_i < m_size) return m_buf[_i]; }
	const tType&   operator[](uint _i) const { APT_ASSERT(_i < m_size) return m_buf[_i]; }

protected:

	/// Always heap-allocated.
	VectorBase();
	/// Vector has a local buffer tType[_localDataSize].
	VectorBase(tType* _localData, uint _localDataSize);

	~VectorBase();

private:
	tType* m_data;
	tType* m_localData;
	uint   m_size;
	uint   m_capacity;

	bool isLocal() const { return m_data != 0 && m_data == m_localData; }
	
}; // class VectorBase


template <typename tType, uint kInitialCapacity>
class Vector: public VectorBase<tType>
{
	storage<tType, kInitialCapacity> m_data; // storage is aligned to tType
public:
	Vector(): VectorBase<tType>(&m_data, kInitialCapacity) {}
};



/*******************************************************************************

                                 Vector

*******************************************************************************/

// PUBLIC

template <typename tType>
inline void VectorBase<tType>::push_back(const tType& _v)
{
	uint n = m_size + 1;
	if_unlikely (m_capacity < n) {
	 // copy _v before realloc in case _v is inside the m_data
		tType* tmp = (tType*)malloc_aligned(sizeof(tType) * n);
		memcpy(tmp, m_data, sizeof(tType) * m_size);
		new(&tmp[m_size]) tType(_v);
		if (!isLocal()) {
			free_aligned(m_data);
		}
		m_data = tmp;
		m_capacity = m_size = n;
	} else {
		new(&m_data[m_size]) tType(_v);
		++m_size
	}
}

template <typename tType>
inline void VectorBase<tType>::push_back(tType&& _v_)
{
	uint n = m_size + 1;
	if_unlikely (m_capacity < n) {
	 // move _v before realloc in case _v is inside the m_data
		tType* tmp = (tType*)malloc_aligned(sizeof(tType) * n);
		memcpy(tmp, m_data, sizeof(tType) * m_size);
		new(&tmp[m_size]) tType(std::move(_v_));
		if (!isLocal()) {
			free_aligned(m_data);
		}
		m_data = tmp;
		m_capacity = m_size = n;
	} else {
		new(&m_data[m_size]) tType(std::move(_v_));
		++m_size
	}
}

template <typename tType>
inline void VectorBase<tType>::pop_back()
{
	APT_ASSERT(!empty());
	back().~tType();
	--m_size;
}

template <typename tType>
inline void VectorBase<tType>::reserve(uint _n)
{
	if_unlikely (m_capacity < _n) {
		if (isLocal()) {
			m_data = malloc_aligned(sizeof(tType) * _n, APT_ALIGNOF(tType));
			memcpy(m_data, m_localData, sizeof(tType) * m_size);
		} else {
			m_data = realloc_aligned(m_data, sizeof(tType) * _n, APT_ALIGNOF(tType);
		}
		m_capacity = _n;
	}
}

template <typename tType>
inline void VectorBase<tType>::clear()
{
	for (auto it = begin(); it != end(); ++it) {
		it->~tType();
	}
	m_size = 0;
}
	
template <typename tType>
inline void VectorBase<tType>::resize(uint _n)
{
	reserve(_n);
	for (uint i = m_size; i < _n; ++i) {
		push_back(tType());
	}
	for (uint i = _n; i < m_size; ++i) {
		pop_back();
	}
}

template <typename tType>
inline void VectorBase<tType>::resize(uint _n, const tType& _v)
{
	reserve(_n);
	for (uint i = m_size; i < _n; ++i) {
		push_back(_v);
	}
	for (uint i = _n; i < m_size; ++i) {
		pop_back();
	}
}

// PROTECTED

template <typename tType>
inline VectorBase<tType>::VectorBase()
	: m_data(0)
	, m_localData(0)
	, m_size(0)
	, m_capacity(0)
{
}

template <typename tType>
inline VectorBase<tType>::VectorBase(tType* _localData, uint _localDataSize)
	: m_data(_localData)
	, m_localData(_localData)
	, m_size(_localDataSize)
	, m_capacity(_localDataSize)
{
}

template <typename tType>
inline VectorBase<tType>::~VectorBase()
{
	if (m_size) {
		for (auto it = begin(); it != end(); ++it) {
			it->~tType();
		}
	}
	if (!isLocal()) {
		free_aligned(m_data);
		m_data = m_localData;
	}
}

} // namespace apt

#endif // apt_Vector_h
