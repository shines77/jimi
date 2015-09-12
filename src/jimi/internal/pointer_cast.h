
#ifndef _JIMI_INTERNAL_POINTER_CAST_H_
#define _JIMI_INTERNAL_POINTER_CAST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"

#if defined(__cplusplus) && (__cplusplus != 0)

#ifndef FORCE_CAST
#define FORCE_CAST(type, ptr)       reinterpret_cast<type>(ptr)
#endif

#ifndef STATIC_CAST
#define STATIC_CAST(type, ptr)      static_cast<type>(ptr)
#endif

#ifndef CONST_CAST
#define CONST_CAST(type, ptr)       const_cast<type>(ptr)
#endif

#ifndef DYNAMIC_CAST
#define DYNAMIC_CAST(type, ptr)     dynamic_cast<type>(ptr)
#endif

#ifndef DYN_CAST
#define DYN_CAST(type, ptr)         DYNAMIC_CAST(type, ptr)
#endif

// For c++ const_cast
#ifndef REMOVE_CONST
#define REMOVE_CONST(ptr, constType, newType) \
    reinterpret_cast<newType>(const_cast<constType>(ptr))
#endif

// Force convert/change a const type?
#ifndef CHANGE_CONST_CAST
#define CHANGE_CONST_CAST(ptr, constType, newConstType) \
    const_cast<const newConstType>(reinterpret_cast<newConstType>(const_cast<constType>(ptr)))
#endif

namespace jimi {

namespace internal {

#if defined(JIMI_USE_ASSERT) && (JIMI_USE_ASSERT != 0)

static void * const poisonedptr = reinterpret_cast<void *>(-1);

//! Set p to invalid pointer value.
template<typename T>
inline void poison_pointer(T *& p) { p = reinterpret_cast<T *>(poisonedptr); }

/** Expected to be used in assertions only, thus no empty form is defined. **/
template<typename T>
inline bool is_poisoned(T * p) { return (p == reinterpret_cast<T *>(poisonedptr)); }

#else  /* !defined(JIMI_USE_ASSERT) */

template<typename T>
inline void poison_pointer(T *) { /* do nothing */ }

template<typename T>
inline bool is_poisoned(T * p) { return false; }

#endif  /* defined(JIMI_USE_ASSERT) */

//! Cast pointer from U * to T.
/** This method should be used sparingly as a last resort for dealing with 
    situations that inherently break strict ISO C++ aliasing rules. */
template<typename T, typename U> 
inline T punned_cast(U * ptr) {
    uintptr_t x = reinterpret_cast<uintptr_t>(ptr);
    return reinterpret_cast<T>(x);
}

}  /* namespace internal */

}  /* namespace jimi */

#else  /* !defined(__cplusplus) */

#ifndef FORCE_CAST
#define FORCE_CAST(type, ptr)       ((type)(ptr))
#endif

#ifndef STATIC_CAST
#define STATIC_CAST(type, ptr)      FORCE_CAST(type, ptr)
#endif

#ifndef CONST_CAST
#define CONST_CAST(type, ptr)       FORCE_CAST(type, ptr)
#endif

#ifndef DYNAMIC_CAST
#define DYNAMIC_CAST(type, ptr)     FORCE_CAST(type, ptr)
#endif

#ifndef DYN_CAST
#define DYN_CAST(type, ptr)         FORCE_CAST(type, ptr)
#endif

#ifndef REMOVE_CONST
#define REMOVE_CONST(ptr, constType, newType) \
                                    FORCE_CAST(newType, ptr)
#endif

#ifndef CHANGE_CONST_CAST
#define CHANGE_CONST_CAST(ptr, constType, newConstType) \
                                    FORCE_CAST(const newConstType, ptr)
#endif

#endif  /* defined(__cplusplus) */

#endif  /* !_JIMI_INTERNAL_POINTER_CAST_H_ */
