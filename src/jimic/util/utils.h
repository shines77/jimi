
#ifndef _JIMIC_UTIL_UTILS_H_
#define _JIMIC_UTIL_UTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#define jmc_abs_32(val)        (val & 0x7FFFFFFFUL)
#define jmc_abs_64(val)        (val & 0x7FFFFFFFFFFFFFFFULL)
#define jmc_abs_marco(val)     (((val) >= 0) ? (val) : -(val))

JMC_INLINE
unsigned int
jmc_abs(int val)
{
#if 1
    return (unsigned int)(val & 0x7FFFFFFFUL);
#else
    if (val >= 0)
        return val;
    else
        return -val;
#endif
}

JMC_INLINE
unsigned long
jmc_labs(long val)
{
#if 1
    if (sizeof(long) == 4)
        return (unsigned long)(val & 0x7FFFFFFFUL);
    else
        return (unsigned long)(val & 0x7FFFFFFFFFFFFFFFULL);
#else
    if (val >= 0)
        return val;
    else
        return -val;
#endif
}

JMC_INLINE
uint64_t
jmc_llabs(int64_t val)
{
#if 1
    return (uint64_t)(val & 0x7FFFFFFFFFFFFFFFULL);
#else
    if (val >= 0)
        return val;
    else
        return -val;
#endif
}

#endif  /* !_JIMIC_UTIL_UTILS_H_ */
