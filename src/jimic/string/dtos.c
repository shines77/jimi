
#include "jimic/string/dtos.h"

#include "jimic/libc/ieee754.h"
#include "jimic/stdio/sprintf_def.h"
#include "jimic/string/itoa.h"
#include "jimic/string/string.h"
#include "jimic/util/utils.h"

#include "jimic/basic/assert.h"

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for define UINT_MAX

static const double pow10_remain_d16[] = {
    1.0E+0, 1.0E+4, 1.0E+9, 1.0E+14
};

static const double pow10_remain_d8[] = {
    1.0E+0,  1.0E+2,  1.0E+4,  1.0E+7,
    1.0E+9,  1.0E+12, 1.0E+14, 1.0E+16,
};

static const double pow10_base_d64[] = {
    1.0E+19,  1.0E+38,  1.0E+57,  1.0E+76,
    1.0E+95,  1.0E+114, 1.0E+133, 1.0E+152,
    1.0E+171, 1.0E+190, 1.0E+209, 1.0E+228,
    1.0E+247, 1.0E+266, 1.0E+285, 1.0E+304,
    1.0E+308,
    // last 3 items is fill for address aligned to 64 bytes only
    1.0E+308, 1.0E+308, 1.0E+308
};

static const uint32_t pow10_scales_int32[] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
    // Fill for address aligned to 64 bytes only
    1, 1, 1, 1, 1, 1
};

static const uint64_t pow10_scales_int64[] = {
    10000000000ULL,         100000000000ULL,
    1000000000000ULL,       10000000000000ULL,
    100000000000000ULL,     1000000000000000ULL,
    10000000000000000ULL,   100000000000000000ULL,
    1000000000000000000ULL, 10000000000000000000ULL,
    // Fill for address aligned to 64 bytes only
    1ULL, 1ULL, 1ULL, 1ULL, 1ULL, 1ULL
};

JMC_DECLARE_NONSTD(int)
jmc_get_fexponent(float * JMC_RESTRICT pval)
{
    jmc_ieee754_float *f32;
    int exponent;
    jimic_assert(pval != NULL);
    f32 = (jmc_ieee754_float *)pval;
    exponent = f32->ieee.exponent - JMC_IEEE754_FLOAT_EXPONENT_BIAS;
    jimic_assert(exponent >= JMC_IEEE754_FLOAT_EXPONENT_MIN
                 && exponent <= JMC_IEEE754_FLOAT_EXPONENT_MAX);
    return exponent;
}

JMC_DECLARE_NONSTD(int)
jmc_get_dexponent(double * JMC_RESTRICT pval)
{
    jmc_ieee754_double *d64;
    int exponent;
    jimic_assert(pval != NULL);
    d64 = (jmc_ieee754_double *)pval;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;
    jimic_assert(exponent >= JMC_IEEE754_DOUBLE_EXPONENT_MIN
                 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX);
    return exponent;
}

JMC_DECLARE_NONSTD(int)
jmc_adjustf_to_bin32(float * JMC_RESTRICT pval, int exponent)
{
    // TODO:
    return 0;
}

JMC_DECLARE_NONSTD(int)
jmc_adjustd_to_bin64(double * JMC_RESTRICT pval, int exponent)
{
    int exp10;
    int base_index;
    int remain_index;
    double pow10_base;

    jimic_assert(pval != NULL);
    jimic_assert(exponent >= JMC_IEEE754_DOUBLE_EXPONENT_MIN
                 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX);

    // zoom coeff must satisfy: zoom < 4,194,304 ( 2^32 / 1024), zoom = 2,525,222

    //if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
    if (exponent >= 0) {
        // if exponent is positive
        // exp10 = exponent * 2525222;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64, exp10 = exp10 >> (17 + 6), 131072 = 2^17;
        exp10 = (int)((unsigned int)exp10 >> 23);

        base_index = (exponent >> 6);
        // if exponent >= 0 and exponent < 64, needn't to adjust
        if (base_index > 0) {
            jimic_assert(base_index > 0 && base_index < 20);
            pow10_base = pow10_base_d64[base_index - 1];

            remain_index = (exponent & 63) >> 3;
            jimic_assert(remain_index >= 0 && remain_index < 8);

            pow10_base *= pow10_remain_d8[remain_index];
            *pval = (*pval) / pow10_base;
        }
        return exp10;
    }
    else {
        // if exponent is negative
        // exp10 = -exponent * 2525222;
        exponent = -exponent;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64, exp10 = exp10 >> (17 + 6), 131072 = 2^17;
        exp10 = (int)((unsigned int)exp10 >> 23);

        base_index = ((exponent + 4) >> 6);
        jimic_assert(base_index >= 0 && base_index < 20);
        pow10_base = pow10_base_d64[base_index];

        remain_index = (exponent & 63) >> 3;
        jimic_assert(remain_index >= 0 && remain_index < 8);

        pow10_base *= pow10_remain_d8[remain_index];
        *pval = (*pval) * pow10_base;
        return -exp10;
    }
}

