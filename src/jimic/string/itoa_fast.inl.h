
#ifndef _JIMIC_STRING_ITOA_FAST_INL_H_
#define _JIMIC_STRING_ITOA_FAST_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_ITOA_FAST_H_
#error "Don't include the internal file <jimic/string/itoa_fast.inl.h> directly; include <jimic/string/itoa_fast.h> instead."
#endif

#include "jimic/stdio/sprintf_def.h"
#include "jimic/libc/int64.h"

#include "jimic/math/int_log10.h"
#include "jimic/string/string.h"
#include "jimic/string/jm_strings.h"

#include "jimic/core/jimic_assert.h"

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

static const unsigned int s_power10[16] = {
    9, 99, 999, 9999, 99999,
    999999, 9999999, 99999999,
    999999999, 4294967295,
    // only fill for aligned to 64 bytes
    0, 0, 0, 0, 0, 0
};

static const uint64_t s_power10_64[16] = {
    9999999999,                 // 10
    99999999999,                // 11
    999999999999,               // 12
    9999999999999,              // 13
    99999999999999,             // 14
    999999999999999,            // 15
    9999999999999999,           // 16
    99999999999999999,          // 17
    999999999999999999,         // 18
    18446744073709551615,       // 19
    // only fill for aligned to 64 bytes
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL
};

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    unsigned int digits_cnt;
    unsigned int exponent, exp10 = 0;
    unsigned char isNonzero;
    char *cur;

    jimic_assert(buf != NULL);

#if 0
    num_digits = jmc_uint_log10(val) + 1;
#elif 0
    //unsigned char isNonzero;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero == 0) {
        num_digits = 1;
    }
    else {
        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#elif 0
    //unsigned char isNonzero;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    num_digits = 1;

    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero != 0) {
        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (num_digits >> 23);

        if (val > s_power10[exp10])
            exp10++;
        num_digits += exp10;
    }
#elif 0
    //unsigned char isNonzero;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    num_digits = 1;

    if (val < 100) {
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        exp10 = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        exp10 = (num_digits >> 23);

        if (val > s_power10[exp10])
            exp10++;
        num_digits += exp10;
    }
#else
    //unsigned char isNonzero;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    if (val < 100) {
        num_digits = 1;
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#endif

    digits_cnt = 0;

    cur = (char *)buf + num_digits;
    *cur = '\0';

    do {
        digit_val = val % 10;
        val /= 10;

        cur--;
        *cur = (char)(digit_val + '0');

        digits_cnt++;
        jimic_assert(digits_cnt > num_digits);
    } while (val != 0);

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast_v1(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    const char *end;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    jimic_assert(buf != NULL);

    end = digits + jm_countof(digits) - 1;
    cur = (char *)end;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur-- = (char)(digit_val + '0');
    } while (val != 0);

    num_digits = (unsigned int)(end - cur);

#if 0
    do {
        ++cur;
        *buf++ = *cur;
    } while (cur != end);
