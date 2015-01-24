
#ifndef _JIMIC_DECLARE_H_
#define _JIMIC_DECLARE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_platform_def.h"
#include "jimi/basic/jimi_compiler_def.h"

#include "jimi/basic/jimi_declare_comm.h"

/**
 * jimic: for inline define
 */
#if (defined(JIMI_MSC_VER) || defined(JIMI_IS_ICC)) && (!defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0))

#define JMC_INLINE              __inline
#define JMC_FORCEINLINE         __forceinline
#define JMC_RESTRICT            __restrict
#define JMC_HAS_INLINE          1

#elif defined(__GNUC__) || (defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG != 0))

#define JMC_INLINE              inline
#define JMC_FORCEINLINE         inline
#define JMC_RESTRICT            __restrict
#define JMC_HAS_INLINE          1

#else

#define JMC_INLINE              inline
#define JMC_FORCEINLINE         inline
#define JMC_RESTRICT
#define JMC_HAS_INLINE          1

#endif  /* JIMI_MSC_VER */

#if (_MSC_VER || __INTEL_COMPILER) && (!defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0))
#define JMC_NOINLINE(decl)      __declspec(noinline) decl
#elif defined(__GNUC__) || (defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG != 0))
#define JMC_NOINLINE(decl)      decl __attribute__ ((noinline))
#else
#define JMC_NOINLINE(decl)      decl
#endif

/**
 * for JMC_DECLARE define
 */
#if (!defined(JIMI_IS_WINDOWS)) || defined(JIMI_IS_DOXYGEN) || defined(__GUNC__) \
    || defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)

#define JMC_DECLARE(type)               type
#define JMC_DECLARE_NONSTD(type)        type
#define JMC_INLINE_DECLARE(type)        static JMC_INLINE type
#define JMC_INLINE_NONSTD(type)         static JMC_INLINE type
#define JMC_FORCEINLINE_DECLARE(type)   static JMC_FORCEINLINE type
#define JMC_FORCEINLINE_NONSTD(type)    static JMC_FORCEINLINE type
#define JMC_DECLARE_DATA

#elif defined(JIMI_DECLARE_IMPORT)

#define JMC_DECLARE(type)               __declspec(dllimport) type __stdcall
#define JMC_DECLARE_NONSTD(type)        __declspec(dllimport) type __cdecl
#define JMC_INLINE_DECLARE(type)        static JMC_INLINE __declspec(dllimport) type __stdcall
#define JMC_INLINE_NONSTD(type)         static JMC_INLINE __declspec(dllimport) type __cdecl
#define JMC_FORCEINLINE_DECLARE(type)   static JMC_FORCEINLINE __declspec(dllimport) type __stdcall
#define JMC_FORCEINLINE_NONSTD(type)    static JMC_FORCEINLINE __declspec(dllimport) type __cdecl
#define JMC_DECLARE_DATA                __declspec(dllimport)

#elif defined(JIMI_DECLARE_EXPORT)

#define JMC_DECLARE(type)               __declspec(dllexport) type __stdcall
#define JMC_DECLARE_NONSTD(type)        __declspec(dllexport) type __cdecl
#define JMC_INLINE_DECLARE(type)        static JMC_INLINE __declspec(dllexport) type __stdcall
#define JMC_INLINE_NONSTD(type)         static JMC_INLINE __declspec(dllexport) type __cdecl
#define JMC_FORCEINLINE_DECLARE(type)   static JMC_FORCEINLINE __declspec(dllexport) type __stdcall
#define JMC_FORCEINLINE_NONSTD(type)    static JMC_FORCEINLINE __declspec(dllexport) type __cdecl
#define JMC_DECLARE_DATA                __declspec(dllexport)

#else

#define JMC_DECLARE(type)               type __stdcall
#define JMC_DECLARE_NONSTD(type)        type __cdecl
#define JMC_INLINE_DECLARE(type)        static JMC_INLINE type __stdcall
#define JMC_INLINE_NONSTD(type)         static JMC_INLINE type __cdecl
#define JMC_FORCEINLINE_DECLARE(type)   static JMC_FORCEINLINE type __stdcall
#define JMC_FORCEINLINE_NONSTD(type)    static JMC_FORCEINLINE type __cdecl
#define JMC_DECLARE_DATA

#endif  /* (!defined(JIMI_IS_WINDOWS)) || defined(JIMI_IS_DOXYGEN) */

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
#if _MSC_VER >= 1400
    #define JIMIC_EXPORTED_FUNC     __cdecl
    #define JIMIC_EXPORTED_METHOD   __thiscall
#else
    #define JIMIC_EXPORTED_FUNC
    #define JIMIC_EXPORTED_METHOD
#endif

#if _MSC_VER || __INTEL_COMPILER
#define JIMIC_NOINLINE(decl)    __declspec(noinline) decl
#elif __GNUC__
#define JIMIC_NOINLINE(decl)    decl __attribute__ ((noinline))
#else
#define JIMIC_NOINLINE(decl)    decl
#endif

#endif  /* _JIMIC_DECLARE_H_ */