JMC_DECLARE_NONSTD(int)
jmc_ftos(char * JMC_RESTRICT buf, float val, int field_width, int precision)
{
    // TODO:
    return 0;
}

JMC_DECLARE_NONSTD(int)
jmc_ftos_ex(char * JMC_RESTRICT buf, size_t count, float val, unsigned int flag,
            unsigned int fill, int field_width, int precision)
{
    // TODO:
    return 0;
}

JMC_DECLARE_NONSTD(int)
jmc_dtos(char * JMC_RESTRICT buf, double val, int field_width, int precision)
{
    int len;
    int64_t i64;
    uint32_t scale32;
    uint64_t scale, frac;
    jmc_ieee754_double *d64;
    int exponent;
    unsigned int n;
    int num_width;
    int exp10;

    d64 = (jmc_ieee754_double *)&val;

    // Check the exponent of IEEE754 double, exponent is the maxium value? It is a NaN or INF or Normal type?
    if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32) !=
                               JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
        exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;
        exp10 = jmc_adjustd_to_bin64(&val, exponent);

        if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            scale = 1000000;
            num_width = field_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else if (precision > 0) {
#if 0
            if (precision <= 9) {
                jimic_assert(precision > 0 && precision <= 9);
                scale = pow10_scales_int32[precision];
            }
            else {
                jimic_assert(precision > 9 && precision <= 20);
                scale = pow10_scales_int64[precision - 11];
            }
#elif 1
            if (precision <= 9) {
                jimic_assert(precision > 0 && precision <= 9);
                scale32 = 10;
                for (n = precision - 1; n > 0; --n)
                    scale32 *= 10;
                scale = scale32;
            }
            else {
                jimic_assert(precision > 9);
                if (precision <= FMT_MAX_DOUBLE_PRECISION) {
                    scale = 10000000000ULL;
                    for (n = precision - 10; n > 0; --n)
                        scale *= 10;
                }
                else {
                    precision = FMT_MAX_DOUBLE_PRECISION;
                    scale = 100000000000000000ULL;
                }
                jimic_assert(precision <= FMT_MAX_DOUBLE_PRECISION);
            }
#else
            jimic_assert(precision >= 0 && precision <= 20);
            scale = 1;
            for (n = precision; n > 0; --n)
                scale *= 10;
#endif
            num_width = field_width - precision - 1;
        }
        else {
            scale = 1ULL;
            num_width = field_width;
        }

        // Get the fractional part of double
        i64 = (int64_t)val;
        //if (val >= 0.0) {
        if (i64 >= 0) {
            frac = (uint64_t)((val - (double)i64) * scale + 0.5);
            if (frac == scale) {
                i64++;
                frac = 0;
            }
        }
        else {
            frac = (uint64_t)(((double)i64 - val) * scale + 0.5);
            if (frac == scale) {
                i64--;
                frac = 0;
            }
        }
