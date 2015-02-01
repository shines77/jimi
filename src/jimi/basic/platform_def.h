
#ifndef _JIMI_BASIC_PLATFORM_DEF_H_
#define _JIMI_BASIC_PLATFORM_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef JIMI_DEFINED
#define JIMI_DEFINED(X)                 ((##X) && (##X != 0))
#endif

/**
 Config of jimi project, per target platform.
 */

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which JIMI uses.
#define JIMI_PLATFORM_UNKNOWN           0
// Windows family
#define JIMI_PLATFORM_WIN32             1
#define JIMI_PLATFORM_WIN64             2
#define JIMI_PLATFORM_WINCE             3
#define JIMI_PLATFORM_WINRT             4
#define JIMI_PLATFORM_WP8               5
// Linux family
#define JIMI_PLATFORM_LINUX             20
// Apple family
#define JIMI_PLATFORM_MAC               40
#define JIMI_PLATFORM_IOS               41
// Mobile family
#define JIMI_PLATFORM_ANDROID           50
#define JIMI_PLATFORM_BLACKBERRY        60
// Other systems
#define JIMI_PLATFORM_MARMALADE         100
#define JIMI_PLATFORM_BADA              101
#define JIMI_PLATFORM_NACL              102
#define JIMI_PLATFORM_EMSCRIPTEN        103
#define JIMI_PLATFORM_TIZEN             104

// Determine target platform by compile environment macro.
#undef  JIMI_TARGET_PLATFORM
#define JIMI_TARGET_PLATFORM            JIMI_PLATFORM_UNKNOWN

#ifndef JIMI_PLATFORM_NAME
#define JIMI_PLATFORM_NAME(X)           JIMI_PLATFORM_##X
#endif

#ifndef JIMI_CHECK_PLATFORM
    #ifdef JIMI_TARGET_PLATFORM
        #define JIMI_CHECK_PLATFORM(X)  ((JIMI_PLATFORM_##X) && (JIMI_TARGET_PLATFORM == JIMI_PLATFORM_##X))
    #else
        #define JIMI_CHECK_PLATFORM(X)  (JIMI_PLATFORM_##X)
    #endif
#endif  // JIMI_CHECK_PLATFORM

#ifndef JIMI_CHECK_OS
#define JIMI_CHECK_OS                   JIMI_CHECK_PLATFORM
#endif

// marmalade
#if defined(MARMALADE)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_MARMALADE
    #ifndef JIMI_IS_MARMALADE
    #define JIMI_IS_MARMALADE           1
    #endif
#endif

// bada
#if defined(SHP)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_BADA
    #ifndef JIMI_IS_BADA
    #define JIMI_IS_BADA                1
    #endif
#endif

// native client
#if defined(__native_client__)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_NACL
    #ifndef JIMI_IS_NACL
    #define JIMI_IS_NACL                1
    #endif
#endif

// Emscripten
#if defined(EMSCRIPTEN)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_EMSCRIPTEN
    #ifndef JIMI_IS_EMSCRIPTEN
    #define JIMI_IS_EMSCRIPTEN          1
    #endif
#endif

// tizen
#if defined(TIZEN)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_TIZEN
    #ifndef JIMI_IS_WINRT
    #define JIMI_IS_TIZEN               1
    #endif
#endif

// qnx
#if defined(__QNX__)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_BLACKBERRY
    #ifndef JIMI_IS_BLACKBERRY
    #define JIMI_IS_BLACKBERRY          1
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_ANDROID
    #ifndef JIMI_IS_ANDROID
    #define JIMI_IS_ANDROID             1
    #endif
#endif

// iphone
#if defined(JIMI_TARGET_OS_IPHONE)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_IOS
    #ifndef JIMI_IS_OS_IPHONE
    #define JIMI_IS_OS_IPHONE           1
    #endif
#endif

// ipad, perhaps this precompiled entry is invalid.
#if defined(JIMI_TARGET_OS_IPAD)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_IOS
    #ifndef JIMI_IS_OS_IPAD
    #define JIMI_IS_OS_IPAD             1
    #endif
#endif

// mac
#if defined(JIMI_TARGET_OS_MAC)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_MAC
    #ifndef JIMI_IS_OS_MAC
    #define JIMI_IS_OS_MAC              1
    #endif
#endif

// linux
#if defined(__linux__) || defined(__linux) || defined(LINUX)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_LINUX
    // JIMI_IS_LINUX has defined below
    #ifndef JIMI_IS_OS_LINUX
    #define JIMI_IS_OS_LINUX            1
    #endif
