
#if defined(_WIN32) || defined(_WIN64)

#include "iconv_win.h"
//#include <jimic/string/iconv_win.h>

#if defined(_WIN32) || defined(_WIN64)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <locale.h>
#include <stdlib.h>
#if defined(_WIN32) || defined(_WIN64)
#include <wchar.h>
#endif

/* backup __STRICT_ANSI__ define for MinGW(GCC) */
#ifdef __STRICT_ANSI__
#undef  _STRICT_ANSI_SAVE_
#define _STRICT_ANSI_SAVE_  __STRICT_ANSI__
#endif // __STRICT_ANSI__

#undef __STRICT_ANSI__
#include <string.h>

/* restore __STRICT_ANSI__ define for MinGW(GCC) */
#ifdef _STRICT_ANSI_SAVE_
#define __STRICT_ANSI__     _STRICT_ANSI_SAVE_
#undef _STRICT_ANSI_SAVE_
#endif // _STRICT_ANSI_SAVE_

#if defined(_WIN32) || defined(_WIN64)
// Used to prevent dialog on memory fault.
// Limits headers included by Windows.h
# ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
# endif

# define NOSERVICE
# define NOMCX
# define NOIME
# define NOSOUND
# define NOCOMM
# define NORPC
# define NOGDI
# define NOUSER
# define NODRIVERS
# define NOLOGERROR
# define NOPROFILER
# define NOMEMMGR
# define NOLFILEIO
# define NOOPENFILE
# define NORESOURCE
# define NOATOM
# define NOLANGUAGE
# define NOLSTRING
# define NODBCS
# define NOKEYBOARDINFO
# define NOGDICAPMASKS
# define NOCOLOR
# define NOGDIOBJ
# define NODRAWTEXT
# define NOTEXTMETRIC
# define NOSCALABLEFONT
# define NOBITMAP
# define NORASTEROPS
# define NOMETAFILE
# define NOSYSMETRICS
# define NOSYSTEMPARAMSINFO
# define NOMSG
# define NOWINSTYLES
# define NOWINOFFSETS
# define NOSHOWWINDOW
# define NODEFERWINDOWPOS
# define NOVIRTUALKEYCODES
# define NOKEYSTATES
# define NOWH
# define NOMENUS
# define NOSCROLL
# define NOCLIPBOARD
# define NOICONS
# define NOMB
# define NOSYSCOMMANDS
# define NOMDI
# define NOCTLMGR
# define NOWINMESSAGES
# include <windows.h>
#endif // if defined(_WIN32) || defined(_WIN64)

#if defined(_WIN32) || defined(_WIN64)
#define iconv_strcmp    _strcmp
#define iconv_stricmp   _stricmp
#define iconv_strdup    _strdup
#define iconv_strlwr    _strlwr
#else
#define iconv_strcmp    strcmp
#define iconv_stricmp   stricmp
#define iconv_strdup    strdup
#define iconv_strlwr    strlwr
#endif

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)

#pragma warning(disable : 4995)         // 禁止不带_s函数的废弃warning信息
#pragma warning(disable : 4996)
#endif  /* _MSC_VER */

#define USE_FAST_TRANSFER_COPY      0

struct iconv_windows_encoding {
    char const *name;
    unsigned    codepage;
    unsigned    was_tested;
};

typedef struct iconv_windows_encoding iconv_windows_encoding_t;

#ifdef __cplusplus
int operator < (iconv_windows_encoding_t const &l, iconv_windows_encoding_t const &r)
{
    return ::strcmp(l.name, r.name) < 0;
}
#endif  /* __cplusplus */

