
#ifndef _JIMIC_SYSTEM_CRTDEG_ENV_H_
#define _JIMIC_SYSTEM_CRTDEG_ENV_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_config.h>
#include <jimic/core/jimic_config.h>

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

//
// C++中基于CRT的内存泄漏检测
//
// Reference: http://www.cnblogs.com/weiym/archive/2013/02/25/2932810.html
//

#if defined(_DEBUG) || !defined(NDEBUG)
  #define DEBUG_CLIENTBLOCK  new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
  #define DEBUG_CLIENTBLOCK
#endif

// 如果不定义这个宏, C方式的malloc泄露不会被记录下来
#define _CRTDBG_MAP_ALLOC

// crtdbg.h must be behind the stdlib.h
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
  #ifndef new
    #define new DEBUG_CLIENTBLOCK
  #endif
#endif

#else

// crtdbg.h must be behind the stdlib.h
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#endif  /* JIMI_USE_CRTDBG_CHECK */

#include <jimic/system/vs_vld.h>

/* 设置CRTDBG的环境(Debug模式下, 检查内存越界和内存泄漏问题) */
void jimi_set_crtdbg_env(int display_memory_leak, int always_check_bounds);

#endif  /* !_JIMIC_SYSTEM_CRTDEG_ENV_H_ */
