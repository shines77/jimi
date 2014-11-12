
#ifndef _JIMIC_STDIO_SPRINTF_INL_H_
#define _JIMIC_STDIO_SPRINTF_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STDIO_SPRINTF_H_
#error "Don't include the internal file <jimic/stdio/sprintf.inl.h> directly; include <jimic/stdio/sprintf.h> instead."
#endif

#include <stdarg.h>

JMC_INLINE_NONSTD(int)
jmc_sprintf(char * JMC_RESTRICT buf, const char * JMC_RESTRICT format, ...)
{
    int     len;
    va_list args;

    va_start(args, format);
    len = jmc_vsnprintf(buf, (size_t)-1, (size_t)-1, format, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(int)
jmc_snprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
             const char * JMC_RESTRICT format, ...)
{
    int     len;
    va_list args;

    va_start(args, format);
    len = jmc_vsnprintf(buf, count_max, count, format, args);
    va_end(args);
    return len;
}

JMC_INLINE_NONSTD(char *)
jmc_slprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
             const char * JMC_RESTRICT format, ...)
{
    char   *last;
    va_list args;

    va_start(args, format);
    last = jmc_vslprintf(buf, count_max, count, format, args);
    va_end(args);
    return last;
}

JMC_INLINE_NONSTD(int)
jmc_vsprintf(char * JMC_RESTRICT buf, const char * JMC_RESTRICT format,
             va_list args)
{
    return jmc_vsnprintf(buf, (size_t)-1, (size_t)-1, format, args);
}

JMC_INLINE_NONSTD(int)
jmc_vsnprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, va_list args)
{
    int     len;
    char   *last;

    last = jmc_vslprintf(buf, count_max, count, format, args);
    len = (int)(last - buf);
    return len;
}

#endif  /* !_JIMIC_STDIO_SPRINTF_INL_H_ */