#if 0
        if (((precision != 0) && ((int)field_width <= (precision + 1 + 1)))
            || ((precision == 0) && (field_width <= (0 + 1)))) {
#else
        if (num_width <= 1) {
#endif
            len = jmc_i64toa_r10(buf, i64);
            buf += len;
        }
        else {
#if 1
            // For the integer part of double
            len = jmc_i64toa_r10_integer(buf, i64, num_width);
#else
            len = jmc_i64toa_r10_ex(buf, -1, i64, FMT_ALIGN_RIGHT, ' ', num_width, num_width);
#endif
            buf += len;
        }

        if (precision > 0) {
            // Output the decimal
            *buf++ = '.';
#if 1
            // For the fractional part of double, fractional always is positive.
            len += jmc_u64toa_r10_frac(buf, frac, precision) + 1;
#else
            len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT, '0', precision, 0) + 1;
#endif
        }

        return len;
    }
    // If mantissa0 and mantissa1 is not equal zero, then it's a NaN double (not a number).
    else if (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0) {
        // is NaN, not a number
        len = JIMIC_MAX(field_width, 3);
        field_width -= 3;
        while (field_width > 0) {
            *buf++ = ' ';
            field_width--;
        }
        *buf        = 'N';
        *(buf + 1)  = 'a';
        *(buf + 2)  = 'N';
        *(buf + 3)  = '\0';
        return len;
    }
    // Otherwise, if mantissa0 and mantissa1 is equal zero, then it's a +Inf or -Inf double. (+/- INFINITY)
    else {
        // is +INF or -INF
        //if (d64->ieee.negative == 0) {
        if ((d64->sign.dword & JMC_IEEE754_DOUBLE_SIGN_MASK32) == 0) {
            len = JIMIC_MAX(field_width, 3);
            field_width -= 3;
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            // '+', 0x2B
            *buf        = 'I';
            *(buf + 1)  = 'n';
            *(buf + 2)  = 'f';
            *(buf + 3)  = '\0';
            return len;
        }
        else {
            len = JIMIC_MAX(field_width, 4);
            field_width -= 4;
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            // '-', 0x2D
//          *buf        = '+' + ((f64->high & JM_DOUBLE_SIGN_BIT32) >> 30);
            *buf        = '-';
            *(buf + 1)  = 'I';
            *(buf + 2)  = 'n';
            *(buf + 3)  = 'f';
            *(buf + 4)  = '\0';
            return len;
        }
    }
}

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex(char * JMC_RESTRICT buf, size_t count, double val, unsigned int flag,
            unsigned int fill, int field_width, int precision)
{
    int len;
    int64_t i64;
    uint64_t val64;
    uint32_t val32;
    unsigned int num_digits;
    uint32_t scale32;
    uint64_t scale, frac;
    register jmc_ieee754_double *d64;
    unsigned int n;
    int exponent;
    int num_width;
    int exp10;
    int frac_prec;
    int tail_zeros;

    d64 = (jmc_ieee754_double *)&val;

    // Check the exponent of IEEE754 double, exponent is the maxium value? It is a NaN or INF or Normal type?
    if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32) !=
                               JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
        // get the exponent value
        exponent = (int)(d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS);
        // if exponent belong range [0, 64), needn't to adjust
        exp10 = 0;
        if (exponent < 0 || exponent >= 64) {
            // adjust double value to range like (1 ~ 2^64) * 10^N
            exp10 = jmc_adjustd_to_bin64(&val, exponent);
        }

        i64 = (int64_t)val;
        d64 = (jmc_ieee754_double *)&i64;
        // calc the digits of the double integer part
        //if (i64 <= (uint64_t)JIMIC_UINT_MAX64) {
        if (d64->u32.high == 0) {
            val32 = (uint32_t)(d64->u32.low);
            num_digits = 0;
            do {
                num_digits++;
                val32 /= 10;
            } while (val32 != 0);
        }
        else {
#if 1
            if (i64 >= 0)
                val64 = i64;
            else
                val64 = -i64;
#else
            val64 = jmc_llabs(i64);
#endif
            num_digits = 0;
            do {
                num_digits++;
                val64 /= 10;
            } while (val64 != 0);
        }

#if 1
        // 这里把小数精度为0也认为是默认精度: FMT_DEFAULT_DOUBLE_PRECISION(6)
        // 如果跟标准的sprintf定义不一致, 则需要修改
        if (precision <= 0)
            precision = FMT_DEFAULT_DOUBLE_PRECISION;

        // 整数部分的字符宽度
        num_width = field_width - precision - 1;
