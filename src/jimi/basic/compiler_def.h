
#ifndef _JIMI_CORE_COMPILER_DEF_H_
#define _JIMI_CORE_COMPILER_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/**
 Config of jimi project, per target compiler.
 */

// Compiler name
#define JIMI_MACRO_TO_STR(x)        #x
#define JIMI_MACRO_VAL2STR(x)       JIMI_MACRO_TO_STR(x)

#if defined(__ICL) || defined(__INTEL_COMPILER)     // Intel C++
#  if defined(__VERSION__)
#    define JIMI_COMPILER_NAME      "Intel C++ " __VERSION__
#  elif defined(__INTEL_COMPILER_BUILD_DATE)
#    define JIMI_COMPILER_NAME      "Intel C++ (" JIMI_MACRO_VAL2STR(__INTEL_COMPILER_BUILD_DATE) ")"
#  else
#    define JIMI_COMPILER_NAME      "Intel C++"
#  endif    // #  if defined(__VERSION__)
#elif defined(_MSC_VER)     // Microsoft VC++
#  if defined(_MSC_FULL_VER)
#    define JIMI_COMPILER_NAME      "Microsoft VC++ (" JIMI_MACRO_VAL2STR(_MSC_FULL_VER) ")"
#  elif defined(_MSC_VER)
#    define JIMI_COMPILER_NAME      "Microsoft VC++ (" JIMI_MACRO_VAL2STR(_MSC_VER) ")"
#  else
#    define JIMI_COMPILER_NAME      "Microsoft VC++"
#  endif    // #  if defined(_MSC_FULL_VER)
#elif defined(__GNUC__)    // GCC
#  if defined(__CYGWIN__)
#    define JIMI_COMPILER_NAME      "GCC(Cygmin) " __VERSION__
#  elif defined(__MINGW32__) || defined(__MINGW64__)
#    define JIMI_COMPILER_NAME      "GCC(MinGW) " __VERSION__
#  else
#    define JIMI_COMPILER_NAME      "GCC " __VERSION__
#  endif    // #  if defined(__CYGWIN__)
#elif defined(__TURBOC__)   // Borland C++
#  if defined(__BCPLUSPLUS__)
#    define JIMI_COMPILER_NAME      "Borland C++ (" JIMI_MACRO_VAL2STR(__BCPLUSPLUS__) ")"
#  elif defined(__BORLANDC__)
#    define JIMI_COMPILER_NAME      "Borland C (" JIMI_MACRO_VAL2STR(__BORLANDC__) ")"
#  else
#    define JIMI_COMPILER_NAME      "Turbo C (" JIMI_MACRO_VAL2STR(__TURBOC__) ")"
#  endif    // #  if defined(_MSC_FULL_VER)
#else
#  define JIMI_COMPILER_NAME        "Unknown Compiler"
#endif  // #if defined(__ICL)    // Intel C++

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target compiler macro which JIMI uses.
#define JIMI_COMPILER_UNKNOWN               0

#define JIMI_COMPILER_MSVC                  1
#define JIMI_COMPILER_MSVC_60               2
#define JIMI_COMPILER_MSVC_70               3
#define JIMI_COMPILER_MSVC_71               4
#define JIMI_COMPILER_MSVC_80               5
#define JIMI_COMPILER_MSVC_90               6
#define JIMI_COMPILER_MSVC_100              7
#define JIMI_COMPILER_MSVC_110              8
#define JIMI_COMPILER_MSVC_120              9

#define JIMI_COMPILER_BORLANDC              20
#define JIMI_COMPILER_BCB                   21
#define JIMI_COMPILER_BC                    22
#define JIMI_COMPILER_TC                    23

#define JIMI_COMPILER_GCC                   30
#define JIMI_COMPILER_GCC_MINGW             31
#define JIMI_COMPILER_GCC_CYGWIN            32
#define JIMI_COMPILER_GCC_LLVM              33

#define JIMI_COMPILER_GPP                   40

#define JIMI_COMPILER_ICC                   50
#define JIMI_COMPILER_ICC_70                51
#define JIMI_COMPILER_ICC_80                52
#define JIMI_COMPILER_ICC_81                53
#define JIMI_COMPILER_ICC_90                54
#define JIMI_COMPILER_ICC_100               55
#define JIMI_COMPILER_ICC_110               56

#define JIMI_COMPILER_XCODE                 60
#define JIMI_COMPILER_XCODE3                61
#define JIMI_COMPILER_XCODE4                62

