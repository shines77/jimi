
#ifndef _JIMIC_STRING_JM_STRINGS_H_
#define _JIMIC_STRING_JM_STRINGS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#include <string.h>

#if defined(JIMI_IS_WINDOWS)
#include <tchar.h>
#include <mbstring.h>
#endif  /* JIMI_IS_WINDOWS */

#include <stdarg.h>

#ifndef jimic_tolower
#define jimic_tolower(c)     (unsigned char) ((c >= 'A' && c <= 'Z') ? (c |  0x20) : c)
#endif

#ifndef jimic_toupper
#define jimic_toupper(c)     (unsigned char) ((c >= 'a' && c <= 'z') ? (c & ~0x20) : c)
#endif

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////
// jm_strlwrs() & jm_struprs()
///////////////////////////////////////////////////////////////////////////

/**
* jm_strlwrs() & jm_struprs()
 */
JMC_INLINE_NONSTD(int)  jm_str2lwr(char *dest, char *src, size_t n);
JMC_INLINE_NONSTD(int)  jm_str2upr(char *dest, char *src, size_t n);
JMC_INLINE_NONSTD(int)  jm_strlwr_s(char *src, size_t n);
JMC_INLINE_NONSTD(int)  jm_strupr_s(char *src, size_t n);
JMC_INLINE_NONSTD(void) jm_strnlwr(char *dest, char *src, size_t n);
JMC_INLINE_NONSTD(void) jm_strnupr(char *dest, char *src, size_t n);
JMC_INLINE_NONSTD(void) jm_strnlwr_s(char *src, size_t n);
JMC_INLINE_NONSTD(void) jm_strnupr_s(char *src, size_t n);


#if defined(JIMI_IS_WINDOWS) && (JIMI_IS_WINDOWS != 0) && !defined(__MINGW32__)

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

#else  /* !JIMI_IS_WINDOWS */

///////////////////////////////////////////////////////////////////////////

/**
 * strlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strlen(const char *_Str);

#define jm_wcslen         jm_strlen
#define jm_tcslen         jm_strlen
#define jm_mbslen         jm_strlen

/**
 * strnlen()
 */
JMC_INLINE_NONSTD(size_t) jm_strnlen(const char *_Str, size_t _MaxCount);

#define jm_wcsnlen        jm_strnlen
#define jm_tcsnlen        jm_strnlen
#define jm_mbsnlen        jm_strnlen

/**
 * strcpy()
 */
JMC_INLINE_NONSTD(char *) jm_strcpy(char *_Dest, size_t _NumberOfElements, const char *_Source);

#define jm_wcscpy         jm_strcpy
#define jm_tcscpy         jm_strcpy
#define jm_mbscpy         jm_strcpy

/**
 * strncpy()
 */
JMC_INLINE_NONSTD(char *) jm_strncpy(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount);

#define jm_wcsncpy        jm_strncpy
#define jm_tcsncpy        jm_strncpy
#define jm_mbsncpy        jm_strncpy

/**
 * strcat()
 */
JMC_INLINE_NONSTD(char *) jm_strcat(char *_Dest, size_t _NumberOfElements, const char *_Source);

#define jm_wcscat         jm_strcat
#define jm_tcscat         jm_strcat
#define jm_mbscat         jm_strcat

/**
 * strncat()
 */
JMC_INLINE_NONSTD(char *) jm_strncat(char *_Dest, size_t _NumberOfElements, const char *_Source, size_t _MaxCount);

#define jm_wcsncat        jm_strncat
#define jm_tcsncat        jm_strncat
#define jm_mbsncat        jm_strncat

/**
 * strdup()
 */
JMC_INLINE_NONSTD(char *) jm_strdup(const char *_Src);

#define jm_wcsdup         jm_strdup
#define jm_tcsdup         jm_strdup
#define jm_mbsdup         jm_strdup

/**
 * strcmp()
 */
JMC_INLINE_NONSTD(int) jm_strcmp(const char *_Str1, const char *_Str2);

#define jm_wcscmp      jm_strcmp
#define jm_tcscmp      jm_strcmp
#define jm_mbscmp      jm_strcmp

/**
 * strncmp()
 */
JMC_INLINE_NONSTD(int) jm_strncmp(const char *_Str1, const char *_Str2, size_t _MaxCount);

#define jm_wcsncmp     jm_strncmp
#define jm_tcsncmp     jm_strncmp
#define jm_mbsncmp     jm_strncmp

/**
 * strstr()
 */

JMC_INLINE_NONSTD(char *) jm_strstr(const char *_Str, const char *_SubStr);

#define jm_wcsstr         jm_strstr
#define jm_tcsstr         jm_strstr
#define jm_mbsstr         jm_strstr

/**
 * strupr()
 */
JMC_INLINE_NONSTD(char *) jm_strupr(char *_Str, size_t _NumberOfElements);

#define jm_wcsupr         jm_strupr
#define jm_tcsupr         jm_strupr
#define jm_mbsupr         jm_strupr

/**
 * strlwr()
 */
JMC_INLINE_NONSTD(char *) jm_strlwr(char *_Str, size_t _NumberOfElements);

#define jm_wcslwr         jm_strlwr
#define jm_tcslwr         jm_strlwr
#define jm_mbslwr         jm_strlwr

/**
 * sprintf()
 */
JMC_INLINE_NONSTD(int) jm_sprintf(char *buffer, size_t numberOfElements, const char *format, ...);

#define jm_swprintf    jm_sprintf
#define jm_stprintf    jm_sprintf

/**
 * vsprintf()
 */
JMC_INLINE_NONSTD(int) jm_vsprintf(char *buffer, size_t numberOfElements, const char *format, va_list arg_list);

#define jm_vswprintf   jm_vsprintf
#define jm_vstprintf   jm_vsprintf

/**
 * snprintf()
 */
JMC_INLINE_NONSTD(int) jm_snprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, ...);

#define jm_snwprintf   jm_snprintf
#define jm_sntprintf   jm_snprintf

/**
 * vsnprintf()
 */
JMC_INLINE_NONSTD(int) jm_vsnprintf(char *buffer, size_t numberOfElements, size_t count, const char *format, va_list arg_list);

#define jm_vsnwprintf  jm_vsnprintf
#define jm_vsntprintf  jm_vsnprintf

///////////////////////////////////////////////////////////////////////////

#endif  /* JIMI_IS_WINDOWS */

#ifdef __cplusplus
}
#endif

///////////////////////////////////////////////////////////////////////////

#include <jimic/string/jm_strings.inl.h>

///////////////////////////////////////////////////////////////////////////

#endif  /* _JIMIC_STRING_JM_STRINGS_H_ */
