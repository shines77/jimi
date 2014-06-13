
#ifndef _JIMIC_STRING_INTEL_JMF_STRLEN64_H_
#define _JIMIC_STRING_INTEL_JMF_STRLEN64_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>

#include <jimic/core/jimic_def.h>
#include <jimic/core/jimic_asm.h>

#ifndef _JIMIC_STRING_JMF_STRINGS_H_
#error "This file [jmf_strlen64.inl.h] only can be inline including in <jmf_strings.h> ."
#endif

#if defined(JIMI_IS_WIN64)

///////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

JIMI_INLINE
size_t __CDECL jmf_strlen(const char *str);

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////

#if (defined(JIMI_IS_MSVC) || defined(JIMI_IS_ICC)) \
    && (defined(JIMI_JMF_USE_ASM) && (JIMI_JMF_USE_ASM != 0))

#define STACK           16
#define ARGS            16

#define ARG_1           DWORD PTR [esp +  4 + STACK + ARGS]
#define ARG_2           DWORD PTR [esp +  8 + STACK + ARGS]
#define ARG_3           DWORD PTR [esp + 12 + STACK + ARGS]
#define ARG_4           DWORD PTR [esp + 16 + STACK + ARGS]
#define ARG_5           DWORD PTR [esp + 20 + STACK + ARGS]
#define ARG_6           DWORD PTR [esp + 24 + STACK + ARGS]
#define ARG_7           DWORD PTR [esp + 28 + STACK + ARGS]
#define ARG_8           DWORD PTR [esp + 32 + STACK + ARGS]
#define ARG_9           DWORD PTR [esp + 36 + STACK + ARGS]
#define ARG_A           DWORD PTR [esp + 40 + STACK + ARGS]
#define ARG_B           DWORD PTR [esp + 44 + STACK + ARGS]
#define ARG_C           DWORD PTR [esp + 48 + STACK + ARGS]
#define ARG_D           DWORD PTR [esp + 52 + STACK + ARGS]
#define ARG_E           DWORD PTR [esp + 56 + STACK + ARGS]
#define ARG_F           DWORD PTR [esp + 60 + STACK + ARGS]

///////////////////////////////////////////////////////////////////////////

JIMI_INLINE
__declspec(naked) __CDECL
size_t jmf_strlen(const char *str)
{
    __asm {
        sub     esp, ARGS      // # Generate Stack Frame

        push    ebp
        push    edi
        push    esi
        push    ebx

        ALIGN_16
strlen_SSE2:


        ALIGN_16
strlen_386:
        mov     eax, 0

        ALIGN_16
L999:
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp

        add     esp, ARGS
        ret
    }
}

///////////////////////////////////////////////////////////////////////////

#else  /* !defined(JIMI_JMF_USE_ASM) */

///////////////////////////////////////////////////////////////////////////

#include <jimic/string/jm_strings.h>

JIMI_INLINE __CDECL
size_t __cdecl jmf_strlen(const char *str)
{
    return ::jm_strlen(str);
}

///////////////////////////////////////////////////////////////////////////

#endif  /* defined(JIMI_JMF_USE_ASM) */

#endif  /* JIMI_IS_WIN64 */

#endif  /* _JIMIC_STRING_INTEL_JMF_STRLEN64_H_ */
