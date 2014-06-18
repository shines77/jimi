
#ifndef _JIMI_LANG_CHAR_TRAITS_H_
#define _JIMI_LANG_CHAR_TRAITS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimic/string/jm_strings.h>

#include <string>
using namespace std;

#define CHAR_TRAITS_STRICT_CHECK_V0     0
#define CHAR_TRAITS_STRICT_CHECK_V1     0
#define CHAR_TRAITS_STRICT_CHECK_V2     0
#define CHAR_TRAITS_STRICT_CHECK_V3     1

#define CHAR_TRAITS_STRICT_CHECK        CHAR_TRAITS_STRICT_CHECK_V0

NS_JIMI_BEGIN

template <class _CharT>
struct JIMI_API char_traits
{
    typedef _CharT char_type;

    static char_type *assign(size_t size);

    static size_t strlen(const char_type *_Str);
    static size_t strnlen(const char_type *_Str, size_t _MaxCount);

    static char_type *strcpy_unsafe(char_type *_Dest, const char_type *_Source);
    static char_type *strncpy_unsafe(char_type *_Dest, const char_type *_Source, size_t _MaxCount);
    static char_type *strncpy_unsafe2(char_type *_Dest, const char_type *_Source, size_t _MaxCount);
    static char_type *strlcpy_unsafe(char_type *_Dest, const char_type *_Source, size_t _MaxCount);

    static char_type *strcpy(char_type *_Dest, size_t _NumberOfElements,
        const char_type *_Source);
    static char_type *strncpy(char_type *_Dest, size_t _NumberOfElements,
        const char_type *_Source, size_t _MaxCount);
    static char_type *strlcpy(char_type *_Dest, size_t _NumberOfElements,
        const char_type *_Source, size_t _MaxCount);

    static char_type *strcpy_e(char_type *_Dest, size_t _NumberOfElements,
        const char_type *_Source);
    static char_type *strncpy_e(char_type *_Dest, size_t _NumberOfElements,
        const char_type *_Source, size_t _MaxCount);
    static char_type *strlcpy_e(char_type *_Dest, size_t _NumberOfElements,
        const char_type *_Source, size_t _MaxCount);

    static int strcmp(const char_type *_Str1, const char_type *_Str2);
    static int strncmp(const char_type *_Str1, const char_type *_Str2, size_t _MaxCount);

    static int streql(const char_type *_Str1, const char_type *_Str2);
    static int strneql(const char_type *_Str1, const char_type *_Str2, size_t _MaxCount);
};

template <class char_type>
inline char_type *char_traits<char_type>::assign(size_t size)
{
    return (char_type *)::malloc(size * sizeof(char_type));
}

template <class char_type>
inline size_t char_traits<char_type>::strlen(const char_type *_Str)
{
    char_type *_Ptr;

#if CHAR_TRAITS_STRICT_CHECK_V3
    if (_Str == NULL)
        return 0;
#endif

    _Ptr = (char_type *)_Str;
    while (*_Ptr != '\0')
        ++_Ptr;
    return (size_t)(_Ptr - _Str);
}

template <class char_type>
inline size_t char_traits<char_type>::strnlen(const char_type *_Str, size_t _MaxCount)
{
    char_type *_Ptr, *_End;
    int n;

#if CHAR_TRAITS_STRICT_CHECK_V3
    if (_Str == NULL)
        return 0;
#endif

    _Ptr = (char_type *)_Str;
    _End = (char_type *)_Str + _MaxCount;
#if 1
    n = _MaxCount;
    while (--n >= 0) {
        if (*_Ptr != '\0')
            ++_Ptr;
    }
#else
    n = 0;  // for void warning
    while ((_Ptr < _End) && (*_Ptr != '\0'))
        ++_Ptr;
#endif
    return (size_t)(_Ptr - _Str);
}