#elif 0
        if (precision > 0) {
            jimic_assert(precision > 0);
            num_width = field_width - precision - 1;
        }
        else if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            num_width = field_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else {
            jimic_assert(precision == 0);
            num_width = field_width;
        }
#else
        if (precision >= 0) {
            jimic_assert(precision >= 0);
            num_width = field_width - precision;
            if (precision > 0)
                num_width--;
        }
        else {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            num_width = field_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
#endif

        if (i64 != 0) {
            if (precision <= (FMT_MAX_DOUBLE_PRECISION - (int)num_digits))
                frac_prec = precision;
            else
                frac_prec = FMT_MAX_DOUBLE_PRECISION - (int)num_digits;
        }
        else {
            if (precision <= FMT_MAX_DOUBLE_PRECISION)
                frac_prec = precision;
            else
                frac_prec = FMT_MAX_DOUBLE_PRECISION;
        }

#if 0
        if (frac_prec > 0) {
            jimic_assert(frac_prec > 0);
            if (frac_prec <= 9) {
                jimic_assert(frac_prec > 0 && frac_prec <= 9);
                scale = pow10_scales_int32[frac_prec];
            }
            else {
                jimic_assert(frac_prec > 9 && frac_prec <= FMT_MAX_DOUBLE_PRECISION);
                scale = pow10_scales_int64[frac_prec - 10];
            }
        }
        else {
            jimic_assert(frac_prec <= 0);
            scale = 1;
        }
#else
        if (frac_prec > 0) {
            jimic_assert(frac_prec > 0);
            if (frac_prec <= 9) {
                jimic_assert(frac_prec > 0 && frac_prec <= 9);
                scale32 = 10UL;
                for (n = frac_prec - 1; n > 0; --n)
                    scale32 *= 10UL;
                scale = scale32;
            }
            else {
                jimic_assert(frac_prec > 9 && frac_prec <= FMT_MAX_DOUBLE_PRECISION);
                if (frac_prec < FMT_MAX_DOUBLE_PRECISION) {
                    scale = 10000000000ULL;
                    for (n = frac_prec - 10; n > 0; --n)
                        scale *= 10;
                }
                else {
                    scale = 100000000000000000ULL;
                }
            }
        }
        else {
            jimic_assert(frac_prec <= 0);
            scale = 1ULL;
        }
#endif

        if (i64 >= 0) {
            frac = (uint64_t)((val - (double)i64) * scale + 0.5);
            if (frac == scale) {
                i64++;
                frac = 0;
            }
        }
        else {
            frac = (uint64_t)(((double)i64 - val) * scale + 0.5);
            if (frac == scale) {
                i64--;
                frac = 0;
            }
        }

        // for integer part of double
        if (((flag & FMT_ALIGN_LEFT) != 0) || (num_width < 0))
            num_width = 0;

        len = jmc_i64toa_r10_ex(buf, -1, i64, flag, fill, num_width, num_width);
        field_width -= len;
        buf += len;

        //if (precision > 0) {
            // output decimal
            *buf++ = '.';

            // for fractional part of double
#if 0
            field_width--;
            if (precision < field_width) {
                num_width = precision;
                precision = field_width;
                field_width = num_width;
            }
            len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                         '0', precision, field_width - 1) + 1;
#else
            field_width--;
            if (precision >= field_width) {
                tail_zeros = precision - frac_prec;
                if (tail_zeros <= 0) {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', precision, field_width) + 1;
                }
                else {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', frac_prec, precision) + tail_zeros + 1;
                    buf += frac_prec;
                    // fill tail zeros
                    do {
                        *buf++ = '0';
                        tail_zeros--;
                    } while (tail_zeros > 0);
                    *buf = '\0';
                }
            }
            else {
                len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                             '0', field_width, precision) + 1;
            }
