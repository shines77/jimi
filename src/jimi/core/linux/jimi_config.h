
#ifndef _JIMI_CORE_LINUX_CONFIG_H_
#define _JIMI_CORE_LINUX_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JIMI_USE_VLD
#define JIMI_USE_VLD                        0
#endif

#ifndef JIMI_USE_CRTDBG_CHECK
#define JIMI_USE_CRTDBG_CHECK               1
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
#define JIMI_USE_DEBUG                      1
#define JIMI_USE_ASSERT                     1
#else
#define JIMI_USE_DEBUG                      0
#define JIMI_USE_ASSERT                     0
#undef  JIMI_USE_VLD
#define JIMI_USE_VLD                        0
#endif

#define JIMI_USE_THREADING_TOOLS            0
#define JIMI_MALLOC_BUILD                   0

#define JIMI_ATTRIBUTE_ALIGNED_PRESENT                  1
//#define JIMI_DECLSPEC_ALIGN_PRESENT                   0
#define JIMI_ALIGNOF_NOT_INSTANTIATED_TYPES_BROKEN      1

#define JIMI_CONSTEXPR

#define JIMI_HAS_DEFAULTED_FUNCTIONS        0
#define JIMI_HAS_DELETED_FUNCTIONS          0

#define JIMI_HAS_CPP11_MOVE_FUNCTIONS       1

#define JIMI_HAS_BOOST                      0
#define JIMI_HAS_BOOST_LOCALE               0

//
// From: http://msdn.microsoft.com/zh-cn/library/e5ewb1h3%28v=vs.90%29.aspx
// From: http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
//
#if defined(JIMI_USE_CRTDBG_CHECK) && (JIMI_USE_CRTDBG_CHECK != 0)
#if defined(_DEBUG) || !defined(NDEBUG)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#endif  /* _DEBUG */
#endif  /* JIMI_USE_CRTDBG_CHECK */

#endif  /* _JIMI_CORE_LINUX_CONFIG_H_ */