iconv_windows_encoding_t all_windows_encodings[] = {
    { "big5",           950,  0 },
    { "cp1250",         1250, 0 },
    { "cp1251",         1251, 0 },
    { "cp1252",         1252, 0 },
    { "cp1253",         1253, 0 },
    { "cp1254",         1254, 0 },
    { "cp1255",         1255, 0 },
    { "cp1256",         1256, 0 },
    { "cp1257",         1257, 0 },
    { "cp874",          874,  0 },
    { "cp932",          932,  0 },
    { "cp936",          936,  0 },
    { "eucjp",          20932, 0 },
    { "euckr",          51949, 0 },
    { "gb18030",        54936, 0 },
    { "gb2312",         20936, 0 },
    { "gbk",            936,   0 },
    { "iso2022jp",      50220, 0 },
    { "iso2022kr",      50225, 0 },
    { "iso88591",       28591, 0 },
    { "iso885913",      28603, 0 },
    { "iso885915",      28605, 0 },
    { "iso88592",       28592, 0 },
    { "iso88593",       28593, 0 },
    { "iso88594",       28594, 0 },
    { "iso88595",       28595, 0 },
    { "iso88596",       28596, 0 },
    { "iso88597",       28597, 0 },
    { "iso88598",       28598, 0 },
    { "iso88599",       28599, 0 },
    { "koi8r",          20866, 0 },
    { "koi8u",          21866, 0 },
    { "ms936",          936,   0 },
    { "shiftjis",       932,   0 },
    { "sjis",           932,   0 },
    { "usascii",        20127, 0 },
    { "utf8",           65001, 0 },
    { "windows1250",    1250, 0 },
    { "windows1251",    1251, 0 },
    { "windows1252",    1252, 0 },
    { "windows1253",    1253, 0 },
    { "windows1254",    1254, 0 },
    { "windows1255",    1255, 0 },
    { "windows1256",    1256, 0 },
    { "windows1257",    1257, 0 },
    { "windows874",     874,  0 },
    { "windows932",     932,  0 },
    { "windows936",     936,  0 },
};

/* get the codepage from the 'codepage' string */
ICONV_INLINE
unsigned long iconv_get_codepage(const char *codepage)
{
    unsigned long cp = ICONV_CP_UNKNOWN;
    char *codepage_ = NULL;
    if (codepage != NULL) {
        codepage_ = iconv_strdup(codepage);
        if (codepage_ != NULL)
            codepage_ = iconv_strlwr(codepage_);
    }
    if (codepage_ != NULL) {
        // Ansi
        if (iconv_stricmp(codepage_, "") == 0 || iconv_stricmp(codepage_, "ansi") == 0 || iconv_stricmp(codepage_, "mbcs") == 0)
            cp = ICONV_CP_ANSI;

        // UTF-XX
        else if (iconv_stricmp(codepage_, "utf8") == 0 || iconv_stricmp(codepage_, "utf-8") == 0)
            cp = ICONV_CP_UTF8;
        else if (iconv_stricmp(codepage_, "utf16") == 0 || iconv_stricmp(codepage_, "utf-16") == 0)
            cp = ICONV_CP_UTF16;
        else if (iconv_stricmp(codepage_, "utf32") == 0 || iconv_stricmp(codepage_, "utf-32") == 0)
            cp = ICONV_CP_UTF32;

        // Unicode
        else if (iconv_stricmp(codepage_, "unicode") == 0 || iconv_stricmp(codepage_, "utf16le") == 0
            || iconv_stricmp(codepage_, "utf-16le") == 0)
            cp = ICONV_CP_UNICODE;

        // English
        else if (iconv_stricmp(codepage_, "iso-8859-1") == 0 || iconv_stricmp(codepage_, "iso_8859_1") == 0)
            cp = ICONV_CP_ISO_8859_1;
        else if (iconv_stricmp(codepage_, "iso-8859-15") == 0 || iconv_stricmp(codepage_, "iso_8859_15") == 0)
            cp = ICONV_CP_ISO_8859_15;
        else if (iconv_stricmp(codepage_, "windows-1252") == 0 || iconv_stricmp(codepage_, "windows_1252") == 0)
            cp = ICONV_CP_WINDOWS_1252;

        // Simplified Chinese
        else if (iconv_stricmp(codepage_, "windows-936") == 0 || iconv_stricmp(codepage_, "windows_936") == 0)
            cp = ICONV_CP_WINDOWS_936;
        else if (iconv_stricmp(codepage_, "gb2312") == 0)
            cp = ICONV_CP_GB2312;
        else if (iconv_stricmp(codepage_, "gb18030") == 0)
            cp = ICONV_CP_GB18030;

        // Traditional Chinese
        else if (iconv_stricmp(codepage_, "gbk") == 0)
            cp = ICONV_CP_GBK;
        else if (iconv_stricmp(codepage_, "big5") == 0)
            cp = ICONV_CP_BIG5;

        free(codepage_);
        codepage_ = NULL;
    }
    return cp;
}

