
#ifndef _JIMIC_SYSTEM_CRTDEG_ENV_H_
#define _JIMIC_SYSTEM_CRTDEG_ENV_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_config.h"
#include "jimic/basic/jimic_config.h"

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

/* 基于CRT的内存泄漏检测 */
#if defined(JIMI_USE_CRTDBG_CHECK) && (JIMI_USE_CRTDBG_CHECK != 0)

// 如果不定义这个宏, C方式的malloc泄露不会被记录下来
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif

// _CRTDBG_MAP_ALLOC 宏必须在 #include <stdlib.h> 之前
#include <stdlib.h>

// crtdbg.h must be behind the stdlib.h
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

//
// C++中基于CRT的内存泄漏检测
//
// Reference: http://www.cnblogs.com/weiym/archive/2013/02/25/2932810.html
//

#if (defined(_DEBUG) || !defined(NDEBUG))
#ifndef DBG_NEW
  #if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #define DBG_NEW  new(_CLIENT_BLOCK, __FILE__, __LINE__)
  #else
    #define DBG_NEW  new(_NORMAL_BLOCK, __FILE__, __LINE__)
  #endif
#else
  #define DBG_NEW
#endif  /* DBG_NEW */
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
  #if defined(_MSC_VER) && (_MSC_VER >= 1600)
    #ifndef new
      #define new DBG_NEW
    #endif
  #endif  /* defined(_MSC_VER) && (_MSC_VER >= 1600) */
#endif  /* defined(_DEBUG) || !defined(NDEBUG) */

#else

// crtdbg.h must be behind the stdlib.h
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#endif  /* JIMI_USE_CRTDBG_CHECK */

#include "jimic/system/vs_vld.h"

#define DONT_DISPLAY_MEMORY_LEAK    0
#define ALLOW_DISPLAY_MEMORY_LEAK   1

#define DONT_CHECK_BOUNDS           0
#define ALWAYS_CHECK_BOUNDS         1

#ifdef __cplusplus
extern "C" {
#endif

/* 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题) */
void jimi_set_crtdbg_env(int display_memory_leak, int always_check_bounds);

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_SYSTEM_CRTDEG_ENV_H_ */
