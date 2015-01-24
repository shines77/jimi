
#ifndef _JIMIC_STRING_INTEL_JMF_STRLWR64_H_
#define _JIMIC_STRING_INTEL_JMF_STRLWR64_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/jimic_platform_def.h"

#include "jimic/basic/jimic_stdint.h"
#include "jimic/basic/jimic_declare.h"

#if !defined(_JIMIC_STRING_JMF_STRINGS_H_)
#error "This file [jmf_strlen64.h] only can be inline including in <jmf_strings.h> ."
#endif

#if defined(JIMI_IS_WIN64)

#ifdef __cplusplus
extern "C" {
#endif

//size_t __FASTCALL jmf_strlen(const char *str);
size_t jmf_strlen(const char *str);

#ifdef __cplusplus
}
#endif

#endif  /* JIMI_IS_WIN64 */

#endif  /* _JIMIC_STRING_INTEL_JMF_STRLWR64_H_ */
