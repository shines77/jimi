
#include <jimic/string/dtos.h>

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for define UINT_MAX

#include <jimic/libc/ieee754.h>
#include <jimic/string/jmc_strings.h>

static const double d_pow10_remain_16[] = {
    1.0E+0, 1.0E+4, 1.0E+9, 1.0E+14
};

static const double d_pow10_remain_8[] = {
    1.0E+0,  1.0E+2,  1.0E+4,  1.0E+7,
    1.0E+9,  1.0E+12, 1.0E+14, 1.0E+16,
};

static const double d_pow10_base_64[] = {
    1.0E+19,  1.0E+38,  1.0E+57,  1.0E+76,
    1.0E+95,  1.0E+114, 1.0E+133, 1.0E+152,
    1.0E+171, 1.0E+190, 1.0E+209, 1.0E+228,
    1.0E+247, 1.0E+266, 1.0E+285, 1.0E+304,
    // last 3 items is fill for address aligned to 64 bytes (cache line size)
    1.0E+308, 1.0E+308, 1.0E+308, 1.0E+308
};

static const uint32_t float_scales_32[] = {
    1, 10, 100, 1000, 10000, 100000,
    1000000, 10000000, 100000000, 1000000000,
    // fill for address aligned to 64 bytes (cache line size)
    1, 1, 1, 1, 1, 1
};

static const uint64_t float_scales_64[] = {
    10000000000ULL,         100000000000ULL,
    1000000000000ULL,       10000000000000ULL,
    100000000000000ULL,     1000000000000000ULL,
    10000000000000000ULL,   100000000000000000ULL,
    1000000000000000000ULL, 10000000000000000000ULL
};

/**
 * log10(2^64) = 19.265919722494796493679289262368
 * 131072 / 100000 = 1.31072
 * log10(2^64) * 131072 / 100000 = 25.25222629866837966019531802197
 * zoom = 2525222.629866837966019531802197
 */

JMC_DECLARE_NONSTD(int)
jmc_log10_fast(double val)
{
    jmc_ieee754_double *d64;
    int exponent;
    int exp10;

    d64 = (jmc_ieee754_double *)&val;
    // exponent = (exponent_mask32 >> (52 - 32)) - 1023;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;

    // exponent is positive (exponent >= 0 && exponent <= 1024)
    if (exponent >= 0 && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX) {
        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (int)((unsigned int)exp10 >> 23);
        return (int)exp10;
    }
    else {
        // exp10 = -exponent * 2525222UL;
        exponent = -exponent;
        exp10 = (unsigned int)exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64, exp10 = exp10 >> (17 + 6), 131072 = 2^17;
        exp10 = (int)((unsigned int)exp10 >> 23);
        return (int)-exp10;
    }   
}

JMC_DECLARE_NONSTD(int)
jmc_dget_exponent(double * JMC_RESTRICT pval)
{
    jmc_ieee754_double *d64;
    int exponent;
    jimic_assert(pval != NULL);
    d64 = (jmc_ieee754_double *)pval;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;
    jimic_assert(exponent >= JMC_IEEE754_DOUBLE_EXPONENT_MIN && exponent <= JMC_IEEE754_DOUBLE_EXPONENT_MAX);
    return exponent;
}

JMC_DECLARE_NONSTD(int)
jmc_fadjust_to_exp32(float * JMC_RESTRICT pval, int exponent)
{
    // TODO:
    return 0;
}

JMC_DECLARE_NONSTD(int)
jmc_dadjust_to_exp64(double * JMC_RESTRICT pval, int exponent)
{
    int exp10;
    int base_index;
    int remain_index;
    double pow10;

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
            pow10 = d_pow10_base_64[base_index - 1];

            remain_index = (exponent & 63) >> 3;
            jimic_assert(remain_index >= 0 && remain_index < 8);

            pow10 *= d_pow10_remain_8[remain_index];
            *pval = (*pval) / pow10;
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
        pow10 = d_pow10_base_64[base_index];

        remain_index = (exponent & 63) >> 3;
        jimic_assert(remain_index >= 0 && remain_index < 8);

        pow10 *= d_pow10_remain_8[remain_index];
        *pval = (*pval) * pow10;
        return -exp10;
    }
}

