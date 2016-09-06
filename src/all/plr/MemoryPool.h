////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_MemoryPool_h
#define plr_MemoryPool_h

#include <plr/def.h>

namespace plr {

////////////////////////////////////////////////////////////////////////////////
/// \class MemoryPool
/// Provides (de)allocations of objects with O(1) complexity given a fixed 
/// object size/alignment.
/// Example usage:	
/// \code
///    MemoryPool mp(sizeof(Foo), PLR_ALIGNOF(Foo), 128);
///    Foo* f = new(mp.alloc()) Foo; // placement new
///    f->~Foo(); // explicit dtor call
///    mp.free(f);
/// \endcode
///
/// \note Any allocated objects should be released via free() before the
///    MemoryPool is destroyed.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
class MemoryPool
{
public:
	/// \param _objectSize Size (bytes) of a single object. Must be at least
	///    sizeof(void*).
	/// \param _objectAlignment Alignment (bytes) of a single object.
	/// \param _blockSize The number of new unused objects to allocate when
	///    allocate() cannot service a new request. Ideally this will
	///    be large enough to service all requests, thus incurring only a
	///    single block allocation.
	MemoryPool(uint _objectSize, uint _objectAlignment, uint _blockSize);

	/// Free all allocated memory. Any allocated objects should be released via
	///    free() before the MemoryPool is destroyed.
	~MemoryPool();


	/// \return Ptr to the next free memory object.
	void* alloc();

	/// \param _object Ptr to a memory object previously returned by a call to 
	///    allocate().
	void free(void* _object);

	/// \return Whether the number of used objects is consistent with the number
	///    of accessible free objects.
	bool validate() const;

private:
	uint   m_objectSize, m_objectAlignment, m_blockSize;
	void*  m_nextFree;
	uint   m_usedCount;
	void** m_blocks;
	uint   m_blockCount;

	/// Allocate/initialize a new block.
	void allocBlock();

}; // class MemoryPool

} // namespace plr

#endif // plr_MemoryPool_h
