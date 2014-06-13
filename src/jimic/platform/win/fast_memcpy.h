/*****************************************************************************
 * fastmemcpy.h : fast memcpy routines
 *****************************************************************************
 * $Id$
 *
 * Authors: various Linux kernel hackers
 *          various MPlayer hackers
 *          Nick Kurshev <nickols_k@mail.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

/*
  aclib - advanced C library ;)
  This file contains functions which improve and expand standard C-library
*/

#include <jimic/core/jimic_def.h>

#if defined(_MSC_VER) || defined(_ICC) || defined(_BCB)

#define USE_PREFETCH        0

//#define PREFETCH_SIZE       ((15 * 8 + 4) * 8)
#define PREFETCH_SIZE       512

#define BLOCK_SIZE          4096
#define CONFUSION_FACTOR    0
/*Feel free to fine-tune the above 2, it might be possible to get some speedup with them :)*/

/*#define FASTMEMCPY_STATISTICS*/

#ifndef JIMI_HAVE_SSE2
/*
   P3 processor has only one SSE decoder so can execute only 1 sse insn per
   cpu clock, but it has 3 mmx decoders (include load/store unit)
   and executes 3 mmx insns per cpu clock.
   P4 processor has some chances, but after reading:
   http://www.emulators.com/pentium4.htm
   I have doubts. Anyway SSE2 version of this code can be written better.
*/
#undef JIMI_HAVE_SSE
#endif

/*
 This part of code was taken by me from Linux-2.4.3 and slightly modified
for MMX, MMX2, SSE instruction set. I have done it since linux uses page aligned
blocks but mplayer uses weakly ordered data and original sources can not
speedup them. Only using PREFETCHNTA and MOVNTQ together have effect!

>From IA-32 Intel Architecture Software Developer's Manual Volume 1,

Order Number 245470:
"10.4.6. Cacheability Control, Prefetch, and Memory Ordering Instructions"

Data referenced by a program can be temporal (data will be used again) or
non-temporal (data will be referenced once and not reused in the immediate
future). To make efficient use of the processor's caches, it is generally
desirable to cache temporal data and not cache non-temporal data. Overloading
the processor's caches with non-temporal data is sometimes referred to as
"polluting the caches".
The non-temporal data is written to memory with Write-Combining semantics.

The PREFETCHh instructions permits a program to load data into the processor
at a suggested cache level, so that it is closer to the processors load and
store unit when it is needed. If the data is already present in a level of
the cache hierarchy that is closer to the processor, the PREFETCHh instruction
will not result in any data movement.
But we should you PREFETCHNTA: Non-temporal data fetch data into location
close to the processor, minimizing cache pollution.

The MOVNTQ (store quadword using non-temporal hint) instruction stores
packed integer data from an MMX register to memory, using a non-temporal hint.
The MOVNTPS (store packed single-precision floating-point values using
non-temporal hint) instruction stores packed floating-point data from an
XMM register to memory, using a non-temporal hint.

The SFENCE (Store Fence) instruction controls write ordering by creating a
fence for memory store operations. This instruction guarantees that the results
of every store instruction that precedes the store fence in program order is
globally visible before any store instruction that follows the fence. The
SFENCE instruction provides an efficient way of ensuring ordering between
procedures that produce weakly-ordered data and procedures that consume that
data.

If you have questions please contact with me: Nick Kurshev: nickols_k@mail.ru.
*/

/* 3dnow memcpy support from kernel 2.4.2 */
/*  by Pontscho/fresh!mindworkz           */

#if defined(JIMI_HAVE_MMX2) || defined(JIMI_HAVE_3DNOW) || defined(JIMI_HAVE_MMX)

#undef JIMI_HAVE_MMX1
#if defined(JIMI_HAVE_MMX) && !defined(JIMI_HAVE_MMX2) && !defined(JIMI_HAVE_3DNOW) && !defined(JIMI_HAVE_SSE)
/*  means: mmx v.1. Note: Since we added alignment of destinition it speedups
    of memory copying on PentMMX, Celeron-1 and P2 upto 12% versus
    standard (non MMX-optimized) version.
    Note: on K6-2+ it speedups memory copying upto 25% and
          on K7 and P3 about 500% (5 times). */
#define JIMI_HAVE_MMX1
#endif

#undef JIMI_HAVE_K6_2PLUS
#if !defined(JIMI_HAVE_MMX2) && defined(JIMI_HAVE_3DNOW)
#define JIMI_HAVE_K6_2PLUS
#endif

/* for small memory blocks (<256 bytes) this version is faster */
#define small_memcpy(dest, src, n)\
    {\
        register unsigned long int dummy;\
        __asm__ __volatile__(\
             "rep; movsb"\
             :"=&D"(dest), "=&S"(src), "=&c"(dummy)\
             /* It's most portable way to notify compiler */\
             /* that edi, esi and ecx are clobbered in asm block. */\
             /* Thanks to A'rpi for hint!!! */\
             :"0" (dest), "1" (src), "2" (n)\
             : "memory");\
    }

