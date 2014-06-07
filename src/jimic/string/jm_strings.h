
#ifndef _JIMIC_STRING_JM_STRINGS_H_
#define _JIMIC_STRING_JM_STRINGS_H_

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
JMC_INLINE_NONSTD(size_t) jm_strlen(const char *_Str);
JMC_INLINE_NONSTD(size_t) jm_wcslen(const wchar_t *_Str);
JMC_INLINE_NONSTD(size_t) jm_tcslen(const TCHAR *_Str);
JMC_INLINE_NONSTD(size_t) jm_mbslen(const unsigned char *_Str);

/**
 * strnlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strnlen(const char *_Str, size_t _MaxCount);
JMC_INLINE_NONSTD(size_t) jm_wcsnlen(const wchar_t *_Str, size_t _MaxCount);
JMC_INLINE_NONSTD(size_t) jm_tcsnlen(const TCHAR *_Str, size_t _MaxCount);
JMC_INLINE_NONSTD(size_t) jm_mbsnlen(const unsigned char *_Str, size_t _MaxCount);

/**
 * strcpy()
 */
JMC_INLINE_NONSTD(char *)          jm_strcpy(char *_Dest, size_t _NumberOfElements, const char *_Source);
JMC_INLINE_NONSTD(wchar_t *)       jm_wcscpy(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source);
JMC_INLINE_NONSTD(TCHAR *)         jm_tcscpy(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source);
JMC_INLINE_NONSTD(unsigned char *) jm_mbscpy(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source);

/**
 * strncpy()
 */
JMC_INLINE_NONSTD(char *)          jm_strncpy(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount);
JMC_INLINE_NONSTD(wchar_t *)       jm_wcsncpy(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source, size_t _MaxCount);
JMC_INLINE_NONSTD(TCHAR *)         jm_tcsncpy(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source, size_t _MaxCount);
JMC_INLINE_NONSTD(unsigned char *) jm_mbsncpy(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source, size_t _MaxCount);

/**
 * strcat()
 */
JMC_INLINE_NONSTD(char *)          jm_strcat(char *_Dest, size_t _NumberOfElements, const char *_Source);
JMC_INLINE_NONSTD(wchar_t *)       jm_wcscat(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source);
JMC_INLINE_NONSTD(TCHAR *)         jm_tcscat(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source);
JMC_INLINE_NONSTD(unsigned char *) jm_mbscat(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source);

/**
 * strncat()
 */
JMC_INLINE_NONSTD(char *)          jm_strncat(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount);
JMC_INLINE_NONSTD(wchar_t *)       jm_wcsncat(wchar_t *_Dest, size_t _NumberOfElements, const wchar_t *_Source, size_t _MaxCount);
JMC_INLINE_NONSTD(TCHAR *)         jm_tcsncat(TCHAR *_Dest, size_t _NumberOfElements, const TCHAR *_Source, size_t _MaxCount);
JMC_INLINE_NONSTD(unsigned char *) jm_mbsncat(unsigned char *_Dest, size_t _NumberOfElements, const unsigned char *_Source, size_t _MaxCount);

/**
 * strdup()
 */
JMC_INLINE_NONSTD(char *)          jm_strdup(const char *_Src);
JMC_INLINE_NONSTD(wchar_t *)       jm_wcsdup(const wchar_t *_Src);
JMC_INLINE_NONSTD(TCHAR *)         jm_tcsdup(const TCHAR *_Src);
JMC_INLINE_NONSTD(unsigned char *) jm_mbsdup(const unsigned char *_Src);

/**
 * strcmp()
 */
JMC_INLINE_NONSTD(int)      jm_strcmp(const char *_Str1, const char *_Str2);
JMC_INLINE_NONSTD(int)      jm_wcscmp(const wchar_t *_Str1, const wchar_t *_Str2);
JMC_INLINE_NONSTD(int)      jm_tcscmp(const TCHAR *_Str1, const TCHAR *_Str2);
JMC_INLINE_NONSTD(int)      jm_mbscmp(const unsigned char *_Str1, const unsigned char *_Str2);

/**
 * strncmp()
 */
JMC_INLINE_NONSTD(int)      jm_strncmp(const char *_Str1, const char *_Str2, size_t _MaxCount);
JMC_INLINE_NONSTD(int)      jm_wcsncmp(const wchar_t *_Str1, const wchar_t *_Str2, size_t _MaxCount);
JMC_INLINE_NONSTD(int)      jm_tcsncmp(const TCHAR *_Str1, const TCHAR *_Str2, size_t _MaxCount);
JMC_INLINE_NONSTD(int)      jm_mbsncmp(const unsigned char *_Str1, const unsigned char *_Str2, size_t _MaxCount);

/**
 * strstr()
 */