template <class char_type>
inline char_type *char_traits<char_type>::strcpy_unsafe(char_type *_Dest, const char_type *_Source)
{
    char_type *dest, *src;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    while (*dest = *src) {
        if (*dest == '\0')
            return _Dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return _Dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strncpy_unsafe(char_type *_Dest, const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = _MaxCount;

    while (--n >= 0) {
        *dest = *src;

        ++dest;
        ++src;
    }

    return _Dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strncpy_unsafe2(char_type *_Dest, const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = _MaxCount;
    if (n == 0)
        return _Dest;

    do {
        *dest = *src;

        ++dest;
        ++src;
    } while (--n > 0);

    return _Dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strlcpy_unsafe(char_type *_Dest, const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = _MaxCount;

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strcpy(char_type *_Dest, size_t _NumberOfElements,
                                                 const char_type *_Source)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    if (_NumberOfElements <= 0)
        return _Dest;

    n = _NumberOfElements - 1;

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return _Dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return _Dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strncpy(char_type *_Dest, size_t _NumberOfElements,
                                                  const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = (int)jimi_min(_MaxCount, _NumberOfElements - 1);

    while (--n >= 0) {
        *dest = *src;

        ++dest;
        ++src;
    }

    return _Dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strlcpy(char_type *_Dest, size_t _NumberOfElements,
                                                  const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = (int)jimi_min(_MaxCount, _NumberOfElements - 1);

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return _Dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return _Dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strcpy_e(char_type *_Dest, size_t _NumberOfElements,
                                                   const char_type *_Source)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    if (_NumberOfElements <= 0)
        return _Dest;

    n = _NumberOfElements - 1;

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strncpy_e(char_type *_Dest, size_t _NumberOfElements,
                                                    const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = jimi::jimi_min((int)_MaxCount, (int)(_NumberOfElements - 1));
    if (n < 0)
        return _Dest;

    while (--n > 0) {
        *dest = *src;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return dest;
}

template <class char_type>
inline char_type *char_traits<char_type>::strlcpy_e(char_type *_Dest, size_t _NumberOfElements,
                                                    const char_type *_Source, size_t _MaxCount)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Dest == NULL || _Source == NULL)
        return _Dest;
#endif

    dest = (char_type *)_Dest;
    src  = (char_type *)_Source;

    n = (int)jimi_min(_MaxCount, _NumberOfElements - 1);

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return dest;
}

template <class char_type>
inline int char_traits<char_type>::strcmp(const char_type *_Str1, const char_type *_Str2)
{
    int equal;
    char_type *str1, *str2;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Str1 == NULL || _Str2 == NULL)
        return (_Str1 != _Str2);
#endif

    str1 = (char_type *)_Str1;
    str2 = (char_type *)_Str2;
    while (*str1 == *str2) {
        if (*str1 != '\0') {
            ++str1;
            ++str2;
        }
        else {
            // equal = !(*str1 == *str2 && *str1 == '\0');
            return 0;
        }
    }
    if (*str1 > *str2)
        equal = 1;
    else
        equal = -1;
    return equal;
}

template <class char_type>
inline int char_traits<char_type>::strncmp(const char_type *_Str1, const char_type *_Str2,
                                           size_t _MaxCount)
{
    int equal = 0;
    char_type *str1, *str2;
    size_t n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Str1 == NULL || _Str2 == NULL)
        return (_Str1 != _Str2);
#endif

    n = _MaxCount;
    if (n == 0)
        return 0;

    str1 = (char_type *)_Str1;
    str2 = (char_type *)_Str2;
    do {
        if (*str1 == *str2) {
            if (*str1 != '\0') {
                ++str1;
                ++str2;
            }
            else {
                // equal = !(*str1 == *str2 && *str1 == '\0');
                return 0;
            }
        }
        else {
            if (*str1 > *str2)
                equal = 1;
            else
                equal = -1;
            return equal;
        }
    } while (--n > 0);

    return equal;
}

template <class char_type>
inline int char_traits<char_type>::streql(const char_type *_Str1, const char_type *_Str2)
{
    int equal = 0;
    char_type *str1, *str2;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Str1 == NULL || _Str2 == NULL)
        return (_Str1 == _Str2);
#endif

    str1 = (char_type *)_Str1;
    str2 = (char_type *)_Str2;
    while (*str1 == *str2) {
        if (*str1 != '\0') {
            ++str1;
            ++str2;
        }
        else {
            // equal = (*str1 == *str2 && *str1 == '\0');
            return 1;
        }
    }
    return equal;
}

template <class char_type>
inline int char_traits<char_type>::strneql(const char_type *_Str1, const char_type *_Str2,
                                           size_t _MaxCount)
{
    int equal = 1;
    char_type *str1, *str2;
    size_t n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_Str1 == NULL || _Str2 == NULL)
        return (_Str1 == _Str2);
#endif
    
    n = _MaxCount;
    if (n == 0)
        return 1;

    str1 = (char_type *)_Str1;
    str2 = (char_type *)_Str2;
    do {
        if (*str1 == *str2) {
            if (*str1 != '\0') {
                ++str1;
                ++str2;
            }
            else {
                // equal = (*str1 == *str2 && *str1 == '\0');
                return 1;
            }
        }
        else {
            // isn't equal
            return 0;
        }
    } while (--n > 0);

    return equal;
}

NS_JIMI_END

#endif  /* _JIMI_LANG_CHAR_TRAITS_H_ */
