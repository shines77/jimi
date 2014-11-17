
#include "jimic/string/jmf_strings.h"

#include "jimic/core/jimic_asm.h"

#if defined(JIMI_IS_WIN32) && !defined(JIMI_IS_WIN64)

#if (defined(JIMI_IS_MSVC) || defined(JIMI_IS_ICC)) \
    && (defined(JIMI_JMF_USE_ASM) && (JIMI_JMF_USE_ASM != 0)) \
    && (!defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0))

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
/* the 'Z', the ascii of 'Z' is 0x5A */
static const uint64_t   LOWER_CHAR_Z            = 0x5A5A5A5A5A5A5A5AULL;
/* not LOWER_CHAR_A_FIRST, value = ~LOWER_CHAR_A_FIRST_NOT; */
static const uint64_t   LOWER_CHAR_A_FIRST_NOT  = 0xBFBFBFBFBFBFBFBFULL;
/* the magic bits of transform to lower or upper */
static const uint64_t   LOWER_CHAR_MAGIC_SIGN   = 0x2020202020202020ULL;

__declspec(naked)
size_t __CDECL jmf_strlwr(char *str)
{
    __asm {
#if defined(ARGS) && (ARGS > 0)
        sub         esp, ARGS      // # Generate Stack Frame
#endif
#if 0
        push        edi
        push        esi
        push        ebp
#endif
        push        ebx

#if 0
        jmp         strlwr_SSE2

        ALIGN_16
strlwr_SSE2:
#endif
        mov         eax,  ARG_1                     ; get pointer to string
        movq        xmm0, LOWER_CHAR_A_FIRST        ; string of '@@@@@@@@'
        movq        xmm1, LOWER_CHAR_Z              ; string of 'ZZZZZZZZ'
        movq        xmm6, LOWER_CHAR_MAGIC_SIGN     ; string of '        '
        mov         ecx,  eax                       ; copy pointer
        punpcklqdq  xmm0, xmm0                      ;
        punpcklqdq  xmm1, xmm1                      ;
        punpcklqdq  xmm6, xmm6                      ;
        pxor        xmm7, xmm7                      ; all set to bit "0"
        and         ecx,  0x0000001F                ; lower 5 bits indicate misalignment
        jz          L220                            ;
        cmp         ecx,  0x00000010                ; < 16 bytes ?
        jb          L100

        mov         ebx,  ecx                       ;
        and         eax,  0xFFFFFFF0                ; align pointer by 16 bytes
        and         ecx,  0x0000000F                ; lower 4 bits indicate misalignment
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm4, xmm2                      ;
        pcmpeqb     xmm2, xmm7                      ; compare low  16 bytes with zero
        pmovmskb    edx,  xmm2                      ; get one bit for each byte result
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L600

//      pshufd      xmm4, xmm4, 0x00                ; 复制4份低32位到128位

        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm5, xmm4                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL        

        shl         ecx,  3                         ; ecx = ecx * 8
        cmp         ecx,  0x00000040                ; when (ecx < 64 bytes) ?
        jb          L050                            ;

        /* when (ecx = [64, 128)) */
        sub         ecx,  64                        ;

        pxor        xmm3, xmm3                      ; all set to bit "0"
        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ecx                       ;
        psllq       mm0,  mm1                       ;
        movq2dq     xmm4, mm0                       ;
//      shufpd      xmm3, xmm4, 0x00                ; 取xmm4的低64位 + xmm3的低64位
        unpcklpd    xmm3, xmm4                      ; 取xmm4的低64位 + xmm3的低64位(高位在前, 下同)

        pand        xmm2, xmm3                      ;

        /* save_string */
        por         xmm5, xmm2                      ;
        movdqa      xmmword ptr [eax], xmm5         ;

        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        jmp         L200                            ;

//      ALIGN_16
L050:
        /* when (ecx = [0, 64)) */

        pcmpeqb     xmm3, xmm3                      ; all set to bit "1"
        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ecx                       ;
        psllq       mm0,  mm1                       ;
        movq2dq     xmm4, mm0                       ;
//      shufpd      xmm4, xmm3, 0x00                ; 取xmm3的低64位 + xmm4的低64位
        unpcklpd    xmm4, xmm3                      ; 取xmm3的低64位 + xmm4的低64位(高位在前, 下同)

        pand        xmm2, xmm4                      ;

        /* save_string */
        por         xmm5, xmm2                      ;
        movdqa      xmmword ptr [eax], xmm5         ;

        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        jmp         L200                            ;

        ALIGN_16
L100:
        and         eax,  0xFFFFFFE0                ; align pointer by 32 bytes
        movdqa      xmm3, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmm3                      ;
        movdqa      xmm4, xmm2                      ;
        pcmpeqb     xmm3, xmm7                      ; compare high 16 bytes with zero
        pcmpeqb     xmm2, xmm7                      ; compare low  16 bytes with zero
        pmovmskb    ebx,  xmm3                      ; get one bit for each byte result
        pmovmskb    edx,  xmm2                      ; get one bit for each byte result
        shl         ebx,  16                        ; high 16 bits result
        or          edx,  ebx                       ; combined into the result 32 bits
        mov         ebx,  ecx                       ;
        shr         edx,  cl                        ; shift out false bits
        shl         edx,  cl                        ; shift back again
        bsf         edx,  edx                       ; find first 1-bit
        jnz         L600

        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm3, xmm5                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm3, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pcmpgtb     xmm5, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pandn       xmm3, xmm5                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL
        pand        xmm3, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        shl         ecx,  3                         ; ecx = ecx * 8
        cmp         ecx,  0x00000040                ; when (ecx < 64 bytes) ?
        jb          L150                            ;

        /* when (ecx = [64, 128)) */
        sub         ecx,  64                        ;

        pxor        xmm5, xmm5                      ; all set to bit "0"
        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ecx                       ;
        psllq       mm0,  mm1                       ;
        movq2dq     xmm4, mm0                       ;
        unpcklpd    xmm5, xmm4                      ; 取xmm4的低64位 + xmm5的低64位(高位在前, 下同)

        pand        xmm2, xmm5                      ;

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
//      unpcklpd    xmm4, xmm4                      ;
//      movq        qword ptr [eax + 8], xmm4       ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movdqa      xmmword ptr [eax + 16], xmm5    ;

        jmp         L200                            ;

//      ALIGN_16
L150:
        /* when (ecx = [0, 64)) */

        pcmpeqb     xmm4, xmm4                      ; all set to bit "1"
        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ecx                       ;
        psllq       mm0,  mm1                       ;
        movq2dq     xmm5, mm0                       ;
//      shufpd      xmm5, xmm4, 0x00                ; 取xmm4的低64位 + xmm5的低64位
        unpcklpd    xmm5, xmm4                      ; 取xmm4的低64位 + xmm5的低64位(高位在前, 下同)

        pand        xmm2, xmm5                      ;

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movdqa      xmmword ptr [eax + 16], xmm5    ;

#if 0
        ALIGN_16
L200:
        ; Main loop, search 32 bytes at a time
        sub         eax,  -0x20                     ; increment pointer by 32
L220:
        movdqa      xmm3, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmm3                      ;
        movdqa      xmm4, xmm2                      ;
        pcmpeqb     xmm3, xmm7                      ; compare high 16 bytes with zero
        pcmpeqb     xmm2, xmm7                      ; compare low  16 bytes with zero
        pmovmskb    ebx,  xmm3                      ; get one bit for each byte result
        pmovmskb    edx,  xmm2                      ; get one bit for each byte result
        shl         ebx,  16                        ; high 16 bits result
        or          edx,  ebx                       ; combined into the result 32 bits
        jnz         L350                            ; if is not zero goto end, otherwise goto tolower
L300:
        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm3, xmm5                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm3, xmm1                      ; great than 'ZZZZZZZZ' ?
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

L350:
        bsf         edx,  edx                       ; find first 1-bit
        ; (moving the bsf out of the loop and using test here would be faster for long strings on old processors,
        ;  but we are assuming that most strings are short, and newer processors have higher priority)
        jz          L300                            ; loop if not found
#else
        ALIGN_16
L200:
        ; Main loop, search 32 bytes at a time
        sub         eax,  -0x20                     ; increment pointer by 32
L220:
        movdqa      xmm3, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        movdqa      xmm2, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmm3                      ;
        movdqa      xmm4, xmm2                      ;
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
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm3, xmm1                      ; great than 'ZZZZZZZZ' ?
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

        ALIGN_16
        /* the remain of less 32 bytes */
L400:
#endif
        cmp         edx,  0                         ;
        jz          L999                            ;

        mov         ebx,  32                        ;
        sub         ebx,  edx                       ;
        shl         ebx,  3                         ; ebx = (32 - edx) * 8
        cmp         ebx,  0x00000080                ; when (ebx < 128 bytes, edx > 16) ?
        jb          L500                            ;

        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm5, xmm4                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        cmp         ebx,  0x000000C0                ; when (ebx < 192 bytes, edx > 8) ?
        jb          L450                            ;

        /* when (ebx = [192, 256), edx = (0, 8]) */
        sub         ebx,  192                       ;

        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm3, mm0                       ;

        pand        xmm2, xmm3                      ;

        /* save_string */
        por         xmm5, xmm2                      ;
        movq        qword ptr [eax], xmm5           ;
        jmp         L999                            ;

//      ALIGN_16
L450:
        /* when (ebx = [128, 192), edx = (8, 16]) */
        sub         ebx,  128                       ;

        pcmpeqb     xmm3, xmm3                      ; all set to bit "1"
        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm4, mm0                       ;
//      shufpd      xmm3, xmm4, 0x00                ; 取xmm4的低64位 + xmm3的低64位
        unpcklpd    xmm3, xmm4                      ; 取xmm4的低64位 + xmm3的低64位(高位在前, 下同)

        pand        xmm2, xmm3                      ;

        /* save_string */
        por         xmm5, xmm2                      ;
        movdqa      xmmword ptr [eax], xmm5         ;
        jmp         L999                            ;

        ALIGN_16
L500:
        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm3, xmm5                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm3, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pcmpgtb     xmm5, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pandn       xmm3, xmm5                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL
        pand        xmm3, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        cmp         ebx,  0x00000040                ; when (ebx < 64 bytes, edx > 24) ?
        jb          L550                            ;

        /* when (ebx = [64, 128), edx = (16, 24]) */
        sub         ebx,  64                        ;

        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm5, mm0                       ;

        pand        xmm3, xmm5                      ;

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movq        qword ptr [eax + 16], xmm5      ;
        jmp         L999                            ;

//      ALIGN_16
L550:
        /* when (ebx = [0, 64), edx = (24, 32]) */
        pcmpeqb     xmm7, xmm7                      ; all set to bit "1"

        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm6, mm0                       ;
//      shufpd      xmm7, xmm6, 0x00                ; 取xmm6的低64位 + xmm7的低64位
        unpcklpd    xmm7, xmm6                      ; 取xmm6的低64位 + xmm7的低64位(高位在前, 下同)

        pand        xmm3, xmm7                      ;

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movdqa      xmmword ptr [eax + 16], xmm5    ;
        jmp         L999                            ;

        /* the first line and the remain of less 32 bytes */
        ALIGN_16
L600:
        cmp         edx,  0                         ;
        jz          L999                            ;

        pcmpeqb     mm3,  mm3                       ; all set to bit "1"

        shl         ecx,  3                         ; ecx = ecx * 8
        cmp         ecx,  0x00000040                ; when (ecx >= 64 bytes) ?
        jae         L620                            ;

        pcmpeqb     mm2,  mm2                       ; all set to bit "1"
        movd        mm5,  ecx                       ;        
        psllq       mm2,  mm5                       ;
        jmp         L630                            ;
L620:
        sub         ecx,  64                        ;
        pxor        mm2,  mm2                       ;
        movd        mm5,  ecx                       ;
        psllq       mm3,  mm5                       ;
L630:
        mov         ecx,  ebx                       ;

        mov         ebx,  32                        ;
        sub         ebx,  edx                       ;
        shl         ebx,  3                         ; ebx = (32 - edx) * 8
        cmp         ebx,  0x00000080                ; when (ebx < 128 bytes, edx > 16) ?
        jb          L700                            ;

        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm5, xmm4                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        cmp         ebx,  0x000000C0                ; when (ebx < 192 bytes, edx > 8) ?
        jb          L650                            ;

        /* when (ebx = [192, 256), edx = (0, 8]) */
        sub         ebx,  192                       ;

        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        pand        mm0,  mm2                       ;
        movq2dq     xmm3, mm0                       ;

        pand        xmm2, xmm3                      ;

        /* save_string */
        por         xmm5, xmm2                      ;
        movq        qword ptr [eax], xmm5           ;
        jmp         L999                            ;

//      ALIGN_16
L650:
        /* when (ebx = [128, 192), edx = (8, 16]) */
        sub         ebx,  128                       ;

        pcmpeqb     xmm3, xmm3                      ; all set to bit "1"
        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm4, mm0                       ;
//      shufpd      xmm3, xmm4, 0x00                ; 取xmm4的低64位 + xmm3的低64位
        unpcklpd    xmm3, xmm4                      ; 取xmm4的低64位 + xmm3的低64位(高位在前, 下同)

        movq2dq     xmm6, mm2                       ;
        movq2dq     xmm7, mm3                       ;
        unpcklpd    xmm6, xmm7                      ; 取xmm7的低64位 + xmm6的低64位(高位在前, 下同)

        pand        xmm2, xmm3                      ;
        pand        xmm2, xmm6                      ;

        /* save_string */
        por         xmm5, xmm2                      ;
        movdqa      xmmword ptr [eax], xmm5         ;
        jmp         L999                            ;

        ALIGN_16
L700:
        /* start_tolower32 */
        movdqa      xmm2, xmm4                      ;
        movdqa      xmm3, xmm5                      ;
        pcmpgtb     xmm2, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm3, xmm1                      ; great than 'ZZZZZZZZ' ?
        pcmpgtb     xmm4, xmm0                      ; great than '@@@@@@@@' ?
        pcmpgtb     xmm5, xmm0                      ; great than '@@@@@@@@' ?
        pandn       xmm2, xmm4                      ;
        pandn       xmm3, xmm5                      ;
        pand        xmm2, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL
        pand        xmm3, xmm6                      ; and the bits of 0x20202020202020202020202020202020ULL

        cmp         ebx,  0x00000040                ; when (ebx < 64 bytes, edx > 24) ?
        jb          L750                            ;

        /* when (ebx = [64, 128), edx = (16, 24]) */
        sub         ebx,  64                        ;

        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm5, mm0                       ;

        movq2dq     xmm6, mm2                       ;
        movq2dq     xmm4, mm3                       ;
        unpcklpd    xmm6, xmm4                      ; 取xmm4的低64位 + xmm6的低64位(高位在前, 下同)

        pand        xmm3, xmm5                      ;

        cmp         ecx,  16                        ; ecx >= 16 ?
        jae         L720                            ;

        pand        xmm2, xmm6                      ;
        jmp         L730                            ;
L720:
        pand        xmm3, xmm6                      ;
L730:

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movq        qword ptr [eax + 16], xmm5      ;
        jmp         L999                            ;

//      ALIGN_16
L750:
        /* when (ebx = [0, 64), edx = (24, 32]) */
        pcmpeqb     xmm7, xmm7                      ; all set to bit "1"

        pcmpeqb     mm0,  mm0                       ; all set to bit "1"
        movd        mm1,  ebx                       ;
        psrlq       mm0,  mm1                       ;
        movq2dq     xmm6, mm0                       ;
//      shufpd      xmm7, xmm6, 0x00                ; 取xmm6的低64位 + xmm7的低64位
        unpcklpd    xmm7, xmm6                      ; 取xmm6的低64位 + xmm7的低64位(高位在前, 下同)

        movq2dq     xmm5, mm2                       ;
        movq2dq     xmm4, mm3                       ;
        unpcklpd    xmm5, xmm4                      ; 取xmm4的低64位 + xmm5的低64位(高位在前, 下同)

        pand        xmm3, xmm7                      ;

        cmp         ecx,  16                        ; ecx >= 16 ?
        jae         L770                            ;

        pand        xmm2, xmm5                      ;
        jmp         L780                            ;
L770:
        pand        xmm3, xmm5                      ;
L780:

        /* save_string */
        movdqa      xmm4, xmmword ptr [eax]         ; read from nearest preceding boundary
        movdqa      xmm5, xmmword ptr [eax + 16]    ; read from nearest preceding boundary
        por         xmm4, xmm2                      ;
        por         xmm5, xmm3                      ;
        movdqa      xmmword ptr [eax], xmm4         ;
        movdqa      xmmword ptr [eax + 16], xmm5    ;
        jmp         L999                            ;

        ALIGN_16
L999:
        ; Zero-byte found. Compute string length
        sub         eax, ARG_1                      ; subtract start address
        add         eax, edx                        ; add byte index

#if 1
        emms                                        ; clear mmx state
#endif

#if 0
        ALIGN_16
strlwr_386:
        mov         eax, 0

        jnz         strlwr_386
#endif

        pop         ebx
#if 0
        pop         ebp
        pop         esi
        pop         edi
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

#include "jimic/string/jm_strings.h"

#if defined(__GNUC__) || defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)

size_t jmf_strlwr(char *str)
{
    jm_strlwr(str, (size_t)-1);
    return jm_strlen(str);
}

#else

size_t __CDECL jmf_strlwr(char *str)
{
    jm_strlwr(str, (size_t)-1);
    return jm_strlen(str);
}

#endif // __GNUC__ or __MINGW__

///////////////////////////////////////////////////////////////////////////

#endif  /* defined(JIMI_JMF_USE_ASM) */

#endif  /* JIMI_IS_WIN32 */

