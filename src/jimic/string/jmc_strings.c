
#include <jimic/string/jmc_strings.h>
#include <jimic/string/jm_strings.h>

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

#if !defined(JMC_DTOS_INLINE_DECLARE) || (JMC_DTOS_INLINE_DECLARE == 0)

JMC_DECLARE_NONSTD(int)
jmc_dtos(jm_char *buf, double val, unsigned int filed_width, int precision)
{
    int len;
    int64_t i64;
    uint32_t scale32;
    uint64_t scale, frac;
    fuint64_s *f64;
    unsigned int n;
    int num_width;
#if 0
    static const uint32_t scales32[] = {
        1, 10, 100, 1000, 10000, 100000,
        1000000, 10000000, 100000000, 1000000000
    };
    static const uint64_t scales64[] = {
        10000000000,     100000000000,     1000000000000,     10000000000000,
        100000000000000, 1000000000000000, 10000000000000000, 100000000000000000,
        1000000000000000000, 10000000000000000000
    };
#endif

    if (sizeof(fuint64_s) != sizeof(double)) {
        // maybe have some error!
#ifndef _MSC_VER
        #error "jmc_dtos() maybe have some error!"
#endif // _MSC_VER
        jimic_assert(sizeof(fuint64_s) == sizeof(double));
        return 0;
    }

    if (precision < 0) {
        jimic_assert(precision < 0);
        precision = FMT_DEFAULT_DOUBLE_PRECISION;
        scale = 1000000;
        num_width = filed_width - FMT_DEFAULT_DOUBLE_PRECISION - 1;
    }
    else if (precision > 0) {
#if 0
        if (precision <= 10) {
            jimic_assert(precision >= 0 && precision <= 10);
            scale = scales32[precision];
        }
        else {
            jimic_assert(precision > 10 && precision <= 20);
            scale = scales64[precision - 11];
        }
#elif 1
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
#else
        jimic_assert(precision >= 0 && precision <= 20);
        scale = 1;
        for (n = precision; n > 0; --n)
            scale *= 10;
#endif
        num_width = filed_width - precision - 1;
    }
    else {
        scale = 1;
        num_width = filed_width;
    }

    f64 = (fuint64_s *)&val;
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
            len = jmc_i64toa_radix10_ex(buf, -1, i64, FMT_ALIGN_RIGHT, ' ', num_width, 0);
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
        *buf        = 'N';
        *(buf + 1)  = 'a';
        *(buf + 2)  = 'N';
        *(buf + 3)  = '\0';
        return 3;
    }
    else {
        // is +INF or -INF
        if ((f64->high & JM_DOUBLE_SIGN_BIT32) == 0) {
            // '+', 0x2B
            *buf        = 'I';
            *(buf + 1)  = 'n';
            *(buf + 2)  = 'f';
            *(buf + 3)  = '\0';
            return 3;
        }
        else {
            // '-', 0x2D
//          *buf        = '+' + ((f64->high & JM_DOUBLE_SIGN_BIT32) >> 30);
            *buf        = '-';
            *(buf + 1)  = 'I';
            *(buf + 2)  = 'n';
            *(buf + 3)  = 'f';
            *(buf + 4)  = '\0';
            return 4;
        }
    }
}

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex(jm_char *buf, size_t count, double val, unsigned int flag,
            unsigned int fill, unsigned int width, int precision)
{
    return 0;
}

#endif  /* !JMC_DTOS_INLINE_DECLARE */
