
///////////////////////////////////////////////////////////////////////////

#ifndef NOT_IS_INLINE_INCLUDE
#define NOT_IS_INLINE_INCLUDE   0
#endif

#include <stdlib.h>
#include <stdio.h>

#if NOT_IS_INLINE_INCLUDE

#include <jimi/platform/jimi_platform_config.h>

#if defined(JIMI_IS_WINDOWS)
#include <tchar.h>
#include <mbstring.h>
#endif
#include <string.h>

#include <jimic/string/jm_strings.h>

#endif  /* NOT_IS_INLINE_INCLUDE */

#undef NOT_IS_INLINE_INCLUDE

///////////////////////////////////////////////////////////////////////////

// the errno can get from _get_errno(), _set_errno()
// _TRUNCATE = ((size_t)-1)

#if defined(JIMI_IS_WINDOWS)

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
    return _mbsdup(_Src);
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

//

///////////////////////////////////////////////////////////////////////////

#endif  /* JIMI_IS_WINDOWS */