#endif

// WinCE (Windows CE)
#if defined(WCE) || defined(_WCE) || defined(WINCE) || defined(_WIN32_WCE)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_WINCE
    #ifndef JIMI_IS_WINCE
    #define JIMI_IS_WINCE               1
    #endif
#endif

// WinRT (Windows Store App)
#if defined(WINRT) || defined(_WINRT)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_WINRT
    #ifndef JIMI_IS_WINRT
    #define JIMI_IS_WINRT               1
    #endif
#endif

// WP8 (Windows Phone 8 App)
#if defined(WP8) || defined(_WP8)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_WP8
    #ifndef JIMI_IS_WP8
    #define JIMI_IS_WP8                 1
    #endif
#endif

// win32
#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS) || defined(__WIN32__) || defined(__NT__)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_WIN32
    #ifndef JIMI_IS_WIN32
    #define JIMI_IS_WIN32               1
    #endif
#endif

// win64
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
    #undef  JIMI_TARGET_PLATFORM
    #define JIMI_TARGET_PLATFORM        JIMI_PLATFORM_WIN64
    #ifndef JIMI_IS_WIN64
    #define JIMI_IS_WIN64               1
    #endif
#endif

#if (defined(JIMI_IS_WIN32) || defined(JIMI_IS_WIN64)) && !(defined(__GNUC__) || defined(__MINGW__) \
    || defined(__MINGW32__) || defined(__MINGW64__))
#ifndef JIMI_IS_WINDOWS
#define JIMI_IS_WINDOWS                 (JIMI_CHECK_PLATFORM(WIN32) || JIMI_CHECK_PLATFORM(WIN64))
#endif
#endif

#if defined(JIMI_IS_WIN32) || defined(JIMI_IS_WIN64) || defined(JIMI_IS_WINCE) \
 || defined(JIMI_IS_WINRT) || defined(JIMI_IS_WP8)
#ifndef JIMI_IS_WINFAMILY
#define JIMI_IS_WINFAMILY               (JIMI_CHECK_PLATFORM(WIN32) || JIMI_CHECK_PLATFORM(WIN64) \
                                      || JIMI_CHECK_PLATFORM(WINCE) || JIMI_CHECK_PLATFORM(WINRT) \
                                      || JIMI_CHECK_PLATFORM(WP8))
#endif
#endif

#if defined(JIMI_IS_OS_UNIX)
#ifndef JIMI_IS_UNIX
#define JIMI_IS_UNIX                    (JIMI_CHECK_PLATFORM(LINUX))
#endif
#endif

#if defined(JIMI_IS_OS_LINUX)
#ifndef JIMI_IS_LINUX
#define JIMI_IS_LINUX                   (JIMI_CHECK_PLATFORM(LINUX))
#endif
#endif

#if defined(JIMI_IS_OS_IPHONE) || defined(JIMI_IS_OS_IPAD)
#ifndef JIMI_IS_IOS
#define JIMI_IS_IOS                     (JIMI_CHECK_PLATFORM(OS_IPHONE) || JIMI_CHECK_PLATFORM(OS_IPAD))
#endif
#endif

#if defined(JIMI_IS_OS_MAC)
#ifndef JIMI_IS_MAC
#define JIMI_IS_MAC                     (JIMI_CHECK_PLATFORM(OS_MAC))
#endif
#endif

#if defined(JIMI_IS_IOS) || defined(JIMI_IS_MAC)
#ifndef JIMI_IS_APPLE
#define JIMI_IS_APPLE                   (JIMI_CHECK_PLATFORM(IOS) || JIMI_CHECK_PLATFORM(MAC))
#endif
#endif

// for DOXYGEN
#if defined(DOXYGEN)
  #ifndef JIMI_IS_DOXYGEN
    #define JIMI_IS_DOXYGEN             1
  #endif
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if (!defined(JIMI_TARGET_PLATFORM)) || (JIMI_TARGET_PLATFORM == JIMI_PLATFORM_UNKNOWN)
    #error "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (JIMI_TARGET_PLATFORM == JIMI_PLATFORM_WIN32 || JIMI_TARGET_PLATFORM == JIMI_PLATFORM_WIN64)
#pragma warning (disable:4127)
#endif  // JIMI_PLATFORM_WIN32

#endif  /* _JIMI_BASIC_PLATFORM_DEF_H_ */