#endif
        //}
        return len;
    }
    // If mantissa0 and mantissa1 is not equal zero, then it's a NaN double (not a number).
    else if (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0) {
        // is NaN, not a number
        len = JIMIC_MAX(field_width, 3);
        field_width -= 3;
        if ((flag & FMT_ALIGN_LEFT) != 0) {
            // align to left
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = ' ';
            buf += 3;
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            *buf = '\0';
            return len;
        }
        else {
            // align to right
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = '\0';
            return len;
        }
    }
    // Otherwise, if mantissa0 and mantissa1 is equal zero, then it's a +Inf or -Inf double. (+/- INFINITY)
    else {
        // is +INF or -INF
        //if (d64->ieee.negative == 0) {
        if ((d64->sign.dword & JMC_IEEE754_DOUBLE_SIGN_MASK32) == 0) {
            // is a +INF
            if ((flag & FMT_PLUS_FLAG) == 0) {
                len = JIMIC_MAX(field_width, 3);
                field_width -= 3;
                if ((flag & FMT_ALIGN_LEFT) != 0) {
                    // align to left
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = ' ';
                    buf += 3;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    // align to right
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = '\0';
                    return len;
                }
            }
            else {
                len = JIMIC_MAX(field_width, 4);
                field_width -= 4;
                if ((flag & FMT_ALIGN_LEFT) != 0) {
                    // align to left
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    buf += 4;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    // align to right
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    *(buf + 4)  = '\0';
                    return len;
                }
            }

        }
        else {
            // is a -INF
            len = JIMIC_MAX(field_width, 4);
            field_width -= 4;
            if ((flag & FMT_ALIGN_LEFT) != 0) {
                // align to left
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                buf += 4;
                while (field_width > 0) {
                    *buf++ = ' ';
                    field_width--;
                }
                *buf = '\0';
                return len;
            }
            else {
                // align to right
                while (field_width > 0) {
                    *buf++ = ' ';
                    field_width--;
                }
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                *(buf + 4)  = '\0';
                return len;
            }
        }
    }
}

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex_old(char * JMC_RESTRICT buf, size_t count, double val, unsigned int flag,
                unsigned int fill, int field_width, int precision)
{
    int len;
    int64_t i64;
    uint64_t val64;
    uint32_t val32;
    unsigned int num_digits;
    uint32_t scale32;
    uint64_t scale, frac;
    register jmc_ieee754_double *u64;
    register jmc_ieee754_double *d64;
    unsigned int n;
    int exponent;
    int num_width;
    int exp10;
    int frac_prec;
    int tail_zeros;

    d64 = (jmc_ieee754_double *)&val;

    // is NaN or INF ? (exponent is maxium ?)
    //if ((f64->high & JM_DOUBLE_EXPONENT_MASK32) != JM_DOUBLE_EXPONENT_MASK32) {
    if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32) !=
                               JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
        // get the exponent value
        exponent = (int)(d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS);
        // if exponent belong range [0, 64), needn't to adjust
        exp10 = 0;
        if (exponent >= 0 && exponent < 64) {
            // adjust double value to range like (1 ~ 2^64) * 10^N
            exp10 = jmc_adjustd_to_bin64(&val, exponent);
        }

        i64 = (int64_t)val;
        u64 = (jmc_ieee754_double *)&i64;
        // calc the digitss of the double integer part
        //if (i64 <= (uint64_t)JIMIC_UINT_MAX64) {
        if (u64->u32.high == 0) {
            val32 = (uint32_t)(u64->u32.low);
            num_digits = 0;
            do {
                num_digits++;
                val32 /= 10;
            } while (val32 != 0);
        }
        else {
            if (i64 >= 0)
                val64 = i64;
            else
                val64 = -i64;
            num_digits = 0;
            do {
                num_digits++;
                val64 /= 10;
            } while (val64 != 0);
        }

#if 1
        if (precision > 0) {
            jimic_assert(precision > 0);
            num_width = field_width - precision - 1;
        }
        else if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            num_width = field_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else {
            jimic_assert(precision == 0);
            num_width = field_width;
        }
#else
        if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            num_width = field_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else {
            jimic_assert(precision >= 0);
            num_width = field_width - precision;
            if (precision > 0)
                num_width--;
        }
