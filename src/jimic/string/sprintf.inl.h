
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
#define FMT_DEFAULT_SIGN        0x0000
#define FMT_SPACE_SIGN          0x0001      // ' ', space sign
#define FMT_SIGN_MASK           0x0002      // sign mask
#define FMT_SHARP_SIGN          0x0004      // '#', number sign
#define FMT_FIELDWIDTH_SIGN     0x0008      // '*', asterisk sign
#define FMT_CURRENCY_SIGN       0x0010      // ',', comma sign

#define FMT_SIGN_MASK_BIT       2           // move bit

// 句号: period

/* format align mode */
#define FMT_ALIGN_PLUS          0x0040      // '+', plus sign
#define FMT_ALIGN_MINUS         0x0080      // '-', minus sign
#define FMT_ALIGN_LEFT          FMT_ALIGN_MINUS
#define FMT_ALIGN_RIGHT         FMT_ALIGN_PLUS
#define FMT_ALIGN_NONE          FMT_ALIGN_RIGHT
                                            // default is FMT_ALIGN_RIGHT, '+'

/* format fill mode */
#define FMT_FILL_SPACE          ' '         // fill space: ' '
#define FMT_FILL_ZERO           '0'         // fill zero:  '0'
#define FMT_FILL_NONE           FMT_FILL_SPACE
                                            // default is FMT_FILL_SPACE, ' '

#define FMT_DEFAULT_FLAG        (FMT_DEFAULT_SIGN | FMT_ALIGN_NONE)

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
    //jm_char         c;
    jm_char        *end, *cur;
    unsigned int    flag, align, fill, width, precision, sign;
    jm_char        *first;
    size_t          len;
    jm_char        *s;
    int             i32;
    double          dbl;
    jm_uchar        ch;
    void           *p;
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
#if 0
    if (end < buf)
        end = (jm_char *)(size_t)-1;
#else
    end = (end >= buf) ? end : ((jm_char *)(size_t)-1);
#endif

    jimic_assert(end >= buf);

