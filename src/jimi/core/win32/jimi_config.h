
#ifndef _JIMI_CONFIG_H_
#define _JIMI_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
#define JIMI_USE_DEBUG                      1
#define JIMI_USE_ASSERT                     1
#else
#define JIMI_USE_DEBUG                      0
#define JIMI_USE_ASSERT                     0
#endif

#define JIMI_USE_THREADING_TOOLS            0
#define JIMI_MALLOC_BUILD                   0

#define JIMI_DECLSPEC_ALIGN_PRESENT                     1
#define JIMI_ALIGNOF_NOT_INSTANTIATED_TYPES_BROKEN      1

#define JIMI_CONSTEXPR

#define JIMI_HAS_DEFAULTED_FUNCTIONS        0
#define JIMI_HAS_DELETED_FUNCTIONS          0

#endif  /* _JIMI_CONFIG_H_ */