/* Allocates descriptor for code conversion from encoding 'fromcode' to
   encoding 'tocode'. */
ICONV_INLINE
iconv_t iconv_open(const char *tocode, const char *fromcode)
{
    iconv_t cd = NULL;
    unsigned long cp_to, cp_from;
    iconv_codepage_t icp;
    if (tocode == NULL)
        cp_to = ICONV_CP_ANSI;
    if (fromcode == NULL)
        cp_from = ICONV_CP_DEFAULT;
    icp = (iconv_codepage_t)malloc(sizeof(icp));
    if (icp != NULL) {
        // get the codepage from the 'tocode' string
        if (tocode != NULL)
            cp_to = iconv_get_codepage(tocode);
        else
            cp_to = ICONV_CP_ANSI;
        
        // get the codepage from the 'fromcode' string
        if (fromcode != NULL)
            cp_from = iconv_get_codepage(tocode);
        else
            cp_from = ICONV_CP_ANSI;

        if (cp_to == ICONV_CP_UNKNOWN || cp_from == ICONV_CP_UNKNOWN)
            return INVALID_ICONV_CP;

        icp->cp_to = cp_to;
        icp->cp_from = cp_from;
        icp->conv_sizes = 0;
        cd = (iconv_t)icp;
    }
    return cd;
}

/* Frees resources allocated for conversion descriptor 'cd'. */
ICONV_INLINE
int iconv_close(iconv_t cd)
{
    int retval = 0;
    iconv_codepage_t icp = NULL;
    if (cd != NULL && cd != INVALID_ICONV_CP) {
        retval = 1;
        icp = (iconv_codepage_t)cd;
        if (icp != NULL) {
            // Check icp->cp_to is a legal value?
            if (icp->cp_to <= ICONV_CP_FIRST || icp->cp_to >= ICONV_CP_LAST)
                retval = 0;
            // Check icp->cp_from is a legal value?
            if (icp->cp_from <= ICONV_CP_FIRST || icp->cp_from >= ICONV_CP_LAST)
                retval = 0;
        }
        free(cd);
        cd = NULL;
        
    }
    return retval;
}

/* Converts, using conversion descriptor 'cd' */
ICONV_INLINE
size_t iconv(iconv_t cd, ICONV_CONST char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    size_t conv_size = 0;
    return conv_size;
}

///////////////////////////////////////////////////////////////////////////
//  ansi ==> unicode
///////////////////////////////////////////////////////////////////////////

ICONV_INLINE
int iconv_ansi_to_unicode(const char *ansi_str, int ansi_size, wchar_t *dest_unicode, int dest_size)
{
    int unicode_size;
    int dest_max;
    wchar_t *unicode_str;

    if (ansi_str == NULL)
        return -1;

    // ansi_size includes the null character
    unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, NULL, 0);

    // if dest_ansi is NULL, only return the result of dest_ansi sizes
    if (dest_unicode == NULL || dest_size < 0)
        return unicode_size;

    // if second time call this function, dest_size must greater than 0.
    if (dest_size == 0)
        return -1;

    if ((unicode_size == 0) && (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)) {
        // ansi_size includes the null character
        ansi_size = (int)strlen(ansi_str);
        unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, NULL, 0);
        if ((unicode_size == 0) && (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)) {
            unicode_size = MultiByteToWideChar(CP_ACP, 0, ansi_str, ansi_size, NULL, 0);
        }
    }

    dest_max = (dest_size - 1) + (ansi_size < 0);
    if (unicode_size <= dest_max) {
        // dest_size is enough to transfer
        unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, dest_unicode, unicode_size);
        if (unicode_size >= 0 && ansi_size != 0) {
            if (ansi_size > 0)
                dest_unicode[unicode_size] = L'\0';
        }
        else if (unicode_size == 0 && ansi_size == 0) {
            dest_unicode[unicode_size] = L'\0';
        }
    }
    else {

#if defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0)

        // if dest_size is not large enough to transfer
        unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, dest_unicode, dest_size - 1);
        if (unicode_size >= 0 && ansi_size != 0) {
            dest_unicode[dest_size - 1] = L'\0';

            // set last error to ERROR_INSUFFICIENT_BUFFER
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            unicode_size = dest_size - 1;
        }
