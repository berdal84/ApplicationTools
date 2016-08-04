////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/memory.h>

#include <cstdlib>

#define HEADER(_p) \
	((void**)((std::uintptr_t)((char*)_p - sizeof(void*)) & ~(sizeof(void*) - 1)))

#define ALIGN(_p, _align) \
	((void*)((((std::uintptr_t)((char*)_p + _align + sizeof(void*))) & ~(_align - 1))))


void* plr::malloc_aligned(uint _size, uint _align) 
{
	PLR_ASSERT(_align != 0);
	PLR_ASSERT((_align & (_align - 1)) == 0); // align must be a power of 2

 // malloc is not guaranteed to return a valid ptr in the case that size == 0
	if (_size == 0) {
		_size = 1;
	}

 // get expansion size (+ size of an address to store the original ptr)
	uint expand = _align + sizeof(void*) - 1;
	void* mem;
	PLR_VERIFY(mem = malloc(_size + expand));
	if (!mem) {
		return 0;
	}
 // get aligned ptr
	uint ptr = ((uint)mem + expand) / _align * _align;
	PLR_ASSERT(ptr % _align == 0); // ptr not aligned correctly
	PLR_ASSERT(ptr >= (uint)mem + sizeof(void*)); // enough extra memory

 // write original ptr as header
	void** header = (void**)(ptr - sizeof(void*));
	PLR_ASSERT((uint)header % PLR_ALIGNOF(void*) == 0); // header not aligned correctly
	*header = mem;

	return (void*)ptr;
}

void* plr::realloc_aligned(void* _p, uint _size, uint _align)
{
	if (!_p) {
		return malloc_aligned(_size, _align);
	}
	PLR_ASSERT((uint)_p % _align == 0); // can't change the alignment
	uint expand = _align + sizeof(void*) - 1;
	_p = *((void**)((uint)_p - sizeof(void*))); // get original ptr
	void* mem;
	PLR_VERIFY(mem = realloc(_p, _size));
	
 // get aligned ptr
	uint ptr = ((uint)mem + expand) / _align * _align;
	PLR_ASSERT(ptr % _align == 0); // ptr not aligned correctly
	PLR_ASSERT(ptr >= (uint)mem + sizeof(void*)); // enough extra memory

 // write original ptr as header
	void** header = (void**)(ptr - sizeof(void*));
	PLR_ASSERT((uint)header % PLR_ALIGNOF(void*) == 0); // header not aligned correctly
	*header = mem;

	return (void*)ptr;
}

void plr::free_aligned(void* _p) 
{
	if (_p) {
		_p = *((void**)((uint)_p - sizeof(void*))); // get original ptr
		free(_p);
	}
}