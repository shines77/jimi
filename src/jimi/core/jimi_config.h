
#ifndef _JIMI_CORE_CONFIG_H_
#define _JIMI_CORE_CONFIG_H_

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#include "jimi/core/win32/jimi_config.h"
#elif defined(__linux__) || defined(__LINUX__)
#include "jimi/core/linux/jimi_config.h"
#elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#include "jimi/core/mingw/jimi_config.h"
#else
#include "jimi/core/default/jimi_config.h"
#endif

#endif  /* !_JIMI_CORE_CONFIG_H_ */
