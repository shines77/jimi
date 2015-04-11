
#ifndef _JIMIC_STRING_INTEL_JMF_STRLEN32_H_
#define _JIMIC_STRING_INTEL_JMF_STRLEN32_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/platform.h"

#include "jimic/basic/stdint.h"
#include "jimic/basic/declare.h"

#if !defined(_JIMIC_STRING_JMF_STRINGS_H_)
#error "This file [jmf_strlen32.h] only can be inline including in <jmf_strings.h> ."
#endif

#if defined(JIMI_IS_WIN32) && !defined(JIMI_IS_WIN64)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)

size_t jmf_strlen(const char *str);

#else

size_t __CDECL jmf_strlen(const char *str);

#endif // __GNUC__ or __MINGW__

#ifdef __cplusplus
}
#endif

#endif  /* JIMI_IS_WIN32 */

#endif  /* !_JIMIC_STRING_INTEL_JMF_STRLEN32_H_ */
