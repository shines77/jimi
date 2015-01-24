
#include "jimic/string/jmf_strings.h"

#include "jimic/basic/asm.h"

#if defined(JIMI_IS_WIN64)

#if (defined(JIMI_IS_MSVC) || defined(JIMI_IS_ICC)) \
    && (defined(JIMI_JMF_USE_ASM) && (JIMI_JMF_USE_ASM != 0)) \
    && (!defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0))

/* Base Stack: rcx, rdx, r8, r9 = 4 x 8 = 32 bytes */
#define STACK_BASE      32

/* Local STACK = 0 bytes */
#define STACK           (STACK_BASE + 0)

/* Local ARGS = 0 bytes */
#define ARGS            0

#if defined(JIMI_IS_WINDOWS)
#define ARG_1           rcx
#define ARG_2           rdx
#define ARG_3           r8
#define ARG_4           r9
#else
#define ARG_1           rdi
#define ARG_2           rdx
#define ARG_3           r8
#define ARG_4           r9
#endif  /* JIMI_IS_WINDOWS */

#define ARG_5           qword ptr [rsp +  8 + STACK + ARGS]
#define ARG_6           qword ptr [rsp + 16 + STACK + ARGS]
#define ARG_7           qword ptr [rsp + 24 + STACK + ARGS]
#define ARG_8           qword ptr [rsp + 32 + STACK + ARGS]
#define ARG_9           qword ptr [rsp + 40 + STACK + ARGS]
#define ARG_A           qword ptr [rsp + 48 + STACK + ARGS]
#define ARG_B           qword ptr [rsp + 56 + STACK + ARGS]
#define ARG_C           qword ptr [rsp + 64 + STACK + ARGS]
#define ARG_D           qword ptr [rsp + 72 + STACK + ARGS]
#define ARG_E           qword ptr [rsp + 80 + STACK + ARGS]
#define ARG_F           qword ptr [rsp + 88 + STACK + ARGS]

/* Local ARGS = 0 bytes */
#define LOCAL_ARG1      qword ptr [rsp +  0 + STACK]
#define LOCAL_ARG2      qword ptr [rsp +  8 + STACK]

///////////////////////////////////////////////////////////////////////////

__declspec(naked)
//size_t __FASTCALL jmf_strlwr(const char *str)
size_t jmf_strlwr(char *str)
{
    __asm {
#if defined(ARGS) && (ARGS > 0)
        sub         rsp, ARGS      // # Generate Stack Frame
#endif
#if 0
        push        rbp
        push        rdi
        push        rsi
        push        rbx
#endif

#if 0
        jmp     strlen_SSE2

        ALIGN_16
strlen_SSE2:
#endif

#if defined(JIMI_IS_WINDOWS)
#define Rscopy      r8
        mov         rax,  ARG_1                     ; get pointer to string
        mov         Rscopy, ARG_1                   ; copy pointer
#else  /* Unix */
#define Rscopy      ARG_1
        mov         rax,  ARG_1                     ; get pointer to string
        mov         ecx,  Rscopy                    ; copy pointer
#endif  /* JIMI_IS_WINDOWS */

        pxor        xmm0, xmm0                      ; set xmm0 to zero
        xor         rdx,  rdx                       ; set rdx to zero
        and         ecx,  0x0000001F                ; lower 5 bits indicate misalignment
        cmp         ecx,  0x00000010                ; > 16 bytes ?
        ja          L100

        and         ecx,  0x0000000F                ; lower 4 bits indicate misalignment
        and         rax,  0xFFFFFFFFFFFFFFF0        ; align pointer by 16 bytes
        movdqa      xmm1, xmmword ptr [rax]         ; read from nearest preceding boundary
        pcmpeqb     xmm1, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    edx,  xmm1                      ; get one bit for each byte result
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L300
        and         rax,  0xFFFFFFFFFFFFFFE0        ; align pointer by 32 bytes
        jmp         L200

        ALIGN_16
L100:
        and         rax,  0xFFFFFFFFFFFFFFE0        ; align pointer by 32 bytes
        movdqa      xmm2, xmmword ptr [rax + 16]    ; read from nearest preceding boundary
        movdqa      xmm1, xmmword ptr [rax]         ; read from nearest preceding boundary
        pcmpeqb     xmm2, xmm0                      ; compare high 16 bytes with zero
        pcmpeqb     xmm1, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    r9d,  xmm2                      ; get one bit for each byte result
        pmovmskb    edx,  xmm1                      ; get one bit for each byte result
        shl         r9d,  16                        ; high 16 bits result
        or          edx,  r9d                       ; combined into the result 32 bits
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L300

        ALIGN_16
L200:
        ; Main loop, search 32 bytes at a time
        sub         rax,  -0x20                     ; increment pointer by 32
        movdqa      xmm2, xmmword ptr [rax + 16]    ; read from nearest preceding boundary
        movdqa      xmm1, xmmword ptr [rax]         ; read from nearest preceding boundary
        pcmpeqb     xmm2, xmm0                      ; compare high 16 bytes with zero
        pcmpeqb     xmm1, xmm0                      ; compare low  16 bytes with zero
        pmovmskb    r9d,  xmm2                      ; get one bit for each byte result
        pmovmskb    edx,  xmm1                      ; get one bit for each byte result
        shl         r9d,  16                        ; high 16 bits result
        or          edx,  r9d                       ; combined into the result 32 bits
        jz          L200                            ; if is zero goto next loop
        bsf         edx,  edx                       ; find first 1-bit
        ; (moving the bsf out of the loop and using test here would be faster for long strings on old processors,
        ;  but we are assuming that most strings are short, and newer processors have higher priority)
        jz          L200                            ; loop if not found

L300:
        ; Zero-byte found. Compute string length
        sub         rax, Rscopy                     ; subtract start address
        add         rax, rdx                        ; add byte index

#if 0
        ALIGN_16
strlen_386:
        mov         rax, 0

        jnz         strlen_386
#endif

#if 0
        pop         rbx
        pop         rsi
        pop         rdi
        pop         rbp
#endif

#if defined(ARGS) && (ARGS > 0)
        add         rsp, ARGS
#endif
        ret
    }
}

#undef Rscopy

#undef STACK_BASE
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

#include "jimic/string/jm_strings.h"

size_t jmf_strlwr(char *str)
{
    jm_strlwr(str, (size_t)-1);
    return jm_strlen(str);
}

///////////////////////////////////////////////////////////////////////////

#endif  /* defined(JIMI_JMF_USE_ASM) */

#endif  /* JIMI_IS_WIN64 */