#ifdef JIMI_HAVE_SSE
#define MMREG_SIZE      16
#else
#define MMREG_SIZE      64      /* 8 */
#endif

/* Small defines (for readability only) ;) */
#ifdef JIMI_HAVE_K6_2PLUS
#define PREFETCH        prefetch
/* On K6 femms is faster of emms. On K7 femms is directly mapped on emms. */
#define EMMS            femms
#else
// prefetchnta, prefetcht0, prefetcht1
#define PREFETCH        prefetcht0
#define EMMS            emms
#endif

#ifdef JIMI_HAVE_MMX2
#define MOVNTQ          movntq
#else
#define MOVNTQ          movq
#endif

#ifdef JIMI_HAVE_MMX1
#define MIN_LEN         2048   /* 2K blocks */
#else
#define MIN_LEN         64     /* 64-byte blocks */
#endif

#ifndef FASTMEMCPY_STATISTICS
#define fast_memcpy     fast_memcpy_impl
#else
#define fast_memcpy     fast_memcpy_stat
#endif

#define STACK           16
#define ARGS            0

#define DEST_ARG        DWORD PTR [esp +  4 + STACK + ARGS]
#define SRC_ARG         DWORD PTR [esp +  8 + STACK + ARGS]
#define LEN_ARG         DWORD PTR [esp + 12 + STACK + ARGS]

/* Local ARGS = 0 bytes */
#define LOCAL_ARG1      DWORD PTR [esp +  0 + STACK]
#define LOCAL_ARG2      DWORD PTR [esp +  4 + STACK]

#define DEST            edi
#define SRC             esi
#define LEN             ecx

#define DELTA           ebx
#define I               eax

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FASTMEMCPY_STATISTICS

static void *fast_memcpy_stat(void *dest, const void *src, size_t len)
{
    static int freq[33];
    static int t = 0;
    int i;
    for (i = 0; len > (1 << i); i++);
    freq[i]++;
    t++;
    if ((t % (1024 * 1024 * 1024)) == 0) {
        for (i = 0; i < 32; i++) {
            printf("freq < %8d %4d\n", 1 << i, freq[i]);
        }
    }

    return fast_memcpy_impl(dest, src, len);
}

#endif  /* FASTMEMCPY_STATISTICS */

