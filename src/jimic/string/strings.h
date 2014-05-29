
#ifndef _JIMIC_STRING_STRINGS_H_
#define _JIMIC_STRING_STRINGS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>

#if defined(JIMI_IS_WINDOWS)
#include <tchar.h>
#include <mbstring.h>
#endif
#include <string.h>

#include <jimic/core/jimic_def.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(JIMI_IS_WINDOWS)

///////////////////////////////////////////////////////////////////////////

/**
 * strlen()
 */
JMC_INLINE_DECLARE(size_t) jm_strlen(const char *_Src);
JMC_INLINE_DECLARE(size_t) jm_wcslen(const wchar_t *_Src);
JMC_INLINE_DECLARE(size_t) jm_tcslen(const TCHAR *_Src);
JMC_INLINE_DECLARE(size_t) jm_mbslen(const unsigned char *_Src);

/**
 * strnlen()
 */
JMC_INLINE_DECLARE(size_t) jm_strnlen(const char *_Src, size_t _MaxCount);
JMC_INLINE_DECLARE(size_t) jm_wcsnlen(const wchar_t *_Src, size_t _MaxCount);
JMC_INLINE_DECLARE(size_t) jm_tcsnlen(const TCHAR *_Src, size_t _MaxCount);
JMC_INLINE_DECLARE(size_t) jm_mbsnlen(const unsigned char *_Src, size_t _MaxCount);

/**
 * strcpy()
 */
JMC_INLINE_DECLARE(char *)          jm_strcpy(char *_Dest, size_t _SizeInBytes, const char *_Source);
JMC_INLINE_DECLARE(wchar_t *)       jm_wcscpy(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source);
JMC_INLINE_DECLARE(TCHAR *)         jm_tcscpy(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source);
JMC_INLINE_DECLARE(unsigned char *) jm_mbscpy(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source);

/**
 * strncpy()
 */
JMC_INLINE_DECLARE(char *)          jm_strncpy(char *_Dest, size_t _SizeInBytes, const char *_Source, size_t _MaxCount);
JMC_INLINE_DECLARE(wchar_t *)       jm_wcsncpy(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source, size_t _MaxCount);
JMC_INLINE_DECLARE(TCHAR *)         jm_tcsncpy(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source, size_t _MaxCount);
JMC_INLINE_DECLARE(unsigned char *) jm_mbsncpy(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source, size_t _MaxCount);

/**
 * strcat()
 */
JMC_INLINE_DECLARE(char *)          jm_strcat(char *_Dest, size_t _SizeInBytes, const char *_Source);
JMC_INLINE_DECLARE(wchar_t *)       jm_wcscat(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source);
JMC_INLINE_DECLARE(TCHAR *)         jm_tcscat(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source);
JMC_INLINE_DECLARE(unsigned char *) jm_mbscat(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source);

/**
 * strncat()
 */
JMC_INLINE_DECLARE(char *)          jm_strncat(char *_Dest, size_t _SizeInBytes, const char *_Source, size_t _MaxCount);
JMC_INLINE_DECLARE(wchar_t *)       jm_wcsncat(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source, size_t _MaxCount);
JMC_INLINE_DECLARE(TCHAR *)         jm_tcsncat(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source, size_t _MaxCount);
JMC_INLINE_DECLARE(unsigned char *) jm_mbsncat(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source, size_t _MaxCount);

/**
 * strdup()
 */
JMC_INLINE_DECLARE(char *)          jm_strdup(const char *_Src);
JMC_INLINE_DECLARE(wchar_t *)       jm_wcsdup(const wchar_t *_Src);
JMC_INLINE_DECLARE(TCHAR *)         jm_tcsdup(const TCHAR *_Src);
JMC_INLINE_DECLARE(unsigned char *) jm_mbsdup(const unsigned char *_Src);

/**
 * strcmp()
 */
JMC_INLINE_DECLARE(int) jm_strcmp(const char *_Str1, const char *_Str2);
JMC_INLINE_DECLARE(int) jm_wcscmp(const wchar_t *_Str1, const wchar_t *_Str2);
JMC_INLINE_DECLARE(int) jm_tcscmp(const TCHAR *_Str1, const TCHAR *_Str2);
JMC_INLINE_DECLARE(int) jm_mbscmp(const unsigned char *_Str1, const unsigned char *_Str2);

/**
 * strstr()
 */
#ifndef jm_strstr
#define jm_strstr   strstr
#endif
#ifndef jm_wcsstr
#define jm_wcsstr   wcsstr
#endif
#ifndef jm_tcsstr
#define jm_tcsstr   _tcsstr
#endif
#ifndef jm_mbsstr
#define jm_mbsstr   _mbsstr
#endif

///////////////////////////////////////////////////////////////////////////

/**
 * strlen()
 */
JMC_INLINE_DECLARE(size_t) jm_strlen(const char *_Src)
{
    return strlen(_Src);
}

JMC_INLINE_DECLARE(size_t) jm_wcslen(const wchar_t *_Src)
{
    return wcslen(_Src);
}

JMC_INLINE_DECLARE(size_t) jm_tcslen(const TCHAR *_Src)
{
    return _tcslen(_Src);
}

JMC_INLINE_DECLARE(size_t) jm_mbslen(const unsigned char *_Src)
{
    return _mbslen(_Src);
}

/**
 * strnlen()
 */
JMC_INLINE_DECLARE(size_t) jm_strnlen(const char *_Src, size_t _MaxCount)
{
    return strnlen_s(_Src, _MaxCount);
}

JMC_INLINE_DECLARE(size_t) jm_wcsnlen(const wchar_t *_Src, size_t _MaxCount)
{
    return wcsnlen_s(_Src, _MaxCount);
}

