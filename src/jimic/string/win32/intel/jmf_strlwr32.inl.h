
#ifndef _JIMIC_STRING_INTEL_JMF_STRLWR32_H_
#define _JIMIC_STRING_INTEL_JMF_STRLWR32_H_

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

#define ARG_1           dword ptr [esp +  4 + STACK + ARGS]
#define ARG_2           dword ptr [esp +  8 + STACK + ARGS]
#define ARG_3           dword ptr [esp + 12 + STACK + ARGS]
#define ARG_4           dword ptr [esp + 16 + STACK + ARGS]
#define ARG_5           dword ptr [esp + 20 + STACK + ARGS]
#define ARG_6           dword ptr [esp + 24 + STACK + ARGS]
#define ARG_7           dword ptr [esp + 28 + STACK + ARGS]
#define ARG_8           dword ptr [esp + 32 + STACK + ARGS]
#define ARG_9           dword ptr [esp + 36 + STACK + ARGS]
#define ARG_A           dword ptr [esp + 40 + STACK + ARGS]
#define ARG_B           dword ptr [esp + 44 + STACK + ARGS]
#define ARG_C           dword ptr [esp + 48 + STACK + ARGS]
#define ARG_D           dword ptr [esp + 52 + STACK + ARGS]
#define ARG_E           dword ptr [esp + 56 + STACK + ARGS]
#define ARG_F           dword ptr [esp + 60 + STACK + ARGS]

/* Local ARGS = 0 bytes */
#define LOCAL_ARG1      dword ptr [esp +  0 + STACK]
#define LOCAL_ARG2      dword ptr [esp +  4 + STACK]

///////////////////////////////////////////////////////////////////////////

/* front of 'A' is '@', and the ascii of '@' is 0x40 */
static const uint64_t   LOWER_CHAR_A_FIRST      = 0x4040404040404040ULL;
/* next of the 'Z' is '[', and the ascii of '[' is 0x5B */
static const uint64_t   LOWER_CHAR_Z_LAST       = 0x5B5B5B5B5B5B5B5BULL;
/* not LOWER_CHAR_A_FIRST */
static const uint64_t   LOWER_CHAR_A_FIRST_NOT  = ~LOWER_CHAR_A_FIRST;
/* the magic bits of transform to lower or upper */
static const uint64_t   LOWER_CHAR_MAGIC_SIGN   = 0x2020202020202020ULL;

