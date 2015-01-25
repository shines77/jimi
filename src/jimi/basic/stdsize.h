
#ifndef _JIMI_BASIC_STDSIZE_H_
#define _JIMI_BASIC_STDSIZE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Define type size_t
#ifndef _SIZE_T_DEFINED
//#include <crtdefs.h>
#include <stddef.h>
#endif  /* _SIZE_T_DEFINED */

// linux maybe need #include <sys/types.h>

#if !(defined(_SSIZE_T_DEFINED) && defined(_SSIZE_T_))
#if (defined(_WIN32) || defined(_WIN64)) \
    && (!(defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__))) \
    && (!defined(__CYGWIN__))
#ifdef _WIN64
typedef signed __int64      ssize_t;
#else  /* !_WIN64 */
typedef _W64 signed int     ssize_t;
#endif  /* _WIN64 */
#else
#ifdef _M_X64
typedef signed long long    ssize_t;
#else  /* !_M_X64 */
typedef signed int          ssize_t;
#endif  /* _M_X64 */
#endif  /* defined(_WIN32) || defined(_WIN64) */
#define _SSIZE_T_DEFINED
#endif  /* _SSIZE_T_DEFINED */

#endif  /* _JIMI_BASIC_STDSIZE_H_ */
