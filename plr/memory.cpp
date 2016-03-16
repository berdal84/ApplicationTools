////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/memory.h>

#include <cstdlib>
#include <new>

void* plr::malloc_aligned(uint _size, uint _align) 
{
	PLR_ASSERT(_align != 0);
	PLR_ASSERT((_align & (_align - 1)) == 0); // align must be a power of 2

	// malloc() is not guaranteed to return a valid ptr in the case that
	// size == 0, hence we must gurantee it
	if (_size == 0) {
		_size = 1;
	}

	// get expansion size (+ size of an address to store the original ptr)
	uint expand = _align + sizeof(void*) - 1;

	for (;;) {
		// attempt to allocate memory
		void* mem = malloc(_size + expand);
		if (mem != 0) {
			// get aligned ptr
			uint ptr = ((uint)mem + expand) / _align * _align;
			PLR_ASSERT(ptr % _align == 0); // ptr not aligned correctly
			PLR_ASSERT(ptr >= (uint)mem + sizeof(void*)); // enough extra memory

			// write original ptr (mem) as header
			void** header = (void**)(ptr - sizeof(void*));
			PLR_ASSERT((uint)header % PLR_ALIGNOF(void*) == 0); // header not aligned correctly
			*header = mem;

			return (void*)ptr;
		}

		PLR_ASSERT(mem != 0);

		// allocation failed, call new_handler
		std::new_handler newHandler = std::set_new_handler(0);
		std::set_new_handler(newHandler);
		if (newHandler) {
			newHandler();
		} else {
			return 0;
		}
	}
}

void plr::free_aligned(void* _p) 
{
	if (_p == 0) { // ignore null ptr
		return;
	}
	_p = *((void**)((std::size_t)_p - sizeof(void*))); // get original ptr
	free(_p);
}