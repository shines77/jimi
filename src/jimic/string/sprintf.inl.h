
#ifndef _JIMIC_STRING_SPRINTF_INL_H_
#define _JIMIC_STRING_SPRINTF_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//#include <jimic/string/sprintf.h>

JMC_INLINE_NONSTD(int)
jmc_sprintf(jm_char *buf, JM_CONST jm_char *fmt, ...)
{
    int         len;
    va_list     args;

    va_start(args, fmt);
    len = jmc_vsnprintf(buf, (size_t)-1, fmt, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(int)
jmc_snprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt, ...)
{
    int         len;
    va_list     args;

    va_start(args, fmt);
    len = jmc_vsnprintf(buf, count, fmt, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(jm_char *)
jmc_slprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt, ...)
{
    jm_char    *last;
    va_list     args;

    va_start(args, fmt);
    last = jmc_vslprintf(buf, count, fmt, args);
    va_end(args);
    return last;
}

JMC_INLINE_NONSTD(int)
jmc_vsprintf(jm_char *buf, JM_CONST jm_char *fmt, va_list args)
{
    return jmc_vsnprintf(buf, (size_t)-1, fmt, args);
}

JMC_INLINE_NONSTD(int)
jmc_vsnprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt, va_list args)
{
    int         len;
    jm_char    *last;
    last = jmc_vslprintf(buf, count, fmt, args);
    len = (int)(last - buf);
    return len;
}

int jmc_utoa_radix10(jm_char *buf, unsigned int val)
{
    //
    return 0;
}

int jmc_itoa_radix10(jm_char *buf, int val)
{
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_utoa_radix10(buf, val);
}

int jmc_ultoa_radix10(jm_char *buf, unsigned long val)
{
    //
    return 0;
}

int jmc_ltoa_radix10(jm_char *buf, long val)
{
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_ultoa_radix10(buf, val);
}

int jmc_u64toa_radix10(jm_char *buf, uint64_t val)
{
    //
    return 0;
}

int jmc_i64toa_radix10(jm_char *buf, int64_t val)
{
    if (val < 0) {
        *buf++ = '-';
        val = -val;
    }
    return jmc_u64toa_radix10(buf, val);
}

int jmc_ftos(jm_char *buf, float val)
{
    //
    return 0;
}

int jmc_dtos(jm_char *buf, double val)
{
    //
    return 0;
}

int jmc_ptohex(jm_char *buf, void *p)
{
    return 0;
}

int jmc_uitohex(jm_char *buf, uint32_t val, int isUpper)
{
    return 0;
}

int jmc_ui64tohex(jm_char *buf, uint64_t val)
{
    return 0;
}

size_t jmc_strlen(jm_char *str)
{
    return 0;
}

size_t jmc_strcpy(jm_char *dest, JM_CONST jm_char *src)
{
    return 0;
}

//
// Printf() 输出格式控制
// Reference: http://bbs.csdn.net/topics/330107715
//

/* format align mode */
#define FORMAT_ALIGN_NONE       0
#define FORMAT_ALIGN_SPACE      (' ')
#define FORMAT_ALIGN_PLUS       ('+')
#define FORMAT_ALIGN_MINUS      ('-')
#define FORMAT_ALIGN_LEFT       FORMAT_ALIGN_MINUS
#define FORMAT_ALIGN_RIGHT      FORMAT_ALIGN_PLUS

/* format fill mode */
#define FORMAT_FILL_NONE        0
#define FORMAT_FILL_ZERO        ('0')
#define FORMAT_FILL_SPACE       (' ')

//
// ASCII码对照表
// Reference: http://ascii.911cha.com/
//

/**
 * Format:
 *   sprintf("%04d, %20s, %0.3f, %03d, 0x%08X, %%3%%", 1, "2", 3.12, 4, 8989);
 *
 */
JMC_INLINE_NONSTD(jm_char *)
jmc_vslprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt, va_list args)
{
    jm_char        *end, *first;
    jm_char        *cur;
    jm_char         c;
    unsigned int    align, fill, width, precision;
    int             len, no_exit;
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

    cur = (jm_char *)fmt;
    end = buf + count;

    while ((c = *cur++) != '\0') {
        // '%' start
        if (c == '%') {
            first = cur;
            align = 0;
            fill = 0;
            width = 0;
            precision = 0;
            len = 0;
            // get align or fill info
            c = *cur++;
            if (c == '0') {
                // '0': 0x30
                fill = FORMAT_FILL_ZERO;
            }
            else if (c >= '1' && c <= '9') {
                width = c - '0';
                c = *cur++;
                while (c >= '0' && c <= '9') {
                    width = width * 10 + c - '0';
                    c = *cur++;
                }
            }
            else if (c == ' ') {
                // ' ': 0x20
                fill = FORMAT_FILL_ZERO;
            }
            else if (c == '+') {
                // '+': 0x2B
                align = FORMAT_ALIGN_RIGHT;
            }
            else if (c == '-') {
                // '-': 0x2D
                align = FORMAT_ALIGN_LEFT;
            }
            if (c == '\0')
                goto exit_sprintf;

            // get another info
            while ((c = *cur++) != '\0') {
                no_exit = 0;
                switch (c) {
                case '%':   // 0x25
                    // specail for "%%"
                    if (no_exit == 0) {
                        *buf++ = c;
                        if (buf >= end)
                            goto exit_sprintf;
                    }
                    else {
                        // not is "%%", do nothing!
                    }
                    break;
                case '.':   // 0x2E
                    // get precision
                    c = *cur++;
                    while (c >= '0' && c <= '9') {
                        precision = precision * 10 + c - '0';
                        c = *cur++;
                    }
                    if (c == '\0')
                        goto exit_sprintf;
                    cur--;
                    no_exit = 1;
                    break;
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                    width = width * 10 + c - '0';
                    no_exit = 1;
                    break;
                case 'c':
                    ch = (jm_uchar)va_arg(args, int);
                    *buf++ = c;
                    if (buf >= end)
                        goto exit_sprintf;
                    break;
                case 'd':
                    // int
                    if ((buf + 11) >= end)
                        goto exit_sprintf;
                    i32 = va_arg(args, int);
                    len = jmc_itoa_radix10(buf, i32);
                    break;
                case 'f':
                    // float & double
                    if ((buf + 32) >= end)
                        goto exit_sprintf;
                    dbl = va_arg(args, double);
                    len = jmc_dtos(buf, dbl);
                    break;
                case 'i':
                    // int64_t
                    if ((buf + 21) >= end)
                        goto exit_sprintf;
                    i64 = va_arg(args, int64_t);
                    len = jmc_i64toa_radix10(buf, i64);
                    break;
                case 'l':
                    // long
                    if ((buf + 11) >= end)
                        goto exit_sprintf;
                    l32 = va_arg(args, long);
                    len = jmc_ltoa_radix10(buf, l32);
                    break;
                case 'p':
                    // void *
                    if ((buf + 32) >= end)
                        goto exit_sprintf;
                    p = va_arg(args, void *);
                    len = jmc_ptohex(buf, p);
                    break;
                case 's':
                    // void *
                    s = va_arg(args, jm_char *);
                    len = jmc_strlen(s);
                    if ((buf + len) >= end)
                        goto exit_sprintf;
                    len = jmc_strcpy(buf, s);
                    break;
                case 't':
                    // uint64_t
                    if ((buf + 20) >= end)
                        goto exit_sprintf;
                    u64 = va_arg(args, uint64_t);
                    len = jmc_u64toa_radix10(buf, u64);
                    break;
                case 'u':
                    // unsigned int
                    if ((buf + 10) >= end)
                        goto exit_sprintf;
                    u32 = va_arg(args, unsigned int);
                    len = jmc_utoa_radix10(buf, u32);
                    break;
                case 'x':
                    // hex32
                    if ((buf + 16) >= end)
                        goto exit_sprintf;
                    hex32 = va_arg(args, uint32_t);
                    len = jmc_uitohex(buf, hex32, FALSE);
                    break;
                default:
                    if (c == 'X') {
                        // HEX32
                        if ((buf + 16) >= end)
                            goto exit_sprintf;
                        hex32 = va_arg(args, uint32_t);
                        len = jmc_uitohex(buf, hex32, TRUE);
                    }
                    else if (c == 'U') {
                        // uint64_t
                        if ((buf + 20) >= end)
                            goto exit_sprintf;
                        u64 = va_arg(args, uint64_t);
                        len = jmc_u64toa_radix10(buf, u64);
                    }
                    else if (c == 'L') {
                        // unsigned long
                        if ((buf + 10) >= end)
                            goto exit_sprintf;
                        ul32 = va_arg(args, unsigned long);
                        len = jmc_ultoa_radix10(buf, ul32);
                    }
                    break;
                }
                if (no_exit == 0)
                    break;
            }
            *buf++ = c;
            if (buf >= end)
                break;
        }
        else {
            // non format info
            *buf++ = c;
            if (buf >= end)
                break;
        }
    }

exit_sprintf:
    return buf;
}

#endif  /* !_JIMIC_STRING_SPRINTF_INL_H_ */
