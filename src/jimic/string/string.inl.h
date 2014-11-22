
#ifndef _JIMIC_STRING_STRIGN_INL_H_
#define _JIMIC_STRING_STRIGN_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_STRING_H_
#error "Don't include the internal file <jimic/string/string.inl.h> directly; include <jimic/string/string.h> instead."
#endif

#include "jimic/core/jimic_stdint.h"
#include "jimic/core/jimic_declare.h"

#include "jimic/libc/ieee754.h"

#include "jimic/string/jm_strings.h"
#include "jimic/core/jimic_assert.h"

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

/* UINT的最大值 */
#define JIMIC_UINT_MAX                  UINT_MAX

/* UINT的最大值 */
#define JIMIC_UINT_MAX64                (0x00000000FFFFFFFFULL)

#ifndef FMT_NULL_STRING
#define FMT_NULL_STRING                 "(null)"
#endif

#define JM_FIS_NORMAL                   0
#define JM_FIS_INF                      0x01
#define JM_FIS_NAN                      0x02
#define JM_FIS_NAN_OR_INF               0x04
#define JM_FIS_ZERO                     0x08
#define JM_FIS_SUBNORMAL                0x10

////////////////////////////////////////////////////////////////////////////////

/* for float */
#define JM_FLOAT_SIGN_BIT               (0x80000000UL)
#define JM_FLOAT_EXPONENT_MASK          (0x7F800000UL)
#define JM_FLOAT_MANTISSA_MASK          (0x007FFFFFUL)

////////////////////////////////////////////////////////////////////////////////

/* for double */
#define JM_DOUBLE_SIGN_BIT              (0x8000000000000000ULL)
#define JM_DOUBLE_EXPONENT_MASK         (0x7FF0000000000000ULL)
#define JM_DOUBLE_MANTISSA_MASK         (0x000FFFFFFFFFFFFFULL)

#define JM_DOUBLE_EXPONENT_BIAS_MASK    (0x3FF0000000000000ULL)

#define JM_DOUBLE_SIGN_SHIFT            63
#define JM_DOUBLE_EXPONENT_SHIFT        52
#define JM_DOUBLE_MANTISSA_SHIFT        0

/* double exponent 0 is 1023 */
#define JM_DOUBLE_EXPONENT_BIAS         ((JM_DOUBLE_EXPONENT_BIAS_MASK) >> (JM_DOUBLE_EXPONENT_SHIFT))

/* for double (high 32 bits) */
#define JM_DOUBLE_SIGN_BIT32            ((uint32_t)(JM_DOUBLE_SIGN_BIT      >> 32))
#define JM_DOUBLE_EXPONENT_MASK32       ((uint32_t)(JM_DOUBLE_EXPONENT_MASK >> 32))
#define JM_DOUBLE_MANTISSA_MASK_HIGH    ((uint32_t)(JM_DOUBLE_MANTISSA_MASK >> 32))
#define JM_DOUBLE_MANTISSA_MASK_LOW     \
                        ((uint32_t)(JM_DOUBLE_MANTISSA_MASK & 0x00000000FFFFFFFFULL))

#define JM_DOUBLE_EXPONENT_BIAS_MASK32  ((uint32_t)(JM_DOUBLE_EXPONENT_BIAS_MASK >> 32))

#define JM_DOUBLE_SIGN_SHIFT32          (63 - 32)
#define JM_DOUBLE_EXPONENT_SHIFT32      (52 - 32)
#define JM_DOUBLE_MANTISSA_SHIFT32      0

/* double exponent 0 is 1023 */
#define JM_DOUBLE_EXPONENT_BIAS_32      (JM_DOUBLE_EXPONENT_BIAS_MASK32 >> JM_DOUBLE_EXPONENT_SHIFT32)

/* double exponent maxium is  1024 = (2047 - 1023) */
#define JM_DOUBLE_EXPONENT_MAX          ((JM_DOUBLE_EXPONENT_MASK32 >> JM_DOUBLE_EXPONENT_SHIFT32) - JM_DOUBLE_EXPONENT_BIAS_32)
/* double exponent minium is -1023 = (0 - 1023) */
#define JM_DOUBLE_EXPONENT_MIN          (0 - JM_DOUBLE_EXPONENT_BIAS_32)

////////////////////////////////////////////////////////////////////////////////

typedef struct variant_t {
    union {
        int64_t         i64;
        uint64_t        u64;
        int             i32;
        unsigned char   u32;
        long            l32;
        unsigned long   ul32;
        char            c;
        unsigned char   b;
        short           s16;
        unsigned short  u16;
        double          d;
        float           f;
    };
} variant_t;

