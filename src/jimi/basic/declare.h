
#ifndef _JIMI_BASIC_DECLARE_H_
#define _JIMI_BASIC_DECLARE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/archdef.h"
#include "jimi/basic/platform.h"
#include "jimi/basic/compiler.h"

#include "jimi/basic/declare_comm.h"

#ifdef __cplusplus
# define JIMI_CRT_INLINE        inline
#else
# if __GNUC_STDC_INLINE__
#  define JIMI_CRT_INLINE       extern inline __attribute__((__gnu_inline__))
# else
#  define JIMI_CRT_INLINE       extern __inline__
# endif
#endif

/* JIMI_CRT_FORCEINLINE will be used when we have a function whose purpose is to return
 * the value of a similar function. This alias function will contain one line
 * of code.
 */
#define JIMI_CRT_FORCEINLINE    JIMI_CRT_INLINE __attribute__((__always_inline__))

/**
 * jimi: for inline and noinline define
 */
#if (_MSC_VER || __INTEL_COMPILER) && (!defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0))

#define JIMI_INLINE              __inline
#define JIMI_FORCEINLINE         __forceinline
#define JIMI_RESTRICT            __restrict
#define JIMI_HAS_INLINE          1

#elif defined(__GNUC__) && (defined(JIMI_MSVC_CLANG) && (JIMI_MSVC_CLANG != 0))

#define JIMI_INLINE              inline
#define JIMI_FORCEINLINE         inline
#define JIMI_RESTRICT            __restrict
#define JIMI_HAS_INLINE          1

#else

#define JIMI_INLINE              inline
#define JIMI_FORCEINLINE         inline
#define JIMI_RESTRICT
#define JIMI_HAS_INLINE          1

#endif

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#define JIMI_NOINLINE                   __declspec(noinline)
#define JIMI_NOINLINE_DECLARE(decl)     __declspec(noinline) decl
#elif defined(__GNUC__) || defined(__MINGW32__)
#define JIMI_NOINLINE                   __attribute__ ((noinline))
#define JIMI_NOINLINE_DECLARE(decl)     decl __attribute__ ((noinline))
#else
#define JIMI_NOINLINE 
#define JIMI_NOINLINE_DECLARE(decl)     decl
#endif

/**
 * for exported func
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define JIMI_EXPORTED_FUNC      __cdecl
    #define JIMI_EXPORTED_METHOD    __thiscall
#else
    #define JIMI_EXPORTED_FUNC
    #define JIMI_EXPORTED_METHOD
#endif

#endif  /* _JIMI_BASIC_DECLARE_H_ */
