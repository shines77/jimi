
#ifndef _JIMIC_STDIO_SPRINTF_H_
#define _JIMIC_STDIO_SPRINTF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

JMC_INLINE_NONSTD(int)
jmc_sprintf(char * JMC_RESTRICT buf , const char * JMC_RESTRICT format, ...);

JMC_INLINE_NONSTD(int)
jmc_snprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
             const char * JMC_RESTRICT format, ...);

JMC_INLINE_NONSTD(char *)
jmc_slprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
             const char * JMC_RESTRICT format, ...);

JMC_INLINE_NONSTD(int)
jmc_vsprintf(char * JMC_RESTRICT buf, const char * JMC_RESTRICT format,
             va_list args);

JMC_INLINE_NONSTD(int)
jmc_vsnprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, va_list args);

JMC_INLINE_NONSTD(char *)
jmc_vslprintf(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, va_list args);

#ifdef __cplusplus
}
#endif

#include <jimic/stdio/sprintf.inl.h>

#endif  /* !_JIMIC_STDIO_SPRINTF_H_ */
