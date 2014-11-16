
#ifndef _JIMI_STDINT_H_
#define _JIMI_STDINT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_platform_def.h"
#include "jimic/core/jimic_compiler_def.h"

// Define integer types with known size: int32_t, uint32_t, int64_t, uint64_t.
// If this doesn't work then insert compiler-specific definitions here:
// (stdint.h defined from visual studio 2010)
#if defined(JIMI_IS_GNUC) || (defined(JIMI_MSC_VER) && JIMI_MSC_VER >= 1600)
  // Compilers supporting C99 or C++0x have stdint.h defining these integer types
  #include <stdint.h>
  #define INT64_SUPPORTED // Remove this if the compiler doesn't support 64-bit integers
#elif defined(JIMI_MSC_VER)
  #include "jimi/core/win32/vs_stdint.h"
#else
  #ifndef _STDINT
    #define _STDINT
    // This works with most compilers
    typedef signed   char         int8_t;
    typedef unsigned char        uint8_t;
    typedef signed   short int   int16_t;
    typedef unsigned short int  uint16_t;
    typedef signed   int         int32_t;
    typedef unsigned int        uint32_t;
    typedef long long            int64_t;
    typedef unsigned long long  uint64_t;
    #define INT64_SUPPORTED  // Remove this if the compiler doesn't support 64-bit integers
  #endif
#endif

// Define type size_t
#ifndef _SIZE_T_DEFINED
#include <crtdefs.h>
#endif  /* _SIZE_T_DEFINED */

// linux maybe need #include <sys/types.h>

#ifndef _SSIZE_T_DEFINED
#if defined(_WIN32) || defined(_WIN64)
#ifdef _WIN64
typedef signed __int64      ssize_t;
#else  /* _WIN64 */
typedef _W64 signed int     ssize_t;
#endif  /* _WIN64 */
#else
#ifdef _x86_x64
typedef signed long long    ssize_t;
#else  /* _x86_x64 */
typedef signed int          ssize_t;
#endif  /* _x86_x64 */
#endif  /* defined(_WIN32) || defined(_WIN64) */
#define _SSIZE_T_DEFINED
#endif  /* _SSIZE_T_DEFINED */

#endif  /* _JIMI_STDINT_H_ */