#endif

        if (i64 != 0) {
            if ((precision + num_digits) <= FMT_MAX_DOUBLE_PRECISION)
                frac_prec = precision;
            else
                frac_prec = FMT_MAX_DOUBLE_PRECISION - num_digits;
        }
        else {
            if (precision <= FMT_MAX_DOUBLE_PRECISION)
                frac_prec = precision;
            else
                frac_prec = FMT_MAX_DOUBLE_PRECISION;
        }

#if 0
        if (frac_prec > 0) {
            jimic_assert(frac_prec > 0);
            if (frac_prec <= 9) {
                jimic_assert(frac_prec > 0 && frac_prec <= 9);
                scale = pow10_scales_int32[frac_prec];
            }
            else {
                jimic_assert(frac_prec > 9 && precision <= FMT_MAX_DOUBLE_PRECISION);
                scale = pow10_scales_int64[frac_prec - 10];
            }
        }
        else {
            jimic_assert(frac_prec <= 0);
            scale = 1;
        }
#else
        if (frac_prec > 0) {
            jimic_assert(frac_prec > 0);
            if (frac_prec <= 9) {
                jimic_assert(frac_prec > 0 && frac_prec <= 9);
                scale32 = 10;
                for (n = frac_prec - 1; n > 0; --n)
                    scale32 *= 10;
                scale = scale32;
            }
            else {
                jimic_assert(frac_prec > 9 && frac_prec <= FMT_MAX_DOUBLE_PRECISION);
                if (frac_prec < FMT_MAX_DOUBLE_PRECISION) {
                    scale = 10000000000ULL;
                    for (n = frac_prec - 10; n > 0; --n)
                        scale *= 10;
                }
                else {
                    scale = 100000000000000000ULL;
                }
            }
        }
        else {
            jimic_assert(frac_prec <= 0);
            scale = 1ULL;
        }
#endif

        if (i64 >= 0) {
            frac = (uint64_t)((val - (double)i64) * scale + 0.5);
            if (frac == scale) {
                i64++;
                frac = 0;
            }
        }
        else {
            frac = (uint64_t)(((double)i64 - val) * scale + 0.5);
            if (frac == scale) {
                i64--;
                frac = 0;
            }
        }

        // for integer part of double
        if (((flag & FMT_ALIGN_LEFT) != 0) || (num_width < 0))
            num_width = 0;

        len = jmc_i64toa_r10_ex(buf, -1, i64, flag, fill, num_width, num_width);
        field_width -= len;
        buf += len;

        if (precision > 0) {
            // output decimal
            *buf++ = '.';

            // for fractional part of double
#if 0
            field_width--;
            if (precision < field_width) {
                num_width = precision;
                precision = field_width;
                field_width = num_width;
            }
            len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                         '0', precision, field_width - 1) + 1;
#else
            field_width--;
            if (precision >= field_width) {
                tail_zeros = precision - frac_prec;
                if (tail_zeros <= 0) {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', precision, field_width) + 1;
                }
                else {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', frac_prec, precision) + tail_zeros + 1;
                    buf += frac_prec;
                    // fill tail zeros
                    do {
                        *buf++ = '0';
                        tail_zeros--;
                    } while (tail_zeros > 0);
                    *buf = '\0';
                }
            }
            else {
                len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                             '0', field_width, precision) + 1;
            }