#else
    cur++;
    while (cur <= end)
        *buf++ = *cur++;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast_v2(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    jimic_assert(buf != NULL);

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = (char)digit_val + '0';
    } while (val != 0);

    num_digits = (unsigned int)(cur - digits);

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#else
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_fast(char *buf, int val)
{
#if 0
    if (val >= 0) {
        return jmc_utoa_r10_fast(buf, (unsigned int)val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10_fast(buf, (unsigned int)val) + 1;
    }
#elif 1
    int sign;
    if (val < 0) {
        *buf++ = '-';
        val = -val;
        sign = 1;
    }
    else {
        sign = 0;
    }
    return jmc_utoa_r10_fast(buf, (unsigned int)val) + sign;
#elif 0
    if (val < 0) {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10_fast(buf, (unsigned int)val) + 1;
    }
    else
        return jmc_utoa_r10_fast(buf, (unsigned int)val);
#else
    register int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_utoa_r10_fast(buf, (unsigned int)val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_ultoa_r10_fast(char *buf, unsigned long val)
{
    unsigned long digit_val, num_digits;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = (char)digit_val + '0';
    } while (val != 0);

    num_digits = (unsigned long)(cur - digits);

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#else
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_ltoa_r10_fast(char *buf, long val)
{
#if 1
    if (val >= 0) {
        return jmc_ultoa_r10_fast(buf, val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_ultoa_r10_fast(buf, val) + 1;
    }        
#else
    int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_ultoa_r10_fast(buf, val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_fast(char *buf, uint64_t val)
{
    unsigned int digit_val, num_digits;
    unsigned int digits_cnt;
    unsigned int exponent;
    unsigned char isNonzero;
    uint32_t val32, val32_high;
    char *cur;
    jmc_uint64_t *u64;

#if 0
    if (val <= (uint64_t)JIMIC_UINT_MAX64) {
        val32 = (uint32_t)val;
#else
    u64 = (jmc_uint64_t *)&val;
    if (u64->dwords.high == 0) {
        // Get the low dword in uint64_t
        val32 = (uint32_t)u64->dwords.low;
#endif

#if 0
        num_digits = jmc_uint_log10(val32) + 1;
#else
        //unsigned char isNonzero;
        //
        // _BitScanReverse, _BitScanReverse64
        // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
        //
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val32);
        if (isNonzero == 0) {
            num_digits = 1;
        }
        else {
            // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
            // exp10 = exponent * 2525222UL;
            num_digits = exponent * 2525222UL;
            // exp10 = exp10 / 131072 / 64;
            num_digits = (num_digits >> 23);

            if (val > s_power10[num_digits])
                num_digits++;
            num_digits++;
        }

        digits_cnt = 0;

        cur = (char *)buf + num_digits;
        *cur = '\0';

        do {
            digit_val = val32 % 10;
            val32 /= 10;

            cur--;
            *cur = (char)(digit_val + '0');

            digits_cnt++;
            jimic_assert(digits_cnt > num_digits);
        } while (val32 != 0);
#endif
    }
    else {
        val32_high = (uint32_t)u64->dwords.high;

#if 0
        num_digits = jmc_uint64_log10(val32) + 1;
#else
        if (val < 10000000000ULL) {
            num_digits = 10;
        }
        else {
            //unsigned char isNonzero;
            //
            // _BitScanReverse, _BitScanReverse64
            // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
            //
            isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val32_high);

            exponent += 32;

            // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
            // exp10 = exponent * 2525222UL;
            num_digits = exponent * 2525222UL;
            // exp10 = exp10 / 131072 / 64;
            num_digits = (num_digits >> 23);

            if (val > s_power10_64[num_digits - 10])
                num_digits++;
            num_digits++;
        }

        digits_cnt = num_digits - 9;

        cur = (char *)buf + num_digits;
        *cur = '\0';

        do {
            digit_val = val % 10;
            val /= 10;

            cur--;
            *cur = (char)(digit_val + '0');

            digits_cnt--;
            jimic_assert(digits_cnt > num_digits);
        } while (digits_cnt > 0 && val != 0);

        jimic_assert(val <= 4294967295ULL);
        val32 = (uint32_t)val;

        digits_cnt = 0;
        do {
            digit_val = val32 % 10;
            val32 /= 10;

            cur--;
            *cur = (char)(digit_val + '0');

            digits_cnt++;
            jimic_assert(digits_cnt > (num_digits - 9));
        } while (val32 != 0);
#endif
    }

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_fast(char *buf, int64_t val)
{
#if 1
    if (val >= 0) {
        return jmc_u64toa_r10_fast(buf, val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_u64toa_r10_fast(buf, val) + 1;
    }        
#else
    int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_u64toa_r10_fast(buf, val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_fast_ex(char *buf, size_t count, unsigned int val, unsigned int flag,
                     unsigned int fill, unsigned int field_width, int length)
{
    unsigned int digit_val, num_digits;
    unsigned int digits_cnt;
    unsigned int exponent;
    char *cur;
    unsigned char isNonzero;
    int sign_char;
    int fill_cnt;
    int padding;

    jimic_assert(buf != NULL);
    jimic_assert(count != 0);

#if 0
    num_digits = jmc_uint_log10(val) + 1;
#elif 1
    //unsigned char isNonzero;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);
    if (isNonzero == 0) {
        num_digits = 1;
    }
    else {
        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#else
    //unsigned char isNonzero;
    //
    // _BitScanReverse, _BitScanReverse64
    // Reference: http://msdn.microsoft.com/en-us/library/fbxyd7zd.aspx
    //
    if (val < 100) {
        num_digits = 1;
        if (val >= 10)
            num_digits++;
    }
    else {
        isNonzero = (unsigned char)_BitScanReverse((unsigned long *)&exponent, (unsigned long)val);

        // must 2,525,222 < 4,194,304 ( 2^32 / 1024)
        // exp10 = exponent * 2525222UL;
        num_digits = exponent * 2525222UL;
        // exp10 = exp10 / 131072 / 64;
        num_digits = (num_digits >> 23);

        if (val > s_power10[num_digits])
            num_digits++;
        num_digits++;
    }
#endif

    if ((flag & (FMT_SIGN_MASK | FMT_SPACE_FLAG | FMT_PLUS_FLAG)) == 0) {
        sign_char = '\0';
        fill_cnt = field_width - num_digits;
    }
    else {
        if ((flag & FMT_SPACE_FLAG) == 0) {
            // '+' is 0x2B, '-' is 0x2D
            sign_char = '+' + (flag & FMT_SIGN_MASK);
            fill_cnt = field_width - num_digits - 1;
        }
        else {
            // ' ' is 0x20, '-' is 0x2D
#if 0
            sign_char = ((flag & FMT_SIGN_MASK) == 0) ? ' ' : '-';
#else
            if ((flag & FMT_SIGN_MASK) == 0)
                sign_char = ' ';
            else
                sign_char = '-';
#endif
            fill_cnt = field_width - num_digits - 1;
        }
    }

    if (fill_cnt <= 0) {
        // add sign
        if (sign_char != '\0')
            *buf++ = (char)sign_char;

        // start to write the digitals
        digits_cnt = 0;

        cur = (char *)buf + num_digits;
        *cur = '\0';

        do {
            digit_val = val % 10;
            val /= 10;

            cur--;
            *cur = (char)(digit_val + '0');

            digits_cnt++;
            jimic_assert(digits_cnt > num_digits);
        } while (val != 0);

        if (sign_char == '\0')
            return num_digits;
        else
            return num_digits + 1;
    }
    else {
        // when legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)field_width) {
            if (fill == FMT_FILL_SPACE) {
                // fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }

                // add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;
            }
            else {
                // add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }
            }
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // align to right, when (length < field_width)
                jimic_assert(length < (int)field_width);

                padding = length - num_digits;
                if (padding > 0) {
                    // fill right padding space
                    while (fill_cnt > padding) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;

                    // fill normal
                    while (fill_cnt > 0) {
                        *buf++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // fill right padding space
                    while (fill_cnt > 0) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;
                }
            }
            else {
                // align to left, when (length < field_width)
                jimic_assert(length < (int)field_width);

                // add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // fill normal
                padding = length - num_digits;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *buf++ = fill;
                        padding--;
                    }
                }

                // start to write the digitals
                digits_cnt = 0;

                cur = (char *)buf + num_digits;

                do {
                    digit_val = val % 10;
                    val /= 10;

                    cur--;
                    *cur = (char)(digit_val + '0');

                    digits_cnt++;
                    jimic_assert(digits_cnt > num_digits);
                } while (val != 0);

                buf += num_digits;

                // fill left padding space
                while (fill_cnt > 0) {
                    *buf++ = ' ';
                    fill_cnt--;
                }

                goto utoa_r10_ex_exit;
            }
        }
    }

    // start to write the digitals
    digits_cnt = 0;

    cur = (char *)buf + num_digits;

    do {
        digit_val = val % 10;
        val /= 10;

        cur--;
        *cur = (char)(digit_val + '0');

        digits_cnt++;
        jimic_assert(digits_cnt > num_digits);
    } while (val != 0);

    buf += num_digits;

utoa_r10_ex_exit:
    *buf = '\0';

    return field_width;
}

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_fast_ex(char *buf, size_t count, int val, unsigned int flag,
                     unsigned int fill, unsigned int field_width, int length)
{
#if 1
    if (val < 0) {
        flag |= FMT_SIGN_MASK;
        val = -val;
    }

    return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
#elif 0
    if ((flag & FMT_SPACE_FLAG) == 0) {
        if (val >= 0) {
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
        }
        else {
            *buf++ = '-';
            val = -val;
            flag |= FMT_SIGN_MASK;
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
        }
    }
    else {
        if (val >= 0) {
            *buf++ = '+';
        }
        else {
            *buf++ = '-';
            val = -val;
        }
        flag |= FMT_SIGN_MASK;
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#elif 0
    if ((flag & FMT_SPACE_FLAG) == 0) {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
        }
        else
            return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
    }
    else {
        if (val >= 0) {
            *buf++ = '+';
        }
        else {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#else
    int sign;
    if ((flag & FMT_SPACE_FLAG) == 0) {
        sign = (val < 0);
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + sign;
    }
    else {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        else {
            *buf++ = '+';
        }
        return jmc_utoa_r10_fast_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_fast_ex(char *buf, size_t count, uint64_t val, unsigned int flag,
                       unsigned int fill, unsigned int field_width, int length)
{
    unsigned int digit_val, num_digits;
    uint32_t val32;
    char *end, *cur;
    int sign_char;
    int fill_cnt;
    int padding;
    jmc_uint64_t *u64;
    char digits[32];    // 实际最多只会用到20个bytes

    if (sizeof(jmc_uint64_t) != sizeof(uint64_t)) {
        // maybe have some error!
        jimic_assert(sizeof(jmc_uint64_t) == sizeof(uint64_t));
        return -1;
    }

    end = digits + jm_countof(digits) - 1;
    cur = end;
#if 0
    if (val <= (uint64_t)JIMIC_UINT_MAX64) {
        val32 = (uint32_t)val;
#else
    u64 = (jmc_uint64_t *)&val;
    if (u64->dwords.high == 0) {
        val32 = (uint32_t)u64->dwords.low;
#endif
        do {
            digit_val = val32 % 10;
            val32 /= 10;

            *cur-- = (char)(digit_val + '0');
        } while (val32 != 0);
    }
    else {
        do {
            digit_val = val % 10;
            val /= 10;

            *cur-- = (char)(digit_val + '0');
        } while (val != 0);
    }

    num_digits = (unsigned int)(end - cur);

    if ((flag & (FMT_SIGN_MASK | FMT_SPACE_FLAG | FMT_PLUS_FLAG)) == 0) {
        sign_char = '\0';
        fill_cnt = field_width - num_digits;
    }
    else {
        if ((flag & FMT_SPACE_FLAG) == 0) {
            // '+' is 0x2B, '-' is 0x2D
            sign_char = '+' + (flag & FMT_SIGN_MASK);
            fill_cnt = field_width - num_digits - 1;
        }
        else {
            // ' ' is 0x20, '-' is 0x2D
#if 0
            sign_char = ((flag & FMT_SIGN_MASK) == 0) ? ' ' : '-';
#else
            if ((flag & FMT_SIGN_MASK) == 0)
                sign_char = ' ';
            else
                sign_char = '-';
#endif
            fill_cnt = field_width - num_digits - 1;
        }
    }

    if (fill_cnt <= 0) {
        // add sign
        if (sign_char != '\0')
            *buf++ = (char)sign_char;
#if 0
        do {
            ++cur;
            *buf++ = *cur;
        } while (cur != end);
#else
        cur++;
        while (cur <= end)
            *buf++ = *cur++;
#endif
        *buf = '\0';

        if (sign_char == '\0')
            return num_digits;
        else
            return num_digits + 1;
    }
    else {
        // when legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)field_width) {
            if (fill == FMT_FILL_SPACE) {
                // fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }

                // add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;
            }
            else {
                // add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // fill normal
                while (fill_cnt > 0) {
                    *buf++ = fill;
                    fill_cnt--;
                }
            }
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // align to right, when (length < field_width)
                jimic_assert(length < (int)field_width);

                padding = length - num_digits;
                if (padding > 0) {
                    // fill right padding space
                    while (fill_cnt > padding) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;

                    // fill normal
                    while (fill_cnt > 0) {
                        *buf++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // fill right padding space
                    while (fill_cnt > 0) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

                    // add sign
                    if (sign_char != '\0')
                        *buf++ = (char)sign_char;
                }
            }
            else {
                // align to left, when (length < field_width)
                jimic_assert(length < (int)field_width);

                // add sign
                if (sign_char != '\0')
                    *buf++ = (char)sign_char;

                // fill normal
                padding = length - num_digits;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *buf++ = fill;
                        padding--;
                    }
                }

#if 0
                do {
                    ++cur;
                    *buf++ = *cur;
                } while (cur != end);
#else
                cur++;
                while (cur <= end)
                    *buf++ = *cur++;
#endif
                // fill left padding space
                while (fill_cnt > 0) {
                    *buf++ = ' ';
                    fill_cnt--;
                }

                goto u64toa_r10_ex_exit;
            }
        }
    }

#if 0
    do {
        ++cur;
        *buf++ = *cur;
    } while (cur != end);
#else
    cur++;
    while (cur <= end)
        *buf++ = *cur++;
#endif

u64toa_r10_ex_exit:
    *buf = '\0';

    return field_width;
}

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_fast_ex(char *buf, size_t count, int64_t val, unsigned int flag,
                       unsigned int fill, unsigned int field_width, int length)
{
    if (val < 0) {
        flag |= FMT_SIGN_MASK;
        val = -val;
    }
    return jmc_u64toa_r10_fast_ex(buf, count, val, flag, fill, field_width, length);
}

#endif  /* _JIMIC_STRING_ITOA_FAST_INL_H_ */
