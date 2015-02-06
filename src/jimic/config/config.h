
#ifndef _JIMIC_CONFIG_CONFIG_H_
#define _JIMIC_CONFIG_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/config/config-pre.h"

#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#include "jimic/config/win32/config.h"
#elif defined(__linux__) || defined(__LINUX__)
#include "jimic/config/linux/config.h"
#elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
#include "jimic/config/mingw/config.h"
#elif defined(__CYGWIN__)
#include "jimic/config/cygwin/config.h"
#else
#include "jimic/config/default/config.h"
#endif

#include "jimic/config/config-comm.h"

#endif  /* !_JIMIC_CONFIG_CONFIG_H_ */
