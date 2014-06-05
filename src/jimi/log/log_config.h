
#ifndef _JIMI_LOG_CONFIG_H_
#define _JIMI_LOG_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// LOG config
#ifndef JIMI_LOG_ENABLE
#define JIMI_LOG_ENABLE             1
#endif

#ifndef JIMI_LOG_TO_FILE
#define JIMI_LOG_TO_FILE            1
#endif

#ifndef JIMI_LOG_TO_SCREEN
#define JIMI_LOG_TO_SCREEN          1
#endif

// about JIMI_LOG_TO_SCREEN
#if defined(JIMI_LOG_TO_SCREEN) && (JIMI_LOG_TO_SCREEN != 0)
    #define JIMI_LOG_TO_CONSOLE         1
    #define JIMI_LOG_TO_WINDOW          1
    // JIMI_LOG_TO_CONSOLE and JIMI_LOG_TO_WINDOW can switch only one,
    // JIMI_LOG_TO_CONSOLE is first select
    #if defined(JIMI_LOG_TO_CONSOLE) && (JIMI_LOG_TO_CONSOLE != 0)
        #undef  JIMI_LOG_TO_WINDOW
        #define JIMI_LOG_TO_WINDOW      0
    #endif
#else
    #define JIMI_LOG_TO_CONSOLE         0
    #define JIMI_LOG_TO_WINDOW          0
#endif

#if (!defined(JIMI_LOG_TO_FILE) || (JIMI_LOG_TO_FILE == 0)) && (!defined(JIMI_LOG_TO_SCREEN) || (JIMI_LOG_TO_SCREEN == 0))
    #undef  JIMI_LOG_ENABLE
    #define JIMI_LOG_ENABLE             0
#endif

// TRACE config
#define JIMI_LOG_TRACE_ENABLE           1

#define JIMI_LOG_TRACE_ON_SCREEN        1
#ifdef _WIN32
    #define JIMI_LOG_TRACE_IN_DEVENV    1
#else
    #define JIMI_LOG_TRACE_IN_DEVENV    0
#endif

#if (!defined(JIMI_LOG_TRACE_ON_SCREEN) || (JIMI_LOG_TRACE_ON_SCREEN == 0)) && (!defined(JIMI_LOG_TRACE_IN_DEVENV) || (JIMI_LOG_TRACE_IN_DEVENV == 0))
    #undef  JIMI_LOG_TRACE_ENABLE
    #define JIMI_LOG_TRACE_ENABLE           0
#endif

#endif  /* _JIMI_LOG_CONFIG_H_ */
