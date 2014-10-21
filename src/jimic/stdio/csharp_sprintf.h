
#ifndef _JIMIC_STDIO_CSHARP_SPRINTF_H_
#define _JIMIC_STDIO_CSHARP_SPRINTF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

JMC_INLINE_NONSTD(int)
jmc_csharp_sprintf(char * JMC_RESTRICT buf, jm_const char * JMC_RESTRICT fmt,
                   const char *arglist, ...);

JMC_INLINE_NONSTD(int)
jmc_csharp_snprintf(char * JMC_RESTRICT buf, size_t count,
                    const char * JMC_RESTRICT fmt,
                    const char *arglist, ...);

JMC_INLINE_NONSTD(char *)
jmc_csharp_slprintf(char * JMC_RESTRICT buf, size_t count,
                    const char * JMC_RESTRICT fmt,
                    const char * JMC_RESTRICT arglist, ...);

JMC_INLINE_NONSTD(int)
jmc_csharp_vsprintf(char * JMC_RESTRICT buf, jm_const char * JMC_RESTRICT fmt,
                    const char * JMC_RESTRICT arglist, va_list args);

JMC_INLINE_NONSTD(int)
jmc_csharp_vsnprintf(char * JMC_RESTRICT buf, size_t count,
                     const char * JMC_RESTRICT fmt,
                     const char * JMC_RESTRICT arglist, va_list args);

JMC_INLINE_NONSTD(char *)
jmc_csharp_vslprintf(char * JMC_RESTRICT buf, size_t count,
                     const char * JMC_RESTRICT fmt,
                     const char * JMC_RESTRICT arglist, va_list args);

#ifdef __cplusplus
}
#endif

#include <jimic/stdio/csharp_sprintf.inl.h>

#endif  /* !_JIMIC_STDIO_CSHARP_SPRINTF_H_ */