#endif
        }
        return len;
    }
    // if mantissa0 and mantissa1 is not equal zero, then it's a NaN (not a number)
    else if (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0) {
        // is NaN, not a number
        len = JIMIC_MAX(field_width, 3);
        field_width -= 3;
        if ((flag & FMT_ALIGN_LEFT) != 0) {
            // align to left
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = ' ';
            buf += 3;
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            *buf = '\0';
            return len;
        }
        else {
            // align to right
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = '\0';
            return len;
        }
    }
    // if mantissa0 and mantissa1 is equal zero, then it's a Inf (-/+ INFINITY)
    else {
        // is +INF or -INF
        //if (d64->ieee.negative == 0) {
        if ((d64->sign.dword & JMC_IEEE754_DOUBLE_SIGN_MASK32) == 0) {
            // is a +INF
            if ((flag & FMT_PLUS_FLAG) == 0) {
                len = JIMIC_MAX(field_width, 3);
                field_width -= 3;
                if ((flag & FMT_ALIGN_LEFT) != 0) {
                    // align to left
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = ' ';
                    buf += 3;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    // align to right
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = '\0';
                    return len;
                }
            }
            else {
                len = JIMIC_MAX(field_width, 4);
                field_width -= 4;
                if ((flag & FMT_ALIGN_LEFT) != 0) {
                    // align to left
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    buf += 4;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    // align to right
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    *(buf + 4)  = '\0';
                    return len;
                }
            }

        }
        else {
            // is a -INF
            len = JIMIC_MAX(field_width, 4);
            field_width -= 4;
            if ((flag & FMT_ALIGN_LEFT) != 0) {
                // align to left
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                buf += 4;
                while (field_width > 0) {
                    *buf++ = ' ';
                    field_width--;
                }
                *buf = '\0';
                return len;
            }
            else {
                // align to right
                while (field_width > 0) {
                    *buf++ = ' ';
                    field_width--;
                }
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                *(buf + 4)  = '\0';
                return len;
            }
        }
    }
}

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex2(char * JMC_RESTRICT buf, size_t count, double val, unsigned int flag,
             unsigned int fill, int field_width, int precision)
{
    int len;
    int64_t i64;
    uint32_t scale32;
    uint64_t scale, frac;
    register jmc_ieee754_double *d64;
    unsigned int n;
    int num_width;

    if ((flag & FMT_ALIGN_LEFT) != 0) {
        // align to left
        if (precision > 0) {
#if 0
            if (precision <= 9) {
                jimic_assert(precision > 0 && precision <= 9);
                scale = pow10_scales_int32[precision];
            }
            else {
                jimic_assert(precision > 9 && precision <= FMT_MAX_DOUBLE_PRECISION);
                scale = pow10_scales_int64[precision - 11];
            }
#else
            if (precision <= 9) {
                jimic_assert(precision >= 0 && precision <= 9);
                scale32 = 10UL;
                for (n = precision; n > 0; --n)
                    scale32 *= 10UL;
                scale = scale32;
            }
            else {
                jimic_assert(precision > 9 && precision <= FMT_MAX_DOUBLE_PRECISION);
                if (precision < FMT_MAX_DOUBLE_PRECISION) {
                    scale = 10000000000ULL;
                    for (n = precision - 10; n > 0; --n)
                        scale *= 10;
                }
                else {
                    scale = 100000000000000000ULL;
                }
            }
#endif
        }
        else if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            scale = 1000000;
        }
        else {
            scale = 1;
        }

        d64 = (jmc_ieee754_double *)&val;

        // is NaN or INF ? (exponent is maxium ?)
        if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32) !=
                                   JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
            i64 = (int64_t)val;
            //if (val >= 0.0) {
            if (i64 >= 0) {
                frac = (uint64_t)((val - (double)i64) * scale + 0.5);
                if (frac == scale) {
                    i64++;
                    frac = 0;
                }
            }
            else {
                frac = (uint64_t)(((double)i64 - val) * scale + 0.5);
                if (frac == scale) {
                    i64--;
                    frac = 0;
                }
            }

            // for integer part of double
            len = jmc_i64toa_r10_ex(buf, -1, i64, flag, fill, 0, 0);
            field_width -= len;
            buf += len;

            if (precision > 0) {
                // output decimal
                *buf++ = '.';

                // for fractional part of double
                if (precision >= field_width - 1) {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', precision, field_width - 1) + 1;
                }
                else {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', field_width - 1, precision) + 1;
                }
            }
            return len;
        }
        // if mantissa0 and mantissa1 is not equal zero, then it's a NaN (not a number)
        else if (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0) {
            // is NaN, not a number
            len = JIMIC_MAX(field_width, 3);
            field_width -= 3;
            // align to left
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = ' ';
            buf += 3;
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            *buf = '\0';
            return len;
        }
        // if mantissa0 and mantissa1 is equal zero, then it's a Inf (-/+ INFINITY)
        else {
            // is +INF or -INF
            //if (d64->ieee.negative == 0) {
            if ((d64->sign.dword & JMC_IEEE754_DOUBLE_SIGN_MASK32) == 0) {
                // is a +INF
                if ((flag & FMT_PLUS_FLAG) == 0) {
                    len = JIMIC_MAX(field_width, 3);
                    field_width -= 3;
                    // align to left
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = ' ';
                    buf += 3;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    len = JIMIC_MAX(field_width, 4);
                    field_width -= 4;
                    // align to left
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    buf += 4;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }

            }
            else {
                len = JIMIC_MAX(field_width, 4);
                field_width -= 4;
                // align to left
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                buf += 4;
                while (field_width > 0) {
                    *buf++ = ' ';
                    field_width--;
                }
                *buf = '\0';
                return len;
            }
        }
    }
    else {
        // align to right
        if (precision > 0) {
#if 0
            if (precision <= 10) {
                jimic_assert(precision >= 0 && precision <= 10);
                scale = scales32[precision];
            }
            else {
                jimic_assert(precision > 10 && precision <= 20);
                scale = scales64[precision - 11];
            }
#else
            if (precision <= 10) {
                jimic_assert(precision >= 0 && precision <= 10);
                scale32 = 1;
                for (n = precision; n > 0; --n)
                    scale32 *= 10;
                scale = scale32;
            }
            else {
                jimic_assert(precision > 10 && precision <= 20);
                scale = 10000000000;
                for (n = precision - 11; n > 0; --n)
                    scale *= 10;
            }
#endif
            num_width = field_width - precision - 1;
        }
        else if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            scale = 1000000;
            num_width = field_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else {
            scale = 1;
            num_width = field_width;
        }

        d64 = (jmc_ieee754_double *)&val;

        // is NaN or INF ? (exponent is maxium ?)
        if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32) !=
                                   JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
            i64 = (int64_t)val;
            //if (val >= 0.0) {
            if (i64 >= 0) {
                frac = (uint64_t)((val - (double)i64) * scale + 0.5);
                if (frac == scale) {
                    i64++;
                    frac = 0;
                }
            }
            else {
                frac = (uint64_t)(((double)i64 - val) * scale + 0.5);
                if (frac == scale) {
                    i64--;
                    frac = 0;
                }
            }

            // for integer part of double
            if (num_width < 0)
                num_width = 0;
            len = jmc_i64toa_r10_ex(buf, -1, i64, flag, fill, num_width, num_width);
            field_width -= len;
            buf += len;

            if (precision > 0) {
                // output decimal
                *buf++ = '.';

                // for fractional part of double
                if (precision >= field_width - 1) {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', precision, field_width - 1) + 1;
                }
                else {
                    len += jmc_u64toa_r10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', field_width - 1, precision) + 1;
                }
            }
            return len;
        }
        // if mantissa0 and mantissa1 is not equal zero, then it's a NaN (not a number)
        else if (d64->ieee.mantissa0 != 0 || d64->ieee.mantissa1 != 0) {
            // is NaN, not a number
            len = JIMIC_MAX(field_width, 3);
            field_width -= 3;
            // align to right
            while (field_width > 0) {
                *buf++ = ' ';
                field_width--;
            }
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = '\0';
            return len;
        }
        // if mantissa0 and mantissa1 is equal zero, then it's a Inf (-/+ INFINITY)
        else {
            // is +INF or -INF
            //if (d64->ieee.negative == 0) {
            if ((d64->sign.dword & JMC_IEEE754_DOUBLE_SIGN_MASK32) == 0) {
                // is a +INF
                if ((flag & FMT_PLUS_FLAG) == 0) {
                    len = JIMIC_MAX(field_width, 3);
                    field_width -= 3;
                    // align to right
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = '\0';
                    return len;
                }
                else {
                    len = JIMIC_MAX(field_width, 4);
                    field_width -= 4;
                    // align to left
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    buf += 4;
                    while (field_width > 0) {
                        *buf++ = ' ';
                        field_width--;
                    }
                    *buf = '\0';
                    return len;
                }

            }
            else {
                // is a -INF
                len = JIMIC_MAX(field_width, 4);
                field_width -= 4;
                // align to right
                while (field_width > 0) {
                    *buf++ = ' ';
                    field_width--;
                }
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                *(buf + 4)  = '\0';
                return len;
            }
        }
    }
}
