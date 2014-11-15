
#ifndef _JIMIC_LIBC_IEEE754_H_
#define _JIMIC_LIBC_IEEE754_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_def.h"

#include "jimic/libc/endian.h"

/* 注意: 预编译的#else部分代表是默认定义, 因为 JIMIC_BYTE_ORDER 宏可以定义为0或未定义. */
/* 单精度浮点float默认认为是小端的. */

union jmc_ieee754_float
{
    /* This is the IEEE 754 single-precision format. */
    struct fieee_t {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
        unsigned int negative:  1;
        unsigned int exponent:  8;
        unsigned int mantissa: 23;
#else
        /* Little endian. */
        unsigned int mantissa: 23;
        unsigned int exponent:  8;
        unsigned int negative:  1;
#endif
    } ieee;

    /* This format makes it easier to see if a NaN is a signalling NaN. */
    struct fieee_NaN_t {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
        unsigned int negative:  1;
        unsigned int exponent:  8;
        unsigned int quiet_nan: 1;
        unsigned int mantissa: 22;
#else
        /* Little endian. */
        unsigned int mantissa: 22;
        unsigned int quiet_nan: 1;
        unsigned int exponent:  8;
        unsigned int negative:  1;
#endif
    } ieee_NaN;

    float f;
    unsigned int u32;

    /* Define the IEEE 754 float signbit mask position in the dwords. */
    struct fsign_t {
         unsigned int dword;    // exponent at here
    } sign;

    /* Define the IEEE 754 float exponent mask position in the dwords. */
    struct fexponent_t {
         unsigned int dword;    // exponent at here
    } exponent;
};

typedef union jmc_ieee754_float jmc_ieee754_float;

/* Added to exponent. */
#define JMC_IEEE754_FLOAT_EXPONENT_BIAS     (0x007FL)

/* The exponent mask is used to detect whether the float is a NaN or Inf? */
#define JMC_IEEE754_FLOAT_EXPONENT_MASK     (0x00FFUL)

/* The minimum value of the exponent about IEEE754 double. */
#define JMC_IEEE754_FLOAT_EXPONENT_MIN      ((int)(0 - JMC_IEEE754_FLOAT_EXPONENT_BIAS))

/* The maximum value of the exponent about IEEE754 float. */
#define JMC_IEEE754_FLOAT_EXPONENT_MAX      ((int)(JMC_IEEE754_FLOAT_EXPONENT_MASK - JMC_IEEE754_FLOAT_EXPONENT_BIAS))

/* 注意: 预编译的#else部分代表是默认定义, 因为 JIMIC_BYTE_ORDER 宏可以定义为0或未定义. */
/* 双精度浮点double默认认为是小端的. */

union jmc_ieee754_double
{
    /* This is the IEEE 754 double-precision format. */
    struct dieee_t {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_LITTLE_ENDIAN
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
#else  /* JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN */
        /* Little endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_BIG_ENDIAN
        /* Float(double) dword order is big endian. */
        unsigned int mantissa0: 20;
        unsigned int exponent:  11;
        unsigned int negative:   1;
        unsigned int mantissa1: 32;
  #else
        /* Together these comprise the mantissa. */
        unsigned int mantissa1: 32;
        unsigned int mantissa0: 20;
        unsigned int exponent:  11;
        unsigned int negative:   1;
  #endif
#endif  /* JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN */
    } ieee;

    /* This format makes it easier to see if a NaN is a signalling NaN. */
    struct dieee_NaN_t {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_LITTLE_ENDIAN
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
#else  /* JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN */
        /* Little endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_BIG_ENDIAN
        /* Float(double) dword order is big endian. */
        unsigned int mantissa0: 19;
        unsigned int quiet_nan:  1;
        unsigned int exponent:  11;
        unsigned int negative:   1;
        unsigned int mantissa1: 32;
  #else
        /* Together these comprise the mantissa.  */
        unsigned int mantissa1: 32;
        unsigned int mantissa0: 19;
        unsigned int quiet_nan:  1;
        unsigned int exponent:  11;
        unsigned int negative:   1;
  #endif
#endif  /* JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN */
    } ieee_NaN;

