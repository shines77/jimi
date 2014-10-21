
#ifndef _JIMIC_LIBC_IEEE754_H_
#define _JIMIC_LIBC_IEEE754_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_platform_def.h>
#include <jimic/core/jimic_def.h>

#include <jimic/libc/endian.h>

union jmc_ieee754_float
{
    float f;

    /* This is the IEEE 754 single-precision format. */
    struct {
#if JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN
        /* Little endian. */
        unsigned int mantissa: 23;
        unsigned int exponent:  8;
        unsigned int negative:  1;
#elif JMC_BYTE_ORDER == JMC_BIG_ENDIAN
        /* Big endian. */
        unsigned int negative:  1;
        unsigned int exponent:  8;
        unsigned int mantissa: 23;
#endif
    } ieee;

    /* This format makes it easier to see if a NaN is a signalling NaN. */
    struct {
#if JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN
        /* Little endian. */
        unsigned int mantissa: 22;
        unsigned int quiet_nan: 1;
        unsigned int exponent:  8;
        unsigned int negative:  1;
#elif JMC_BYTE_ORDER == JMC_BIG_ENDIAN
        /* Big endian. */
        unsigned int negative:  1;
        unsigned int exponent:  8;
        unsigned int quiet_nan: 1;
        unsigned int mantissa: 22;
#endif  /* JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN */
    } ieee_NaN;
};

typedef union jmc_ieee754_float jmc_ieee754_float;

/* Added to exponent. */
#define JMC_IEEE754_FLOAT_BIAS     0x7F

union jmc_ieee754_double
{
    double d;

    /* This is the IEEE 754 double-precision format. */
    struct {
#if JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN
        /* Little endian. */
  #if JMC_FLOAT_DWORD_ORDER == JMC_LITTLE_ENDIAN
        /* Together these comprise the mantissa. */
        unsigned int mantissa1: 32;
        unsigned int mantissa0: 20;
        unsigned int exponent:  11;
        unsigned int negative:   1;
  #else
        /* Float(double) dword order is big endian. */
        unsigned int mantissa0: 20;
        unsigned int exponent:  11;
        unsigned int negative:   1;
        unsigned int mantissa1: 32;
  #endif
#else  /* JMC_BYTE_ORDER == JMC_BIG_ENDIAN */
        /* Big endian. */
  #if JMC_FLOAT_DWORD_ORDER == JMC_LITTLE_ENDIAN
        /* Float(double) dword order is little endian. */
        unsigned int mantissa1: 32;
        unsigned int negative:   1;
        unsigned int exponent:  11;
        unsigned int mantissa0: 20;
  #else
        /* Float(double) dword order is big endian too. */
        unsigned int negative:   1;
        unsigned int exponent:  11;
        /* Together these comprise the mantissa. */
        unsigned int mantissa0: 20;
        unsigned int mantissa1: 32;
  #endif
#endif  /* JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN */
    } ieee;

    /* This format makes it easier to see if a NaN is a signalling NaN. */
    struct {
#if JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN
        /* Little endian. */
  #if JMC_FLOAT_DWORD_ORDER == JMC_LITTLE_ENDIAN
        /* Together these comprise the mantissa.  */
        unsigned int mantissa1: 32;
        unsigned int mantissa0: 19;
        unsigned int quiet_nan:  1;
        unsigned int exponent:  11;
        unsigned int negative:   1;
  #else
        /* Float(double) dword order is big endian. */
        unsigned int mantissa0: 19;
        unsigned int quiet_nan:  1;
        unsigned int exponent:  11;
        unsigned int negative:   1;
        unsigned int mantissa1: 32;
  #endif
#else  /* JMC_BYTE_ORDER == JMC_BIG_ENDIAN */
        /* Big endian. */
  #if JMC_FLOAT_DWORD_ORDER == JMC_LITTLE_ENDIAN
        /* Float(double) dword order is little endian. */
        unsigned int mantissa1: 32;
        unsigned int negative:   1;
        unsigned int exponent:  11;
        unsigned int quiet_nan:  1;
        unsigned int mantissa0: 19;
  #else
        /* Float(double) dword order is big endian too. */
        unsigned int negative:   1;
        unsigned int exponent:  11;
        unsigned int quiet_nan:  1;
        /* Together these comprise the mantissa.  */
        unsigned int mantissa0: 19;
        unsigned int mantissa1: 32;
  #endif
#endif  /* JMC_BYTE_ORDER == JMC_LITTLE_ENDIAN */
    } ieee_NaN;
};

typedef union jmc_ieee754_double jmc_ieee754_double;

/* Added to exponent. */
#define JMC_IEEE754_DOUBLE_BIAS     0x3FF

#ifdef __cplusplus
extern "C" {
#endif

//

#ifdef __cplusplus
}
#endif

#include <jimic/stdio/sprintf.inl.h>

#endif  /* !_JIMIC_LIBC_IEEE754_H_ */
