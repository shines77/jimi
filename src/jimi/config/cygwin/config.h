
#ifndef _JIMI_CONFIG_CYGWIN_CONFIG_H_
#define _JIMI_CONFIG_CYGWIN_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(__CYGWIN__)

#define JIMI_USE_DEFAULT_CONFIG             0

/* The compiler is clang-cl.exe for Visual Studio 20xx ? */
#ifndef JIMI_MSVC_CLANG
#define JIMI_MSVC_CLANG                     0
#endif

/* Use Visual Leaker Dector for Visual Studio ? */
#ifndef JIMI_USE_VLD
#define JIMI_USE_VLD                        1
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

#include "jimic/libc/endian.h"

/* 小端或大端, 非0表示小端存储 */
#if (JIMIC_BYTE_ORDER == JIMIC_LITTLE_ENDIAN)
#define JIMI_IS_LITTLE_ENDIAN               1
#else
#define JIMI_IS_LITTLE_ENDIAN               0
#endif

#define JIMI_USE_THREADING_TOOLS            0
#define JIMI_MALLOC_BUILD                   0

#if defined(JIMI_MSVC_CLANG) && (JIMI_MSVC_CLANG != 0)
#define JIMI_ATTRIBUTE_ALIGNED_PRESENT                  1
#else
#define JIMI_DECLSPEC_ALIGN_PRESENT                     1
#endif

#define JIMI_ALIGNOF_NOT_INSTANTIATED_TYPES_BROKEN      1

#define JIMI_CONSTEXPR

/* 介绍C++11标准的变长参数模板       */
/*   Reference: http://www.cnblogs.com/zenny-chen/archive/2013/02/03/2890917.html */

/* 编译器是否支持C++11的变长参数模板 */
#define JIMI_HAS_CXX11_VARIADIC_TEMPLATES   0

#define JIMI_HAS_DEFAULTED_FUNCTIONS        0
#define JIMI_HAS_DELETED_FUNCTIONS          0

#define JIMI_HAS_CXX11_MOVE_FUNCTIONS       0

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

#endif  /* _MSC_VER || __ICL || __INTEL_COMPILER */

#endif  /* __CYGWIN__ */

#endif  /* _JIMI_CONFIG_CYGWIN_CONFIG_H_ */