JMC_INLINE_DECLARE(size_t) jm_tcsnlen(const TCHAR *_Src, size_t _MaxCount)
{
    return _tcsnlen(_Src, _MaxCount);
}

JMC_INLINE_DECLARE(size_t) jm_mbsnlen(const unsigned char *_Src, size_t _MaxCount)
{
    return _mbsnlen(_Src, _MaxCount);
}

/**
 * strcpy()
 */
JMC_INLINE_DECLARE(char *) jm_strcpy(char *_Dest, size_t _SizeInBytes, const char *_Source)
{
    errno_t err = strcpy_s(_Dest, _SizeInBytes, _Source);
    //_Dest = (err == 0) : _Dest ? NULL;
    return _Dest;
}

JMC_INLINE_DECLARE(wchar_t *) jm_wcscpy(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source)
{
    errno_t err = wcscpy_s(_Dest, _SizeInBytes, _Source);
    return _Dest;
}

JMC_INLINE_DECLARE(TCHAR *) jm_tcscpy(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source)
{
    errno_t err = _tcscpy_s(_Dest, _SizeInBytes, _Source);
    return _Dest;
}

JMC_INLINE_DECLARE(unsigned char *) jm_mbscpy(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source)
{
    errno_t err = _mbscpy_s(_Dest, _SizeInBytes, _Source);
    return _Dest;
}

/**
 * strncpy()
 */
JMC_INLINE_DECLARE(char *) jm_strncpy(char *_Dest, size_t _SizeInBytes, const char *_Source, size_t _MaxCount)
{
    errno_t err = strncpy_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    //_Dest = (err == 0) : _Dest ? NULL;
    return _Dest;
}

JMC_INLINE_DECLARE(wchar_t *) jm_wcsncpy(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source, size_t _MaxCount)
{
    errno_t err = wcsncpy_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_DECLARE(TCHAR *) jm_tcsncpy(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source, size_t _MaxCount)
{
    errno_t err = _tcsncpy_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_DECLARE(unsigned char *) jm_mbsncpy(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source, size_t _MaxCount)
{
    errno_t err = _mbsncpy_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    return _Dest;
}

/**
 * strcat()
 */
JMC_INLINE_DECLARE(char *) jm_strcat(char *_Dest, size_t _SizeInBytes, const char *_Source)
{
    errno_t err = strcat_s(_Dest, _SizeInBytes, _Source);
    //_Dest = (err == 0) : _Dest ? NULL;
    return _Dest;
}

JMC_INLINE_DECLARE(wchar_t *) jm_wcscat(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source)
{
    errno_t err = wcscat_s(_Dest, _SizeInBytes, _Source);
    return _Dest;
}

JMC_INLINE_DECLARE(TCHAR *) jm_tcscat(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source)
{
    errno_t err = _tcscat_s(_Dest, _SizeInBytes, _Source);
    return _Dest;
}

JMC_INLINE_DECLARE(unsigned char *) jm_mbscat(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source)
{
    errno_t err = _mbscat_s(_Dest, _SizeInBytes, _Source);
    return _Dest;
}

/**
 * strncat()
 */
JMC_INLINE_DECLARE(char *) jm_strncat(char *_Dest, size_t _SizeInBytes, const char *_Source, size_t _MaxCount)
{
    errno_t err = strncat_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    //_Dest = (err == 0) : _Dest ? NULL;
    return _Dest;
}

JMC_INLINE_DECLARE(wchar_t *) jm_wcsncat(wchar_t *_Dest, size_t _SizeInBytes, const wchar_t *_Source, size_t _MaxCount)
{
    errno_t err = wcsncat_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_DECLARE(TCHAR *) jm_tcsncat(TCHAR *_Dest, size_t _SizeInBytes, const TCHAR *_Source, size_t _MaxCount)
{
    errno_t err = _tcsncat_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    return _Dest;
}

JMC_INLINE_DECLARE(unsigned char *) jm_mbsncat(unsigned char *_Dest, size_t _SizeInBytes, const unsigned char *_Source, size_t _MaxCount)
{
    errno_t err = _mbsncat_s(_Dest, _SizeInBytes, _Source, _MaxCount);
    return _Dest;
}

/**
 * strdup()
 */
JMC_INLINE_DECLARE(char *) jm_strdup(const char *_Src)
{
    return strdup(_Src);
}

JMC_INLINE_DECLARE(wchar_t *) jm_wcsdup(const wchar_t *_Src)
{
    return wcsdup(_Src);
}

JMC_INLINE_DECLARE(TCHAR *) jm_tcsdup(const TCHAR *_Src)
{
    return _tcsdup(_Src);
}

JMC_INLINE_DECLARE(unsigned char *) jm_mbsdup(const unsigned char *_Src)
{
    return _mbsdup(_Src);
}

/**
 * strcmp()
 */
JMC_INLINE_DECLARE(int) jm_strcmp(const char *_Str1, const char *_Str2)
{
    return strcmp(_Str1, _Str2);
}

JMC_INLINE_DECLARE(int) jm_wcscmp(const wchar_t *_Str1, const wchar_t *_Str2)
{
    return wcscmp(_Str1, _Str2);
}

JMC_INLINE_DECLARE(int) jm_tcscmp(const TCHAR *_Str1, const TCHAR *_Str2)
{
    return _tcscmp(_Str1, _Str2);
}

JMC_INLINE_DECLARE(int) jm_mbscmp(const unsigned char *_Str1, const unsigned char *_Str2)
{
    return _mbscmp(_Str1, _Str2);
}

///////////////////////////////////////////////////////////////////////////

#else  /* !JIMI_IS_WINDOWS */

//

#endif  /* JIMI_IS_WINDOWS */

#ifdef __cplusplus
}
#endif

#endif  /* _JIMIC_STRING_STRINGS_H_ */