JMC_INLINE_NONSTD(int)
jmc_isnan_f(float val)
{
    uint32_t *u32;
#if defined(_DEBUG)
    uint32_t exponent;
#endif

    static_assert(sizeof(uint32_t) == sizeof(float), "jmc_isnan_f() maybe have some error!");

    if (sizeof(uint32_t) == sizeof(float)) {
        u32 = (uint32_t *)&val;
#if defined(_DEBUG)
        exponent = *u32 & JM_FLOAT_EXPONENT_MASK;
#endif
        if (((*u32 & JM_FLOAT_EXPONENT_MASK) == JM_FLOAT_EXPONENT_MASK)
            && ((*u32 & JM_FLOAT_MANTISSA_MASK) != 0))
            return 1;
        else
            return 0;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(uint32_t) == sizeof(float));
        return -1;
    }
}

JMC_INLINE_NONSTD(int)
jmc_isinf_f(float val)
{
    uint32_t *u32;
#if defined(_DEBUG)
    uint32_t exponent;
#endif

    static_assert(sizeof(uint32_t) == sizeof(float), "jmc_isinf_f() maybe have some error!");

    if (sizeof(uint32_t) == sizeof(float)) {
        u32 = (uint32_t *)&val;
#if defined(_DEBUG)
        exponent = *u32 & JM_FLOAT_EXPONENT_MASK;
#endif
        if (((*u32 & JM_FLOAT_EXPONENT_MASK) == JM_FLOAT_EXPONENT_MASK)
            && ((*u32 & JM_FLOAT_MANTISSA_MASK) == 0))
            return 1;
        else
            return 0;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(uint32_t) == sizeof(float));
        return -1;
    }
}

JMC_INLINE_NONSTD(int)
jmc_is_nan_or_inf_f(float val)
{
    uint32_t *u32;

    static_assert(sizeof(uint32_t) == sizeof(float), "jmc_is_nan_or_inf_f() maybe have some error!");

    if (sizeof(uint32_t) == sizeof(float)) {
        u32 = (uint32_t *)&val;
        if ((*u32 & JM_FLOAT_EXPONENT_MASK) == JM_FLOAT_EXPONENT_MASK)
            return 1;
        else
            return 0;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(uint32_t) == sizeof(float));
        return -1;
    }
}

JMC_INLINE_NONSTD(int)
jmc_isnan_d(double val)
{
    jmc_ieee754_double *d64;
#if defined(_DEBUG)
    uint32_t exponent;
#endif

    if (sizeof(jmc_ieee754_double) == sizeof(double)) {
        d64 = (jmc_ieee754_double *)&val;
#if defined(_DEBUG)
        exponent = d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32;
#endif
        if (((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32)
                                 == JMC_IEEE754_DOUBLE_EXPONENT_MASK32)
            && (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0))
            return 1;
        else
            return 0;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(jmc_ieee754_double) == sizeof(double));
        return -1;
    }
}

JMC_INLINE_NONSTD(int)
jmc_isinf_d(double val)
{
    jmc_ieee754_double *d64;
#if defined(_DEBUG)
    uint32_t exponent;
#endif

    if (sizeof(jmc_ieee754_double) == sizeof(double)) {
        d64 = (jmc_ieee754_double *)&val;
#if defined(_DEBUG)
        exponent = d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32;
#endif
        if (((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32)
                                 == JMC_IEEE754_DOUBLE_EXPONENT_MASK32)
            && (d64->ieee.mantissa0 == 0 || d64->ieee.mantissa1 == 0))
            return 1;
        else
            return 0;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(jmc_ieee754_double) == sizeof(double));
        return -1;
    }
}

JMC_INLINE_NONSTD(int)
jmc_is_nan_or_inf_d(double val)
{
    jmc_ieee754_double *d64;

    if (sizeof(jmc_ieee754_double) == sizeof(double)) {
        d64 = (jmc_ieee754_double *)&val;
        if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32)
                                == JMC_IEEE754_DOUBLE_EXPONENT_MASK32)
            return 1;
        else
            return 0;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(jmc_ieee754_double) == sizeof(double));
        return -1;
    }
}

//
// Zero:   exponent(指数)为0, 且mantissa(尾数)为0
//
// 下溢数: exponent(指数)为0, 且mantissa(尾数)不为0
//
// +/-INF: 无穷大或无穷小, exponent(指数)为最大值, 且mantissa(尾数)为0
//
// NaN:    Not a Number, exponent(指数)为最大值, 且mantissa(尾数)不为0
//

JMC_INLINE_NONSTD(int)
jmc_ftest(float val)
{
    uint32_t *u32;
    uint32_t exponent;

    if (sizeof(uint32_t) == sizeof(float)) {
        u32 = (uint32_t *)&val;
        exponent = *u32 & JM_FLOAT_EXPONENT_MASK;
        if (exponent == JM_FLOAT_EXPONENT_MASK) {
            if ((*u32 & JM_FLOAT_MANTISSA_MASK) != 0)
                return (JM_FIS_NAN_OR_INF | JM_FIS_NAN);
            else
                return (JM_FIS_NAN_OR_INF | JM_FIS_INF);
        }
        else if (exponent == 0) {
            if ((*u32 & JM_FLOAT_MANTISSA_MASK) == 0)
                return JM_FIS_ZERO;
            else
                return JM_FIS_SUBNORMAL;
        }
        return JM_FIS_NORMAL;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(uint32_t) == sizeof(float));
        return JM_FIS_NORMAL;
    }
}