JMC_DECLARE_NONSTD(int)
jmc_ftos(char * JMC_RESTRICT buf, float val, int filed_width, int precision)
{
    // TODO:
    return 0;
}

JMC_DECLARE_NONSTD(int)
jmc_ftos_ex(char * JMC_RESTRICT buf, size_t count, float val, unsigned int flag,
            unsigned int fill, int filed_width, int precision)
{
    // TODO:
    return 0;
}

JMC_DECLARE_NONSTD(int)
jmc_dtos(char * JMC_RESTRICT buf, double val, int filed_width, int precision)
{
    int len;
    int64_t i64;
    uint32_t scale32;
    uint64_t scale, frac;
    fuint64_t *f64;
    jmc_ieee754_double *d64;
    int exponent;
    unsigned int n;
    int num_width;
    int exp10;

    static_assert(sizeof(fuint64_t) == sizeof(double), "jmc_dtos() maybe have some error!");

    if (sizeof(fuint64_t) != sizeof(double)) {
        // maybe have some error!
        jimic_assert(sizeof(fuint64_t) == sizeof(double));
        return -1;
    }

    d64 = (jmc_ieee754_double *)&val;
    exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;
    exp10 = jmc_dadjust_to_exp64(&val, exponent);

    if (precision < 0) {
        jimic_assert(precision < 0);
        precision = FMT_DEFAULT_DOUBLE_PRECISION;
        scale = 1000000;
        num_width = filed_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
    }
    else if (precision > 0) {
#if 0
        if (precision <= 9) {
            jimic_assert(precision > 0 && precision <= 9);
            scale = float_scales_32[precision];
        }
        else {
            jimic_assert(precision > 9 && precision <= 20);
            scale = float_scales_64[precision - 11];
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
        num_width = filed_width - precision - 1;
    }
    else {
        scale = 1ULL;
        num_width = filed_width;
    }

    f64 = (fuint64_t *)&val;
    // is NaN or INF ? (exponent is maxium ?)
    if ((f64->high & JM_DOUBLE_EXPONENT_MASK32) != JM_DOUBLE_EXPONENT_MASK32) {
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
        if (((precision != 0) && ((int)filed_width <= (precision + 1 + 1)))
            || ((precision == 0) && (filed_width <= (0 + 1)))) {
#else
        if (num_width <= 1) {
#endif
            len = jmc_i64toa_radix10(buf, i64);
            buf += len;
        }
        else {
#if 1
            // for integer part of double
            len = jmc_i64toa_radix10_for_integer_part(buf, i64, num_width);
#else
            len = jmc_i64toa_radix10_ex(buf, -1, i64, FMT_ALIGN_RIGHT, ' ', num_width, num_width);
#endif
            buf += len;
        }

        if (precision > 0) {
            // output decimal
            *buf++ = '.';
#if 1
            // for fractional part of double
            len += jmc_u64toa_radix10_for_frac_part(buf, frac, precision) + 1;
#else
            len += jmc_u64toa_radix10_ex(buf, -1, frac, FMT_ALIGN_LEFT, '0', precision, 0) + 1;
#endif
        }

        return len;
    }
    else if (((f64->high & JM_DOUBLE_MANTISSA_MASK_HIGH) != 0)
             || ((f64->low & JM_DOUBLE_MANTISSA_MASK_LOW) != 0)) {
        // is NaN, not a number
        len = JIMIC_MAX(filed_width, 3);
        filed_width -= 3;
        while (filed_width > 0) {
            *buf++ = ' ';
            filed_width--;
        }
        *buf        = 'N';
        *(buf + 1)  = 'a';
        *(buf + 2)  = 'N';
        *(buf + 3)  = '\0';
        return len;
    }
    else {
        // is +INF or -INF
        if ((f64->high & JM_DOUBLE_SIGN_BIT32) == 0) {
            len = JIMIC_MAX(filed_width, 3);
            filed_width -= 3;
            while (filed_width > 0) {
                *buf++ = ' ';
                filed_width--;
            }
            // '+', 0x2B
            *buf        = 'I';
            *(buf + 1)  = 'n';
            *(buf + 2)  = 'f';
            *(buf + 3)  = '\0';
            return len;
        }
        else {
            len = JIMIC_MAX(filed_width, 4);
            filed_width -= 4;
            while (filed_width > 0) {
                *buf++ = ' ';
                filed_width--;
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

#if defined(JMC_DTOS_INLINE_DECLARE) && (JMC_DTOS_INLINE_DECLARE != 0)
JMC_INLINE_NONSTD(int)
#else
JMC_DECLARE_NONSTD(int)
#endif
jmc_dtos_ex(char * JMC_RESTRICT buf, size_t count, double val, unsigned int flag,
            unsigned int fill, int filed_width, int precision)
{
    int len;
    int64_t i64;
    uint64_t val64;
    uint32_t val32;
    unsigned int digital;
    uint32_t scale32;
    uint64_t scale, frac;
    fuint64_t *f64;
    register jmc_ieee754_double *u64;
    register jmc_ieee754_double *d64;
    unsigned int n;
    unsigned int exponent;
    int num_width;
    int exp10;
    int frac_prec;
    int tail_zeros;

    static_assert(sizeof(fuint64_t) == sizeof(double), "jmc_dtos_ex() maybe have some error!");

    if (sizeof(fuint64_t) != sizeof(double)) {
        // maybe have some error!
        jimic_assert(sizeof(fuint64_t) == sizeof(double));
        return -1;
    }

    d64 = (jmc_ieee754_double *)&val;

    f64 = (fuint64_t *)&val;
    // is NaN or INF ? (exponent is maxium ?)
    //if ((f64->high & JM_DOUBLE_EXPONENT_MASK32) != JM_DOUBLE_EXPONENT_MASK32) {
    if ((d64->exponent.dword & JMC_IEEE754_DOUBLE_EXPONENT_MASK32) != JMC_IEEE754_DOUBLE_EXPONENT_MASK32) {
        // get the exponent value
        exponent = d64->ieee.exponent - JMC_IEEE754_DOUBLE_EXPONENT_BIAS;
        // if exponent belong range [0, 64), needn't to adjust
        if (exponent < 0 || exponent >= 64) {
            // adjust double value to range like (1 ~ 2^64) * 10^N
            exp10 = jmc_dadjust_to_exp64(&val, exponent);
        }
        else {
            exp10 = 0;
        }

        i64 = (int64_t)val;
        u64 = (jmc_ieee754_double *)&i64;
        // calc the digitals of the double integer part
        //if (i64 <= (uint64_t)JIMIC_UINT_MAX64) {
        if (u64->u32.high == 0) {
            val32 = (uint32_t)(u64->u32.low);
            digital = 0;
            do {
                digital++;
                val32 /= 10;
            } while (val32 != 0);
        }
        else {
            if (i64 >= 0)
                val64 = i64;
            else
                val64 = -i64;
            digital = 0;
            do {
                digital++;
                val64 /= 10;
            } while (val64 != 0);
        }

#if 1
        if (precision > 0) {
            jimic_assert(precision > 0);
            num_width = filed_width - precision - 1;
        }
        else if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            num_width = filed_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else {
            jimic_assert(precision == 0);
            num_width = filed_width;
        }
#else
        if (precision < 0) {
            jimic_assert(precision < 0);
            precision = FMT_DEFAULT_DOUBLE_PRECISION;
            num_width = filed_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
        }
        else {
            jimic_assert(precision >= 0);
            num_width = filed_width - precision;
            if (precision > 0)
                num_width--;
        }
#endif

        if (i64 != 0) {
            if ((precision + digital) <= FMT_MAX_DOUBLE_PRECISION)
                frac_prec = precision;
            else
                frac_prec = FMT_MAX_DOUBLE_PRECISION - digital;
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
                scale = float_scales_32[frac_prec];
            }
            else {
                jimic_assert(frac_prec > 9 && precision <= FMT_MAX_DOUBLE_PRECISION);
                scale = float_scales_64[frac_prec - 10];
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

        len = jmc_i64toa_radix10_ex(buf, -1, i64, flag, fill, num_width, num_width);
        filed_width -= len;
        buf += len;

        if (precision > 0) {
            // output decimal
            *buf++ = '.';

            // for fractional part of double
#if 0
            filed_width--;
            if (precision < filed_width) {
                num_width = precision;
                precision = filed_width;
                filed_width = num_width;
            }
            len += jmc_u64toa_radix10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                         '0', precision, filed_width - 1) + 1;
#else
            filed_width--;
            if (precision >= filed_width) {
                tail_zeros = precision - frac_prec;
                if (tail_zeros <= 0) {
                    len += jmc_u64toa_radix10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                                 '0', precision, filed_width) + 1;
                }
                else {
                    len += jmc_u64toa_radix10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
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
                len += jmc_u64toa_radix10_ex(buf, -1, frac, FMT_ALIGN_LEFT,
                                             '0', filed_width, precision) + 1;
            }
#endif
        }
        return len;
    }
    else if (((f64->high & JM_DOUBLE_MANTISSA_MASK_HIGH) != 0)
             || ((f64->low & JM_DOUBLE_MANTISSA_MASK_LOW) != 0)) {
        // is NaN, not a number
        len = JIMIC_MAX(filed_width, 3);
        filed_width -= 3;
        if ((flag & FMT_ALIGN_LEFT) != 0) {
            // align to left
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = ' ';
            buf += 3;
            while (filed_width > 0) {
                *buf++ = ' ';
                filed_width--;
            }
            *buf = '\0';
            return len;
        }
        else {
            // align to right
            while (filed_width > 0) {
                *buf++ = ' ';
                filed_width--;
            }
            *buf        = 'N';
            *(buf + 1)  = 'a';
            *(buf + 2)  = 'N';
            *(buf + 3)  = '\0';
            return len;
        }
    }
    else {
        // is +INF or -INF
        if ((f64->high & JM_DOUBLE_SIGN_BIT32) == 0) {
            if ((flag & FMT_PLUS_FLAG) == 0) {
                len = JIMIC_MAX(filed_width, 3);
                filed_width -= 3;
                if ((flag & FMT_ALIGN_LEFT) != 0) {
                    // align to left
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = ' ';
                    buf += 3;
                    while (filed_width > 0) {
                        *buf++ = ' ';
                        filed_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    // align to right
                    while (filed_width > 0) {
                        *buf++ = ' ';
                        filed_width--;
                    }
                    *buf        = 'I';
                    *(buf + 1)  = 'n';
                    *(buf + 2)  = 'f';
                    *(buf + 3)  = '\0';
                    return len;
                }
            }
            else {
                len = JIMIC_MAX(filed_width, 4);
                filed_width -= 4;
                if ((flag & FMT_ALIGN_LEFT) != 0) {
                    // align to left
                    // '+', 0x2B
                    *buf        = '+';
                    *(buf + 1)  = 'I';
                    *(buf + 2)  = 'n';
                    *(buf + 3)  = 'f';
                    buf += 4;
                    while (filed_width > 0) {
                        *buf++ = ' ';
                        filed_width--;
                    }
                    *buf = '\0';
                    return len;
                }
                else {
                    // align to right
                    while (filed_width > 0) {
                        *buf++ = ' ';
                        filed_width--;
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
            len = JIMIC_MAX(filed_width, 4);
            filed_width -= 4;
            if ((flag & FMT_ALIGN_LEFT) != 0) {
                // align to left
                // '-', 0x2D
                *buf        = '-';
                *(buf + 1)  = 'I';
                *(buf + 2)  = 'n';
                *(buf + 3)  = 'f';
                buf += 4;
                while (filed_width > 0) {
                    *buf++ = ' ';
                    filed_width--;
                }
                *buf = '\0';
                return len;
            }
            else {
                // align to right
                while (filed_width > 0) {
                    *buf++ = ' ';
                    filed_width--;
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
