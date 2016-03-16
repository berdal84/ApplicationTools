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

/// Free memory previous allocated vai malloc_aligned
/// \param _p ptr to memory previously allocated by malloc_aligned().
/// \ingroup plr_core
void free_aligned(void* _p);

} // namespace plr

#endif // plr_memory_h
