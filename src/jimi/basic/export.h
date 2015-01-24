
#ifndef _JIMI_BASIC_EXPORT_H_
#define _JIMI_BASIC_EXPORT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/platform_def.h"
#include "jimi/basic/compiler_def.h"

#if defined(BUILD_DLL) || defined(JIMI_BUILD_DLL) || defined(_USRDLL) \
    || defined(JIMI_EXPORT) || defined(JIMI_DECLARE_EXPORT) || defined(DECLARE_EXPORT) \
    || defined(JIMI_SHARED) || defined(JIMI_ENABLE_SHARED) || defined(ENABLE_SHARED)
    /* is a dll library */
    #undef JIMI_DECLARE_STATIC
    #undef JIMI_DECLARE_IMPORT
    #ifndef JIMI_DECLARE_EXPORT
        #define JIMI_DECLARE_EXPORT
    #endif
    #undef JIMIC_DECLARE_STATIC
    #undef JIMIC_DECLARE_IMPORT
    #ifndef JIMIC_DECLARE_EXPORT
        #define JIMIC_DECLARE_EXPORT
    #endif
#elif defined(USE_DLL) || defined(JIMI_USE_DLL) || defined(JIMI_HAS_DLL) \
    || defined(JIMI_IMPORT) || defined(JIMI_DECLARE_IMPORT) || defined(DECLARE_IMPORT) \
    || defined(JIMI_USE_SHARED) || defined(USE_SHARED)
    /* use a dll library */
    #undef JIMI_DECLARE_STATIC
    #undef JIMI_DECLARE_EXPORT
    #ifndef JIMI_DECLARE_IMPORT
        #define JIMI_DECLARE_IMPORT
    #endif
    #undef JIMIC_DECLARE_STATIC
    #undef JIMIC_DECLARE_EXPORT
    #ifndef JIMIC_DECLARE_IMPORT
        #define JIMIC_DECLARE_IMPORT
    #endif
#else /* defined(_LIB) || defined(USE_LIB) || defined(BUILD_LIB) \
         || defined(JIMI_LIB) || defined(JIMI_IS_LIB) \
         || defined(JIMI_USE_LIB) || defined(JIMI_USE_STATIC) \
         || defined(JIMI_STATIC) || defined(JIMI_DECLARE_STATIC) || defined(DECLARE_STATIC) \
         || defined(JIMI_ENABLED_STATIC) || defined(ENABLED_STATIC) */
    /* is a static library or use a static library */
    #undef JIMI_DECLARE_EXPORT
    #undef JIMI_DECLARE_IMPORT
    #ifndef JIMI_DECLARE_STATIC
        #define JIMI_DECLARE_STATIC
    #endif
    #undef JIMIC_DECLARE_EXPORT
    #undef JIMIC_DECLARE_IMPORT
    #ifndef JIMIC_DECLARE_STATIC
        #define JIMIC_DECLARE_STATIC
    #endif
#endif

/**************************************************************************************
   Dynamic Library import / export / static control conditional
   (Define JIMI_DECLARE_EXPORT to export symbols, else they are imported or static)
**************************************************************************************/

#ifdef JIMI_DLL
#undef JIMI_DLL
#endif

#if defined(JIMI_IS_MSVC)  /* is microsoft visual studio ? */
    #if defined(JIMI_DECLARE_EXPORT)              /* is a dll library */
        #define JIMI_DLL                __declspec(dllexport)
        #define JIMI_DLL_TPL            __declspec(dllexport)
        #define JIMI_PRIVATE
        #define JIMI_EXPIMP_TEMPLATE
    #elif defined(JIMI_DECLARE_IMPORT)            /* use a dll library */
        #define JIMI_DLL                __declspec(dllimport)
        #define JIMI_DLL_TPL            __declspec(dllimport)   // or don't defined it!
        #define JIMI_PRIVATE
        #define JIMI_EXPIMP_TEMPLATE    extern
    #else /* defined(JIMI_DECLARE_STATIC) */      /* is a static library or use static library */
        #define JIMI_DLL
        #define JIMI_DLL_TPL
        #define JIMI_PRIVATE
        #define JIMI_EXPIMP_TEMPLATE
    #endif
#elif defined(JIMI_IS_GNUC)   
    #define JIMI_DLL                    __attribute__ ((visibility ("default")))
    #define JIMI_DLL                    __attribute__ ((visibility ("default")))
    #define JIMI_DLL_TPL                __attribute__ ((visibility ("default")))
    #define JIMI_PRIVATE                __attribute__ ((visibility ("default")))
    #if defined(JIMI_DECLARE_IMPORT)
        #define JIMI_EXPIMP_TEMPLATE    extern
    #else
        #define JIMI_EXPIMP_TEMPLATE
    #endif
#else  /* not is msvc and not is gunc! */
    #define JIMI_DLL
    #define JIMI_DLL_TPL
    #define JIMI_PRIVATE
    #if defined(JIMI_DECLARE_IMPORT)
        #define JIMI_EXPIMP_TEMPLATE    extern
    #else
        #define JIMI_EXPIMP_TEMPLATE
    #endif
#endif

#ifdef JIMIC_DLL
#undef JIMIC_DLL
#endif

#ifdef JIMIC_PRIVATE
#undef JIMIC_PRIVATE
#endif

#define JIMIC_DLL               JIMI_DLL
#define JIMIC_PRIVATE           JIMI_PRIVATE

#endif  /* _JIMI_BASIC_EXPORT_H_ */
