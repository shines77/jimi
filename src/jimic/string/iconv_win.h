
#ifndef _LIBICONV_WIN_H_
#define _LIBICONV_WIN_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "jimic/core/jimic_stdint.h"
#include "jimic/core/jimic_declare.h"

#if defined(_WIN32) || defined(_WIN64)

/* win-iconv 内存分配是否使用new操作符(C++ only). */
#define WIN_ICONV_MALLOC_USE_NEW        0

#if defined(__cplusplus) && (defined(WIN_ICONV_MALLOC_USE_NEW) && (WIN_ICONV_MALLOC_USE_NEW != 0))

#define WIN_ICONV_MALLOC(size, type) \
    new type[(size)]

#define WIN_ICONV_MALLOC_EX(size, type, alignment, offset) \
    new type[(size)]

#define WIN_ICONV_FREE(p)   \
    delete [] p

#else

#define WIN_ICONV_MALLOC(size, type) \
    (type *)malloc((size_t)(size) * sizeof(type))

#define WIN_ICONV_MALLOC_EX(size, type, alignment, offset) \
    (type *)malloc((size_t)(size) * sizeof(type))

#define WIN_ICONV_FREE(p)   \
    free((void *)(p))

#endif

#define iconv_malloc(size, type)    WIN_ICONV_MALLOC(size, type)
#define iconv_free(p)               WIN_ICONV_FREE(p)

/**
 * win_iconv: for inline and noinline define
 */
#if _MSC_VER || __INTEL_COMPILER

//#define ICONV_INLINE              __inline
#define ICONV_INLINE
#define ICONV_FORCE_INLINE        __forceinline
#define ICONV_RESTRICT            __restrict
#define ICONV_HAS_INLINE          1

#elif defined(__GNUC__)

#define ICONV_INLINE              inline
#define ICONV_FORCE_INLINE        inline
#define ICONV_RESTRICT            restrict
#define ICONV_HAS_INLINE          1

#else

#define ICONV_INLINE              inline
#define ICONV_FORCE_INLINE        inline
#define ICONV_RESTRICT
#define ICONV_HAS_INLINE          1

#endif

#if defined(_WIN32) || defined(_WIN64)
#include <tchar.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
//#define LIBICONV_DLL_BUILD      /* defined when building dll */
//#define LIBICONV_DLL            /* defined when import from dll */
#endif

/* libiconv whether use "lib" prefix? */
#define LIBICONV_PLUG

#define _LIBICONV_VERSION       0x010E    /* version number: (major << 8) + minor */

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW__) || defined(__MINGW32__)
    #if defined(LIBICONV_DLL_BUILD) || defined(BUILDING_LIBICONV)
        #define LIBICONV_DLL_EXPORTED   __declspec(dllexport)
    #elif defined(LIBICONV_DLL)
        #define LIBICONV_DLL_EXPORTED   __declspec(dllimport)
    #else
        #define LIBICONV_DLL_EXPORTED
    #endif
#else
    /* GCC or another compliers */
    #if BUILDING_LIBICONV
        #define LIBICONV_DLL_EXPORTED   __attribute__((__visibility__("default")))
    #else
        #define LIBICONV_DLL_EXPORTED
    #endif
#endif  // !_WIN32

#define LIBICONV_API            LIBICONV_DLL_EXPORTED
#define LIBICONV_DLL_VARIABLE   LIBICONV_DLL_EXPORTED
#define ICONV_CONST             const

extern LIBICONV_DLL_VARIABLE int _libiconv_version; /* Likewise */

/* We would like to #include any system header file which could define
   iconv_t, 1. in order to eliminate the risk that the user gets compilation
   errors because some other system header file includes /usr/include/iconv.h
   which defines iconv_t or declares iconv after this file, 2. when compiling
   for LIBICONV_PLUG, we need the proper iconv_t type in order to produce
   binary compatible code.
   But gcc's #include_next is not portable. Thus, once libiconv's iconv.h
   has been installed in /usr/local/include, there is no way any more to
   include the original /usr/include/iconv.h. We simply have to get away
   without it.
   Ad 1. The risk that a system header file does
   #include "iconv.h"  or  #include_next "iconv.h"
   is small. They all do #include <iconv.h>.
   Ad 2. The iconv_t type is a pointer type in all cases I have seen. (It
   has to be a scalar type because (iconv_t)(-1) is a possible return value
   from iconv_open().) */

/* Define iconv_t ourselves. */
#undef iconv_t
#define iconv_t libiconv_t
typedef void * iconv_t;

/* Get size_t declaration.
   Get wchar_t declaration if it exists. */
#include <stddef.h>

/* Get errno declaration and values. */
#include <errno.h>
/* Some systems, like SunOS 4, don't have EILSEQ. Some systems, like BSD/OS,
   have EILSEQ in a different header.  On these systems, define EILSEQ
   ourselves. */
