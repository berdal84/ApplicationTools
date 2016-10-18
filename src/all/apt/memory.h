#pragma once
#ifndef apt_memory_h
#define apt_memory_h

#include <apt/def.h>

namespace apt {

/// Allocate _size bytes with _align aligment.
/// \ingroup APT_core
void* malloc_aligned(uint _size, uint _align);

/// Reallocate memory previously allocated via malloc_aligned (or allocate a new
/// block if _p is null). The alignment of a previously allocated block may not
/// be changed.
/// \ingroup APT_core
void* realloc_aligned(void* _p, uint _size, uint _align);

/// Free memory previous allocated via malloc_aligned().
/// \ingroup APT_core
void free_aligned(void* _p);


namespace internal {
	template <uint kAlignment> struct aligned_base;
		template<> struct APT_ALIGN(1)   aligned_base<1>   {};
		template<> struct APT_ALIGN(2)   aligned_base<2>   {};
		template<> struct APT_ALIGN(4)   aligned_base<4>   {};
		template<> struct APT_ALIGN(8)   aligned_base<8>   {};
		template<> struct APT_ALIGN(16)  aligned_base<16>  {};
		template<> struct APT_ALIGN(32)  aligned_base<32>  {};
		template<> struct APT_ALIGN(64)  aligned_base<64>  {};
		template<> struct APT_ALIGN(128) aligned_base<128> {};
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
/// \ingroup APT_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType, uint kAlignment>
struct aligned: private internal::aligned_base<kAlignment>
{
	aligned()                                           { APT_ASSERT((uint)this % kAlignment == 0); }

	// note that malloc_aligned is called with APT_ALIGNOF(tType) which will be
	// the min of the natural alignment of tType and kAlignment
	void* operator new(std::size_t _size)               { return malloc_aligned(_size, APT_ALIGNOF(tType)); }
	void  operator delete(void* _ptr)                   { free_aligned(_ptr); }
	void* operator new[](std::size_t _size)             { return malloc_aligned(_size, APT_ALIGNOF(tType)); }
	void  operator delete[](void* _ptr)                 { free_aligned(_ptr); }
	void* operator new(std::size_t _size, void* _ptr)   { APT_ASSERT((uint)_ptr % kAlignment == 0); return _ptr; }
	void  operator delete(void*, void*)                 { ; } // dummy, matches placement new

}; // class aligned

////////////////////////////////////////////////////////////////////////////////
/// \class storage
/// Provides aligned storage for kCount objects of type tType. Suitable for 
/// allocating static blocks of uninitialized memory for use with placement
/// new.
/// \ingroup APT_core
////////////////////////////////////////////////////////////////////////////////
template <typename tType, uint kCount>
class storage: private internal::aligned_base< APT_ALIGNOF(tType) >
{
public:
	operator tType*() { return (tType*)m_buf; }
private:
	char m_buf[sizeof(tType) * kCount];
}; // class storage

} // namespace apt

#endif // apt_memory_h
