
#ifndef _JIMIC_UTIL_UTILS_H_
#define _JIMIC_UTIL_UTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#define jimi_abs_marco(val)     (((val) >= 0) ? (val) : -(val))

JIMI_INLINE
int jimi_abs(const int val)
{
    if (val < 0)
        return -val;
    else
        return val;
}

JIMI_INLINE
long jimi_labs(const long val)
{
    if (val < 0)
        return -val;
    else
        return val;
}

JIMI_INLINE
int64_t jimi_llabs(const int64_t val)
{
    if (val < 0)
        return -val;
    else
        return val;
}

#endif  /* !_JIMIC_UTIL_UTILS_H_ */