#else
        // if dest_size is not large enough to transfer
        //unicode_str = new wchar_t[unicode_size + 1];
        unicode_str = WIN_ICONV_MALLOC(unicode_size + 1, wchar_t);
        if (unicode_str != NULL) {
            unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, unicode_str, unicode_size);
            if (unicode_size >= 0 && ansi_size != 0) {
                if (ansi_size > 0)
                    unicode_str[unicode_size] = L'\0';
                memcpy(dest_unicode, unicode_str, (dest_size - 1) * sizeof(wchar_t));
                dest_unicode[dest_size - 1] = L'\0';

                // set last error to ERROR_INSUFFICIENT_BUFFER
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                unicode_size = dest_size - 1;
            }
            if (unicode_str != NULL)
                //delete[] unicode_str;
                WIN_ICONV_FREE(unicode_str);
        }
        else {
            unicode_size = -1;
        }

#endif  /* defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0) */

    }

    return unicode_size;
}

ICONV_INLINE
int iconv_ansi_to_unicode_auto(const char *ansi_str, int ansi_size, wchar_t **dest_unicode)
{
    int unicode_size;
    wchar_t *unicode_str;

#if 0
	if (dest_unicode == NULL)
		return -1;

    if (ansi_str == NULL) {
        *dest_unicode = NULL;
        return -1;
    }
#endif

    // Tips: Set second parameter to MB_ERR_INVALID_CHARS is faster than set to 0 when call the MultiByteToWideChar() function

    // ansi_size not includes the null character, if unicode_size not includes the null character,
    // if unicode_size < 0, the function processes the entire input string unicode_str, including the terminating null character.
    unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, NULL, 0);
    if ((unicode_size == 0) && (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)) {
        // ansi_size includes the null character
        ansi_size = (int)strlen(ansi_str);
        unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, NULL, 0);
        if ((unicode_size == 0) && (GetLastError() == ERROR_NO_UNICODE_TRANSLATION)) {
            unicode_size = MultiByteToWideChar(CP_ACP, 0, ansi_str, ansi_size, NULL, 0);
        }
    }
    if (unicode_size > 0) {
        //unicode_str = new wchar_t[unicode_size + 1];
        unicode_str = WIN_ICONV_MALLOC(unicode_size + 1, wchar_t);
        if (unicode_str != NULL) {
            unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, unicode_str, unicode_size);
            if (unicode_size >= 0) {
                unicode_str[unicode_size] = L'\0';

                *dest_unicode = unicode_str;
            }
        }
        else {
            *dest_unicode = NULL;
            unicode_size = -1;
        }
    }
    else {
        unicode_str = WIN_ICONV_MALLOC(1, wchar_t);
        unicode_str[0] = L'\0';

        *dest_unicode = unicode_str;
    }

    return unicode_size;
}

///////////////////////////////////////////////////////////////////////////
//  unicode ==> ansi
///////////////////////////////////////////////////////////////////////////