#if 0

#ifndef jm_strstr
#define jm_strstr(_Str, _SubStr)    strstr((char *)_Str, (const char *)_SubStr)
#endif
#ifndef jm_wcsstr
#define jm_wcsstr(_Str, _SubStr)    wcsstr((wchar_t *)_Str, (const wchar_t *)_SubStr)
#endif
#ifndef jm_tcsstr
#define jm_tcsstr(_Str, _SubStr)    _tcsstr((TCHAR *)_Str, (const TCHAR *)_SubStr)
#endif
#ifndef jm_mbsstr
#define jm_mbsstr(_Str, _SubStr)    _mbsstr((unsigned char *)_Str, (const unsigned char *)_SubStr)
#endif

#else

JMC_INLINE_NONSTD(char *)           jm_strstr(const char *_Str, const char *_SubStr);
JMC_INLINE_NONSTD(wchar_t *)        jm_wcsstr(const wchar_t *_Str, const wchar_t *_SubStr);
JMC_INLINE_NONSTD(TCHAR *)          jm_tcsstr(const TCHAR *_Str, const TCHAR *_SubStr);
JMC_INLINE_NONSTD(unsigned char *)  jm_mbsstr(const unsigned char *_Str, const unsigned char *_SubStr);

#endif  /* strstr() */

/**
 * strupr()
 */
JMC_INLINE_NONSTD(char *)           jm_strupr(char *_Str, size_t _NumberOfElements);
JMC_INLINE_NONSTD(wchar_t *)        jm_wcsupr(wchar_t *_Str, size_t _NumberOfElements);
JMC_INLINE_NONSTD(TCHAR *)          jm_tcsupr(TCHAR *_Str, size_t _NumberOfElements);
JMC_INLINE_NONSTD(unsigned char *)  jm_mbsupr(unsigned char *_Str, size_t _NumberOfElements);

/**
 * strlwr()
 */
JMC_INLINE_NONSTD(char *)           jm_strlwr(char *_Str, size_t _NumberOfElements);
JMC_INLINE_NONSTD(wchar_t *)        jm_wcslwr(wchar_t *_Str, size_t _NumberOfElements);
JMC_INLINE_NONSTD(TCHAR *)          jm_tcslwr(TCHAR *_Str, size_t _NumberOfElements);
JMC_INLINE_NONSTD(unsigned char *)  jm_mbslwr(unsigned char *_Str, size_t _NumberOfElements);

/**
 * sprintf()
 */
JMC_INLINE_NONSTD(int)      jm_sprintf(char *buffer, size_t numberOfElements, const char *format, ...);
JMC_INLINE_NONSTD(int)      jm_swprintf(wchar_t *buffer, size_t numberOfElements, const wchar_t *format, ...);
JMC_INLINE_NONSTD(int)      jm_stprintf(TCHAR *buffer, size_t numberOfElements, const TCHAR *format, ...);

/**
 * vsprintf()
 */
JMC_INLINE_NONSTD(int)      jm_vsprintf(char *buffer, size_t numberOfElements, const char *format, va_list arg_list);
JMC_INLINE_NONSTD(int)      jm_vswprintf(wchar_t *buffer, size_t numberOfElements, const wchar_t *format, va_list arg_list);
JMC_INLINE_NONSTD(int)      jm_vstprintf(TCHAR *buffer, size_t numberOfElements, const TCHAR *format, va_list arg_list);

/**
 * snprintf()
 */
JMC_INLINE_NONSTD(int)      jm_snprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, ...);
JMC_INLINE_NONSTD(int)      jm_snwprintf(wchar_t *buffer, size_t numberOfElements, size_t count, const wchar_t *format, ...);
JMC_INLINE_NONSTD(int)      jm_sntprintf(TCHAR *buffer, size_t numberOfElements, size_t count, const TCHAR *format, ...);

/**
 * vsnprintf()
 */
JMC_INLINE_NONSTD(int)      jm_vsnprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, va_list arg_list);
JMC_INLINE_NONSTD(int)      jm_vsnwprintf(wchar_t *buffer, size_t numberOfElements, size_t count, const wchar_t *format, va_list arg_list);
JMC_INLINE_NONSTD(int)      jm_vsntprintf(TCHAR *buffer, size_t numberOfElements, size_t count, const TCHAR *format, va_list arg_list);

///////////////////////////////////////////////////////////////////////////

#include <jimic/string/jm_strings.inl.h>

///////////////////////////////////////////////////////////////////////////

#else  /* !JIMI_IS_WINDOWS */

//

#endif  /* JIMI_IS_WINDOWS */

#ifdef __cplusplus
}
#endif

#endif  /* _JIMIC_STRING_JM_STRINGS_H_ */
