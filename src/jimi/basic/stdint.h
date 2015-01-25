
#ifndef _JIMI_BASIC_STDINT_H_
#define _JIMI_BASIC_STDINT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Define integer types with known size: int32_t, uint32_t, int64_t, uint64_t.
// If this doesn't work then insert compiler-specific definitions here:
// (stdint.h defined from visual studio 2010)

#if defined(_MSC_VER) && (_MSC_VER < 1700)
#include "jimi/basic/msvc/stdint.h"
#else
// Compilers supporting C99 or C++0x have stdint.h defining these integer types
#include <stdint.h>
#endif  /* _MSC_VER */

#if !defined(_STDINT) && !defined(_STDINT_H)
  #define _STDINT
  #define _STDINT_H
  // This works with most compilers
  typedef signed   char         int8_t;
  typedef unsigned char        uint8_t;
  typedef signed   short int   int16_t;
  typedef unsigned short int  uint16_t;
  typedef signed   int         int32_t;
  typedef unsigned int        uint32_t;
  typedef long long            int64_t;
  typedef unsigned long long  uint64_t;
#endif

#define INT64_SUPPORTED  // Remove this if the compiler doesn't support 64-bit integers

// For define sizt_t and ssize_t
#include "jimi/basic/stdsize.h"

#endif  /* _JIMI_BASIC_STDINT_H_ */