ICONV_INLINE
int iconv_unicode_to_ansi(const wchar_t *unicode_str, int unicode_size, char *dest_ansi, int dest_size)
{
    int ansi_size;
    char *ansi_str;
    int dest_max;
    if (unicode_str == NULL)
        return -1;

    // ansi_size includes the null character
    ansi_size = WideCharToMultiByte(CP_ACP, 0, unicode_str, unicode_size, NULL, 0, NULL, NULL);

    // if dest_ansi is NULL, only return the result of dest_ansi sizes
    if (dest_ansi == NULL || dest_size < 0)
        return ansi_size;

    // if second time call this function, dest_size must greater than 0.
    if (dest_size == 0)
        return -1;

    dest_max = (dest_size - 1) + (unicode_size < 0);
    if (ansi_size <= dest_max) {
        // dest_size is enough to transfer
        ansi_size = WideCharToMultiByte(CP_ACP, 0, unicode_str, unicode_size, dest_ansi, ansi_size, NULL, NULL);
        if (ansi_size >= 0 && unicode_size != 0) {
            if (unicode_size > 0)
                dest_ansi[ansi_size] = '\0';
        }
        else if (ansi_size == 0 && unicode_size == 0) {
            dest_ansi[ansi_size] = '\0';
        }
    }
    else {

#if defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0)

        // if dest_size is not large enough to transfer
        ansi_size = WideCharToMultiByte(CP_ACP, 0, unicode_str, unicode_size, dest_ansi, dest_size - 1, NULL, NULL);
        if (ansi_size >= 0 && unicode_size != 0) {
            dest_ansi[dest_size - 1] = '\0';

            // set last error to ERROR_INSUFFICIENT_BUFFER
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            ansi_size = dest_size - 1;
        }
#else
        // if dest_size is not large enough to transfer
        //ansi_str = new char[ansi_size + 1];
        ansi_str = WIN_ICONV_MALLOC(ansi_size + 1, char);
        if (ansi_str != NULL) {
            ansi_size = WideCharToMultiByte(CP_ACP, 0, unicode_str, unicode_size, ansi_str, ansi_size, NULL, NULL);
            if (ansi_size >= 0 && unicode_size != 0) {
                if (unicode_size > 0)
                    ansi_str[ansi_size] = '\0';
                memcpy(dest_ansi, ansi_str, (dest_size - 1) * sizeof(char));
                dest_ansi[dest_size - 1] = '\0';

                // set last error to ERROR_INSUFFICIENT_BUFFER
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                ansi_size = dest_size - 1;
            }
            if (ansi_str != NULL)
                //delete[] ansi_str;
                WIN_ICONV_FREE(ansi_str);
        }
        else {
            ansi_size = -1;
        }

#endif  /* defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0) */

    }

    return ansi_size;
}

ICONV_INLINE
int iconv_unicode_to_ansi_auto(const wchar_t *unicode_str, int unicode_size, char **dest_ansi)
{
    char *ansi_str;
    int ansi_size;
	if (dest_ansi == NULL)
		return -1;

    if (unicode_str == NULL)
        return -1;

	*dest_ansi = NULL;

    // ansi_size not includes the null character, if unicode_size not includes the null character,
    // if unicode_size < 0, the function processes the entire input string unicode_str, including the terminating null character.
    ansi_size = WideCharToMultiByte(CP_ACP, 0, unicode_str, unicode_size, NULL, 0, NULL, NULL);
    if (ansi_size > 0 || (ansi_size == 0 && unicode_size == 0)) {
        //ansi_str = new char[ansi_size + 1];
        ansi_str = WIN_ICONV_MALLOC(ansi_size + 1, char);
        if (ansi_str != NULL) {
            ansi_size = WideCharToMultiByte(CP_ACP, 0, unicode_str, unicode_size, ansi_str, ansi_size, NULL, NULL);
            if (ansi_size >= 0 && unicode_size != 0) {
                if (unicode_size > 0)
                    ansi_str[ansi_size] = '\0';

                *dest_ansi = ansi_str;
            }
            else if (ansi_size == 0 && unicode_size == 0) {
                ansi_str[ansi_size] = '\0';

                *dest_ansi = ansi_str;
            }
        }
        else {
            ansi_size = -1;
        }
    }

    return ansi_size;
}