__declspec(naked)
static void * __cdecl fast_memcpy_impl(void *dest, const void *src, size_t len)
{
    __asm {
#if defined(ARGS) && (ARGS > 0)
        sub     esp, ARGS      // # Generate Stack Frame
#endif

        push    ebp
        push    edi
        push    esi
        push    ebx

        mov     SRC,  SRC_ARG
        mov     DEST, DEST_ARG
        mov     LEN,  LEN_ARG

#if (!defined(JIMI_HAVE_MMX1)) || (JIMI_HAVE_MMX1 == 0)

#if USE_PREFETCH
        /* PREFETCH has effect even for MOVSB instruction ;) */
        PREFETCH byte ptr [esi +   0]
        PREFETCH byte ptr [esi +  64]
        PREFETCH byte ptr [esi + 128]
        PREFETCH byte ptr [esi + 192]
        PREFETCH byte ptr [esi + 256]
#endif

#endif  /* JIMI_HAVE_MMX1 */

        // if (len >= MIN_LEN) {
        cmp     ecx, MIN_LEN
        jl      L900
        
        mov     DELTA, DEST
        and     DELTA, (MMREG_SIZE - 1)
        // if (delta) {
        je      L100

        neg     DELTA
        add     DELTA, MMREG_SIZE
        
        sub     LEN, DELTA

        /* small_memcpy(dest, src, delta) */
        rep     movsb

        align 16
L100:
        mov     I, LEN
        shr     I, 6        ; i = len / 64;
        and     LEN, 63

        /*
           This algorithm is top effective when the code consequently
           reads and writes blocks which have size of cache line.
           Size of cache line is processor-dependent.
           It will, however, be a minimum of 32 bytes on any processors.
           It would be better to have a number of instructions which
           perform reading and writing to be multiple to a number of
           processor's decoders, but it's not always possible.
        */

        /* Only P3 (may be Cyrix3) */
#if JIMI_HAVE_SSE

        cmp     SRC, 15
        jz      L300

        align 16
L200:
        /* if SRC is misaligned */
#if USE_PREFETCH
        PREFETCH byte ptr [SRC + PREFETCH_SIZE]
#endif
        movups  xmm0, xmmword ptr [SRC +  0]
        movups  xmm1, xmmword ptr [SRC + 16]
        movups  xmm2, xmmword ptr [SRC + 32]
        movups  xmm3, xmmword ptr [SRC + 48]

        movntps xmmword ptr [DEST +  0], xmm0
        movntps xmmword ptr [DEST + 16], xmm1
        movntps xmmword ptr [DEST + 32], xmm2
        movntps xmmword ptr [DEST + 48], xmm3

        sub     SRC,  -64
        sub     DEST, -64
        dec     I
        jg      L200
        jmp     L888

        /*
           Only if SRC is aligned on 16-byte boundary.
           It allows to use movaps instead of movups, which required data
           to be aligned or a general-protection exception (#GP) is generated.
        */

        align 16
L300:

#if USE_PREFETCH
        PREFETCH byte ptr [SRC + PREFETCH_SIZE]
#endif
        movaps  xmm0, xmmword ptr [SRC +  0]
        movaps  xmm1, xmmword ptr [SRC + 16]
        movaps  xmm2, xmmword ptr [SRC + 32]
        movaps  xmm3, xmmword ptr [SRC + 48]

        movntps xmmword ptr [DEST +  0], xmm0
        movntps xmmword ptr [DEST + 16], xmm1
        movntps xmmword ptr [DEST + 32], xmm2
        movntps xmmword ptr [DEST + 48], xmm3

        sub     SRC,  -64
        sub     DEST, -64
        dec     I
        jg      L300

#else  /* !JIMI_HAVE_SSE */

        /* Align destination at BLOCK_SIZE boundary */
        cmp     SRC, 7
        jnz     L500

        /*
           Only if SRC is aligned on 8-byte boundary.
           It allows to use movaps instead of movups, which required data
           to be aligned or a general-protection exception (#GP) is generated.
        */
        align 16
L400:

#if USE_PREFETCH
        PREFETCH byte ptr [SRC + PREFETCH_SIZE]
#endif
        movq  mm0, qword ptr [SRC +  0]
        movq  mm1, qword ptr [SRC +  8]
        movq  mm2, qword ptr [SRC + 16]
        movq  mm3, qword ptr [SRC + 24]
        movq  mm4, qword ptr [SRC + 32]
        movq  mm5, qword ptr [SRC + 40]
        movq  mm6, qword ptr [SRC + 48]
        movq  mm7, qword ptr [SRC + 56]

        MOVNTQ qword ptr [DEST +  0], mm0
        MOVNTQ qword ptr [DEST +  8], mm1
        MOVNTQ qword ptr [DEST + 16], mm2
        MOVNTQ qword ptr [DEST + 24], mm3
        MOVNTQ qword ptr [DEST + 32], mm4
        MOVNTQ qword ptr [DEST + 40], mm5
        MOVNTQ qword ptr [DEST + 48], mm6
        MOVNTQ qword ptr [DEST + 56], mm7

        sub     SRC,  -64
        sub     DEST, -64
        dec     I
        jg      L400
        jmp     L888

        /* if SRC is misaligned */
        align 16
L500:

#if USE_PREFETCH
        PREFETCH byte ptr [SRC + PREFETCH_SIZE]
#endif
        movups  xmm0, xmmword ptr [SRC +  0]
        movups  xmm1, xmmword ptr [SRC + 16]
        movups  xmm2, xmmword ptr [SRC + 32]
        movups  xmm3, xmmword ptr [SRC + 48]

        movntps xmmword ptr [DEST +  0], xmm0
        movntps xmmword ptr [DEST + 16], xmm1
        movntps xmmword ptr [DEST + 32], xmm2
        movntps xmmword ptr [DEST + 48], xmm3

        sub     SRC,  -64
        sub     DEST, -64
        dec     I
        jg      L500

#endif  /* JIMI_HAVE_SSE */

        align 16
L888:

#if JIMI_HAVE_MMX2
        /* since movntq is weakly-ordered, a "sfence"
         * is needed to become ordered again. */
        sfence
#endif

#if (!defined(JIMI_HAVE_SSE)) || (JIMI_HAVE_SSE == 0)
        /* enables to use FPU */
        EMMS
#endif
        align 16
L900:
        /*
         *    Now do the tail of the block
         */

        // if (len) { small_memcpy(dest, src, len); }
        cmp     LEN, 0
        jz      L999_END

        /* small_memcpy(dest, src, delta) */
        rep     movsb

        align 16
L999_END:
        mov     eax, DEST       // return dest;

        pop     ebx
        pop     esi
        pop     edi
        pop     ebp

#if defined(ARGS) && (ARGS > 0)
        add     esp, ARGS
#endif
        ret
    }

}

#undef STACK
#undef ARGS

#undef LOCAL_ARG1
#undef LOCAL_ARG2

#ifdef __cplusplus
}
#endif

#endif  /* #if defined(JIMI_HAVE_MMX2) || defined(JIMI_HAVE_3DNOW) || defined(JIMI_HAVE_MMX) */

#endif  /* #if defined(_MSC_VER) || defined(_ICC) || defined(_BCB) */
