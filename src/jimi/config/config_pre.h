
#ifndef _JIMI_CONFIG_CONFIG_PRE_H_
#define _JIMI_CONFIG_CONFIG_PRE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(__linux__) || defined(__LINUX__)
//
#elif defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
//
#elif defined(__CYGWIN__)
//
#elif defined(_WIN32) || defined(_MSC_VER) || defined(__ICL) || defined(__INTEL_COMPILER)
//
#else
//
#endif

#endif  /* !_JIMI_CONFIG_CONFIG_PRE_H_ */
