
///////////////////////////////////////////////////////////////////////////

#ifndef _JIMIC_STRING_JM_STRINGS_INL_H_
#define _JIMIC_STRING_JM_STRINGS_INL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_JM_STRINGS_H_
#error "Don't include the internal file <jimic/string/jm_strings.inl.h> directly; include <jimic/string/jm_strings.h> instead."
#endif

#include "jimic/core/jimic_def.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef NOT_IS_INLINE_INCLUDE
#define NOT_IS_INLINE_INCLUDE   0
#endif

#if NOT_IS_INLINE_INCLUDE

#if defined(JIMI_IS_WINDOWS)
#include <tchar.h>
#include <mbstring.h>
#endif  /* JIMI_IS_WINDOWS */

#endif  /* NOT_IS_INLINE_INCLUDE */

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
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

#if defined(__MINGW__) || defined(__MINGW32__) || defined(__MINGW64__)

/**
 * strdup error on g++ with c++0x
 *
 * refrence: http://stackoverflow.com/questions/5573775/strdup-error-on-g-with-c0x
 */

/* extern declare strdup() & _strdup() for MinGW(gcc) on Windows */
extern _CRTIMP char* __cdecl __MINGW_NOTHROW	_strupr (char*);
extern _CRTIMP char* __cdecl __MINGW_NOTHROW	strdup (const char*) __MINGW_ATTRIB_MALLOC;

#endif  /* __MINGW__, __MINGW32__, __MINGW64__ */

///////////////////////////////////////////////////////////////////////////

// the errno can get from _get_errno(), _set_errno()
// _TRUNCATE = ((size_t)-1)