#ifndef EILSEQ
#define EILSEQ      "EILSEQ"
#endif

#define INVALID_ICONV_CP        ((iconv_t)(-1))

#ifdef __cplusplus
extern "C" {
#endif

/* define the codepages in windows */
enum iconv_codepages_e {
    ICONV_CP_FIRST = 0,
    // default (current system locale settings)
    ICONV_CP_UNKNOWN = ICONV_CP_FIRST,
    ICONV_CP_DEFAULT,
    // Generic
    ICONV_CP_ANSI,
    ICONV_CP_UTF8,
    ICONV_CP_UTF16,
    ICONV_CP_UTF32,
    ICONV_CP_UNICODE,
    // English
    ICONV_CP_ISO_8859_1,
    ICONV_CP_ISO_8859_15,
    ICONV_CP_WINDOWS_1252,
    ICONV_CP_IBM_850,
    // Simplified Chinese
    ICONV_CP_WINDOWS_936,
    ICONV_CP_GB2312,
    ICONV_CP_GB18030,
    // Traditional Chinese
    ICONV_CP_GBK,
    ICONV_CP_BIG5,
    // Last
    ICONV_CP_LAST
};

typedef enum iconv_codepages_e iconv_codepages_e;

struct iconv_codepage_s {
    unsigned long   cp_to;
    unsigned long   cp_from;
    size_t          conv_sizes;
};

typedef struct iconv_codepage_s * iconv_codepage_t;

/* Allocates descriptor for code conversion from encoding 'fromcode' to
   encoding 'tocode'. */
#ifndef LIBICONV_PLUG
#define iconv_open      libiconv_open
#endif
extern LIBICONV_DLL_EXPORTED iconv_t iconv_open(const char *tocode, const char *fromcode);

/* Frees resources allocated for conversion descriptor 'cd'. */
#ifndef LIBICONV_PLUG
#define iconv_close     libiconv_close
#endif
extern LIBICONV_DLL_EXPORTED int iconv_close(iconv_t cd);

/* Converts, using conversion descriptor 'cd', at most '*inbytesleft' bytes
   starting at '*inbuf', writing at most '*outbytesleft' bytes starting at
   '*outbuf'.
   Decrements '*inbytesleft' and increments '*inbuf' by the same amount.
   Decrements '*outbytesleft' and increments '*outbuf' by the same amount. */
#ifndef LIBICONV_PLUG
#define iconv           libiconv
#endif
extern LIBICONV_DLL_EXPORTED size_t iconv(iconv_t cd, ICONV_CONST char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);

/* get the codepage from the 'codepage' string */
extern LIBICONV_DLL_EXPORTED unsigned long iconv_get_codepage(const char *codepage);

// ansi <==> unicode
int LIBICONV_API iconv_ansi_to_unicode     (const char *utf8_str, int ansi_size, wchar_t * dest_unicode, int dest_size);
int LIBICONV_API iconv_ansi_to_unicode_auto(const char *utf8_str, int ansi_size, wchar_t **dest_unicode);

int LIBICONV_API iconv_unicode_to_ansi     (const wchar_t *unicode_str, int unicode_size, char * dest_ansi, int dest_size);
int LIBICONV_API iconv_unicode_to_ansi_auto(const wchar_t *unicode_str, int unicode_size, char **dest_ansi);

// utf8 <==> unicode

int LIBICONV_API iconv_utf8_to_unicode     (const char *utf8_str, int utf8_size, wchar_t * dest_unicode, int dest_size);
int LIBICONV_API iconv_utf8_to_unicode_auto(const char *utf8_str, int utf8_size, wchar_t **dest_unicode);

int LIBICONV_API iconv_unicode_to_utf8     (const wchar_t *unicode_str, int unicode_size, char * dest_utf8, int dest_size);
int LIBICONV_API iconv_unicode_to_utf8_auto(const wchar_t *unicode_str, int unicode_size, char **dest_utf8);

// ansi <==> utf8

int LIBICONV_API iconv_ansi_to_utf8     (const char *ansi_str, int ansi_size, char * dest_utf8, int dest_size);
int LIBICONV_API iconv_ansi_to_utf8_auto(const char *ansi_str, int ansi_size, char **dest_utf8);

int LIBICONV_API iconv_utf8_to_ansi     (const char *utf8_str, int utf8_size, char * dest_ansi, int dest_size);
int LIBICONV_API iconv_utf8_to_ansi_auto(const char *utf8_str, int utf8_size, char **dest_ansi);

#ifdef __cplusplus
}
#endif

#endif  /* _WIN32 || _WIN64 */

#endif  /* _LIBICONV_WIN_H_ */
