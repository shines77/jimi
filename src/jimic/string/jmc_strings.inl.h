
#ifndef _JIMIC_STRING_JMC_STRIGNS_INL_H_
#define _JIMIC_STRING_JMC_STRIGNS_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_JMC_STRINGS_H_
#include <jimic/string/jmc_strings.h>
#endif

#include <vadefs.h>

//
// Printf() 输出格式控制
// Reference: http://bbs.csdn.net/topics/330107715
//

#ifndef _JIMIC_STRING_SPRINTF_FORMAT_DEF_
#define _JIMIC_STRING_SPRINTF_FORMAT_DEF_

/* format sign */
#define FORMAT_DEFAULT_SIGN     0x0000
#define FORMAT_SPACE_SIGN       0x0001      // ' ', space sign
#define FORMAT_SHARP_SIGN       0x0002      // '#', number sign
#define FORMAT_CURRENCY_SIGN    0x0004      // ',', comma sign
#define FORMAT_FIELDWIDTH_SIGN  0x0008      // '*', asterisk sign

// 句号: period

/* format align mode */
#define FORMAT_ALIGN_PLUS       0x0010      // '+', plus sign
#define FORMAT_ALIGN_MINUS      0x0020      // '-', minus sign
#define FORMAT_ALIGN_LEFT       FORMAT_ALIGN_MINUS
#define FORMAT_ALIGN_RIGHT      FORMAT_ALIGN_PLUS
#define FORMAT_ALIGN_NONE       FORMAT_ALIGN_RIGHT
                                            // default is FORMAT_ALIGN_RIGHT, '+'

/* format fill mode */
#define FORMAT_FILL_SPACE       ' '         // fill space: ' '
#define FORMAT_FILL_ZERO        '0'         // fill zero:  '0'
#define FORMAT_FILL_NONE        FORMAT_FILL_SPACE
                                            // default is FORMAT_FILL_SPACE, ' '

#define FORMAT_SIGN_MASK        0x8000      // sign mask

#define FORMAT_DEFAULT_FLAG     (FORMAT_DEFAULT_SIGN | FORMAT_ALIGN_NONE)

#endif  /* !_JIMIC_STRING_SPRINTF_FORMAT_DEF_ */

//
// ASCII码对照表
// Reference: http://ascii.911cha.com/
//

JMC_INLINE_NONSTD(int)
jmc_utoa_radix10(jm_char *buf, unsigned int val)
{
    unsigned int digval, digital;
    jm_char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digval = val % 10;
        val /= 10;

        *cur++ = (jm_char)digval + '0';
    } while (val != 0);

    digital = cur - digits;

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

    return digital;
}

JMC_INLINE_NONSTD(int)
jmc_itoa_radix10(jm_char *buf, int val)
{
    int sign;
    sign = (val < 0);
    if (sign) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_utoa_radix10(buf, val) + sign;
}

JMC_INLINE_NONSTD(int)
jmc_ultoa_radix10(jm_char *buf, unsigned long val)
{
    unsigned long digval, digital;
    jm_char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digval = val % 10;
        val /= 10;

        *cur++ = (jm_char)digval + '0';
    } while (val != 0);

    digital = cur - digits;

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

    return digital;
}

JMC_INLINE_NONSTD(int)
jmc_ltoa_radix10(jm_char *buf, long val)
{
    int sign;
    sign = (val < 0);
    if (sign) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_ultoa_radix10(buf, val) + sign;
}

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10(jm_char *buf, uint64_t val)
{
    unsigned int digval, digital;
    jm_char *cur;
    char digits[32];    // 实际最多只会用到20个bytes

    cur = digits;
    do {
        digval = val % 10;
        val /= 10;

        *cur++ = (jm_char)digval + '0';
    } while (val != 0);

    digital = cur - digits;

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

    return digital;
}

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10(jm_char *buf, int64_t val)
{
    int sign;
    sign = (val < 0);
    if (sign) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_u64toa_radix10(buf, val) + sign;
}

JMC_INLINE_NONSTD(int)
jmc_utoa_radix10_ex(jm_char *buf, size_t count, unsigned int val, unsigned int flag,
                    unsigned int fill, unsigned int width, unsigned int length)
{
    unsigned int digval, digital;
    int fill_cnt;
    int padding;
    jm_char *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digval = val % 10;
        val /= 10;

        *cur++ = (jm_char)digval + '0';
    } while (val != 0);

    digital = cur - digits;
    fill_cnt = width - digital;

    if (fill_cnt > 0) {
        // when legnth == 0 || legnth >= witdh, align to right or left is same
        if (length == 0 || length >= width) {
            // fill normal
            do {
                *buf++ = fill;
                fill_cnt--;
            } while (fill_cnt > 0);
        }
        else {
            if ((flag & FORMAT_ALIGN_LEFT) == 0) {
                // align to right, when (length < width)
                jimic_assert(length < width);

                padding = length - digital;
                if (padding > 0) {
                    // fill right padding space
                    while (fill_cnt > padding) {
                        *buf++ = ' ';
                        fill_cnt--;
                    }

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
                }
            }
            else {
                // align to left, when (length < width)
                jimic_assert(length < width);

                // fill normal
                padding = length - digital;
                if (padding > 0) {
                    fill_cnt -= padding;
                    do {
                        *buf++ = fill;
                        padding--;
                    } while (padding > 0);
                }
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
                // fill left padding space
                while (fill_cnt > 0) {
                    *buf++ = ' ';
                    fill_cnt--;
                }

                goto utoa_radix10_ex_exit;                
            }
        }
    }

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

