
#ifndef _JIMI_COMMON_H_
#define _JIMI_COMMON_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_platform_def.h>
#include <jimi/core/jimi_compiler_def.h>

#if JIMI_IS_WINDOWS
    #include <jimi/core/win32/jimi_targetver.h>
#endif

#include <jimi/core/win32/jimi_config.h>
#include <jimi/core/jimi_export.h>

#if JIMI_IS_MSVC
    #include <jimi/core/win32/vs_stdint.h>
    #define snprintf _snprintf
#else
    #include <stdint.h>
#endif

#define JIMI_LIKELY(x)      (x)
#define JIMI_UNLIKELY(x)    (x)

/**
 * for jimi_winapi
 */
#if defined(JIMI_IS_MSVC)
#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

#define JIMI_WINAPI             __CLR_OR_STD_CALL
#define JMC_WINAPI              JIMI_WINAPI
#else
#define JIMI_WINAPI
#define JMC_WINAPI
#endif  /* JIMI_IS_MSVC */

/**
 * define about __cdecl, __stdcall and __fastcall
 */
#undef __CDECL
#undef __STDCALL
#undef __FASTCALL

#undef _CDECL_SUFFIX
#undef _STDCALL_SUFFIX
#undef _FASTCALL_SUFFIX

/**
 * define __cdecl, __stdcall and __fastcall for windows
 */
#if defined(_MSC_VER)

#ifndef __CDECL
#define __CDECL             __cdecl
#endif

#ifndef __STDCALL
#define __STDCALL           __stdcall
#endif

#ifndef __FASTCALL
#define __FASTCALL          __fastcall
#endif

#define _CDECL_SUFFIX
#define _STDCALL_SUFFIX
#define _FASTCALL_SUFFIX

/**
 * define __cdecl, __stdcall and __fastcall for linux
 */
#elif defined(__GNUC__)

#define __CDECL
#define __STDCALL
#define __FASTCALL

#ifndef _CDECL_SUFFIX
#define _CDECL_SUFFIX       __attribute__ ((__cdecl__))
#endif

#ifndef _STDCALL_SUFFIX
#define _STDCALL_SUFFIX
#endif

#ifndef _FASTCALL_SUFFIX
#define _FASTCALL_SUFFIX    __attribute__ ((fastcall))
#endif

/**
 * define __cdecl, __stdcall and __fastcall for another os
 */
#else  /* !_MSC_VER && !__GNUC__ */

#define __CDECL
#define __STDCALL
#define __FASTCALL

#define _CDECL_SUFFIX
#define _STDCALL_SUFFIX
#define _FASTCALL_SUFFIX

#endif  /* _MSC_VER */

/**
 * for exported func
 */
#if _MSC_VER >= 1400
    #define JIMI_EXPORTED_FUNC      __cdecl
    #define JIMI_EXPORTED_METHOD    __thiscall
#else
    #define JIMI_EXPORTED_FUNC
    #define JIMI_EXPORTED_METHOD
#endif

#ifdef __cplusplus
# define JIMI_CRT_INLINE        inline
#else
# if __GNUC_STDC_INLINE__
#  define JIMI_CRT_INLINE       extern inline __attribute__((__gnu_inline__))
# else
#  define JIMI_CRT_INLINE       extern __inline__
# endif
#endif

/* _CRTALIAS will be used when we have a function whose purpose is to return
 * the value of a similar function. This alias function will contain one line
 * of code.
 */
#define JIMI_CRT_FORCEINLINE    __CRT_INLINE __attribute__((__always_inline__))

/**
 * jimi: for inline and noinline define
 */
#if (_MSC_VER || __INTEL_COMPILER) && (!defined(JIMI_MSC_CLANG) || (JIMI_MSC_CLANG == 0))

#define JIMI_INLINE              __inline
#define JIMI_FORCEINLINE         __forceinline
#define JIMI_RESTRICT            __restrict
#define JIMI_HAS_INLINE          1

#elif defined(__GNUC__) && (defined(JIMI_MSC_CLANG) && (JIMI_MSC_CLANG != 0))

#define JIMI_INLINE              inline
#define JIMI_FORCEINLINE         inline
#define JIMI_RESTRICT            restrict
#define JIMI_HAS_INLINE          1

#else

#define JIMI_INLINE              inline
#define JIMI_FORCEINLINE         inline
#define JIMI_RESTRICT
#define JIMI_HAS_INLINE          1

#endif

#if _MSC_VER || __INTEL_COMPILER
#define JIMI_NOINLINE(decl)     __declspec(noinline) decl
#elif __GNUC__
#define JIMI_NOINLINE(decl)     decl __attribute__ ((noinline))
#else
#define JIMI_NOINLINE(decl)     decl
#endif

/**
 * jimic: for inline define
 */
#if (defined(JIMI_MSC_VER) || defined(JIMI_IS_ICC)) && (!defined(JIMI_MSC_CLANG) || (JIMI_MSC_CLANG == 0))

#define JMC_INLINE              __inline
#define JMC_FORCEINLINE         __forceinline
#define JMC_HAS_INLINE          1

#else

#define JMC_INLINE              inline
#define JMC_FORCEINLINE         inline
#define JMC_HAS_INLINE          1

#endif  /* JIMI_MSC_VER */

#if (_MSC_VER || __INTEL_COMPILER) && (!defined(JIMI_MSC_CLANG) || (JIMI_MSC_CLANG == 0))
#define JMC_NOINLINE(decl)      __declspec(noinline) decl
#elif __GNUC__ || (defined(JIMI_MSC_CLANG) || (JIMI_MSC_CLANG != 0))
#define JMC_NOINLINE(decl)      decl __attribute__ ((noinline))
#else
#define JMC_NOINLINE(decl)      decl
#endif

/**
 * for jm_countof helper
 */
#if defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || defined(_M_IA64) || defined(_M_AMD64)
#define ALIGNMENT_MACHINE
#define UNALIGNED       __unaligned
#if defined(_WIN64)
#define UNALIGNED64     __unaligned
#else
#define UNALIGNED64
#endif
#else
#undef ALIGNMENT_MACHINE
#ifndef UNALIGNED
#define UNALIGNED
#endif
#ifndef UNALIGNED64
#define UNALIGNED64
#endif
#endif

/**
 * jm_countof helper
 */
#if !defined(jm_countof)

#if !defined(__cplusplus)

#define jm_countof(_Array)      (sizeof(_Array) / sizeof(_Array[0]))

#else

extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (*__jm_countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];

#define jm_countof(_Array)      sizeof(*__jm_countof_helper(_Array))
}

#endif  /* !defined(__cplusplus) */

#endif  /* !defined(jm_countof) */

#endif  /* _JIMI_COMMON_H_ */
