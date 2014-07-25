
#ifndef _JIMIC_CORE_LINUX_CONFIG_H_
#define _JIMIC_CORE_LINUX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

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

#endif  /* !_JIMIC_CORE_LINUX_CONFIG_H_ */
