////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#include <plr/memory.h>

#include <cstdlib>

void* plr::malloc_aligned(uint _size, uint _align) 
{
#ifdef PLR_COMPILER_MSVC
	return _aligned_malloc(_size, _align);
#else
	aligned_alloc(_align, _size);
#endif
}

void* plr::realloc_aligned(void* _p, uint _size, uint _align)
{
#ifdef PLR_COMPILER_MSVC
	return _aligned_realloc(_p, _size, _align);
#else
	if (_p) {
		return realloc(_p, _size);
	} else {
		return malloc_aligned(_size, _align);
	}
#endif
}

void plr::free_aligned(void* _p) 
{
#ifdef PLR_COMPILER_MSVC
	_aligned_free(_p);
#else
	free(_p);
#endif
}