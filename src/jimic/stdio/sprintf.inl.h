
#ifndef _JIMIC_STDIO_SPRINTF_INL_H_
#define _JIMIC_STDIO_SPRINTF_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STDIO_SPRINTF_H_
#include <jimic/stdio/sprintf.h>
#endif

#include <jimic/string/jmc_strings.h>
#include <jimic/string/dtos.h>

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)
#pragma warning(disable : 4102)         // 禁止warning C4102: "未引用的标签"
#endif  /* _MSC_VER */

//
// Printf() 输出格式控制
// Reference: http://bbs.csdn.net/topics/330107715
//

#ifndef _JIMIC_STDIO_SPRINTF_FORMAT_DEF_
#define _JIMIC_STDIO_SPRINTF_FORMAT_DEF_

/* format sign */
#define FMT_DEFAULT_FLAG        0x00000000      // default flag
#define FMT_SPACE_FLAG          0x00010000      // ' ', space flag
#define FMT_PLUS_FLAG           0x00020000      // '+', plus flag
#define FMT_SHARP_FLAG          0x00040000      // '#', number flag
#define FMT_FIELDWIDTH_FLAG     0x00080000      // '*', asterisk flag
#define FMT_CURRENCY_FLAG       0x00100000      // ',', comma flag

#define FMT_SIGN_MASK           0x00000002      // sign mask
#define FMT_SIGN_MASK_BIT       2               // sign move bit

// 句号: period

/* format align mode */
#define FMT_ALIGN_RIGHT         0x00200000      // align to right
#define FMT_ALIGN_LEFT          0x00400000      // align to left
#define FMT_ALIGN_CENTER        0x00800000      // align to center
#define FMT_ALIGN_MINUS         FMT_ALIGN_LEFT  // '-', minus sign
#define FMT_ALIGN_DEFAULT       FMT_ALIGN_RIGHT
                                                // default is FMT_ALIGN_RIGHT, '+'

/* format fill mode */
#define FMT_FILL_SPACE          ' '             // fill space: ' '
#define FMT_FILL_ZERO           '0'             // fill zero:  '0'
#define FMT_FILL_DEFAULT        FMT_FILL_SPACE
                                                // default is FMT_FILL_SPACE, ' '

#define FMT_DEFAULT_STATUS      (FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT | FMT_FILL_DEFAULT)

#endif  /* !_JIMIC_STDIO_SPRINTF_FORMAT_DEF_ */

//
// ASCII码对照表
// Reference: http://ascii.911cha.com/
//