    double d;
    uint64_t u64;

    struct {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
        unsigned int high;
        unsigned int low;
#else
        /* Little endian. */
        unsigned int low;
        unsigned int high;
#endif
    } u32;

    /* 这里的high, low跟 uint64_t 的high, low定义不一样,
       high 永远代表符号位 negative 所在的 dword 位置. */
    struct {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_LITTLE_ENDIAN
        /* Float(double) dword order is little endian. */
        unsigned int low;
        unsigned int high;      // negative at here
  #else
        /* Float(double) dword order is big endian too. */
        unsigned int high;      // negative at here
        unsigned int low;
  #endif
#else
        /* Little endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_BIG_ENDIAN
        /* Float(double) dword order is big endian. */
        unsigned int high;      // negative at here
        unsigned int low;
  #else
        /* Float(double) dword order is little endian too. */
        unsigned int low;
        unsigned int high;      // negative at here
  #endif
#endif
    } d64;

    /* Define the IEEE 754 float signbit mask position in the dwords. */
    struct dsign_t {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_LITTLE_ENDIAN
        /* Float(double) dword order is little endian. */
        unsigned int reserve;
        unsigned int dword;     // signbit at here
  #else
         /* Float(double) dword order is big endian too. */
        unsigned int dword;     // signbit at here
        unsigned int reserve;
  #endif
#else
        /* Little endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_BIG_ENDIAN
        /* Float(double) dword order is big endian. */
        unsigned int dword;     // signbit at here
        unsigned int reserve;
  #else
        /* Float(double) dword order is little endian too. */
        unsigned int reserve;
        unsigned int dword;     // signbit at here
  #endif
#endif
    } sign;

    /* Define the IEEE 754 double exponent mask position in the dwords. */
    struct dexponent_t {
#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
        /* Big endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_LITTLE_ENDIAN
        /* Float(double) dword order is little endian. */
        unsigned int reserve;
        unsigned int dword;     // exponent at here
  #else
         /* Float(double) dword order is big endian too. */
        unsigned int dword;     // exponent at here
        unsigned int reserve;
  #endif
#else
        /* Little endian. */
  #if JIMIC_FLOAT_DWORD_ORDER == JIMIC_BIG_ENDIAN
        /* Float(double) dword order is big endian. */
        unsigned int dword;     // exponent at here
        unsigned int reserve;
  #else
        /* Float(double) dword order is little endian too. */
        unsigned int reserve;
        unsigned int dword;     // exponent at here
  #endif
#endif
    } exponent;
};

typedef union jmc_ieee754_double jmc_ieee754_double;

/* Added to exponent. */
#define JMC_IEEE754_DOUBLE_EXPONENT_BIAS    (0x03FFL)

/* The exponent mask is used to detect whether the double is a NaN or Inf? */
#define JMC_IEEE754_DOUBLE_EXPONENT_MASK    (0x07FFUL)

/* The minimum value of the exponent about IEEE754 double. */
#define JMC_IEEE754_DOUBLE_EXPONENT_MIN     ((int)(0 - JMC_IEEE754_DOUBLE_EXPONENT_BIAS))

/* The maximum value of the exponent about IEEE754 double. */
#define JMC_IEEE754_DOUBLE_EXPONENT_MAX     ((int)(JMC_IEEE754_DOUBLE_EXPONENT_MASK - JMC_IEEE754_DOUBLE_EXPONENT_BIAS))

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1700)
static const jmc_ieee754_float s_f32_exponent_mask = {
    // .ieee
    {
        0,                                  // .mantissa
        JMC_IEEE754_FLOAT_EXPONENT_MASK,    // .exponent
        0                                   // .negative
    }
};
#else  /* __GNUC__ || _MSC_VER >= 1700 */
static const jmc_ieee754_float s_f32_exponent_mask = {
    // .ieee
    .ieee.mantissa = 0,
    .ieee.exponent = JMC_IEEE754_FLOAT_EXPONENT_MASK,
    .ieee.negative = 0
};
#endif  /* defined(_MSC_VER) && (_MSC_VER < 1700) */

