#include <apt/memory.h>

#include <cstdlib>

void* apt::malloc_aligned(uint _size, uint _align) 
{
#ifdef APT_COMPILER_MSVC
	return _aligned_malloc(_size, _align);
#else
	aligned_alloc(_align, _size);
#endif
}

void* apt::realloc_aligned(void* _p, uint _size, uint _align)
{
#ifdef APT_COMPILER_MSVC
	return _aligned_realloc(_p, _size, _align);
#else
	if (_p) {
		return realloc(_p, _size);
	} else {
		return malloc_aligned(_size, _align);
	}
#endif
}

void apt::free_aligned(void* _p) 
{
#ifdef APT_COMPILER_MSVC
	_aligned_free(_p);
#else
	free(_p);
#endif
}
