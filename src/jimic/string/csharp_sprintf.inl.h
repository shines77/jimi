
#ifndef _JIMIC_STRING_CSHARP_SPRINTF_INL_H_
#define _JIMIC_STRING_CSHARP_SPRINTF_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//#include <jimic/string/csharp_sprintf.h>

JMC_INLINE_NONSTD(int)
jmc_csharp_sprintf(jm_char *buf, JM_CONST jm_char *fmt,
                   JM_CONST jm_char *arglist, ...)
{
    int         len;
    va_list     args;

    va_start(args, fmt);
    len = jmc_csharp_vsnprintf(buf, (size_t)-1, fmt, arglist, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(int)
jmc_csharp_snprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt,
                    JM_CONST jm_char *arglist, ...)
{
    int         len;
    va_list     args;

    va_start(args, fmt);
    len = jmc_csharp_vsnprintf(buf, count, fmt, arglist, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(jm_char *)
jmc_csharp_slprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt,
                    JM_CONST jm_char *arglist, ...)
{
    jm_char    *last;
    va_list     args;

    va_start(args, fmt);
    last = jmc_csharp_vslprintf(buf, count, fmt, arglist, args);
    va_end(args);
    return last;
}

JMC_INLINE_NONSTD(int)
jmc_csharp_vsprintf(jm_char *buf, JM_CONST jm_char *fmt,
                    JM_CONST jm_char *arglist, va_list args)
{
    return jmc_csharp_vsnprintf(buf, (size_t)-1, fmt, arglist, args);
}

JMC_INLINE_NONSTD(int)
jmc_csharp_vsnprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt,
                     JM_CONST jm_char *arglist, va_list args)
{
    int         len;
    jm_char    *last;
    last = jmc_csharp_vslprintf(buf, count, fmt, arglist, args);
    len = (int)(last - buf);
    return len;
}

/**
 * Format:
 *   csharp_sprintf("{0:%04d}, {1:%20s}, {2:%0.3f}, {2:%03d}, {4:0x%08X}, {{3}", 1, "2", 3.12, 4, 8989);
 *
 */
JMC_INLINE_NONSTD(jm_char *)
jmc_csharp_vslprintf(jm_char *buf, size_t count, JM_CONST jm_char *fmt,
                     JM_CONST jm_char *arglist, va_list args)
{
    jm_char    *end, *first;
    jm_char    *cur;
    jm_char     c;
    size_t      index;
    int         len;

    jimic_assert(buf != NULL);
    jimic_assert(fmt != NULL);

    cur = (jm_char *)fmt;
    end = buf + count;

    while ((c = *cur++) != '\0') {
        // '{' start
        if (c == '{') {
            first = cur;
            c = *cur++;
            // for "{{"
            if (c != '{') {
                index = c - '0';
                if (index >= 0 && index <= 9) {
                    while ((c = *cur++) != '\0') {
#if 1
                        if (c == ':' && (c = *cur++) == '%') {
#else
                        if (c == ':') {
#endif
                            do {
                                c = *cur++;
                                if ((c >= '0' && c <= '9') || c == '.') {
                                    *buf++ = c;
                                    if (buf >= end)
                                        break;
                                }
                                else {
                                    *buf++ = c;
                                    break;
                                }
                            } while (c != '\0');
                            if (buf >= end)
                                break;
                        }
                        else if (c == '}') {
                            // '}' end of
                            break;
                        }
                        else if (c >= '0' && c <= '9') {
                            index = index * 10 + (c - '0');
                        }
                        else {
                            // get a error index number
#if 0
                            len = cur - first - 1;
                            if (buf + len >= end)
                                break;
#endif
                            // write '{'
                            *buf++ = '{';
                            len = cur - first;
                            if (buf + len >= end)
                                break;
                            // write '0-9'
                            *buf++ = *first++;
                            if (c != '{') {
                                while (first < cur) {
                                    *buf++ = *first++;
                                }
                            }
                            else {
                                while (first < cur - 1) {
                                    *buf++ = *first++;
                                }
                                // if is '{', rollback a char
                                cur--;
                            }
                            break;
                        }
                    }
                    if (buf >= end)
                        break;
                }
                else {
                    // write '{'
                    *buf++ = '{';
                    if ((buf + 1) >= end)
                        break;
                    *buf++ = c;
                }
            }
            else {
                // special for "{{"
                *buf++ = c;
                if (buf >= end)
                    break;
            }
        }
        else {
            // no format info
            *buf++ = c;
            if (buf >= end)
                break;
        }
    }

    return buf;
}

#endif  /* !_JIMIC_STRING_CSHARP_SPRINTF_INL_H_ */
