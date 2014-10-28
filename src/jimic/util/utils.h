
#ifndef _JIMIC_UTIL_UTILS_H_
#define _JIMIC_UTIL_UTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#include <jimic/libc/endian.h>

#define jmc_abs_32(val)        (((int32_t)(val) >= 0) ? (val) : -(val))
#define jmc_abs_64(val)        (((int64_t)(val) >= 0) ? (val) : -(val))
#define jmc_abs_ex(val, type)  (((type)(val) >= 0) ? (val) : -(val))

union jmc_int64_t
{
    int64_t  i64;
    uint64_t u64;

    struct {
#if JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN
        /* Little endian. */
        uint32_t low;
        uint32_t high;
#else
        /* Big endian. */
        uint32_t high;
        uint32_t low;
#endif
    } u32;

    struct {
#if JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN
        /* Little endian. */
        uint32_t low:     32;
        uint32_t high:    31;
        uint32_t negative: 1;
#else
        /* Big endian. */
        uint32_t negative: 1;
        uint32_t high:    31;
        uint32_t low:     32;        
#endif
    } i32;
};

typedef union jmc_int64_t jmc_int64_t;

union jmc_uint64_t
{
    uint64_t u64;
    int64_t  i64;

    struct {
#if JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN
        /* Little endian. */
        uint32_t low;
        uint32_t high;
#else
        /* Big endian. */
        uint32_t high;
        uint32_t low;
#endif
    } u32;

    struct {
#if JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN
        /* Little endian. */
        uint32_t low:     32;
        uint32_t high:    31;
        uint32_t negative: 1;
#else
        /* Big endian. */
        uint32_t negative: 1;
        uint32_t high:    31;
        uint32_t low:     32;        
#endif
    } i32;
};

typedef union jmc_uint64_t jmc_uint64_t;

JMC_INLINE
unsigned int
jmc_abs(int val)
{
#if 1
    return (unsigned int)((val >= 0) ? val : -val);
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
        return (unsigned long)((val >= 0) ? val : -val);
    else
        return (unsigned long)((val >= 0) ? val : -val);
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
    return (uint64_t)((val >= 0) ? val : -val);
#else
    if (val >= 0)
        return val;
    else
        return -val;
#endif
}

JMC_INLINE
void
jmc_llabs_ex(int64_t *val)
{
#if 1
    jmc_int64_t *i64;
    i64 = (jmc_int64_t *)&val;
    if (i64->i32.negative != 0)
        *val = - *val;
#else
    if (*val < 0)
        return -*val;
#endif
}

#endif  /* !_JIMIC_UTIL_UTILS_H_ */