//
// Zero:   exponent(指数)为0, 且mantissa(尾数)为0
//
// 下溢数: exponent(指数)为0, 且mantissa(尾数)不为0
//
// +/-INF: 无穷大或无穷小, exponent(指数)为最大值, 且mantissa(尾数)为0
//
// NaN:    Not a Number, exponent(指数)为最大值, 且mantissa(尾数)不为0
//
// Reference: http://zh.wikipedia.org/wiki/IEEE_754
// Reference: http://zh.wikipedia.org/wiki/%E9%9B%99%E7%B2%BE%E5%BA%A6%E6%B5%AE%E9%BB%9E%E6%95%B8
//

JMC_INLINE_NONSTD(int)
jmc_dtest(double val)
{
    jmc_ieee754_double *d64;
    uint32_t exponent;

    if (sizeof(jmc_ieee754_double) == sizeof(double)) {
        d64 = (jmc_ieee754_double *)&val;
        exponent = d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32;
        if (exponent == JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
            if (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0)
                return (JM_FIS_NAN_OR_INF | JM_FIS_NAN);
            else
                return (JM_FIS_NAN_OR_INF | JM_FIS_INF);
        }
        else if (exponent == 0) {
             if (d64->ieee.mantissa0 == 0 || d64->ieee.mantissa1 == 0)
                return JM_FIS_ZERO;
            else
                return JM_FIS_SUBNORMAL;
        }
        return JM_FIS_NORMAL;
    }
    else {
        // maybe have some error!
        jimic_assert(sizeof(jmc_ieee754_double) == sizeof(double));
        return JM_FIS_NORMAL;
    }
}

JMC_INLINE_NONSTD(int)
jmc_ptohex(char *buf, void *p)
{
    return 0;
}

JMC_INLINE_NONSTD(int)
jmc_uitohex(char *buf, uint32_t val, int isUpper)
{
    return 0;
}

JMC_INLINE_NONSTD(int)
jmc_ui64tohex(char *buf, uint64_t val)
{
    return 0;
}

JMC_INLINE_NONSTD(size_t)
jmc_strlen(char *str)
{
    return jm_strlen(str);
}

JMC_INLINE_NONSTD(size_t)
jmc_strcpy(char *dest, jm_const char *src)
{
    return 0;
}

JMC_INLINE_NONSTD(size_t)
jmc_strncpy(char *dest, size_t countOfElements, jm_const char *src, size_t count)
{
    char *end;
    if (countOfElements == (size_t)-1) {
        // do nothing !!
    }
    else
        count = JIMIC_MIN(count, countOfElements - 1);
    end = (char *)src + count;
    while (src < end) {
        *dest++ = *src++;
    }
    //*dest = '\0';
    return count;
}

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_null(char *dest, size_t countOfElements, jm_const char *src, size_t count)
{
    if (countOfElements == (size_t)-1) {
        // do nothing !!
    }
    else
        count = JIMIC_MIN(count, countOfElements - 1);
    // has including the null terminator.
    memcpy(dest, src, (count + 1) * sizeof(char));
    return count;
}

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_fast(char *dest, size_t countOfElements, jm_const char *src, size_t count)
{
    if (countOfElements == (size_t)-1) {
        // do nothing !!
    }
    else
        count = JIMIC_MIN(count, countOfElements - 1);
    memcpy(dest, src, count * sizeof(char));
    return count;
}

JMC_INLINE_NONSTD(size_t)
jmc_out_null_string(char *dest, size_t countOfElements)
{
    /* Write "(null)" if there's space.  */
    if (countOfElements == (size_t)-1) {
        *(dest + 0)  = '(';
        *(dest + 1)  = 'n';
        *(dest + 2)  = 'u';
        *(dest + 3)  = 'l';
        *(dest + 4)  = 'l';
        *(dest + 5)  = ')';
        *(dest + 6)  = '\0';
        *(dest + 7)  = '\0';
        return 6;
    }
    else {
        if (countOfElements >= (size_t)sizeof(FMT_NULL_STRING)) {
            *(dest + 0)  = '(';
            *(dest + 1)  = 'n';
            *(dest + 2)  = 'u';
            *(dest + 3)  = 'l';
            *(dest + 4)  = 'l';
            *(dest + 5)  = ')';
            *(dest + 6)  = '\0';
            return 6;
        }
        else {
            *dest = '\0';
            return 0;
        }
    }
}

#if defined(JMC_STRNCPY_EX_INLINE_DECLARE) && (JMC_STRNCPY_EX_INLINE_DECLARE != 0)

/* implement source code for .cpp or .inl.h */
#include "jimic/string/string.impl.inl>

#endif  /* JMC_STRNCPY_EX_INLINE_DECLARE */

#endif  /* !_JIMIC_STRING_STRIGN_INL_H_ */
