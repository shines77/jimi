
#ifndef _JIMIC_BASIC_DECLARE_H_
#define _JIMIC_BASIC_DECLARE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/archdef.h"
#include "jimi/basic/platform.h"
#include "jimi/basic/compiler.h"

#include "jimi/basic/declare_comm.h"

/**
 * jimic: for inline define
 */
#if (defined(JIMI_MSC_VER) || defined(JIMI_IS_ICC)) && (!defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0))

#define JMC_INLINE              static __inline
#define JMC_FORCEINLINE         static __forceinline
#define JMC_RESTRICT            __restrict
#define JMC_HAS_INLINE          1

#elif defined(__GNUC__) || (defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG != 0))

#define JMC_INLINE              static inline
#define JMC_FORCEINLINE         static inline
#define JMC_RESTRICT            __restrict
#define JMC_HAS_INLINE          1

#else

#define JMC_INLINE              static inline
#define JMC_FORCEINLINE         static inline
#define JMC_RESTRICT
#define JMC_HAS_INLINE          1

#endif  /* JIMI_MSC_VER */

/**
 * for JMC_DECLARE define
 */
#if (!defined(JIMI_IS_WINDOWS)) || defined(JIMI_IS_DOXYGEN) || defined(__GUNC__) \
    || defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)

#define JMC_DECLARE(type)               type
#define JMC_DECLARE_NONSTD(type)        type
#define JMC_INLINE_DECLARE(type)        JMC_INLINE type
#define JMC_INLINE_NONSTD(type)         JMC_INLINE type
#define JMC_FORCEINLINE_DECLARE(type)   JMC_FORCEINLINE type
#define JMC_FORCEINLINE_NONSTD(type)    JMC_FORCEINLINE type
#define JMC_DECLARE_DATA

#elif defined(JIMI_DECLARE_IMPORT)

#define JMC_DECLARE(type)               __declspec(dllimport) type __stdcall
#define JMC_DECLARE_NONSTD(type)        __declspec(dllimport) type __cdecl
#define JMC_INLINE_DECLARE(type)        JMC_INLINE __declspec(dllimport) type __stdcall
#define JMC_INLINE_NONSTD(type)         JMC_INLINE __declspec(dllimport) type __cdecl
#define JMC_FORCEINLINE_DECLARE(type)   JMC_FORCEINLINE __declspec(dllimport) type __stdcall
#define JMC_FORCEINLINE_NONSTD(type)    JMC_FORCEINLINE __declspec(dllimport) type __cdecl
#define JMC_DECLARE_DATA                __declspec(dllimport)

#elif defined(JIMI_DECLARE_EXPORT)

#define JMC_DECLARE(type)               __declspec(dllexport) type __stdcall
#define JMC_DECLARE_NONSTD(type)        __declspec(dllexport) type __cdecl
#define JMC_INLINE_DECLARE(type)        JMC_INLINE __declspec(dllexport) type __stdcall
#define JMC_INLINE_NONSTD(type)         JMC_INLINE __declspec(dllexport) type __cdecl
#define JMC_FORCEINLINE_DECLARE(type)   JMC_FORCEINLINE __declspec(dllexport) type __stdcall
#define JMC_FORCEINLINE_NONSTD(type)    JMC_FORCEINLINE __declspec(dllexport) type __cdecl
#define JMC_DECLARE_DATA                __declspec(dllexport)

#else

#define JMC_DECLARE(type)               type __stdcall
#define JMC_DECLARE_NONSTD(type)        type __cdecl
#define JMC_INLINE_DECLARE(type)        JMC_INLINE type __stdcall
#define JMC_INLINE_NONSTD(type)         JMC_INLINE type __cdecl
#define JMC_FORCEINLINE_DECLARE(type)   JMC_FORCEINLINE type __stdcall
#define JMC_FORCEINLINE_NONSTD(type)    JMC_FORCEINLINE type __cdecl
#define JMC_DECLARE_DATA

#endif  /* (!defined(JIMI_IS_WINDOWS)) || defined(JIMI_IS_DOXYGEN) */

#if (defined(_MSC_VER) || defined(__INTEL_COMPILER)) && (!defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0))
#define JMC_NOINLINE                    __declspec(noinline)
#define JMC_NOINLINE_DECLARE(decl)      __declspec(noinline) decl
#elif defined(__GNUC__) || defined(__MINGW32__) || (defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG != 0))
#define JMC_NOINLINE                    __attribute__ ((noinline))
#define JMC_NOINLINE_DECLARE(decl)      decl __attribute__ ((noinline))
#else
#define JMC_NOINLINE 
#define JMC_NOINLINE_DECLARE(decl)      decl
#endif

/**
 * for thread func define
 */
#if defined(JIMI_IS_WINDOWS)
#define JIMIC_THREAD_FUNC           __stdcall
#else
#define JIMIC_THREAD_FUNC
#endif  /* JIMI_IS_WINDOWS */

#if !defined(__GNUC__) && !defined(__attribute__)
#define __attribute__(__x)
#endif

/**
 * for exported func
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    #define JIMIC_EXPORTED_FUNC     __cdecl
    #define JIMIC_EXPORTED_METHOD   __thiscall
#else
    #define JIMIC_EXPORTED_FUNC
    #define JIMIC_EXPORTED_METHOD
#endif

#endif  /* _JIMIC_BASIC_DECLARE_H_ */