#define JIMI_COMPILER_MINGW                 70
#define JIMI_COMPILER_CYGWIN                80

#define JIMI_COMPILER_MSC_VER               0
#define JIMI_COMPILER_C99                   0
#define JIMI_COMPILER_C0X11                 0

// Determine target compiler by compile environment macro.
#undef  JIMI_TARGET_COMPILER_MAIN
#undef  JIMI_TARGET_COMPILER

#define JIMI_TARGET_COMPILER_MAIN           JIMI_COMPILER_UNKNOWN
#define JIMI_TARGET_COMPILER                JIMI_COMPILER_UNKNOWN

// Borland C++
#if defined(__TURBOC__)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_BORLANDC
    #if defined(__BCPLUSPLUS__)
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_BCB
    #elif defined(__BORLANDC__)
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_BC
    #else
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_TC
    #endif

    #ifndef JIMI_IS_BCB
    #define JIMI_IS_BCB             1
    #endif
#endif

// Intel C++
#if defined(__ICL) || defined(__INTEL_COMPILER)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_ICC
    #undef  JIMI_TARGET_COMPILER
    #define JIMI_TARGET_COMPILER            JIMI_COMPILER_ICC

    #ifndef JIMI_IS_ICC
    #define JIMI_IS_ICC             1
    #endif
#endif

// cygwin
#if defined(__CYGWIN__)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_CYGWIN
    #undef  JIMI_TARGET_COMPILER
    #define JIMI_TARGET_COMPILER            JIMI_COMPILER_CYGWIN

    #ifndef JIMI_IS_CYGWIN
    #define JIMI_IS_CYGWIN          1
    #endif
#endif

// MinGW
#if defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_MINGW
    #undef  JIMI_TARGET_COMPILER
    #define JIMI_TARGET_COMPILER            JIMI_COMPILER_MINGW

    #ifndef JIMI_IS_MINGW
    #define JIMI_IS_MINGW           1
    #endif

    #if defined(__MINGW32__)
        #ifndef JIMI_IS_MINGW32
        #define JIMI_IS_MINGW32     1
        #endif
    #endif

    #if defined(__MINGW64__)
        #ifndef JIMI_IS_MINGW64
        #define JIMI_IS_MINGW64     1
        #endif
    #endif
#endif

// GNU C++
#if defined(__GNUC__)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_GCC
    #if defined(__CYGWIN__)     // cygwin
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_GCC_CYGWIN
    #elif defined(__MINGW32__) || defined(__MINGW64__)  // MinGW
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_GCC_MINGW
    #elif defined(_LLVM)        // LLVM
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_GCC_LLVM
        #ifndef JIMI_IS_LLVM
        #define JIMI_IS_LLVM        1
        #endif
    #else
        #undef  JIMI_TARGET_COMPILER
        #define JIMI_TARGET_COMPILER        JIMI_COMPILER_GCC
    #endif

    #ifndef JIMI_IS_GNUC
    #define JIMI_IS_GNUC        1
    #endif

    #ifndef JIMI_IS_GCC
    #define JIMI_IS_GCC         1
    #endif
#endif

// GNU G++
#if defined(__GNUCPP__)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_GPP
    #undef  JIMI_TARGET_COMPILER
    #define JIMI_TARGET_COMPILER            JIMI_COMPILER_GPP

    #ifndef JIMI_IS_GNUCPP
    #define JIMI_IS_GNUCPP      1
    #endif

    #ifndef JIMI_IS_GPP
    #define JIMI_IS_GPP         1
    #endif
#endif

// MSVC
#if defined(_MSC_VER)
    #undef  JIMI_TARGET_COMPILER_MAIN
    #define JIMI_TARGET_COMPILER_MAIN       JIMI_COMPILER_MSVC
    #undef  JIMI_TARGET_COMPILER
    #define JIMI_TARGET_COMPILER            JIMI_COMPILER_MSVC
    #undef  JIMI_COMPILER_MSC_VER
    #define JIMI_COMPILER_MSC_VER           _MSC_VER

    #ifndef JIMI_IS_MSVC
    #define JIMI_IS_MSVC        1
    #endif

    #ifndef JIMI_MSC_VER
    #define JIMI_MSC_VER        _MSC_VER
    #endif
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set compiler
#if !JIMI_TARGET_COMPILER
  #ifndef _MSC_VER
    #error "Cannot recognize the target compiler; are you targeting an unsupported compiler?"
  #endif
#endif

#endif  /* _JIMI_CORE_COMPILER_DEF_H_ */
