
#ifndef _JIMIC_BASIC_STDDEF_H_
#define _JIMIC_BASIC_STDDEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/arch_def.h"
#include "jimic/basic/platform_def.h"
#include "jimic/basic/compiler_def.h"

#if defined(_WIN32) || defined(__MINGW32__)
    #include "jimic/basic/targetver.h"
#endif

#include "jimic/config/config.h"
#include "jimic/basic/export.h"

#include "jimic/basic/stdint.h"
#include "jimic/basic/common.h"

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
 * define jimic function declare type and inline defines
 */
#include "jimic/basic/declare.h"

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

/* for static_assert() under VisualStudio 2010 */
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#ifndef static_assert
#define static_assert(expr, msg)
#endif
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

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

/**
  Done with badly written headers
 */
#if defined(_MSC_VER) && _MSC_VER >= 1200
//#pragma warning(pop)
//#pragma warning(disable: 4996)
#endif

#endif  /* _JIMIC_BASIC_STDDEF_H_ */
