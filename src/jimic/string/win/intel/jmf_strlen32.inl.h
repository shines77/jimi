
#ifndef _JIMIC_STRING_INTEL_JMF_STRLEN32_H_
#define _JIMIC_STRING_INTEL_JMF_STRLEN32_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>

#if defined(JIMI_IS_WINDOWS)
//
#endif  /* JIMI_IS_WINDOWS */

#include <jimic/core/jimic_def.h>
#include <jimic/core/jimic_asm.h>

#if defined(JIMI_IS_WINDOWS)

///////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

JIMI_INLINE
size_t __cdecl jmf_strlen(const char *str);

#ifdef __cplusplus
}
#endif

JIMI_INLINE
__declspec(naked)
size_t jmf_strlen(const char *str)
{
    __asm {
        mov     eax, 0
        ret
    }
}

///////////////////////////////////////////////////////////////////////////

#else  /* !JIMI_IS_WINDOWS */

///////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

inline
size_t __cdecl jmf_strlen(const char *str);

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////

#endif  /* JIMI_IS_WINDOWS */



#endif  /* _JIMIC_STRING_INTEL_JMF_STRLEN32_H_ */