vslprintf_try_next:
    while ((c = *cur) != '\0') {
        // '%' start
        if (c == '%') {
            flag  = FMT_DEFAULT_SIGN;
            align = FMT_ALIGN_NONE;
            fill  = FMT_FILL_NONE;
            width = 0;
            precision = 0;
            cur++;
            first = cur;
            //sign = 0;

#if defined(VSLPRINTF_USE_PRE_TREATMENT) && (VSLPRINTF_USE_PRE_TREATMENT != 0)

            // get align or fill info
            c = *cur;
#if 1
            if (c <= '9' && c >= '0') {
                if (c == '0') {
                    // '0': 0x30
                    fill = FMT_FILL_ZERO;
                    c = *++cur;
                    if (c <= '0' || c > '9')
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
#if 0
                // special for '%0#.#d'
                else if (c == '.') {
                    //cur++;
                    goto vslprintf_get_precision;
                }
                // special for '%0##d'
                else if (c == 'd') {
                    goto vslprintf_out_int;
                }
#endif
                else {
                    // do nothing!
                }
            }
#else
            if (c == '0') {
                // '0': 0x30
                fill = FMT_FILL_ZERO;
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
                align = FMT_ALIGN_LEFT;
                c = *++cur;
            }
            else if (c == '+') {
                // '+': 0x2B
                align = FMT_ALIGN_RIGHT;
                c = *++cur;
            }
            else if (c == ' ') {
                // ' ': 0x20
                flag = FMT_FILL_SPACE;
                c = *++cur;
            }
            else if (c == '\0') {
                goto vslprintf_exit;
            }
#endif
            else {
                // do nothing!
            }

#endif  /* VSLPRINTF_USE_PRE_TREATMENT */

            // get another info
            //while ((c = *cur) != '\0') {
            for ( ;; ) {
                c = *cur;
                //if (c == '\0')
                //    goto vslprintf_try_next;
vslprintf_continue:
                switch (c) {
#if 1
                case '\0':
                    goto vslprintf_try_next;
                    //goto vslprintf_exit;

                case '\t':
                case '\b':
                case '\n':
                case '\r':
                    *buf++ = c;
                    if (buf >= end)
                        goto vslprintf_exit;
                    cur++;
                    break;
#endif
                case ' ':   // 0x20
                    flag |= FMT_SPACE_SIGN;
                    cur++;
                    break;

                case '#':   // 0x23
                    flag |= FMT_SHARP_SIGN;
                    cur++;
                    break;

                case '%':   // 0x25
                    // specail for "%%"
#if 0
                    *buf++ = c;
                    if (buf >= end)
                        goto vslprintf_exit;
#else
                    //if ((cur - first) == 1) {
                    if (cur == first) {
                        *buf++ = c;
                        if (buf >= end)
                            goto vslprintf_exit;
                        cur++;
                    }
                    else {
                        // not is "%%", do nothing!
                        cur++;
                    }
#endif
                    break;

                case '*':   // 0x2A
                    flag |= FMT_FIELDWIDTH_SIGN;
                    // get filed width from the next argument
                    width = (unsigned int)va_arg(args, int);
                    if ((int)width < 0) {
                        align = FMT_ALIGN_LEFT;
                        width = (unsigned int)(-(int)width);
                    }
                    cur++;
                    //goto vslprintf_continue;
                    break;

                case '+':   // 0x2B
                    align = FMT_ALIGN_RIGHT;
                    cur++;
                    break;

                case ',':   // 0x2C
                    flag |= FMT_CURRENCY_SIGN;
                    cur++;
                    break;

                case '-':   // 0x2D
                    align = FMT_ALIGN_LEFT;
                    cur++;
                    break;

                case '.':   // 0x2E
vslprintf_get_precision:
                    // get precision
                    c = *++cur;
                    if (c >= '0' && c <= '9') {
                        precision = c - '0';
                        c = *++cur;
                        while (c <= '9' && c >= '0') {
                            precision = precision * 10 + c - '0';
                            c = *++cur;
                        }
                        goto vslprintf_continue;
                    }
                    else if (c == '*') {
                        // get filed precision from the next argument
                        i32 = (jm_uchar)va_arg(args, int);
                        /* ignore negative precision */
                        if (i32 >= 0)
                            precision = i32;
                        c = *++cur;
                        //continue;
                    }
                    goto vslprintf_continue;

                case '0':
                    fill = FMT_FILL_ZERO;
                    c = *++cur;
                    if (c >= '0' && c <= '9') {
                        width = c - '0';
                        c = *++cur;
                        while (c <= '9' && c >= '0') {
                            width = width * 10 + c - '0';
                            c = *++cur;
                        }
                    }
                    goto vslprintf_continue;

                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    width = c - '0';
                    c = *++cur;
                    while (c <= '9' && c >= '0') {
                        width = width * 10 + c - '0';
                        c = *++cur;
                    }
                    goto vslprintf_continue;

                case 'c':
                    ch = (jm_uchar)va_arg(args, int);
                    *buf++ = (jm_char)ch;
                    if (buf >= end)
                        goto vslprintf_exit;
                    cur++;
                    goto vslprintf_try_next;

                case 'd':
vslprintf_out_int:
                    // int
                    if ((buf + JIMIC_MAX(11, width)) >= end)
                        goto vslprintf_exit;
                    i32 = va_arg(args, int);
#if 1
                    //if (width == 0 && flag == FMT_DEFAULT_SIGN) {
                    if ((width | flag) == (0 | FMT_DEFAULT_SIGN)) {
                        len = jmc_itoa_radix10(buf, i32);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        flag |= align;
                        len = jmc_itoa_radix10_ex(buf, -1, i32, flag, fill, width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
#else
                    flag |= align;
                    len = jmc_itoa_radix10_ex(buf, -1, i32, flag, fill, width, precision);
                    cur++;
                    goto vslprintf_try_next;
#endif

                case 'e':
                    // float & double, exponential expression
                    cur++;
                    goto vslprintf_try_next;

                case 'f':
vslprintf_out_double:
                    // float & double
                    if ((buf + 32) >= end)
                        goto vslprintf_exit;
                    dbl = va_arg(args, double);
                    len = jmc_dtos(buf, dbl);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'i':
                    // int64_t
                    if ((buf + 21) >= end)
                        goto vslprintf_exit;
                    i64 = va_arg(args, int64_t);
                    len = jmc_i64toa_radix10(buf, i64);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'l':
                    // long
                    if ((buf + 11) >= end)
                        goto vslprintf_exit;
                    l32 = va_arg(args, long);
                    len = jmc_ltoa_radix10(buf, l32);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'p':
                    // void *
                    if ((buf + 32) >= end)
                        goto vslprintf_exit;
                    p = va_arg(args, void *);
                    len = jmc_ptohex(buf, p);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 's':
vslprintf_out_string:
                    // char *
                    s = va_arg(args, jm_char *);
                    len = jmc_strlen(s);
                    if ((buf + JIMIC_MAX(len, width)) >= end)
                        goto vslprintf_exit;
                    //if (width == 0 && flag == FMT_DEFAULT_SIGN) {
                    if ((width | flag) == (0 | FMT_DEFAULT_SIGN)) {
                        len = jmc_strncpy(buf, (size_t)-1, s, len);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        flag |= align;
                        len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flag, fill, width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }

                case 't':
                    // uint64_t
                    if ((buf + 20) >= end)
                        goto vslprintf_exit;
                    u64 = va_arg(args, uint64_t);
                    len = jmc_u64toa_radix10(buf, u64);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'u':
                    // unsigned int
                    if ((buf + 10) >= end)
                        goto vslprintf_exit;
                    u32 = va_arg(args, unsigned int);
                    len = jmc_utoa_radix10(buf, u32);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'x':
                    // hex32
                    if ((buf + 16) >= end)
                        goto vslprintf_exit;
                    hex32 = va_arg(args, uint32_t);
                    len = jmc_uitohex(buf, hex32, FALSE);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                default:
                    if (c >= 'A' && c <= 'Z') {
                        if (c == 'X') {
                            // HEX32
                            if ((buf + 16) >= end)
                                goto vslprintf_exit;
                            hex32 = va_arg(args, uint32_t);
                            len = jmc_uitohex(buf, hex32, TRUE);
                            buf += len;
                            cur++;
                            goto vslprintf_try_next;
                        }
                        else if (c == 'U') {
                            // uint64_t
                            if ((buf + 20) >= end)
                                goto vslprintf_exit;
                            u64 = va_arg(args, uint64_t);
                            len = jmc_u64toa_radix10(buf, u64);
                            buf += len;
                            cur++;
                            goto vslprintf_try_next;
                        }
                        else if (c == 'L') {
                            // unsigned long
                            if ((buf + 10) >= end)
                                goto vslprintf_exit;
                            ul32 = va_arg(args, unsigned long);
                            len = jmc_ultoa_radix10(buf, ul32);
                            buf += len;
                            cur++;
                            goto vslprintf_try_next;
                        }
                    }
#if 0
                    else if (c == '\0') {
                        //goto vslprintf_exit;
                        goto vslprintf_try_next;
                    }
#endif
                    else {
                        *buf++ = c;
                        if (buf >= end)
                            goto vslprintf_exit;
                        cur++;
                    }
                    break;
                }
            }
        }
        else {
            // non format info
            *buf++ = c;
            if (buf >= end)
                break;
            cur++;
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