JMC_INLINE_NONSTD(int)
jmc_sprintf(char * JMC_RESTRICT buf, const char * JMC_RESTRICT fmt, ...)
{
    int     len;
    va_list args;

    va_start(args, fmt);
    len = jmc_vsnprintf(buf, (size_t)-1, (size_t)-1, fmt, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(int)
jmc_snprintf(char * JMC_RESTRICT buf, size_t countOfElements, size_t count,
             const char * JMC_RESTRICT fmt, ...)
{
    int     len;
    va_list args;

    va_start(args, fmt);
    len = jmc_vsnprintf(buf, countOfElements, count, fmt, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(char *)
jmc_slprintf(char * JMC_RESTRICT buf, size_t countOfElements, size_t count,
             const char * JMC_RESTRICT fmt, ...)
{
    char   *last;
    va_list args;

    va_start(args, fmt);
    last = jmc_vslprintf(buf, countOfElements, count, fmt, args);
    va_end(args);
    return last;
}

JMC_INLINE_NONSTD(int)
jmc_vsprintf(char * JMC_RESTRICT buf, const char * JMC_RESTRICT fmt, va_list args)
{
    return jmc_vsnprintf(buf, (size_t)-1, (size_t)-1, fmt, args);
}

JMC_INLINE_NONSTD(int)
jmc_vsnprintf(char * JMC_RESTRICT buf, size_t countOfElements, size_t count,
              const char * JMC_RESTRICT fmt, va_list args)
{
    int     len;
    char   *last;
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
JMC_INLINE_NONSTD(char *)
jmc_vslprintf(char * JMC_RESTRICT buf, size_t countOfElements, size_t count,
              const char * JMC_RESTRICT fmt, va_list args)
{
    register char           c;

    char                   *end, *cur;
    unsigned int            flag, align, fill, width;
    int                     precision;
    char                   *first;
    size_t                  len;
    register char          *s;
    register int            i32;
    register int            prec_val;
    register double         dbl;
    register char           ch;
    register void          *p;
    register unsigned int   u32;
    register long           l32;
    register unsigned long  ul32;
    register int64_t        i64;
    register uint64_t       u64;
    register int32_t        hex32;

    jimic_assert(buf != NULL);
    jimic_assert(fmt != NULL);
    jimic_assert(count != 0);

    cur = (char *)fmt;
    end = buf + JIMIC_MIN(count, countOfElements - 1);
#if 0
    if (end < buf)
        end = (jm_char *)(size_t)-1;
#else
    end = (end >= buf) ? end : ((char *)(size_t)-1);
#endif

    jimic_assert(end >= buf);

#if 0
vslprintf_try_next:
    while ((c = *cur) != '\0') {
#else
vslprintf_try_next:
    for ( ;; ) {
        c = *cur;
#endif
        // '%' start
        if (c != '%') {
            // non format info
            if (c == '\0')
                break;
            *buf++ = c;
            if (buf >= end)
                break;
            cur++;
        }
        else {
            flag  = FMT_DEFAULT_FLAG;
            align = FMT_ALIGN_DEFAULT;
            fill  = FMT_FILL_DEFAULT;
            width = 0;
            precision = -1;
            cur++;
            first = cur;

            c = *cur;

#if defined(VSLPRINTF_USE_PRE_TREATMENT) && (VSLPRINTF_USE_PRE_TREATMENT != 0)

            // get align or fill info
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
                //c = *cur;
vslprintf_continue:
#if 0
                if (c == '\0')
                    goto vslprintf_try_next;
#endif
                switch (c) {
#if 1
                case '\0':
                    // 这里为什么不直接跳 vslprintf_exit 呢, 从逻辑上是应该跳 vslprintf_exit 的.
                    // 但是不知道为什么, 跳 vslprintf_try_next 执行的时候反而还要快一些,
                    // 可能 vslprintf_try_next 在代码缓存里是比较"热"的, 分支预测也是"热"的.
                    // 所以, 即使多执行了一些语句, 还是要稍微快一些......
                    // 不过, 不是所有情况都快了, 但是即使不是最快的时候, 也没慢多少, 还是值得的!
                    goto vslprintf_try_next;
                    //goto vslprintf_exit;
#endif

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
                        prec_val = va_arg(args, int);
                        /* ignore negative precision */
                        if (prec_val >= 0)
                            precision = prec_val;
                        c = *++cur;
                        //continue;
                    }
                    goto vslprintf_continue;

                case 'd':
vslprintf_out_int:
                    // int
                    if ((buf + JIMIC_MAX(11, width)) >= end)
                        goto vslprintf_exit;
                    i32 = va_arg(args, int);
#if 1
                    //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                    if ((width | flag) == (0 | FMT_DEFAULT_FLAG)) {
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
#elif 0
                    flag |= align;
                    //if (width == 0 && flag == FMT_DEFAULT_FLAG && fill == FMT_ALIGN_DEFAULT) {
                    if ((width | flag) == (0 | FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT)) {
                        len = jmc_itoa_radix10(buf, i32);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        len = jmc_itoa_radix10_ex(buf, -1, i32, flag, fill, width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
#else
                    flag |= align;
                    len = jmc_itoa_radix10_ex(buf, -1, i32, flag, fill, width, precision);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;
#endif

                case 'f':
vslprintf_out_double:
                    // float & double
                    if ((buf + 32) >= end)
                        goto vslprintf_exit;
                    dbl = va_arg(args, double);
                    flag |= align;
                    // FMT_DEFAULT_STATUS = (FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT | FMT_FILL_DEFAULT);
                    //if (width == 0 && flag == FMT_DEFAULT_FLAG
                    //    && align == FMT_ALIGN_DEFAULT && fill == FMT_FILL_DEFAULT) {
                    //if ((flag | fill) == (FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT | FMT_FILL_DEFAULT)) {
                    if ((flag | fill) == FMT_DEFAULT_STATUS) {
                        len = jmc_dtos(buf, dbl, (int)width, precision);
                        //len = jmc_dtos_ex(buf, -1, dbl, flag, fill, (int)width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        len = jmc_dtos_ex(buf, -1, dbl, flag, fill, (int)width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }

                case 's':
vslprintf_out_string:
                    // char *
                    s = (char *)va_arg(args, char *);
                    if (s != NULL) {
                        len = jmc_strlen(s);
                        if ((buf + JIMIC_MAX(len, width)) >= end)
                            goto vslprintf_exit;
#if 1
                        if (len < 256) {
                            //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                            if ((width | flag) == (0 | FMT_DEFAULT_FLAG)) {
                                len = jmc_strncpy(buf, (size_t)-1, s, len);
                                buf += len;
                                // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                                //*buf = (jm_char)'\0';
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
                        }
                        else {
                            //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                            if ((width | flag) == (0 | FMT_DEFAULT_FLAG)) {
                                len = jmc_strncpy_fast(buf, (size_t)-1, s, len);
                                buf += len;
                                // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                                //*buf = (jm_char)'\0';
                                cur++;
                                goto vslprintf_try_next;
                            }
                            else {
                                flag |= align;
                                len = jmc_strncpy_ex_fast(buf, (size_t)-1, s, len, flag, fill, width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
                        }
#else
                        //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                        if ((width | flag) == (0 | FMT_DEFAULT_FLAG)) {
                            len = jmc_strncpy(buf, (size_t)-1, s, len);
                            buf += len;
                            // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                            //*buf = (jm_char)'\0';
                            cur++;
                            goto vslprintf_try_next;
                        }
                        else {
                            flag |= align;
#if 0
                            if (len < 256) {
                                len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flag, fill, width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
                            else {
                                len = jmc_strncpy_ex_fast(buf, (size_t)-1, s, len, flag, fill, width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
#else
                            if (len < 256)
                                len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flag, fill, width, precision);
                            else
                                len = jmc_strncpy_ex_fast(buf, (size_t)-1, s, len, flag, fill, width, precision);

                            buf += len;
                            cur++;
                            goto vslprintf_try_next;
#endif
                        }
#endif
                    }
                    else {
                        // string is null
                        len = sizeof(FMT_NULL_STRING) - 1;
                        if ((buf + JIMIC_MAX(len, width)) >= end)
                            goto vslprintf_exit;

                        /* Write "(null)" if there's space. */
#if 1
                        if ((width | flag) == (0 | FMT_DEFAULT_FLAG)) {
                            len = jmc_out_null_string(buf, (size_t)-1);
                        }
                        else {
                            flag |= align;
                            len = jmc_out_null_string_ex(buf, (size_t)-1, flag, fill, width, precision);
                        }
#elif 0
                        flag |= align;
                        len = jmc_out_null_string_ex(buf, (size_t)-1, flag, fill, width, precision);
#else
                        flag |= align;
                        len = jmc_strncpy_ex(buf, (size_t)-1, FMT_NULL_STRING, len, flag, fill, width, precision);
#endif
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }

                case 'c':
                    // char
                    ch = (char)va_arg(args, int);
                    *buf++ = (char)ch;
                    if (buf >= end)
                        goto vslprintf_exit;
                    // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                    //*buf = (jm_char)'\0';
                    cur++;
                    goto vslprintf_try_next;

                case 'X':
                    // HEX32
                    if ((buf + 16) >= end)
                        goto vslprintf_exit;
                    hex32 = va_arg(args, uint32_t);
                    len = jmc_uitohex(buf, hex32, TRUE);
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

                case 'u':
                    // unsigned int
                    if ((buf + 10) >= end)
                        goto vslprintf_exit;
                    u32 = va_arg(args, unsigned int);
                    len = jmc_utoa_radix10(buf, u32);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'e':
                    // float & double, exponential expression
                    cur++;
                    goto vslprintf_try_next;

                case '-':   // 0x2D
                    align = FMT_ALIGN_LEFT;
                    cur++;
                    c = *cur;
                    break;

                case ' ':   // 0x20
                    flag |= FMT_SPACE_FLAG;
                    cur++;
                    c = *cur;
                    break;

                case '+':   // 0x2B
                    align = FMT_PLUS_FLAG;
                    cur++;
                    c = *cur;
                    break;

                case '*':   // 0x2A
                    flag |= FMT_FIELDWIDTH_FLAG;
                    // get filed width from the next argument
                    width = (unsigned int)va_arg(args, int);
                    if ((int)width < 0) {
                        align = FMT_ALIGN_LEFT;
                        width = (unsigned int)(-(int)width);
                    }
                    cur++;
                    c = *cur;
                    //goto vslprintf_continue;
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
                        c = *cur;
                    }
                    else {
                        // not is "%%", do nothing!
                        cur++;
                        c = *cur;
                    }
#endif
                    break;

                case '#':   // 0x23
                    flag |= FMT_SHARP_FLAG;
                    cur++;
                    c = *cur;
                    break;

                case ',':   // 0x2C
                    flag |= FMT_CURRENCY_FLAG;
                    cur++;
                    c = *cur;
                    break;
#if 1
                case 'L':
                    // unsigned long
                    if ((buf + 10) >= end)
                        goto vslprintf_exit;
                    ul32 = va_arg(args, unsigned long);
                    len = jmc_ultoa_radix10(buf, ul32);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'U':
                    // uint64_t
                    if ((buf + 20) >= end)
                        goto vslprintf_exit;
                    u64 = va_arg(args, uint64_t);
                    len = jmc_u64toa_radix10(buf, u64);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;
#endif

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

                case 't':
                    // uint64_t
                    if ((buf + 20) >= end)
                        goto vslprintf_exit;
                    u64 = va_arg(args, uint64_t);
                    len = jmc_u64toa_radix10(buf, u64);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                default:
#if 0
#if 0
                    if (c == '\0') {
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
#else
    #if 0
                    if (c == '\0') {
                        //goto vslprintf_exit;
                        goto vslprintf_try_next;
                    }
                    else {
                        *buf++ = c;
                        if (buf >= end)
                            goto vslprintf_exit;
                        cur++;
                        c = *cur;
                    }
    #else
                    *buf++ = c;
                    if (buf >= end)
                        goto vslprintf_exit;
                    cur++;
                    c = *cur;
    #endif
#endif
                    break;
                }
            }
        }
#if 0
        else {
            // non format info
            *buf++ = c;
            if (buf >= end)
                break;
            cur++;
        }
#endif
    }

vslprintf_exit:
    *buf = '\0';
    return buf;
}

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)                    // 恢复warning状态
#endif

#endif  /* !_JIMIC_STDIO_SPRINTF_INL_H_ */
