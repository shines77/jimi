
#ifndef _JIMI_BASIC_COMMON_H_
#define _JIMI_BASIC_COMMON_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/platform.h"
#include "jimi/basic/compiler.h"

#if JIMI_IS_MSVC
    #define snprintf _snprintf
#endif

#define JIMI_LIKELY(x)      (x)
#define JIMI_UNLIKELY(x)    (x)

#include "jimi/basic/stdint.h"

/**
 * for jmc_countof and jm_countof helper
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

#endif  /* _JIMI_BASIC_COMMON_H_ */
