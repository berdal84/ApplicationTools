////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 John Chapman -- http://john-chapman.net
// This software is distributed freely under the terms of the MIT License.
// See http://opensource.org/licenses/MIT
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef plr_memory_h
#define plr_memory_h

#include <plr/def.h>

namespace plr {

/// Allocate memory with a specified alignment.
/// \param _size number of bytes to allocate. malloc_aligned() guarantees to
///    return a distinct non-null ptr if size is zero (except if the allocation
///    fails).
/// \param _align required alignment.
/// \return void* to allocated block, which must be freed via free_aligned()
///    This may be a null ptr if the allocation fails and std::new_handler 
///    is null. If std::new_handler is not null it is called and the allocation
///    is attempted again.
/// \ingroup plr_core
void* malloc_aligned(uint _size, uint _align);

/// Free memory previous allocated via malloc_aligned().
/// \param _p ptr to memory previously allocated by malloc_aligned().
/// \ingroup plr_core
void free_aligned(void* _p);

namespace internal {
	template <uint kAlignment> struct aligned_base;
		template<> struct PLR_ALIGN(1)   aligned_base<1>   {};
		template<> struct PLR_ALIGN(2)   aligned_base<2>   {};
		template<> struct PLR_ALIGN(4)   aligned_base<4>   {};
		template<> struct PLR_ALIGN(8)   aligned_base<8>   {};
		template<> struct PLR_ALIGN(16)  aligned_base<16>  {};
		template<> struct PLR_ALIGN(32)  aligned_base<32>  {};
		template<> struct PLR_ALIGN(64)  aligned_base<64>  {};
		template<> struct PLR_ALIGN(128) aligned_base<128> {};
} // namespace internal

////////////////////////////////////////////////////////////////////////////////
/// \class aligned
/// Mixin class, provides template-based memory alignment for deriving classes.
/// Use cautiously, especially with multiple inheritance.
/// Usage:
/// \code
///    class Foo: public aligned<Foo, 16>
///    { // ...
/// \endcode
/// \tparam tType Deriving type.
/// \tparam kAlignment Required alignment.
/// \note Alignment can only be increased. If the deriving class has a higher
///    natural alignment than kAlignment, the higher alignment is used.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType, uint kAlignment>
struct aligned: private internal::aligned_base<kAlignment>
{
	aligned()                                           { PLR_ASSERT((uint)this % kAlignment == 0); }

	// note that malloc_aligned is called with PLR_ALIGNOF(tType) which will be
	// the min of the natural alignment of tType and kAlignment
	void* operator new(std::size_t _size)               { return malloc_aligned(_size, PLR_ALIGNOF(tType)); }
	void  operator delete(void* _ptr)                   { free_aligned(_ptr); }
	void* operator new[](std::size_t _size)             { return malloc_aligned(_size, PLR_ALIGNOF(tType)); }
	void  operator delete[](void* _ptr)                 { free_aligned(_ptr); }
	void* operator new(std::size_t _size, void* _ptr)   { PLR_ASSERT((uint)_ptr % kAlignment == 0); return _ptr; }
	void  operator delete(void*, void*)                 { ; } // dummy, matches placement new

}; // class aligned

////////////////////////////////////////////////////////////////////////////////
/// \class storage
/// Provides aligned storage for kCount objects of type tType. Suitable for 
/// allocating static blocks of uninitialized memory for use with placement
/// new.
/// \ingroup plr_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType, uint kCount>
class storage: private internal::aligned_base< PLR_ALIGNOF(tType) >
{
public:
	operator tType*() { return (tType*)m_buf; }
private:
	char m_buf[sizeof(tType) * kCount];
}; // class storage

} // namespace plr

#endif // plr_memory_h