//static const unsigned int s_float_exponent_mask = s_f32_exponent_mask.ieee.exponent;
//static const unsigned int s_float_exponent_mask = (0xFFFFFFFFUL & s_f32_exponent_mask.ieee.exponent);

#if defined(_MSC_VER) && (_MSC_VER < 1700)
static const jmc_ieee754_double s_d64_exponent_mask = {
    // .ieee
    {
        0,                                  // .mantissa0
        0,                                  // .mantissa1
        JMC_IEEE754_DOUBLE_EXPONENT_MASK,   // .exponent
        0                                   // .negative
    }
};
#else  /* __GNUC__ || _MSC_VER >= 1700 */
static const jmc_ieee754_double s_d64_exponent_mask = {
    // .ieee
    .ieee.mantissa0 = 0,
    .ieee.mantissa1 = 0,
    .ieee.exponent  = JMC_IEEE754_DOUBLE_EXPONENT_MASK,
    .ieee.negative  = 0
};
#endif  /* defined(_MSC_VER) && (_MSC_VER < 1700) */

//static const unsigned int s_double_exponent_mask = s_d64_exponent_mask.ieee.exponent;
//static const unsigned int s_double_exponent_mask = (0xFFFFFFFFFFFFFFFFULL & s_d64_exponent_mask.ieee.exponent);

#define JMC_IEEE754_FLOAT_EXPONENT_MASK32   (s_f32_exponent_mask.exponent.dword)
#define JMC_IEEE754_DOUBLE_EXPONENT_MASK32  (s_d64_exponent_mask.exponent.dword)

#define kFloatExponentMask32                JMC_IEEE754_FLOAT_EXPONENT_MASK32
#define kDoubleExponentMask32               JMC_IEEE754_DOUBLE_EXPONENT_MASK32

#if defined(_MSC_VER) && (_MSC_VER < 1700)
static const jmc_ieee754_float s_f32_sign_mask = {
    // .ieee
    {
        0,      // .mantissa
        0,      // .exponent
        1       // .negative
    }
};
#else  /* __GNUC__ || _MSC_VER >= 1700 */
static const jmc_ieee754_float s_f32_sign_mask = {
    // .ieee
    .ieee.mantissa = 0,
    .ieee.exponent = 0,
    .ieee.negative = 1
};
#endif  /* defined(_MSC_VER) && (_MSC_VER < 1700) */

#if defined(_MSC_VER) && (_MSC_VER < 1700)
static const jmc_ieee754_double s_d64_sign_mask = {
    // .ieee
    {
        0,      // .mantissa0
        0,      // .mantissa1
        0,      // .exponent
        1       // .negative
    }
};
#else  /* __GNUC__ || _MSC_VER >= 1700 */
static const jmc_ieee754_double s_d64_sign_mask = {
    // .ieee
    .ieee.mantissa0 = 0,
    .ieee.mantissa1 = 0,
    .ieee.exponent  = 0,
    .ieee.negative  = 1
};
#endif  /* defined(_MSC_VER) && (_MSC_VER < 1700) */

#define JMC_IEEE754_FLOAT_SIGN_MASK32       (s_f32_sign_mask.sign.dword)
#define JMC_IEEE754_DOUBLE_SIGN_MASK32      (s_d64_sign_mask.sign.dword)

#define kFloatSignBitMask32                 JMC_IEEE754_FLOAT_SIGN_MASK32
#define kDoubleSignBitMask32                JMC_IEEE754_DOUBLE_SIGN_MASK32

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_LIBC_IEEE754_H_ */
