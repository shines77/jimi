
#ifndef _JIMI_CORE_CONFIG_H_
#define _JIMI_CORE_CONFIG_H_

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#include "jimi/basic/win32/jimi_config.h"
#elif defined(__linux__) || defined(__LINUX__)
#include "jimi/basic/linux/jimi_config.h"
#elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#include "jimi/basic/mingw/jimi_config.h"
#else
#include "jimi/basic/default/jimi_config.h"
#endif

#endif  /* !_JIMI_CORE_CONFIG_H_ */
