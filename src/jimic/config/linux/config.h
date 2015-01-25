
#ifndef _JIMIC_CONFIG_LINUX_CONFIG_H_
#define _JIMIC_CONFIG_LINUX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(__linux__) || defined(__LINUX__)

#define JIMIC_USE_DEFAULT_CONFIG            0

#if defined(_DEBUG) || !defined(NDEBUG)
#define JIMIC_USE_DEBUG                     1
#define JIMIC_USE_ASSERT                    1
#define JIMIC_USE_THREADING_TOOLS           1
#else
#define JIMIC_USE_DEBUG                     0
#define JIMIC_USE_ASSERT                    0
#define JIMIC_USE_THREADING_TOOLS           0
#endif

#define JIMI_JMF_USE_ASM                    1

#define JIMI_HAVE_MMX                       1
#define JIMI_HAVE_MMX2                      1
#define JIMI_HAVE_SSE                       1
#define JIMI_HAVE_SSE2                      1

#include <jimic/libc/endian.h>

/* 小端或大端, 非0表示小端存储 */
#if (JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN)
#define JIMIC_IS_LITTLE_ENDIAN              1
#else
#define JIMIC_IS_LITTLE_ENDIAN              0
#endif

#endif  /* __linux__ || __LINUX__ */

#endif  /* !_JIMIC_CONFIG_LINUX_CONFIG_H_ */
