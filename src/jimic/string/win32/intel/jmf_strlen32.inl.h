
#ifndef _JIMIC_STRING_INTEL_JMF_STRLEN32_H_
#define _JIMIC_STRING_INTEL_JMF_STRLEN32_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>

#include <jimic/core/jimic_def.h>
#include <jimic/core/jimic_asm.h>

#ifndef _JIMIC_STRING_JMF_STRINGS_H_
#error "This file [jmf_strlen32.inl.h] only can be inline including in <jmf_strings.h> ."
#endif

#if defined(JIMI_IS_WIN32) && !defined(JIMI_IS_WIN64)

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

#if (defined(JIMI_IS_MSVC) || defined(JIMI_IS_ICC)) && (defined(JIMI_JMF_USE_ASM) && (JIMI_JMF_USE_ASM != 0))

/* Local STACK = 4 bytes */
#define STACK           4

/* Local ARGS = 0 bytes */
#define ARGS            0

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

/* Local ARGS = 0 bytes */
#define LOCAL_ARG1      DWORD PTR [esp +  0 + STACK]
#define LOCAL_ARG2      DWORD PTR [esp +  4 + STACK]

///////////////////////////////////////////////////////////////////////////

JIMI_INLINE
__declspec(naked)
size_t __CDECL jmf_strlen(const char *str)
{
    __asm {
#if defined(ARGS) && (ARGS > 0)
        sub     esp, ARGS      // # Generate Stack Frame
#endif
#if 0
        push    ebp
        push    edi
        push    esi
#endif
        push    ebx

#if 0
        jmp     strlen_SSE2

        ALIGN_16
strlen_SSE2:
#endif
        mov         eax,  ARG_1                     ; get pointer to string
        mov         ecx,  eax                       ; copy pointer
        pxor        xmm0, xmm0                      ; set xmm0 to zero
        and         ecx,  0x0000001F                ; lower 5 bits indicate misalignment
        cmp         ecx,  0x00000010                ; > 16 bytes ?
        ja          L100

        and         ecx,  0x0000000F                ; lower 4 bits indicate misalignment
        and         eax,  0xFFFFFFF0                ; align pointer by 16 bytes
        movdqa      xmm1, xmmword ptr [eax]         ; read from nearest preceding boundary
        pcmpeqb     xmm1, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    edx,  xmm1                      ; get one bit for each byte result
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L300
        //sub         eax,  0x10
        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        jmp         L200

        ALIGN_16
L100:
        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        movdqa      xmm2, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        movdqa      xmm1, xmmword ptr [eax]         ; read from nearest preceding boundary
        pcmpeqb     xmm2, xmm0                      ; compare high 16 bytes with zero
        pcmpeqb     xmm1, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    ebx,  xmm2                      ; get one bit for each byte result
        pmovmskb    edx,  xmm1                      ; get one bit for each byte result
        shl         ebx,  16                        ; high 16 bits result
        or          edx,  ebx                       ; combined into the result 32 bits
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L300

        ALIGN_16
L200:
        ; Main loop, search 32 bytes at a time
        sub         eax,  -0x20                     ; increment pointer by 32
        movdqa      xmm2, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        movdqa      xmm1, xmmword ptr [eax]         ; read from nearest preceding boundary
        pcmpeqb     xmm2, xmm0                      ; compare high 16 bytes with zero
        pcmpeqb     xmm1, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    ebx,  xmm2                      ; get one bit for each byte result
        pmovmskb    edx,  xmm1                      ; get one bit for each byte result
        shl         ebx,  16                        ; high 16 bits result
        or          edx,  ebx                       ; combined into the result 32 bits
        jz          L200                            ; if is zero goto next loop
        bsf         edx,  edx                       ; find first 1-bit
        ; (moving the bsf out of the loop and using test here would be faster for long strings on old processors,
        ;  but we are assuming that most strings are short, and newer processors have higher priority)
        jz          L200                            ; loop if not found

L300:
        ; Zero-byte found. Compute string length
        sub         eax, ARG_1                      ; subtract start address
        add         eax, edx                        ; add byte index

#if 0
        ALIGN_16
strlen_386:
        mov     eax, 0

        jnz     strlen_386
#endif

        pop     ebx
#if 0
        pop     esi
        pop     edi
        pop     ebp
#endif

#if defined(ARGS) && (ARGS > 0)
        add     esp, ARGS
#endif
        ret
    }
}

#undef STACK
#undef ARGS

#undef ARG_1
#undef ARG_2
#undef ARG_3
#undef ARG_4
#undef ARG_5
#undef ARG_6
#undef ARG_7
#undef ARG_8
#undef ARG_9
#undef ARG_A
#undef ARG_B
#undef ARG_C
#undef ARG_D
#undef ARG_E
#undef ARG_F

#undef LOCAL_ARG1
#undef LOCAL_ARG2

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

#endif  /* JIMI_IS_WIN32 */

#endif  /* _JIMIC_STRING_INTEL_JMF_STRLEN32_H_ */
