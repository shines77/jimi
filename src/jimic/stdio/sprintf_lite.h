
#ifndef _JIMIC_STDIO_SPRINTF_LITE_H_
#define _JIMIC_STDIO_SPRINTF_LITE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_stdint.h"
#include "jimic/core/jimic_declare.h"

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

JMC_DECLARE_NONSTD(int)
sprintf_lite(char * JMC_RESTRICT buf , const char * JMC_RESTRICT format, ...);

JMC_DECLARE_NONSTD(int)
snprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, ...);

JMC_DECLARE_NONSTD(char *)
slprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
              const char * JMC_RESTRICT format, ...);

JMC_DECLARE_NONSTD(int)
vsprintf_lite(char * JMC_RESTRICT buf, const char * JMC_RESTRICT format,
              va_list args);

JMC_DECLARE_NONSTD(int)
vsnprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
               const char * JMC_RESTRICT format, va_list args);

JMC_DECLARE_NONSTD(char *)
vslprintf_lite(char * JMC_RESTRICT buf, size_t count_max, size_t count,
               const char * JMC_RESTRICT format, va_list args);

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_STDIO_SPRINTF_LITE_H_ */
