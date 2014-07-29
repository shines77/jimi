
#ifndef _JIMIC_DEF_H_
#define _JIMIC_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_def.h>
#include <jimi/platform/jimi_compiler_def.h>

#if JIMI_IS_WINDOWS
    #include <jimic/platform/jimic_targetver.h>
#endif

#include <jimic/core/jimic_config.h>
#include <jimic/core/jimic_export.h>

// Define type size_t
#ifndef _SIZE_T_DEFINED
#include <stddef.h>
#endif

// linux maybe need #include <sys/types.h>

// Define integer types with known size: int32_t, uint32_t, int64_t, uint64_t.
// If this doesn't work then insert compiler-specific definitions here:
// (stdint.h defined from visual studio 2010)
#if defined(JIMI_IS_GNUC) || (defined(JIMI_MSC_VER) && JIMI_MSC_VER >= 1600)
  // Compilers supporting C99 or C++0x have stdint.h defining these integer types
  #include <stdint.h>
  #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
#elif defined(JIMI_MSC_VER)
  #include <jimic/core/win32/vs_stdint.h>
#else
  #ifndef _STDINT
    #define _STDINT
    // This works with most compilers
    typedef signed   short int  int16_t;
    typedef unsigned short int uint16_t;
    typedef signed   int        int32_t;
    typedef unsigned int       uint32_t;
    typedef long long           int64_t;
    typedef unsigned long long uint64_t;
    #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
  #endif
#endif

#include <jimi/core/jimi_common.h>

/**
 * @file jimic_def.h
 * @brief JIMIC Platform Definitions
 * @remark This is a generated header generated from include/jimic_def.h.in by
 * ./configure, or copied from include/jimic_def.hw or include/jimic_def.hnw 
 * for Win32 or Netware by those build environments, respectively.
 */

#if defined(JIMI_IS_WINDOWS) || defined(JIMI_IS_DOXYGEN)

/* Ignore most warnings (back down to /W3) for poorly constructed headers
 */
#if defined(_MSC_VER) && _MSC_VER >= 1200
//#pragma warning(push, 3)
#endif

/* disable or reduce the frequency of...
 *   C4057: indirection to slightly different base types
 *   C4075: slight indirection changes (unsigned short* vs short[])
 *   C4100: unreferenced formal parameter
 *   C4127: conditional expression is constant
 *   C4163: '_rotl64' : not available as an intrinsic function
 *   C4201: nonstandard extension nameless struct/unions
 *   C4244: int to char/short - precision loss
 *   C4514: unreferenced inline function removed
 */
//#pragma warning(disable: 4100 4127 4163 4201 4514; once: 4057 4075 4244)

/* Ignore Microsoft's interpretation of secure development
 * and the POSIX string handling API
 */
#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable: 4996)
#endif  /* _MSC_VER */

/* Has windows.h already been included?  If so, our preferences don't matter,
 * but we will still need the winsock things no matter what was included.
 * If not, include a restricted set of windows headers to our tastes.
 */
#ifndef _WINDOWS_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  /* WIN32_LEAN_AND_MEAN */

#ifndef _WIN32_WINNT
/* Restrict the server to a subset of Windows XP header files by default
 */
#define _WIN32_WINNT    0x0501
#endif  /* _WIN32_WINNT */

#ifndef NOUSER
#define NOUSER
#endif

#ifndef NOMCX
#define NOMCX
#endif

#ifndef NOIME
#define NOIME
#endif

#include <windows.h>

#ifdef _MSC_VER

/* 
 * Add a _very_few_ declarations missing from the restricted set of headers
 * (If this list becomes extensive, re-enable the required headers above!)
 * winsock headers were excluded by WIN32_LEAN_AND_MEAN, so include them now
 */
#define SW_HIDE             0
#ifndef _WIN32_WCE
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#else
#include <winsock.h>
#endif  /* _WIN32_WCE */

#endif  /* _MSC)VER */

#endif  /* _WINDOWS_ */

#endif  /* defined(JIMI_IS_WINDOWS) || defined(JIMI_IS_DOXYGEN) */

#define JIMIC_MIN(a, b)         ((a) < (b) ? (a) : (b))
#define JIMIC_MAX(a, b)         ((a) > (b) ? (a) : (b))

#ifndef TRUE
#define TRUE                    1
#endif  // TRUE

#ifndef FALSE
#define FALSE                   0
#endif  // FALSE

// for precompiled macro to string
#define JIMIC_STRING_ESCAPE(x)  #x
#define JIMIC_TO_STRING(x)      JIMIC_STRING_ESCAPE(x)

#define JIMIC_MAKE_STRING(x)    { #x, JIMIC_STRING_ESCAPE(x) }

typedef struct JIMIC_MACRO_T
{
    const char *name;
    const char *value;
} JIMIC_MACRO_T;

#ifndef JM_CONST
#define JM_CONST        const
#endif  /* JM_CONST */

#ifndef JM_CHAR_DEFINED
#define JM_CHAR_DEFINED
typedef char            jm_char;
#endif  /* JM_CHAR_DEFINED */

#ifndef JM_UCHAR_DEFINED
#define JM_UCHAR_DEFINED
typedef unsigned char   jm_uchar;
#endif  /* JM_UCHAR_DEFINED */

/**
 * JMC public API wrap for C++ compilers.
 */
#ifdef __cplusplus
#define JMC_BEGIN_DECLS     extern "C" {
#define JMC_END_DECLS       }
#else
#define JMC_BEGIN_DECLS
#define JMC_END_DECLS
#endif  /* __cplusplus */

/**
 * for jimic assert defines
 */
#include <jimic/core/jimic_assert.h>

/**
 * for JMC_DECLARE define
 */
#if (!defined(JIMI_IS_WINDOWS)) || defined(JIMI_IS_DOXYGEN)

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

/**
 * for jmc_countof helper
 */
#if defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || defined(_M_IA64) || defined(_M_AMD64)
#define ALIGNMENT_MACHINE
#define UNALIGNED   __unaligned
#if defined(_WIN64)
#define UNALIGNED64 __unaligned
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
 * jmc_countof helper
 */
#if !defined(jmc_countof)

#if !defined(__cplusplus)

#define jmc_countof(_Array)     (sizeof(_Array) / sizeof(_Array[0]))

#else

extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (*__jmc_countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];

#define jmc_countof(_Array)     sizeof(*__jmc_countof_helper(_Array))
}

#endif  /* !defined(__cplusplus) */

#endif  /* !defined(jmc_countof) */

/* Done with badly written headers
 */
#if defined(_MSC_VER) && _MSC_VER >= 1200
//#pragma warning(pop)
//#pragma warning(disable: 4996)
#endif

#endif  /* _JIMIC_DEF_H_ */