utoa_radix10_ex_exit:
    *buf = '\0';

#if 0
    return width;
#else
    if (fill_cnt >= 0)
        return width;
    else
        return digital;
#endif
}

JMC_INLINE_NONSTD(int)
jmc_itoa_radix10_ex(jm_char *buf, size_t count, int val, unsigned int flag,
                    unsigned int fill, unsigned int width, unsigned int length)
{
#if 0
    if ((flag & FORMAT_SPACE_SIGN) == 0) {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
            return jmc_utoa_radix10_ex(buf, count, val, flag, fill, width, length) + 1;
        }
        else
            return jmc_utoa_radix10_ex(buf, count, val, flag, fill, width, length);
    }
    else {
        if (val >= 0) {
            *buf++ = '+';
        }
        else {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_radix10_ex(buf, count, val, flag, fill, width, length) + 1;
    }
#else
    int sign;
    if ((flag & FORMAT_SPACE_SIGN) == 0) {
        sign = (val < 0);
        if (sign) {
            *buf++ = '-';
            val = -val;
        }
        return jmc_utoa_radix10_ex(buf, count, val, flag, fill, width, length) + sign;
    }
    else {
        if (val < 0) {
            *buf++ = '-';
            val = -val;
        }
        else {
            *buf++ = '+';
        }
        return jmc_utoa_radix10_ex(buf, count, val, flag, fill, width, length) + 1;
    }
#endif
}

JMC_INLINE_NONSTD(int)
jmc_ftos(jm_char *buf, float val)
{
    //
    return 0;
}

JMC_INLINE_NONSTD(int)
jmc_dtos(jm_char *buf, double val)
{
    //
    return 0;
}

JMC_INLINE_NONSTD(int)
jmc_ptohex(jm_char *buf, void *p)
{
    return 0;
}

JMC_INLINE_NONSTD(int)
jmc_uitohex(jm_char *buf, uint32_t val, int isUpper)
{
    return 0;
}

JMC_INLINE_NONSTD(int)
jmc_ui64tohex(jm_char *buf, uint64_t val)
{
    return 0;
}

JMC_INLINE_NONSTD(size_t)
jmc_strlen(jm_char *str)
{
    return jm_strlen(str);
}

JMC_INLINE_NONSTD(size_t)
jmc_strcpy(jm_char *dest, JM_CONST jm_char *src)
{
    return 0;
}

JMC_INLINE_NONSTD(size_t)
jmc_strncpy(jm_char *dest, size_t countOfElements, JM_CONST jm_char *src, size_t count)
{
    return 0;
}

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_fast(jm_char *dest, size_t countOfElements, JM_CONST jm_char *src, size_t count)
{
    jm_char *end;
    count = JIMIC_MIN(count, countOfElements - 1);
    end = (jm_char *)src + count;
    while (src < end) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return count;
}

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_ex(jm_char *dest, size_t countOfElements, JM_CONST jm_char *src, size_t count,
               unsigned int flag, unsigned int fill, unsigned int width, unsigned int length)
{
    jm_char *end;
    size_t copy_len, len;
    int fill_cnt, padding;
    count = JIMIC_MIN(count, countOfElements - 1);
    copy_len = JIMIC_MIN(JIMIC_MIN(length, width), count);
    len = JIMIC_MAX(width, count);
    end = (jm_char *)src + copy_len;

    fill_cnt = width - copy_len;
    if (fill_cnt <= 0) {
        // copy from src
        while (src < end) {
            *dest++ = *src++;
        }
        *dest = '\0';
        return copy_len;
    }
    else {
        // when legnth == 0 || legnth >= witdh, align to right or left is same
        if (length == 0 || length >= width) {
            // fill normal
            do {
                *dest++ = fill;
                fill_cnt--;
            } while (fill_cnt > 0);
        }
        else {
            if ((flag & FORMAT_ALIGN_LEFT) == 0) {
                // align to right, when (length < width)
                jimic_assert(length < width);

                // fill right padding space
                padding = length - count;
                if (padding > 0) {
                    while (fill_cnt > padding) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }

                    // fill normal
                    while (fill_cnt > 0) {
                        *dest++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // fill right padding space
                    while (fill_cnt > 0) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }
                }
            }
            else {
                // align to left, when (length < width)
                jimic_assert(length < width);

                // fill normal
                padding = length - count;
                if (padding > 0) {
                    fill_cnt -= padding;
                    do {
                        *dest++ = fill;
                        padding--;
                    } while (padding > 0);
                }

                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }

                // fill left padding space
                while (fill_cnt > 0) {
                    *dest++ = ' ';
                    fill_cnt--;
                }
                goto jmc_strncpy_ex_exit;
            }
        }
    }

    // copy from src
    while (src < end) {
        *dest++ = *src++;
    }

jmc_strncpy_ex_exit:
    *dest = '\0';
    return len;
}

#endif  /* !_JIMIC_STRING_JMC_STRIGNS_INL_H_ */
