
#ifndef _JIMIC_LIBC_INT64_H_
#define _JIMIC_LIBC_INT64_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/jimic_stdint.h"
#include "jimic/libc/endian.h"

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
    } dwords;

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
    } s64;
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
    } dwords;

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
    } s64;
};

typedef union jmc_uint64_t jmc_uint64_t;

#endif  /* !_JIMIC_LIBC_INT64_H_ */
