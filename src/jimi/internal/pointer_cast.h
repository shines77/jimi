
#ifndef _JIMI_INTERNAL_POINTER_CAST_H_
#define _JIMI_INTERNAL_POINTER_CAST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

NS_JIMI_BEGIN

namespace internal {

#if JIMI_USE_ASSERT

static void * const poisoned_ptr = reinterpret_cast<void *>(-1);

//! Set p to invalid pointer value.
template<typename T>
inline void poison_pointer(T *& p) { p = reinterpret_cast<T*>(poisoned_ptr); }

/** Expected to be used in assertions only, thus no empty form is defined. **/
template<typename T>
inline bool is_poisoned(T * p) { return p == reinterpret_cast<T*>(poisoned_ptr); }

#else

template<typename T>
inline void poison_pointer(T *) { /* do nothing */ }

#endif  /* !JIMI_USE_ASSERT */

//! Cast pointer from U* to T.
/** This method should be used sparingly as a last resort for dealing with 
    situations that inherently break strict ISO C++ aliasing rules. */
template<typename T, typename U> 
inline T punned_cast(U * ptr) {
    uintptr_t x = reinterpret_cast<uintptr_t>(ptr);
    return reinterpret_cast<T>(x);
}

}  /* namespace internal */

NS_JIMI_END

#endif  /* !_JIMI_INTERNAL_POINTER_CAST_H_ */