///////////////////////////////////////////////////////////////////////////
//  utf8 ==> unicode
///////////////////////////////////////////////////////////////////////////

ICONV_INLINE
int iconv_utf8_to_unicode(const char *utf8_str, int utf8_size, wchar_t *dest_unicode, int dest_size)
{
    int unicode_size;
    wchar_t *unicode_str;
    int dest_max;
    if (utf8_str == NULL)
        return -1;

    // utf8_size includes the null character
    unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, NULL, 0);

    // if dest_unicode is NULL, only return the result of dest_unicode sizes
    if (dest_unicode == NULL || dest_size < 0)
        return unicode_size;

    // if second time call this function, dest_size must greater than 0.
    if (dest_size == 0)
        return -1;

    dest_max = (dest_size - 1) + (utf8_size < 0);
    if (unicode_size <= dest_max) {
        // dest_size is enough to transfer
        unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, dest_unicode, unicode_size);
        if (unicode_size >= 0 && utf8_size != 0) {
            if (utf8_size > 0)
                dest_unicode[unicode_size] = L'\0';
        }
        else if (unicode_size == 0 && utf8_size == 0) {
            dest_unicode[unicode_size] = L'\0';
        }
    }
    else {

#if defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0)

        // if dest_size is not large enough to transfer
        unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, dest_unicode, dest_size - 1);
        if (unicode_size >= 0 && utf8_size != 0) {
            dest_unicode[dest_size - 1] = L'\0';

            // set last error to ERROR_INSUFFICIENT_BUFFER
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            unicode_size = dest_size - 1;
        }
#else
        // if dest_size is not large enough to transfer
        //unicode_str = new wchar_t[unicode_size + 1];
        unicode_str = WIN_ICONV_MALLOC(unicode_size + 1, wchar_t);
        if (unicode_str != NULL) {
            unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, unicode_str, unicode_size);
            if (unicode_size >= 0 && utf8_size != 0) {
                if (utf8_size > 0)
                    unicode_str[unicode_size] = L'\0';
                memcpy(dest_unicode, unicode_str, (dest_size - 1) * sizeof(wchar_t));
                dest_unicode[dest_size - 1] = L'\0';

                // set last error to ERROR_INSUFFICIENT_BUFFER
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                unicode_size = dest_size - 1;
            }
            if (unicode_str != NULL)
                //delete[] unicode_str;
                WIN_ICONV_FREE(unicode_str);
        }
        else {
            unicode_size = -1;
        }

#endif  /* defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0) */

    }

    return unicode_size;
}

ICONV_INLINE
int iconv_utf8_to_unicode_auto(const char *utf8_str, int utf8_size, wchar_t **dest_unicode)
{
    wchar_t *unicode_str;
    int unicode_size;
	if (dest_unicode == NULL)
		return -1;

    if (utf8_str == NULL)
        return -1;

	*dest_unicode = NULL;

    // unicode_size not includes the null character, if utf8_size not includes the null character,
    // if utf8_size < 0, the function processes the entire input string utf8_str, including the terminating null character.
    unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, NULL, 0);
    if (unicode_size > 0 || (unicode_size == 0 && utf8_size == 0)) {
        //unicode_str = new wchar_t[unicode_size + 1];
        unicode_str = WIN_ICONV_MALLOC(unicode_size + 1, wchar_t);
        if (unicode_str != NULL) {
            unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, unicode_str, unicode_size);
            if (unicode_size >= 0 && utf8_size != 0) {
                if (utf8_size > 0)
                    unicode_str[unicode_size] = L'\0';

                *dest_unicode = unicode_str;
            }
            else if (unicode_size == 0 && utf8_size == 0) {
                unicode_str[unicode_size] = L'\0';

                *dest_unicode = unicode_str;
            }
        }
        else {
            unicode_size = -1;
        }
    }

    return unicode_size;
}

///////////////////////////////////////////////////////////////////////////
//  unicode ==> utf8
///////////////////////////////////////////////////////////////////////////

