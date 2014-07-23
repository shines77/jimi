
#ifndef _JIMIC_STRING_SPRINTF_INL_H_
#define _JIMIC_STRING_SPRINTF_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_SPRINTF_H_
#include <jimic/string/sprintf.h>
#endif

#include <jimic/string/jmc_strings.h>

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)
#pragma warning(disable : 4102)         // 禁止warning C4102: "未引用的标签"
#endif  /* _MSC_VER */

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

#define FORMAT_DEFAULT_FLAG     (FORMAT_DEFAULT_SIGN | FORMAT_ALIGN_NONE | FORMAT_FILL_NONE)

#endif  /* !_JIMIC_STRING_SPRINTF_FORMAT_DEF_ */

//
// ASCII码对照表
// Reference: http://ascii.911cha.com/
//

JMC_INLINE_NONSTD(int)
jmc_sprintf(jm_char *buf, JM_CONST jm_char *fmt, ...)
{
    int         len;
    va_list     args;

    va_start(args, fmt);
    len = jmc_vsnprintf(buf, (size_t)-1, (size_t)-1, fmt, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(int)
jmc_snprintf(jm_char *buf, size_t countOfElements, size_t count,
             JM_CONST jm_char *fmt, ...)
{
    int         len;
    va_list     args;

    va_start(args, fmt);
    len = jmc_vsnprintf(buf, countOfElements, count, fmt, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(jm_char *)
jmc_slprintf(jm_char *buf, size_t countOfElements, size_t count,
             JM_CONST jm_char *fmt, ...)
{
    jm_char    *last;
    va_list     args;

    va_start(args, fmt);
    last = jmc_vslprintf(buf, countOfElements, count, fmt, args);
    va_end(args);
    return last;
}

JMC_INLINE_NONSTD(int)
jmc_vsprintf(jm_char *buf, JM_CONST jm_char *fmt, va_list args)
{
    return jmc_vsnprintf(buf, (size_t)-1, (size_t)-1, fmt, args);
}

JMC_INLINE_NONSTD(int)
jmc_vsnprintf(jm_char *buf, size_t countOfElements, size_t count,
              JM_CONST jm_char *fmt, va_list args)
{
    int         len;
    jm_char    *last;
    last = jmc_vslprintf(buf, countOfElements, count, fmt, args);
    len = (int)(last - buf);
    return len;
}

/* jmc_vslprintf()是否使用前置处理, 使用为1 */
#define VSLPRINTF_USE_PRE_TREATMENT     1

/**
 * Format:
 *   sprintf("%04d, %20s, %0.3f, %03d, 0x%08X, %%3%%", 1, "2", 3.12, 4, 8989);
 *
 */
JMC_INLINE_NONSTD(jm_char *)
jmc_vslprintf(jm_char *buf, size_t countOfElements, size_t count,
              JM_CONST jm_char *fmt, va_list args)
{
    register jm_char c;

    jm_char        *end, *cur;
    jm_char        *first;
    unsigned int    flag, align, fill, width, precision, sign;
    size_t          len;
    jm_char        *s;
    int             i32;
    double          dbl;
    jm_uchar        ch;
    void *          p;
    unsigned int    u32;
    long            l32;
    unsigned long   ul32 = 0;
    int64_t         i64;
    uint64_t        u64;
    int32_t         hex32;

    jimic_assert(buf != NULL);
    jimic_assert(fmt != NULL);
    jimic_assert(count != 0);

    sign = 0;

    cur = (jm_char *)fmt;
    end = buf + JIMIC_MIN(count, countOfElements - 1);
    if (end < buf)
        end = (jm_char *)(size_t)-1;

    jimic_assert(end >= buf);

vslprintf_try_next:
    while ((c = *cur++) != '\0') {
        // '%' start
        if (c == '%') {
            first = cur;
            flag  = FORMAT_DEFAULT_SIGN;
            align = FORMAT_ALIGN_NONE;
            fill  = FORMAT_FILL_NONE;
            width = 0;
            precision = 0;
            //sign = 0;
            //len = 0;

            // get align or fill info
            c = *cur;
#if defined(VSLPRINTF_USE_PRE_TREATMENT) && (VSLPRINTF_USE_PRE_TREATMENT != 0)
#if 1
            if (c <= '9' && c >= '0') {
                if (c == '0') {
                    // '0': 0x30
                    fill = FORMAT_FILL_ZERO;
                    c = *++cur;
                    if (c > '9' || c <= '0')
                        goto vslprintf_continue;
                }
                width = c - '0';
                c = *++cur;
                while (c >= '0' && c <= '9') {
                    width = width * 10 + c - '0';
                    c = *++cur;
                }

                // special for something
                if (0) {
                    // do nothing!
                }
                else if (c == '.') {
                    goto vslprintf_get_precision;
                }
#if 0
                // special for '%0##d'
                else if (c == 'd') {
                    goto vslprintf_out_int;
                }
#endif
            }
#else
            if (c == '0') {
                // '0': 0x30
                fill = FORMAT_FILL_ZERO;
                c = *++cur;
            }
            if (c >= '1' && c <= '9') {
                width = c - '0';
                c = *++cur;
                while (c >= '0' && c <= '9') {
                    width = width * 10 + c - '0';
                    c = *++cur;
                }
            }
#endif
            // special for something
            if (0) {
                // do nothing!
            }
#if 0
            // special for '%0##d'
            else if (c == 'd') {
                goto vslprintf_out_int;
            }
            else if (c == 'f') {
                goto vslprintf_out_double;
                //continue;
            }
            else if (c == 's') {
                //
                goto vslprintf_out_string;
                continue;
            }
            else if (c == '-') {
                // '-': 0x2D
                align = FORMAT_ALIGN_LEFT;
                c = *++cur;
            }
            else if (c == '+') {
                // '+': 0x2B
                align = FORMAT_ALIGN_RIGHT;
                c = *++cur;
            }
            else if (c == ' ') {
                // ' ': 0x20
                flag = FORMAT_FILL_SPACE;
                c = *++cur;
            }
            else if (c == '\0') {
                goto vslprintf_exit;
            }
#endif

#endif  /* VSLPRINTF_USE_PRE_TREATMENT */

            // get another info
            while ((c = *cur++) != '\0') {
vslprintf_continue:
                switch (c) {
                case ' ':   // 0x20
                    flag |= FORMAT_SPACE_SIGN;
                    break;

                case '#':   // 0x23
                    flag |= FORMAT_SHARP_SIGN;
                    break;

                case '%':   // 0x25
                    // specail for "%%"
#if 1
                    *buf++ = c;
                    if (buf >= end)
                        goto vslprintf_exit;
#else
                    if (no_exit == 0) {
                        *buf++ = c;
                        if (buf >= end)
                            goto vslprintf_exit;
                    }
                    else {
                        // not is "%%", do nothing!
                    }
#endif
                    break;

                case '*':   // 0x2A
                    flag |= FORMAT_FIELDWIDTH_SIGN;
                    // get filed width from the next argument
                    i32 = (jm_uchar)va_arg(args, int);
                    if (i32 < 0) {
                        align = FORMAT_ALIGN_LEFT;
                        width = -i32;
                    }
                    else
                        width = i32;
                    //goto vslprintf_continue;
                    break;

                case '+':   // 0x2B
                    align = FORMAT_ALIGN_RIGHT;
                    break;

                case ',':   // 0x2C
                    flag |= FORMAT_CURRENCY_SIGN;
                    break;

                case '-':   // 0x2D
                    align = FORMAT_ALIGN_LEFT;
                    break;

                case '.':   // 0x2E
vslprintf_get_precision:
                    // get precision
                    c = *cur++;
                    if (c >= '0' && c <= '9') {
                        precision = c - '0';
                        c = *cur++;
                        while (c >= '0' && c <= '9') {
                            precision = precision * 10 + c - '0';
                            c = *cur++;
                        }
                    }
                    else if (c == '*') {
                        // get filed precision from the next argument
                        i32 = (jm_uchar)va_arg(args, int);
                        /* ignore negative precision */
                        if (i32 >= 0)
                            precision = i32;
                        continue;
                    }
                    goto vslprintf_continue;

                case '0':
                    fill = FORMAT_FILL_ZERO;
                    c = *cur++;
                    if (c >= '0' && c <= '9') {
                        width = c - '0';
                        c = *cur++;
                        while (c >= '0' && c <= '9') {
                            width = width * 10 + c - '0';
                            c = *cur++;
                        }
                    }
                    goto vslprintf_continue;

                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    width = c - '0';
                    c = *cur++;
                    while (c >= '0' && c <= '9') {
                        width = width * 10 + c - '0';
                        c = *cur++;
                    }
                    goto vslprintf_continue;

                case 'c':
                    ch = (jm_uchar)va_arg(args, int);
                    *buf++ = (jm_char)ch;
                    if (buf >= end)
                        goto vslprintf_exit;
                    goto vslprintf_try_next;

                case 'd':
vslprintf_out_int:
                    // int
                    if ((buf + JIMIC_MAX(11, width)) >= end)
                        goto vslprintf_exit;
                    i32 = va_arg(args, int);
                    //if (width == 0 && flag == FORMAT_DEFAULT_SIGN) {
                    if ((width & flag) == (0 | FORMAT_DEFAULT_SIGN)) {
                        len = jmc_itoa_radix10(buf, i32);
                    }
                    else {
                        flag |= align;
                        len = jmc_itoa_radix10_ex(buf, -1, i32, flag, fill, width, precision);
                    }
                    buf += len;
                    goto vslprintf_try_next;

                case 'f':
vslprintf_out_double:
                    // float & double
                    if ((buf + 32) >= end)
                        goto vslprintf_exit;
                    dbl = va_arg(args, double);
                    len = jmc_dtos(buf, dbl);
                    buf += len;
                    goto vslprintf_try_next;

                case 'i':
                    // int64_t
                    if ((buf + 21) >= end)
                        goto vslprintf_exit;
                    i64 = va_arg(args, int64_t);
                    len = jmc_i64toa_radix10(buf, i64);
                    buf += len;
                    goto vslprintf_try_next;

                case 'l':
                    // long
                    if ((buf + 11) >= end)
                        goto vslprintf_exit;
                    l32 = va_arg(args, long);
                    len = jmc_ltoa_radix10(buf, l32);
                    buf += len;
                    goto vslprintf_try_next;

                case 'p':
                    // void *
                    if ((buf + 32) >= end)
                        goto vslprintf_exit;
                    p = va_arg(args, void *);
                    len = jmc_ptohex(buf, p);
                    goto vslprintf_try_next;

                case 's':
vslprintf_out_string:
                    // char *
                    s = va_arg(args, jm_char *);
                    len = jmc_strlen(s);
                    if ((buf + JIMIC_MAX(len, width)) >= end)
                        goto vslprintf_exit;
                    //if (width == 0 && flag == FORMAT_DEFAULT_SIGN) {
                    if ((width & flag) == (0 | FORMAT_DEFAULT_SIGN)) {
                        len = jmc_strncpy(buf, (size_t)-1, s, len);
                    }
                    else {
                        flag |= align;
                        len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flag, fill, width, precision);
                    }
                    buf += len;
                    goto vslprintf_try_next;

                case 't':
                    // uint64_t
                    if ((buf + 20) >= end)
                        goto vslprintf_exit;
                    u64 = va_arg(args, uint64_t);
                    len = jmc_u64toa_radix10(buf, u64);
                    buf += len;
                    goto vslprintf_try_next;

                case 'u':
                    // unsigned int
                    if ((buf + 10) >= end)
                        goto vslprintf_exit;
                    u32 = va_arg(args, unsigned int);
                    len = jmc_utoa_radix10(buf, u32);
                    buf += len;
                    goto vslprintf_try_next;

                case 'x':
                    // hex32
                    if ((buf + 16) >= end)
                        goto vslprintf_exit;
                    hex32 = va_arg(args, uint32_t);
                    len = jmc_uitohex(buf, hex32, FALSE);
                    buf += len;
                    break;

                default:
                    if (c == 'X') {
                        // HEX32
                        if ((buf + 16) >= end)
                            goto vslprintf_exit;
                        hex32 = va_arg(args, uint32_t);
                        len = jmc_uitohex(buf, hex32, TRUE);
                        buf += len;
                        goto vslprintf_try_next;
                    }
                    else if (c == 'U') {
                        // uint64_t
                        if ((buf + 20) >= end)
                            goto vslprintf_exit;
                        u64 = va_arg(args, uint64_t);
                        len = jmc_u64toa_radix10(buf, u64);
                        buf += len;
                        goto vslprintf_try_next;
                    }
                    else if (c == 'L') {
                        // unsigned long
                        if ((buf + 10) >= end)
                            goto vslprintf_exit;
                        ul32 = va_arg(args, unsigned long);
                        len = jmc_ultoa_radix10(buf, ul32);
                        buf += len;
                        goto vslprintf_try_next;
                    }
                    else {
                        *buf++ = c;
                        if (buf >= end)
                            goto vslprintf_exit;
                    }
                    break;
                }
            }
#if 1
            *buf++ = c;
            if (buf >= end)
                break;
#endif
        }
        else {
            // non format info
            *buf++ = c;
            if (buf >= end)
                break;
        }
    }

vslprintf_exit:
    *buf = '\0';
    return buf;
}

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)                    // 恢复warning状态
#endif

#endif  /* !_JIMIC_STRING_SPRINTF_INL_H_ */