JIMI_INLINE
__declspec(naked)
size_t __CDECL jmf_strlwr(char *str)
{
    __asm {
#if defined(ARGS) && (ARGS > 0)
        sub         esp, ARGS      // # Generate Stack Frame
#endif
#if 0
        push        ebp
        push        edi
        push        esi
#endif
        push        ebx

#if 0
        jmp         strlwr_SSE2

        ALIGN_16
strlwr_SSE2:
#endif
        mov         eax,  ARG_1                     ; get pointer to string
        mov         ecx,  eax                       ; copy pointer
        movq        xmm0, LOWER_CHAR_A_FIRST        ; char[] of '@@@@@@@@'
        movq        xmm1, LOWER_CHAR_Z_LAST         ; char[] of '[[[[[[[['
        movq        xmm6, LOWER_CHAR_MAGIC_SIGN     ; char[] of '        '
        punpcklqdq  xmm0, xmm0                      ;
        punpcklqdq  xmm1, xmm1                      ;
        punpcklqdq  xmm6, xmm6                      ;
        pxor        xmm7, xmm7                      ; all set to bit "0"
        and         ecx,  0x0000001F                ; lower 5 bits indicate misalignment
        jz          L050                            ;
        cmp         ecx,  0x00000010                ; > 16 bytes ?
        ja          L100

        and         ecx,  0x0000000F                ; lower 4 bits indicate misalignment
        and         eax,  0xFFFFFFF0                ; align pointer by 16 bytes
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        pcmpeqb     xmm2, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    edx,  xmm2                      ; get one bit for each byte result
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L300
        //sub         eax,  0x10
        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        jmp         L200

L050:
        sub         eax,  0x20                      ; decrement pointer by 32
        jmp         L200

        ALIGN_16
L100:
        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm3, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        pcmpeqb     xmm2, xmm0                      ; compare low  16 bytes with zero
        pcmpeqb     xmm3, xmm0                      ; compare high 16 bytes with zero
        pmovmskb    edx,  xmm2                      ; get one bit for each byte result
        pmovmskb    ebx,  xmm3                      ; get one bit for each byte result
        shl         ebx,  16                        ; high 16 bits result
        or          edx,  ebx                       ; combined into the result 32 bits
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L400

        ALIGN_16
L200:
        ; Main loop, search 32 bytes at a time
        sub         eax,  -0x20                     ; increment pointer by 32
        movdqa      xmm3, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm4, xmm2                      ;
        movdqa      xmm5, xmm3                      ;
        pcmpeqb     xmm3, xmm7                      ; compare high 16 bytes with zero
        pcmpeqb     xmm2, xmm7                      ; compare low  16 bytes with zero
        pmovmskb    ebx,  xmm3                      ; get one bit for each byte result
        pmovmskb    edx,  xmm2                      ; get one bit for each byte result
        shl         ebx,  16                        ; high 16 bits result
        or          edx,  ebx                       ; combined into the result 32 bits
        jz          L300                            ; if is not zero goto end, otherwise goto tolower
        bsf         edx,  edx                       ; find first 1-bit
        ; (moving the bsf out of the loop and using test here would be faster for long strings on old processors,
        ;  but we are assuming that most strings are short, and newer processors have higher priority)
        jnz         L400                            ; loop if not found

        ALIGN_16
L300:
        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm3, xmm5                      ;
        pcmpgtb     xmm2, xmm1                      ; great than '[[[[[[[[' ?
        pcmpgtb     xmm3, xmm1                      ; great than '[[[[[[[[' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pcmpgtb     xmm5, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pandn       xmm3, xmm5                      ;
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL
        pand        xmm3, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        /* save_string */
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movdqa      xmmword ptr [eax + 16], xmm5    ;
        jmp         L200                            ; jump to Main loop

        /* the remain of less 32 bytes */
L400:
        pcmpeqb     xmm7, xmm7                      ; all set to bit "1"

        mov         ecx, 32                         ;
        sub         ecx, edx                        ;
        shl         ecx, 3                          ; ecx = (32 - edx) * 8
        cmp         ecx, 0x00000080                 ; when (ecx < 128 bytes, edx > 16) ?
        jb          L500                            ;

        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm5, xmm4                      ;
        pcmpgtb     xmm2, xmm1                      ; great than '[[[[[[[[' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        cmp         ecx, 0x000000C0                 ; when (ecx < 192 bytes, edx > 8) ?
        jb          L420                            ;

        /* when (ecx = [192, 256), edx = (0, 8]) */
        sub         ecx,  192                       ;
        movdqa      xmm3, xmm7                      ;
        psrlq       xmm3, ecx                       ;
        pand        xmm2, xmm3                      ;
        /* save_string */
        por         xmm5, xmm2                      ;
        movq        qword ptr [eax], xmm5           ;
        jmp         L600                            ;

L420:
        /* when (ecx = [128, 192), edx = (8, 16]) */
        sub         ecx,  128                       ;
        movdqa      xmm3, xmm7                      ;
        psrlq       xmm3, ecx                       ;
//      shufpd      xmm7, xmm3, 0x02                ; 取xmm3的高64位 + xmm7的高64位
        unpckhpd    xmm7, xmm3                      ; 取xmm3的高64位 + xmm7的高64位
        pand        xmm2, xmm7                      ;
        /* save_string */
        por         xmm5, xmm2                      ;
        movq        qword ptr [eax], xmm5           ;
        jmp         L600                            ;

        ALIGN_16
L500:
        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm3, xmm5                      ;
        pcmpgtb     xmm2, xmm1                      ; great than '[[[[[[[[' ?
        pcmpgtb     xmm3, xmm1                      ; great than '[[[[[[[[' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pcmpgtb     xmm5, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pandn       xmm3, xmm5                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL
        pand        xmm3, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        cmp         ecx, 0x00000040                 ; when (ecx < 64 bytes, edx > 24) ?
        jb          L520                            ;

        /* when (ecx = [64, 128), edx = (16, 24]) */
        sub         ecx,  64                        ;

        movdqa      xmm6, xmm7                      ;
        psrlq       xmm6, ecx                       ;
        pand        xmm3, xmm6                      ;
        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movq        qword ptr [eax + 16], xmm5      ;
        jmp         L600                            ;

L520:
        /* when (ecx = [0, 64), edx = (24, 32]) */
        movdqa      xmm6, xmm7                      ;
        psrlq       xmm6, ecx                       ;
//      shufpd      xmm7, xmm6, 0x02                ; 取xmm6的高64位 + xmm7的高64位
        unpckhpd    xmm7, xmm6                      ; 取xmm6的高64位 + xmm7的高64位
        pand        xmm3, xmm7                      ;

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movdqa      xmmword ptr [eax + 16], xmm5    ;

L600:
        ; Zero-byte found. Compute string length
        sub         eax, ARG_1                      ; subtract start address
        add         eax, edx                        ; add byte index

#if 0
        ALIGN_16
strlwr_386:
        mov         eax, 0

        jnz         strlwr_386
#endif

        pop         ebx
#if 0
        pop         esi
        pop         edi
        pop         ebp
#endif

#if defined(ARGS) && (ARGS > 0)
        add         esp, ARGS
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
size_t __cdecl jmf_strlwr(char *str)
{
    return ::jm_strlwr(str);
}

///////////////////////////////////////////////////////////////////////////

#endif  /* defined(JIMI_JMF_USE_ASM) */

#endif  /* JIMI_IS_WIN32 */

#endif  /* _JIMIC_STRING_INTEL_JMF_STRLWR32_H_ */
