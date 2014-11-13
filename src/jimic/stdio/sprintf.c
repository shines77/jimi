
#include "jimic/stdio/sprintf.h"

#include "jimic/string/itoa.h"
#include "jimic/string/dtos.h"
#include "jimic/string/string.h"

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)
#pragma warning(disable : 4102)         // 禁止warning C4102: "未引用的标签"
#endif  /* _MSC_VER */

/* jmc_vslprintf()是否使用前置处理, 使用为1 */
#define VSLPRINTF_USE_PRE_TREATMENT     1

/**
 * Format:
 *   sprintf("%04d, %20s, %0.3f, %03d, 0x%08X, %%3%%", 1, "2", 3.12, 4, 8989);
 *
 */
JMC_DECLARE_NONSTD(char *)
jmc_vslprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, va_list args)
{
    register char           ch;

    char                   *end, *cur;
    unsigned int            flags, align_mode, fill_char, field_width;
    int                     precision;
    char                   *token_start;
    size_t                  len;
    register char          *s;
    register int            i32;
    register int            prec_val;
    register double         dbl;
    register char           chr;
    register void          *p;
    register unsigned int   u32;
    register long           l32;
    register unsigned long  ul32;
    register int64_t        i64;
    register uint64_t       u64;
    register uint32_t       hex32;

    jimic_assert(buf != NULL);
    jimic_assert(format != NULL);
    jimic_assert(count != 0);

    cur = (char *)format;
    end = buf + JIMIC_MIN(count, count_max - 1);
#if 0
    if (end < buf)
        end = (jm_char *)(size_t)-1;
#else
    end = (end >= buf) ? end : ((char *)(size_t)(-1));
#endif

    jimic_assert(end >= buf);

#if 0
vslprintf_try_next:
    while ((ch = *cur) != '\0') {
#else
vslprintf_try_next:
    for (;;) {
        ch = *cur;
#endif
        // '%' start
        if (ch != '%') {
            // non format info
            if (ch == '\0')
                break;
            *buf++ = ch;
            if (buf >= end)
                break;
            cur++;
        }
        else {
            flags       = FMT_DEFAULT_FLAG;
            align_mode  = FMT_ALIGN_DEFAULT;
            fill_char   = FMT_FILL_DEFAULT;
            field_width = 0;
            precision   = -1;
            cur++;
            token_start = cur;

            ch = *cur;

#if defined(VSLPRINTF_USE_PRE_TREATMENT) && (VSLPRINTF_USE_PRE_TREATMENT != 0)

            // get align or fill info
#if 1
            if (ch <= '9' && ch >= '0') {
                if (ch == '0') {
                    // '0': 0x30
                    fill_char = FMT_FILL_ZERO;
                    ch = *++cur;
                    if (ch <= '0' || ch > '9')
                        goto vslprintf_continue;
                }
                field_width = ch - '0';
                ch = *++cur;
                while (ch >= '0' && ch <= '9') {
                    field_width = field_width * 10 + ch - '0';
                    ch = *++cur;
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
            if (ch == '0') {
                // '0': 0x30
                fill_char = FMT_FILL_ZERO;
                ch = *++cur;
            }
            if (ch >= '1' && ch <= '9') {
                field_width = ch - '0';
                ch = *++cur;
                while (ch >= '0' && ch <= '9') {
                    field_width = field_width * 10 + ch - '0';
                    ch = *++cur;
                }
            }
#endif
            // special for something
            if (0) {
                // do nothing!
            }
#if 0
            // special for '%0##d'
            else if (ch == 'd') {
                goto vslprintf_out_int;
            }
            else if (ch == 'f') {
                goto vslprintf_out_double;
                //continue;
            }
            else if (ch == 's') {
                //
                goto vslprintf_out_string;
                continue;
            }
            else if (ch == '-') {
                // '-': 0x2D
                align_mode = FMT_ALIGN_LEFT;
                ch = *++cur;
            }
            else if (ch == '+') {
                // '+': 0x2B
                align_mode = FMT_ALIGN_RIGHT;
                ch = *++cur;
            }
            else if (ch == ' ') {
                // ' ': 0x20
                flags = FMT_FILL_SPACE;
                ch = *++cur;
            }
            else if (ch == '\0') {
                goto vslprintf_exit;
            }
#endif
            else {
                // do nothing!
            }

#endif  /* VSLPRINTF_USE_PRE_TREATMENT */

            // get another info
            //while ((ch = *cur) != '\0') {
            for (;;) {
                //ch = *cur;
vslprintf_continue:
#if 0
                if (ch == '\0')
                    goto vslprintf_try_next;
#endif
                switch (ch) {
#if 1
                case '\0':
                    // 这里为什么不直接跳 vslprintf_exit ?, 从逻辑上是应该跳 vslprintf_exit 的.
                    // 但是不知道为什么, 跳 vslprintf_try_next 执行的时候反而还要快一些,
                    // 可能 vslprintf_try_next 在代码缓存里是比较"热"的, 分支预测也是"热"的.
                    // 所以, 即使多执行了一些语句, 还是要稍微快一些......
                    // 不过, 不是所有情况都快了, 但是即使不是最快的时候, 也没慢多少, 还是值得的!
                    goto vslprintf_try_next;
                    //goto vslprintf_exit;
#endif

                case '0':
                    fill_char = FMT_FILL_ZERO;
                    ch = *++cur;
                    if (ch >= '0' && ch <= '9') {
                        field_width = ch - '0';
                        ch = *++cur;
                        while (ch <= '9' && ch >= '0') {
                            field_width = field_width * 10 + ch - '0';
                            ch = *++cur;
                        }
                    }
                    goto vslprintf_continue;

                case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    field_width = ch - '0';
                    ch = *++cur;
                    while (ch <= '9' && ch >= '0') {
                        field_width = field_width * 10 + ch - '0';
                        ch = *++cur;
                    }
                    goto vslprintf_continue;

                case '.':   // 0x2E
vslprintf_get_precision:
                    // get precision
                    ch = *++cur;
                    if (ch >= '0' && ch <= '9') {
                        precision = ch - '0';
                        ch = *++cur;
                        while (ch <= '9' && ch >= '0') {
                            precision = precision * 10 + ch - '0';
                            ch = *++cur;
                        }
                        goto vslprintf_continue;
                    }
                    else if (ch == '*') {
                        // get filed precision from the next argument
                        prec_val = va_arg(args, int);
                        /* ignore negative precision */
                        if (prec_val >= 0)
                            precision = prec_val;
                        ch = *++cur;
                        //continue;
                    }
                    goto vslprintf_continue;

                case 'd':
vslprintf_out_int:
                    // int
                    if ((buf + JIMIC_MAX(11, field_width)) >= end)
                        goto vslprintf_exit;
                    i32 = va_arg(args, int);
#if 1
                    //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                    if ((field_width | flags) == (0 | FMT_DEFAULT_FLAG)) {
                        len = jmc_itoa_r10(buf, i32);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        flags |= align_mode;
                        len = jmc_itoa_r10_ex(buf, -1, i32, flags, fill_char, field_width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
#elif 0
                    flags |= align_mode;
                    //if (width == 0 && flag == FMT_DEFAULT_FLAG && fill == FMT_ALIGN_DEFAULT) {
                    if ((field_width | flags) == (0 | FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT)) {
                        len = jmc_itoa_r10(buf, i32);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        len = jmc_itoa_r10_ex(buf, -1, i32, flags, fill_char, field_width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
#else
                    flags |= align_mode;
                    len = jmc_itoa_r10_ex(buf, -1, i32, flags, fill_char, field_width, precision);
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
                    flags |= align_mode;
                    // FMT_DEFAULT_STATUS = (FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT | FMT_FILL_DEFAULT);
                    //if (width == 0 && flag == FMT_DEFAULT_FLAG
                    //    && align == FMT_ALIGN_DEFAULT && fill == FMT_FILL_DEFAULT) {
                    //if ((flag | fill) == (FMT_DEFAULT_FLAG | FMT_ALIGN_DEFAULT | FMT_FILL_DEFAULT)) {
                    if ((flags | fill_char) == FMT_DEFAULT_STATUS) {
                        len = jmc_dtos(buf, dbl, (int)field_width, precision);
                        //len = jmc_dtos_ex(buf, -1, dbl, flag, fill, (int)width, precision);
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }
                    else {
                        len = jmc_dtos_ex(buf, -1, dbl, flags, fill_char, (int)field_width, precision);
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
                        if ((buf + JIMIC_MAX(len, field_width)) >= end)
                            goto vslprintf_exit;
#if 1
                        if (len < 256) {
                            //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                            if ((field_width | flags) == (0 | FMT_DEFAULT_FLAG)) {
                                len = jmc_strncpy(buf, (size_t)-1, s, len);
                                buf += len;
                                // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                                //*buf = (jm_char)'\0';
                                cur++;
                                goto vslprintf_try_next;
                            }
                            else {
                                flags |= align_mode;
                                len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flags, fill_char, field_width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
                        }
                        else {
                            //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                            if ((field_width | flags) == (0 | FMT_DEFAULT_FLAG)) {
                                len = jmc_strncpy_fast(buf, (size_t)-1, s, len);
                                buf += len;
                                // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                                //*buf = (jm_char)'\0';
                                cur++;
                                goto vslprintf_try_next;
                            }
                            else {
                                flags |= align_mode;
                                len = jmc_strncpy_ex_fast(buf, (size_t)-1, s, len, flags, fill_char, field_width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
                        }
#else
                        //if (width == 0 && flag == FMT_DEFAULT_FLAG) {
                        if ((field_width | flags) == (0 | FMT_DEFAULT_FLAG)) {
                            len = jmc_strncpy(buf, (size_t)-1, s, len);
                            buf += len;
                            // 在snprintf内部, 写入'\0'这个操作是多余的, 我们在snprintf结尾总会补一个'\0'
                            //*buf = (jm_char)'\0';
                            cur++;
                            goto vslprintf_try_next;
                        }
                        else {
                            flags |= align_mode;
#if 0
                            if (len < 256) {
                                len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flags, fill_char, field_width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
                            else {
                                len = jmc_strncpy_ex_fast(buf, (size_t)-1, s, len, flags, fill_char, field_width, precision);
                                buf += len;
                                cur++;
                                goto vslprintf_try_next;
                            }
#else
                            if (len < 256)
                                len = jmc_strncpy_ex(buf, (size_t)-1, s, len, flags, fill_char, field_width, precision);
                            else
                                len = jmc_strncpy_ex_fast(buf, (size_t)-1, s, len, flags, fill_char, field_width, precision);

                            buf += len;
                            cur++;
                            goto vslprintf_try_next;
#endif
                        }
#endif
                    }
                    else {
                        // it is a null string
                        len = sizeof(FMT_NULL_STRING) - 1;
                        if ((buf + JIMIC_MAX(len, field_width)) >= end)
                            goto vslprintf_exit;

                        /* Write "(null)" if there's space. */
#if 1
                        if ((field_width | flags) == (0 | FMT_DEFAULT_FLAG)) {
                            len = jmc_out_null_string(buf, (size_t)-1);
                        }
                        else {
                            flags |= align_mode;
                            len = jmc_out_null_string_ex(buf, (size_t)-1, flags, fill_char, field_width, precision);
                        }
#elif 0
                        flags |= align_mode;
                        len = jmc_out_null_string_ex(buf, (size_t)-1, flags, fill_char, field_width, precision);
#else
                        flags |= align_mode;
                        len = jmc_strncpy_ex(buf, (size_t)-1, FMT_NULL_STRING, len, flags, fill_char, field_width, precision);
#endif
                        buf += len;
                        cur++;
                        goto vslprintf_try_next;
                    }

                case 'c':
                    // char
                    chr = (char)va_arg(args, int);
                    *buf++ = (char)chr;
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
                    len = jmc_utoa_r10(buf, u32);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'e':
                    // float & double, exponential expression
                    cur++;
                    goto vslprintf_try_next;

                case '-':   // 0x2D
                    align_mode = FMT_ALIGN_LEFT;
                    cur++;
                    ch = *cur;
                    break;

                case ' ':   // 0x20
                    flags |= FMT_SPACE_FLAG;
                    cur++;
                    ch = *cur;
                    break;

                case '+':   // 0x2B
                    align_mode = FMT_PLUS_FLAG;
                    cur++;
                    ch = *cur;
                    break;

                case '*':   // 0x2A
                    flags |= FMT_FIELDWIDTH_FLAG;
                    // get filed width from the next argument
                    field_width = (unsigned int)va_arg(args, int);
                    if ((int)field_width < 0) {
                        align_mode = FMT_ALIGN_LEFT;
                        field_width = (unsigned int)(-(int)field_width);
                    }
                    cur++;
                    ch = *cur;
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
                    if (cur == token_start) {
                        *buf++ = ch;
                        if (buf >= end)
                            goto vslprintf_exit;
                        cur++;
                        ch = *cur;
                    }
                    else {
                        // not is "%%", do nothing!
                        cur++;
                        ch = *cur;
                    }
#endif
                    break;

                case '#':   // 0x23
                    flags |= FMT_SHARP_FLAG;
                    cur++;
                    ch = *cur;
                    break;

                case ',':   // 0x2C
                    flags |= FMT_CURRENCY_FLAG;
                    cur++;
                    ch = *cur;
                    break;
#if 1
                case 'L':
                    // unsigned long
                    if ((buf + 10) >= end)
                        goto vslprintf_exit;
                    ul32 = va_arg(args, unsigned long);
                    len = jmc_ultoa_r10(buf, ul32);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'U':
                    // uint64_t
                    if ((buf + 20) >= end)
                        goto vslprintf_exit;
                    u64 = va_arg(args, uint64_t);
                    len = jmc_u64toa_r10(buf, u64);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;
#endif

                case 'i':
                    // int64_t
                    if ((buf + 21) >= end)
                        goto vslprintf_exit;
                    i64 = va_arg(args, int64_t);
                    len = jmc_i64toa_r10(buf, i64);
                    buf += len;
                    cur++;
                    goto vslprintf_try_next;

                case 'l':
                    // long
                    if ((buf + 11) >= end)
                        goto vslprintf_exit;
                    l32 = va_arg(args, long);
                    len = jmc_ltoa_r10(buf, l32);
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
                    len = jmc_u64toa_r10(buf, u64);
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
                        *buf++ = ch;
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
                    *buf++ = ch;
                    if (buf >= end)
                        goto vslprintf_exit;
                    cur++;
                    ch = *cur;
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