ICONV_INLINE
int iconv_unicode_to_utf8(const wchar_t *unicode_str, int unicode_size, char *dest_utf8, int dest_size)
{
    char *utf8_str;
    int utf8_size;
    int dest_max;
    if (unicode_str == NULL)
        return -1;

    // utf8_size includes the null character
    utf8_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, unicode_size, NULL, 0, NULL, NULL);

    // if dest_utf8 is NULL, only return the result of dest_utf8 sizes
    if (dest_utf8 == NULL || dest_size < 0)
        return utf8_size;

    // if second time call this function, dest_size must greater than 0.
    if (dest_size == 0)
        return -1;

    dest_max = (dest_size - 1) + (unicode_size < 0);
    if (utf8_size <= dest_max) {
        // dest_size is enough to transfer
        utf8_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, unicode_size, dest_utf8, utf8_size, NULL, NULL);
        if (utf8_size >= 0 && unicode_size != 0) {
            if (unicode_size > 0)
                dest_utf8[utf8_size] = '\0';
        }
        else if (utf8_size == 0 && unicode_size == 0) {
            dest_utf8[utf8_size] = '\0';
        }
    }
    else {
#if defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0)
        // if dest_size is not large enough to transfer
        utf8_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, unicode_size, dest_utf8, dest_size - 1, NULL, NULL);
        if (utf8_size >= 0 && unicode_size != 0) {
            dest_utf8[dest_size - 1] = '\0';

            // set last error to ERROR_INSUFFICIENT_BUFFER
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            utf8_size = dest_size - 1;
        }
#else
        // if dest_size is not large enough to transfer
        //utf8_str = new char[utf8_size + 1];
        utf8_str = WIN_ICONV_MALLOC(utf8_size + 1, char);
        if (utf8_str != NULL) {
            utf8_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, unicode_size, utf8_str, utf8_size, NULL, NULL);
            if (utf8_size >= 0 && unicode_size != 0) {
                if (unicode_size > 0)
                    utf8_str[utf8_size] = '\0';
                memcpy(dest_utf8, utf8_str, (dest_size - 1) * sizeof(char));
                dest_utf8[dest_size - 1] = '\0';

                // set last error to ERROR_INSUFFICIENT_BUFFER
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                utf8_size = dest_size - 1;
            }
            if (utf8_str != NULL)
                //delete[] utf8_str;
                WIN_ICONV_FREE(utf8_str);
        }
        else {
            utf8_size = -1;
        }
#endif  /* defined(USE_FAST_TRANSFER_COPY) && (USE_FAST_TRANSFER_COPY != 0) */
    }

    return utf8_size;
}

ICONV_INLINE
int iconv_unicode_to_utf8_auto(const wchar_t *unicode_str, int unicode_size, char **dest_utf8)
{
    char *utf8_str;
    int utf8_size;
	if (dest_utf8 == NULL)
		return -1;

    if (unicode_str == NULL)
        return -1;

	*dest_utf8 = NULL;

    // utf8_size not includes the null character, if unicode_size not includes the null character,
    // if unicode_size < 0, the function processes the entire input string unicode_str, including the terminating null character.
    utf8_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, unicode_size, NULL, 0, NULL, NULL);
    if (utf8_size > 0 || (utf8_size == 0 && unicode_size == 0)) {
        //utf8_str = new char[utf8_size + 1];
        utf8_str = WIN_ICONV_MALLOC(utf8_size + 1, char);
        if (utf8_str != NULL) {
            utf8_size = WideCharToMultiByte(CP_UTF8, 0, unicode_str, unicode_size, utf8_str, utf8_size, NULL, NULL);
            if (utf8_size >= 0 && unicode_size != 0) {
                if (unicode_size > 0)
                    utf8_str[utf8_size] = '\0';

                *dest_utf8 = utf8_str;
            }
            else if (utf8_size == 0 && unicode_size == 0) {
                utf8_str[utf8_size] = '\0';

                *dest_utf8 = utf8_str;
            }
        }
        else {
            utf8_size = -1;
        }
    }

    return utf8_size;
}

