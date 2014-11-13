
#ifndef _JIMIC_ASM_H_
#define _JIMIC_ASM_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_def.h"

#ifndef __MMX__
#define __MMX__
#endif

#ifndef __SSE__
#define __SSE__
#endif

#ifndef __SSE2__
#define __SSE2__
#endif

#ifndef __SSE3__
#define __SSE3__
#endif

#if _MSC_VER
#if !defined(JIMIC_MSVC_CLANG) || (JIMIC_MSVC_CLANG == 0)
#include <intrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#endif
#else
#include <xmmintrin.h>
#include <emmintrin.h>
#endif  /* _MSC_VER */

#ifdef _MSC_VER
#define ALIGN_4         ALIGN 4
#define ALIGN_8         ALIGN 8
#define ALIGN_16        ALIGN 16
#define ALIGN_32        ALIGN 32
#define ALIGN_64        ALIGN 64
#elif defined(__APPLE__)
#define ALIGN_4         .align 2
#define ALIGN_8         .align 3
#define ALIGN_16        .align 4
#define ALIGN_32        .align 5
#define ALIGN_64        .align 6
#elif defined(__GNUC__)
#define ALIGN_4         .align 4
#define ALIGN_8         .align 8
#define ALIGN_16        .align 16
#define ALIGN_32        .align 32
#define ALIGN_64        .align 64
#else
#define ALIGN_4
#define ALIGN_8
#define ALIGN_16
#define ALIGN_32
#define ALIGN_64
#endif

//
// Intel x86 0x2E/0x3E Prefix Branch Prediction actually used?
// Reference: http://stackoverflow.com/questions/14332848/intel-x86-0x2e-0x3e-prefix-branch-prediction-actually-used
//
#if defined(__GNUC__)
    #define BRANCH		.byte 0x3e
    #define NOBRANCH	.byte 0x2e
    #define PADDING		.byte 0x66;
    #define HALT	    hlt
#elif defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)
    #define BRANCH		_emit 0x3e
    #define NOBRANCH	_emit 0x2e
    #define PADDING		_emit 0x66;
    #define HALT		hlt
#else
    #define BRANCH
    #define NOBRANCH
    #define PADDING     nop
    #define HALT		hlt
#endif

#ifndef xorpd
#define xorpd           xorps
#endif

#ifndef movapd
#define movapd          movaps
#endif

#endif  /* _JIMIC_ASM_H_ */
