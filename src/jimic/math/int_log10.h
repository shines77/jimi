
#ifndef _JIMIC_MATH_INT_LOG10_H_
#define _JIMIC_MATH_INT_LOG10_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/stddef.h"

#ifdef __has_builtin
#if __has_builtin(__builtin_clz)
#define __has_builtin_clz
#endif
#if __has_builtin(__builtin_clzll)
#define __has_builtin_clzll
#endif
#endif // __has_builtin

#if defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)
// for _BitScanReverse()
#include <intrin.h>
#pragma intrinsic(_BitScanReverse)
#endif  // _MSC_VER

#ifdef __cplusplus
extern "C" {
#endif

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_sys(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_comp(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_v2(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_v3(uint32_t val);

#if defined(_WIN64) || defined(_M_X64)

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint64_log10(uint64_t val);

#endif

#if defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)
// Do nothing!! in MSVC or Intel C++ Compiler
#elif defined(__MINGW32__) || defined(__has_builtin_clz) || (__GNUC__ >= 4)
JMC_INLINE
JMC_DECLARE_NONSTD(unsigned char)
_BitScanReverse(unsigned long *firstBit1Index, unsigned long scanNum);
#endif

#if defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)
// Do nothing!! in MSVC or Intel C++ Compiler
#elif defined(_M_X64) && (defined(__MINGW32__) || defined(__has_builtin_clzll) || (__GNUC__ >= 4))
JMC_INLINE
JMC_DECLARE_NONSTD(unsigned char)
_BitScanReverse64(unsigned long *firstBit1Index, uint64_t scanNum);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_MATH_INT_LOG10_H_ */
