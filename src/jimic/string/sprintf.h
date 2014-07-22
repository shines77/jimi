
#ifndef _JIMIC_STRING_SPRINTF_H_
#define _JIMIC_STRING_SPRINTF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_def.h>
#include <jimic/core/jimic_def.h>

#include <vadefs.h>

#ifdef __cplusplus
extern "C" {
#endif

JMC_INLINE_NONSTD(int)
jmc_sprintf(jm_char *buf, JM_CONST jm_char *fmt, ...);

JMC_INLINE_NONSTD(int)
jmc_snprintf(jm_char *buf, size_t countOfElements, size_t count,
             JM_CONST jm_char *fmt, ...);

JMC_INLINE_NONSTD(jm_char *)
jmc_slprintf(jm_char *buf, size_t countOfElements, size_t count,
             JM_CONST jm_char *fmt, ...);

JMC_INLINE_NONSTD(int)
jmc_vsprintf(jm_char *buf, JM_CONST jm_char *fmt, va_list args);

JMC_INLINE_NONSTD(int)
jmc_vsnprintf(jm_char *buf, size_t countOfElements, size_t count,
                    JM_CONST jm_char *fmt, va_list args);

JMC_INLINE_NONSTD(jm_char *)
jmc_vslprintf(jm_char *buf, size_t countOfElements, size_t count,
                    JM_CONST jm_char *fmt, va_list args);

#ifdef __cplusplus
}
#endif

#include <jimic/string/sprintf.inl.h>

#endif  /* !_JIMIC_STRING_SPRINTF_H_ */