#ifdef __cplusplus
extern "C" {
#endif

#if defined(JIMI_IS_WINDOWS) && (JIMI_IS_WINDOWS != 0) && !defined(__MINGW32__)

/**
 * strlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strlen(const char *_Str)
{
    return strlen(_Str);
}

JMC_INLINE_NONSTD(size_t) jm_wcslen(const wchar_t *_Str)
{
    return wcslen(_Str);
}

JMC_INLINE_NONSTD(size_t) jm_tcslen(const TCHAR *_Str)
{
    return _tcslen(_Str);
}

JMC_INLINE_NONSTD(size_t) jm_mbslen(const unsigned char *_Str)
{
    return _mbslen(_Str);
}

/**
 * strnlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strnlen(const char *_Str, size_t _MaxCount)
{
    return strnlen_s(_Str, _MaxCount);
}

JMC_INLINE_NONSTD(size_t) jm_wcsnlen(const wchar_t *_Str, size_t _MaxCount)
{
    return wcsnlen_s(_Str, _MaxCount);
}

JMC_INLINE_NONSTD(size_t) jm_tcsnlen(const TCHAR *_Str, size_t _MaxCount)
{
    return _tcsnlen(_Str, _MaxCount);
}

JMC_INLINE_NONSTD(size_t) jm_mbsnlen(const unsigned char *_Str, size_t _MaxCount)
{
    return _mbsnlen(_Str, _MaxCount);
}

/**
 * strcpy()
 */
JMC_INLINE_NONSTD(char *) jm_strcpy(char *_Dest, size_t _NumberOfElements, const char *_Source)
{
    errno_t err = strcpy_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcscpy(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source)
{
    errno_t err = wcscpy_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcscpy(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source)
{
    errno_t err = _tcscpy_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbscpy(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source)
{
    errno_t err = _mbscpy_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

/**
 * strncpy()
 */
JMC_INLINE_NONSTD(char *) jm_strncpy(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount)
{
    errno_t err = strncpy_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcsncpy(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source, size_t _MaxCount)
{
    errno_t err = wcsncpy_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcsncpy(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source, size_t _MaxCount)
{
    errno_t err = _tcsncpy_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbsncpy(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source, size_t _MaxCount)
{
    errno_t err = _mbsncpy_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

/**
 * strcat()
 */
JMC_INLINE_NONSTD(char *) jm_strcat(char *_Dest, size_t _NumberOfElements, const char *_Source)
{
    errno_t err = strcat_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcscat(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source)
{
    errno_t err = wcscat_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcscat(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source)
{
    errno_t err = _tcscat_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbscat(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source)
{
    errno_t err = _mbscat_s(_Dest, _NumberOfElements, _Source);
    return _Dest;
}

/**
 * strncat()
 */
JMC_INLINE_NONSTD(char *) jm_strncat(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount)
{
    errno_t err = strncat_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcsncat(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source, size_t _MaxCount)
{
    errno_t err = wcsncat_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcsncat(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source, size_t _MaxCount)
{
    errno_t err = _tcsncat_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbsncat(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source, size_t _MaxCount)
{
    errno_t err = _mbsncat_s(_Dest, _NumberOfElements, _Source, _MaxCount);
    return _Dest;
}

/**
 * strdup()
 */
JMC_INLINE_NONSTD(char *) jm_strdup(const char *_Src)
{
    return _strdup(_Src);
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcsdup(const wchar_t *_Src)
{
    return _wcsdup(_Src);
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcsdup(const TCHAR *_Src)
{
    return _tcsdup(_Src);
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbsdup(const unsigned char *_Src)
{
#if defined(_MSC_VER) && (defined(_DEBUG) || !defined(NDEBUG))
#ifdef __cplusplus
    //const char *_Ptr = CONST_CAST_CONST(_Src, unsigned char *, char *);
    const char *_Ptr = const_cast<const char *>(reinterpret_cast<char *>
        (const_cast<unsigned char *>(_Src)));
    return reinterpret_cast<unsigned char *>(_strdup(_Ptr));
#else
    return (unsigned char *)_strdup((const char *)_Src);
#endif
#else
    return _mbsdup(_Src);
#endif
}

/**
 * strcmp()
 */
JMC_INLINE_NONSTD(int) jm_strcmp(const char *_Str1, const char *_Str2)
{
    return strcmp(_Str1, _Str2);
}

JMC_INLINE_NONSTD(int) jm_wcscmp(const wchar_t *_Str1, const wchar_t *_Str2)
{
    return wcscmp(_Str1, _Str2);
}

JMC_INLINE_NONSTD(int) jm_tcscmp(const TCHAR *_Str1, const TCHAR *_Str2)
{
    return _tcscmp(_Str1, _Str2);
}

JMC_INLINE_NONSTD(int) jm_mbscmp(const unsigned char *_Str1, const unsigned char *_Str2)
{
    return _mbscmp(_Str1, _Str2);
}

/**
 * strncmp()
 */
JMC_INLINE_NONSTD(int) jm_strncmp(const char *_Str1, const char *_Str2, size_t _MaxCount)
{
    return strncmp(_Str1, _Str2, _MaxCount);
}

JMC_INLINE_NONSTD(int) jm_wcsncmp(const wchar_t *_Str1, const wchar_t *_Str2, size_t _MaxCount)
{
    return wcsncmp(_Str1, _Str2, _MaxCount);
}

JMC_INLINE_NONSTD(int) jm_tcsncmp(const TCHAR *_Str1, const TCHAR *_Str2, size_t _MaxCount)
{
    return _tcsncmp(_Str1, _Str2, _MaxCount);
}

JMC_INLINE_NONSTD(int) jm_mbsncmp(const unsigned char *_Str1, const unsigned char *_Str2, size_t _MaxCount)
{
    return _mbsncmp(_Str1, _Str2, _MaxCount);
}

/**
 * strstr()
 */
JMC_INLINE_NONSTD(char *) jm_strstr(const char *_Str, const char *_SubStr)
{
    return (char *)strstr(_Str, _SubStr);
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcsstr(const wchar_t *_Str, const wchar_t *_SubStr)
{
    return (wchar_t *)wcsstr(_Str, _SubStr);
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcsstr(const TCHAR *_Str, const TCHAR *_SubStr)
{
    return (TCHAR *)_tcsstr(_Str, _SubStr);
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbsstr(const unsigned char *_Str, const unsigned char *_SubStr)
{
    return (unsigned char *)_mbsstr(_Str, _SubStr);
}

/**
 * strupr()
 */
JMC_INLINE_NONSTD(char *) jm_strupr(char *_Str, size_t _NumberOfElements)
{
    errno_t err = _strupr_s(_Str, _NumberOfElements);
    return _Str;
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcsupr(wchar_t *_Str, size_t _NumberOfElements)
{
    errno_t err = _wcsupr_s(_Str, _NumberOfElements);
    return _Str;
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcsupr(TCHAR *_Str, size_t _NumberOfElements)
{
    errno_t err = _tcsupr_s(_Str, _NumberOfElements);
    return _Str;
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbsupr(unsigned char *_Str, size_t _NumberOfElements)
{
    errno_t err = _mbsupr_s(_Str, _NumberOfElements);
    return _Str;
}

/**
 * strlwr()
 */
JMC_INLINE_NONSTD(char *) jm_strlwr(char *_Str, size_t _NumberOfElements)
{
    errno_t err = _strlwr_s(_Str, _NumberOfElements);
    return _Str;
}

JMC_INLINE_NONSTD(wchar_t *) jm_wcslwr(wchar_t *_Str, size_t _NumberOfElements)
{
    errno_t err = _wcslwr_s(_Str, _NumberOfElements);
    return _Str;
}

JMC_INLINE_NONSTD(TCHAR *) jm_tcslwr(TCHAR *_Str, size_t _NumberOfElements)
{
    errno_t err = _tcslwr_s(_Str, _NumberOfElements);
    return _Str;
}

JMC_INLINE_NONSTD(unsigned char *) jm_mbslwr(unsigned char *_Str, size_t _NumberOfElements)
{
    errno_t err = _mbslwr_s(_Str, _NumberOfElements);
    return _Str;
}

/**
 * sprintf()
 */
JMC_INLINE_NONSTD(int) jm_sprintf(char *buffer, size_t numberOfElements, const char *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    va_start(arg_list, format);
    ret_cnt = vsprintf_s(buffer, numberOfElements, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

JMC_INLINE_NONSTD(int) jm_swprintf(wchar_t *buffer, size_t numberOfElements, const wchar_t *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    va_start(arg_list, format);
    ret_cnt = vswprintf_s(buffer, numberOfElements, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

JMC_INLINE_NONSTD(int) jm_stprintf(TCHAR *buffer, size_t numberOfElements, const TCHAR *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    va_start(arg_list, format);
    ret_cnt = _vstprintf_s(buffer, numberOfElements, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

/**
 * vsprintf()
 */
JMC_INLINE_NONSTD(int) jm_vsprintf(char *buffer, size_t numberOfElements, const char *format, va_list arg_list)
{
    return vsprintf_s(buffer, numberOfElements, format, arg_list);
}

JMC_INLINE_NONSTD(int) jm_vswprintf(wchar_t *buffer, size_t numberOfElements, const wchar_t *format, va_list arg_list)
{
    return vswprintf_s(buffer, numberOfElements, format, arg_list);
}

JMC_INLINE_NONSTD(int) jm_vstprintf(TCHAR *buffer, size_t numberOfElements, const TCHAR *format, va_list arg_list)
{
    return _vstprintf_s(buffer, numberOfElements, format, arg_list);
}

/**
 * snprintf()
 */
JMC_INLINE_NONSTD(int) jm_snprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    va_start(arg_list, format);
    ret_cnt = _vsnprintf_s(buffer, numberOfElements, count, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

JMC_INLINE_NONSTD(int) jm_snwprintf(wchar_t *buffer, size_t numberOfElements, size_t count, const wchar_t *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    va_start(arg_list, format);
    ret_cnt = _vsnwprintf_s(buffer, numberOfElements, count, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

JMC_INLINE_NONSTD(int) jm_sntprintf(TCHAR *buffer, size_t numberOfElements, size_t count, const TCHAR *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    va_start(arg_list, format);
    ret_cnt = _vsntprintf_s(buffer, numberOfElements, count, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

/**
 * vsnprintf()
 */
JMC_INLINE_NONSTD(int) jm_vsnprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, va_list arg_list)
{
    return _vsnprintf_s(buffer, numberOfElements, count, format, arg_list);
}

JMC_INLINE_NONSTD(int) jm_vsnwprintf(wchar_t *buffer, size_t numberOfElements, size_t count, const wchar_t *format, va_list arg_list)
{
    return _vsnwprintf_s(buffer, numberOfElements, count, format, arg_list);
}

JMC_INLINE_NONSTD(int) jm_vsntprintf(TCHAR *buffer, size_t numberOfElements, size_t count, const TCHAR *format, va_list arg_list)
{
    return _vsntprintf_s(buffer, numberOfElements, count, format, arg_list);
}

///////////////////////////////////////////////////////////////////////////

#else  /* !JIMI_IS_WINDOWS */

///////////////////////////////////////////////////////////////////////////

/**
 * strlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strlen(const char *_Str)
{
    return strlen(_Str);
}

/**
 * strnlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strnlen(const char *_Str, size_t _MaxCount)
{
    return strlen(_Str);
}

/**
 * strcpy()
 */
JMC_INLINE_NONSTD(char *) jm_strcpy(char *_Dest, size_t _NumberOfElements, const char *_Source)
{
    char *_DestNew;
    _DestNew = strncpy(_Dest, _Source, _NumberOfElements - 1);
    _Dest[_NumberOfElements - 1] = '\0';
    return _DestNew;
}

/**
 * strncpy()
 */
JMC_INLINE_NONSTD(char *) jm_strncpy(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount)
{
    size_t n;
    n = JIMIC_MIN(_MaxCount, _NumberOfElements - 1);
    return strncpy(_Dest, _Source, n);
}

/**
 * strcat()
 */
JMC_INLINE_NONSTD(char *) jm_strcat(char *_Dest, size_t _NumberOfElements, const char *_Source)
{
    char *_DestNew;
    if (_NumberOfElements != 0)
        _DestNew = strncat(_Dest, _Source, _NumberOfElements - 1);
    else
        _DestNew = strncat(_Dest, _Source, 0);
    return _DestNew;
}

/**
 * strncat()
 */
JMC_INLINE_NONSTD(char *) jm_strncat(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount)
{
    char *_DestNew;
    size_t n;
    n = JIMIC_MIN(_MaxCount, _NumberOfElements - 1);
    _DestNew = strncat(_Dest, _Source, n);
    return _DestNew;
}

/**
 * strdup()
 */
JMC_INLINE_NONSTD(char *) jm_strdup(const char *_Src)
{
    return strdup(_Src);
}

/**
 * strcmp()
 */
JMC_INLINE_NONSTD(int) jm_strcmp(const char *_Str1, const char *_Str2)
{
    return strcmp(_Str1, _Str2);
}

/**
 * strncmp()
 */
JMC_INLINE_NONSTD(int) jm_strncmp(const char *_Str1, const char *_Str2, size_t _MaxCount)
{
    return strncmp(_Str1, _Str2, _MaxCount);
}

/**
 * strstr()
 */
JMC_INLINE_NONSTD(char *) jm_strstr(const char *_Str, const char *_SubStr)
{
    return (char *)strstr(_Str, _SubStr);
}

/**
 * strlwr()
 */
JMC_INLINE_NONSTD(char *) jm_strlwr(char *_Str, size_t _NumberOfElements)
{
    if (_NumberOfElements != 0)
        jm_strlwr_s(_Str, _NumberOfElements - 1);
    return _Str;
}

/**
 * strupr()
 */
JMC_INLINE_NONSTD(char *) jm_strupr(char *_Str, size_t _NumberOfElements)
{
    if (_NumberOfElements != 0)
        jm_strupr_s(_Str, _NumberOfElements - 1);
    return _Str;
}

/**
 * sprintf()
 */
JMC_INLINE_NONSTD(int) jm_sprintf(char *buffer, size_t numberOfElements, const char *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    size_t n;
    if (numberOfElements != 0)
        n = numberOfElements - 1;
    else
        n = 0;
    va_start(arg_list, format);
    ret_cnt = vsnprintf(buffer, n, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

/**
 * vsprintf()
 */
JMC_INLINE_NONSTD(int) jm_vsprintf(char *buffer, size_t numberOfElements, const char *format, va_list arg_list)
{
    if (numberOfElements != 0)
        return vsnprintf(buffer, numberOfElements - 1, format, arg_list);
    else
        return vsnprintf(buffer, 0, format, arg_list);
}

/**
 * snprintf()
 */
JMC_INLINE_NONSTD(int) jm_snprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, ...)
{
    int ret_cnt;
    va_list arg_list;
    size_t n;
    n = JIMIC_MIN(count, numberOfElements - 1);
    va_start(arg_list, format);
    ret_cnt = vsnprintf(buffer, n, format, arg_list);
    va_end(arg_list);
    return ret_cnt;
}

/**
 * vsnprintf()
 */
JMC_INLINE_NONSTD(int) jm_vsnprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, va_list arg_list)
{
    size_t n;
    n = JIMIC_MIN(count, numberOfElements - 1);
    return vsnprintf(buffer, n, format, arg_list);
}

///////////////////////////////////////////////////////////////////////////

#endif  /* JIMI_IS_WINDOWS */

///////////////////////////////////////////////////////////////////////////
// jm_strlwrs() & jm_struprs()
///////////////////////////////////////////////////////////////////////////

/**
 * str2lwr()
 */
JMC_INLINE_NONSTD(int) jm_str2lwr(char *dest, char *src, size_t n)
{
    char *orig = dest;
    while (n > 0) {
        if (*src != '\0') {
            *dest = jimic_tolower(*src);
            ++dest;
            ++src;
        }
        else
            break;
        --n;
    }
    *dest = '\0';
    return (int)(dest - orig);
}

/**
 * str2upr()
 */
JMC_INLINE_NONSTD(int) jm_str2upr(char *dest, char *src, size_t n)
{
    char *orig = dest;
    while (n > 0) {
        if (*src != '\0') {
            *dest = jimic_toupper(*src);
            ++dest;
            ++src;
        }
        else
            break;
        --n;
    }
    *dest = '\0';
    return (int)(dest - orig);
}

/**
 * strlwr_s()
 */
JMC_INLINE_NONSTD(int) jm_strlwr_s(char *src, size_t n)
{
    char *orig = src;
    while (n > 0) {
        if (*src != '\0') {
            *src = jimic_tolower(*src);
            ++src;
        }
        else
            break;
        --n;
    }
    return (int)(src - orig);
}

/**
 * strupr_s()
 */
JMC_INLINE_NONSTD(int) jm_strupr_s(char *src, size_t n)
{
    char *orig = src;
    while (n > 0) {
        if (*src != '\0') {
            *src = jimic_toupper(*src);
            ++src;
        }
        else
            break;
        --n;
    }
    return (int)(src - orig);
}

/**
 * strnlwr()
 */
JMC_INLINE_NONSTD(void) jm_strnlwr(char *dest, char *src, size_t n)
{
    while (n > 0) {
        *dest = jimic_tolower(*src);
        ++dest;
        ++src;
        --n;
    }
}

/**
 * strnupr()
 */
JMC_INLINE_NONSTD(void) jm_strnupr(char *dest, char *src, size_t n)
{
    while (n > 0) {
        *dest = jimic_toupper(*src);
        ++dest;
        ++src;
        --n;
    }
}

/**
 * strnlwr_s()
 */
JMC_INLINE_NONSTD(void) jm_strnlwr_s(char *src, size_t n)
{
    while (n > 0) {
        *src = jimic_tolower(*src);
        ++src;
        --n;
    }
}

/**
 * strnupr_s()
 */
JMC_INLINE_NONSTD(void) jm_strnupr_s(char *src, size_t n)
{
    while (n > 0) {
        *src = jimic_toupper(*src);
        ++src;
        --n;
    }
}

#ifdef __cplusplus
}
#endif

#endif  /* _JIMIC_STRING_JM_STRINGS_INL_H_ */
