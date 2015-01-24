
#ifndef _JIMIC_STRING_ITOA_INL_H_
#define _JIMIC_STRING_ITOA_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_ITOA_H_
#error "Don't include the internal file <jimic/string/itoa.inl.h> directly; include <jimic/string/itoa.h> instead."
#endif

#include "jimic/stdio/sprintf_def.h"
#include "jimic/libc/int64.h"

#include "jimic/string/string.h"
#include "jimic/string/jm_strings.h"

#include "jimic/basic/jimic_assert.h"

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

JMC_INLINE_NONSTD(int)
jmc_utoa_r10(char *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    unsigned int digits_cnt;
    char *cur;
    char digits[16];    // 实际最多只会用到10个bytes
    const char * const end = digits + jm_countof(digits);

    jimic_assert(buf != NULL);

    //end = (const char * const)(digits + jm_countof(digits));
    cur = (char *)end;
    do {
        cur--;
        digit_val = val % 10;
        val /= 10;

        *cur = (char)(digit_val + '0');
    } while (val != 0);

    num_digits = (unsigned int)(end - cur);

#if 1
    digits_cnt = num_digits;
    while (digits_cnt > 0) {
        *buf++ = *cur++;
        digits_cnt--;
    }
#elif 0
    do {
        *buf++ = *cur++;
    } while (cur != end);
#else
    while (cur < end)
        *buf++ = *cur++;
#endif
    *buf = '\0';

    return num_digits;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_v1(char *buf, unsigned int val)
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
jmc_utoa_r10_v2(char *buf, unsigned int val)
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
jmc_itoa_r10(char *buf, int val)
{
#if 1
    if (val >= 0) {
        return jmc_utoa_r10(buf, (unsigned int)val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10(buf, (unsigned int)val) + 1;
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
    return jmc_utoa_r10(buf, (unsigned int)val) + sign;
#elif 0
    if (val < 0) {
        *buf++ = '-';
        val = -val;
        return jmc_utoa_r10(buf, (unsigned int)val) + 1;
    }
    else
        return jmc_utoa_r10(buf, (unsigned int)val);
#else
    register int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_utoa_r10(buf, (unsigned int)val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_ultoa_r10(char *buf, unsigned long val)
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
jmc_ltoa_r10(char *buf, long val)
{
#if 1
    if (val >= 0) {
        return jmc_ultoa_r10(buf, val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_ultoa_r10(buf, val) + 1;
    }        
#else
    int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_ultoa_r10(buf, val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10(char *buf, uint64_t val)
{
    unsigned int digit_val, num_digits;
    uint32_t val32;
    char *cur;
    jmc_uint64_t *u64;
    char digits[32];    // 实际最多只会用到20个bytes

    cur = digits;
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

            *cur++ = (char)(digit_val + '0');
        } while (val32 != 0);
    }
    else {
        do {
            digit_val = val % 10;
            val /= 10;

            *cur++ = (char)(digit_val + '0');
        } while (val != 0);
    }

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
jmc_i64toa_r10(char *buf, int64_t val)
{
#if 1
    if (val >= 0) {
        return jmc_u64toa_r10(buf, val);
    }
    else {
        *buf++ = '-';
        val = -val;
        return jmc_u64toa_r10(buf, val) + 1;
    }        
#else
    int sign;
    sign = (val < 0);
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_u64toa_r10(buf, val) + sign;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_utoa_r10_ex(char *buf, size_t count, unsigned int val, unsigned int flag,
                unsigned int fill, unsigned int field_width, int length)
{
    unsigned int digit_val, num_digits;
    char *end, *cur;
    int sign_char;
    int fill_cnt;
    int padding;
    char digits[16];    // 实际最多只会用到10个bytes

    jimic_assert(buf != NULL);
    jimic_assert(count != 0);

    end = digits + jm_countof(digits) - 1;
    cur = end;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur-- = (char)(digit_val + '0');
    } while (val != 0);

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

                goto utoa_r10_ex_exit;
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

utoa_r10_ex_exit:
    *buf = '\0';

    return field_width;
}

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_ex(char *buf, size_t count, int val, unsigned int flag,
                unsigned int fill, unsigned int field_width, int length)
{
#if 1
    if (val < 0) {
        flag |= FMT_SIGN_MASK;
        val = -val;
    }

    return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length);
#elif 0
    if ((flag & FMT_SPACE_FLAG) == 0) {
        if (val >= 0) {
            return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length);
        }
        else {
            *buf++ = '-';
            val = -val;
            flag |= FMT_SIGN_MASK;
            return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length) + 1;
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
        return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#elif 0
    if ((flag & FMT_SPACE_FLAG) == 0) {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
            return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length) + 1;
        }
        else
            return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length);
    }
    else {
        if (val >= 0) {
            *buf++ = '+';
        }
        else {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#else
    int sign;
    if ((flag & FMT_SPACE_FLAG) == 0) {
        sign = (val < 0);
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length) + sign;
    }
    else {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        else {
            *buf++ = '+';
        }
        return jmc_utoa_r10_ex(buf, count, val, flag, fill, field_width, length) + 1;
    }
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_ex(char *buf, size_t count, uint64_t val, unsigned int flag,
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
jmc_i64toa_r10_ex(char *buf, size_t count, int64_t val, unsigned int flag,
                  unsigned int fill, unsigned int field_width, int length)
{
    if (val < 0) {
        flag |= FMT_SIGN_MASK;
        val = -val;
    }
    return jmc_u64toa_r10_ex(buf, count, val, flag, fill, field_width, length);
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_integer(char *buf, uint64_t val, int sign,
                                    unsigned int field_width)
{
    unsigned int digit_val, num_digits;
    uint32_t val32;
    char *cur;
    int padding;
    jmc_uint64_t *u64;
    char digits[32];    // 实际最多只会用到20个bytes
    const unsigned int fill = ' ';

    cur = digits;
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

            *cur++ = (char)(digit_val + '0');
        } while (val32 != 0);
    }
    else {
        do {
            digit_val = val % 10;
            val /= 10;

            *cur++ = (char)(digit_val + '0');
        } while (val != 0);
    }

#if 1
    num_digits = (unsigned int)(cur - digits) + sign;
    padding = field_width - num_digits;
    if (padding < 0) {
        field_width = num_digits;
    }
    else {
        // align to right, fill ' ' into right padding space
        while (padding > 0) {
            // fill is ' '
            *buf++ = (char)fill;
            padding--;
        }
    }
#else
    num_digits = cur - digits;
    if (sign == 0) {
        padding = field_width - num_digits;
        if (padding < 0)
            field_width = num_digits;
    }
    else {
        padding = field_width - num_digits - 1;
        if (padding < 0)
            field_width = num_digits + 1;
    }

    // align to right
    while (padding > 0) {
        // fill is ' '
        *buf++ = (char)fill;
        padding--;
    }
#endif

    if (sign != 0)
        *buf++ = (char)'-';

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
    return field_width;
}

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_integer(char *buf, int64_t val,
                       unsigned int field_width)
{
#if 0
    if (val >= 0)
        return jmc_u64toa_r10_integer(buf,  val, 0, field_width);
    else
        return jmc_u64toa_r10_integer(buf, -val, 1, field_width);   
#else
    int sign;
    if (val >= 0) {
        sign = 0;
    }
    else {
        sign = 1;
        val = -val;
    }
    return jmc_u64toa_r10_integer(buf, val, sign, field_width);
#endif
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_frac(char *buf, uint64_t val, unsigned int precision)
{
    unsigned int digit_val, num_digits;
    uint32_t val32;
    char *cur;
    int padding;
    jmc_uint64_t *u64;
    char digits[32];    // 实际最多只会用到20个bytes
    const unsigned int fill = '0';

    cur = digits;
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

            *cur++ = (char)(digit_val + '0');
        } while (val32 != 0);
    }
    else {
        do {
            digit_val = val % 10;
            val /= 10;

            *cur++ = (char)(digit_val + '0');
        } while (val != 0);
    }

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

    padding = precision - num_digits;
    if (padding <= 0) {
        *buf = '\0';
        return num_digits;
    }
    else {
        // fractional part's tail is like as "0000000...."

        while (padding > 0) {
            // fill is '0'
            *buf++ = (char)fill;
            padding--;
        }

        *buf = '\0';
        return precision;
    }
}

#endif  /* _JIMIC_STRING_ITOA_INL_H_ */