///////////////////////////////////////////////////////////////////////////
//  ansi ==> utf8
///////////////////////////////////////////////////////////////////////////

ICONV_INLINE
int iconv_ansi_to_utf8(const char *ansi_str, int ansi_size, char *dest_utf8, int dest_size)
{
    return 0;
}

ICONV_INLINE
int iconv_ansi_to_utf8_auto(const char *ansi_str, int ansi_size, char **dest_utf8)
{
    wchar_t *unicode_str;
    int unicode_size;
    int utf8_size = -1;

	if (dest_utf8 == NULL)
		return -1;

    if (ansi_str == NULL)
        return -1;

	*dest_utf8 = NULL;

    // unicode_size not includes the null character, if ansi_size not includes the null character,
    // if ansi_size < 0, the function processes the entire input string ansi_str, including the terminating null character.
    unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, NULL, 0);
    if (unicode_size > 0 || (unicode_size == 0 && ansi_size == 0)) {
        //unicode_str = new wchar_t[unicode_size + 1];
        unicode_str = WIN_ICONV_MALLOC(unicode_size + 1, wchar_t);
        if (unicode_str != NULL) {
            unicode_size = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, ansi_size, unicode_str, unicode_size);
            if (unicode_size >= 0 && ansi_size != 0) {
                if (ansi_size > 0)
                    unicode_str[unicode_size] = L'\0';

                utf8_size = iconv_unicode_to_utf8_auto(unicode_str, unicode_size, dest_utf8);
            }
            else if (unicode_size == 0 && ansi_size == 0) {
                unicode_str[unicode_size] = L'\0';

                utf8_size = iconv_unicode_to_utf8_auto(unicode_str, unicode_size, dest_utf8);
            }
            if (unicode_str != NULL)
                //delete[] unicode_str;
                WIN_ICONV_FREE(unicode_str);
        }
        else {
            unicode_size = -1;
        }
    }

    return utf8_size;
}

///////////////////////////////////////////////////////////////////////////
//  utf8 ==> ansi
///////////////////////////////////////////////////////////////////////////

ICONV_INLINE
int iconv_utf8_to_ansi(const char *utf8_str, int utf8_size, char *dest_ansi, int dest_size)
{
    return 0;
}

ICONV_INLINE
int iconv_utf8_to_ansi_auto(const char *utf8_str, int utf8_size, char **dest_ansi)
{
    wchar_t *unicode_str;
    int unicode_size;
    int ansi_size = -1;
	if (dest_ansi == NULL)
		return -1;

    if (utf8_str == NULL)
        return -1;

	*dest_ansi = NULL;

    // unicode_size not includes the null character, if utf8_size not includes the null character,
    // if utf8_size < 0, the function processes the entire input string utf8_str, including the terminating null character.
    unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, NULL, 0);
    if (unicode_size > 0 || (unicode_size == 0 && utf8_size == 0)) {
        //unicode_str = new wchar_t[unicode_size + 1];
        unicode_str = WIN_ICONV_MALLOC(unicode_size + 1, wchar_t);
        if (unicode_str != NULL) {
            unicode_size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8_str, utf8_size, unicode_str, unicode_size);
            if (unicode_size >= 0 && utf8_size != 0) {
                if (utf8_size > 0)
                    unicode_str[unicode_size] = L'\0';

                ansi_size = iconv_unicode_to_ansi_auto(unicode_str, unicode_size, dest_ansi);
            }
            else if (unicode_size == 0 && utf8_size == 0) {
                unicode_str[unicode_size] = L'\0';

                ansi_size = iconv_unicode_to_ansi_auto(unicode_str, unicode_size, dest_ansi);
            }
            if (unicode_str != NULL)
                //delete[] unicode_str;
                WIN_ICONV_FREE(unicode_str);
        }
        else {
            unicode_size = -1;
        }
    }

    return ansi_size;
}

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)                    // 恢复warning状态
#endif

#endif  /* _WIN32 || _WIN64 */
