
#ifndef _JIMI_LANG_CHAR_TRAITS_H_
#define _JIMI_LANG_CHAR_TRAITS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimic/string/jm_strings.h>

#include <string>

#define CHAR_TRAITS_STRICT_CHECK_V0     0
#define CHAR_TRAITS_STRICT_CHECK_V1     0
#define CHAR_TRAITS_STRICT_CHECK_V2     0
#define CHAR_TRAITS_STRICT_CHECK_V3     1

#define CHAR_TRAITS_STRICT_CHECK        CHAR_TRAITS_STRICT_CHECK_V0

NS_JIMI_BEGIN

template <typename _CharT>
struct JIMI_DLL char_traits
{
    typedef _CharT char_type;

    // malloc()
    static char_type *mem_alloc(size_t size);

    // strlen(), strnlen()
    static size_t length(const char_type *str);
    static size_t strlen(const char_type *_str);
    static size_t strnlen(const char_type *_str, size_t _count);

    // strcpy_unsafe(), strncpy_unsafe()
    static char_type *strcpy_unsafe(char_type *_dest, const char_type *_src);
    static char_type *strncpy_unsafe(char_type *_dest, const char_type *_src, size_t _count);
    static char_type *strncpy_unsafe2(char_type *_dest, const char_type *_src, size_t _count);
    static char_type *strlcpy_unsafe(char_type *_dest, const char_type *_src, size_t _count);

    static char_type *strncpy_align4_unsafe(char_type *_dest, const char_type *_src,
        size_t _count);

    // strcpy(), strncpy()
    static char_type *strcpy(char_type *_dest, size_t numberOfElements,
        const char_type *_src);
    static char_type *strncpy(char_type *_dest, size_t numberOfElements,
        const char_type *_src, size_t _count);
    static char_type *strlcpy(char_type *_dest, size_t numberOfElements,
        const char_type *_src, size_t _count);

    static char_type *strcpy_e(char_type *_dest, size_t numberOfElements,
        const char_type *_src);
    static char_type *strncpy_e(char_type *_dest, size_t numberOfElements,
        const char_type *_src, size_t _count);
    static char_type *strlcpy_e(char_type *_dest, size_t numberOfElements,
        const char_type *_src, size_t _count);

    // strcmp()
    static int strcmp(const char_type *_str1, const char_type *_str2);
    static int strncmp(const char_type *_str1, const char_type *_str2, size_t _count);

    // streql()
    static int streql(const char_type *_str1, const char_type *_str2);
    static int strneql(const char_type *_str1, const char_type *_str2, size_t _count);

    // itoa() & utoa()
    static int itoa(char_type *buf, int val, const int radix);
    static int utoa(char_type *buf, unsigned int val, const int radix);

    static int itoa_radix10(char_type *buf, int val);
    static int utoa_radix10(char_type *buf, unsigned int val);

    static int itoa_radix10_fast(char_type *buf, int val, const int last);
    static int utoa_radix10_fast(char_type *buf, unsigned int val, const int last);

    // ltoa() & ultoa()
    static int ltoa_radix10(char_type *buf, long val);
    static int ultoa_radix10(char_type *buf, unsigned long val);

    // i64toa() & u64toa()
    static int i64toa_radix10(char_type *buf, int64_t val);
    static int u64toa_radix10(char_type *buf, uint64_t val);
};

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::mem_alloc(size_t size)
{
    return (char_type *)::malloc(size * sizeof(char_type));
}

template <typename char_type>
JIMI_INLINE
size_t char_traits<char_type>::length(const char_type *str)
{
    return char_traits<char_type>::strlen(str);
}

template <>
JIMI_INLINE
size_t char_traits<char>::length(const char *str)
{
    return ::jm_strlen(str);
}

template <>
JIMI_INLINE
size_t char_traits<wchar_t>::length(const wchar_t *str)
{
    return ::jm_wcslen(str);
}

template <typename char_type>
JIMI_INLINE
size_t char_traits<char_type>::strlen(const char_type *_str)
{
    char_type *_ptr;

#if CHAR_TRAITS_STRICT_CHECK_V3
    if (_str == NULL)
        return 0;
#endif

    _ptr = (char_type *)_str;
    while (*_ptr != '\0')
        ++_ptr;
    return (size_t)(_ptr - _str);
}

template <typename char_type>
JIMI_INLINE
size_t char_traits<char_type>::strnlen(const char_type *_str,
                                       size_t _count)
{
    char_type *_ptr, *_end;
    int n;

#if CHAR_TRAITS_STRICT_CHECK_V3
    if (_str == NULL)
        return 0;
#endif

    _ptr = (char_type *)_str;
    _end = (char_type *)_str + _count;
#if 1
    n = _count;
    while (--n >= 0) {
        if (*_ptr != '\0')
            ++_ptr;
    }
#else
    n = 0;  // for void warning
    while ((_ptr < _end) && (*_ptr != '\0'))
        ++_ptr;
#endif
    return (size_t)(_ptr - _str);
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strcpy_unsafe(char_type *_dest,
                                                 const char_type *_src)
{
    char_type *dest, *src;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    while (*dest = *src) {
        if (*dest == '\0')
            return _dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strncpy_unsafe(char_type *_dest,
                                                  const char_type *_src,
                                                  size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = _count;

    while (--n >= 0) {
        *dest = *src;

        ++dest;
        ++src;
    }

    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strncpy_unsafe2(char_type *_dest,
                                                   const char_type *_src,
                                                   size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = _count;
    if (n == 0)
        return _dest;

    do {
        *dest = *src;

        ++dest;
        ++src;
    } while (--n > 0);

    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strncpy_align4_unsafe(char_type *_dest,
                                                         const char_type *_src,
                                                         size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    /* if _dest align 4 or 8 bytes, copy 4 or 8 bytes one time */
    if (((uintptr_t)_dest & (sizeof(uintptr_t) - 1)) == 0) {
        uintptr_t *dest8, *src8;
        dest8 = (uintptr_t *)_dest;
        src8  = (uintptr_t *)_src;

        n = (_count + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

        while (--n >= 0) {
            *dest8 = *src8;

            ++dest8;
            ++src8;
        }
    }
    /* if is x64 mode and _dest align 4 bytes, copy 4 bytes one time */
    else if ((sizeof(uintptr_t) == 8)
        && (sizeof(uintptr_t) != sizeof(uint32_t))
        && (((uint32_t)_dest & (sizeof(uint32_t) - 1)) == 0)) {
        uint32_t *dest4, *src4;
        dest4 = (uint32_t *)_dest;
        src4  = (uint32_t *)_src;

        n = (_count + sizeof(uint32_t) - 1) / sizeof(uint32_t);

        while (--n >= 0) {
            *dest4 = *src4;

            ++dest4;
            ++src4;
        }
    }
    /* copy 1 bytes one time */
    else {
        dest = (char_type *)_dest;
        src  = (char_type *)_src;

        n = _count;

        while (--n >= 0) {
            *dest = *src;

            ++dest;
            ++src;
        }
    }
    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strlcpy_unsafe(char_type *_dest,
                                                  const char_type *_src,
                                                  size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = _count;

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

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strcpy(char_type *_dest,
                                          size_t numberOfElements,
                                          const char_type *_src)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    if (numberOfElements <= 0)
        return _dest;

    n = numberOfElements - 1;

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return _dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strncpy(char_type *_dest,
                                           size_t numberOfElements,
                                           const char_type *_src,
                                           size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = (int)jimi_min(_count, numberOfElements - 1);

    while (--n >= 0) {
        *dest = *src;

        ++dest;
        ++src;
    }

    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strlcpy(char_type *_dest,
                                           size_t numberOfElements,
                                           const char_type *_src,
                                           size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = (int)jimi_min(_count, numberOfElements - 1);

    while (--n >= 0) {
        *dest = *src;

        if (*dest == '\0')
            return _dest;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return _dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strcpy_e(char_type *_dest,
                                            size_t numberOfElements,
                                            const char_type *_src)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    if (numberOfElements <= 0)
        return _dest;

    n = numberOfElements - 1;

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

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strncpy_e(char_type *_dest,
                                             size_t numberOfElements,
                                             const char_type *_src,
                                             size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = jimi::jimi_min((int)_count, (int)(numberOfElements - 1));
    if (n < 0)
        return _dest;

    while (--n > 0) {
        *dest = *src;

        ++dest;
        ++src;
    }

    *dest = '\0';
    return dest;
}

template <typename char_type>
JIMI_INLINE
char_type *char_traits<char_type>::strlcpy_e(char_type *_dest,
                                             size_t numberOfElements,
                                             const char_type *_src,
                                             size_t _count)
{
    char_type *dest, *src;
    int n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_dest == NULL || _src == NULL)
        return _dest;
#endif

    dest = (char_type *)_dest;
    src  = (char_type *)_src;

    n = (int)jimi_min(_count, numberOfElements - 1);

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

template <typename char_type>
JIMI_INLINE
int char_traits<char_type>::strcmp(const char_type *_str1,
                                   const char_type *_str2)
{
    int equal;
    char_type *str1, *str2;

#if CHAR_TRAITS_STRICT_CHECK
    if (_str1 == NULL || _str2 == NULL)
        return (_str1 != _str2);
#endif

    str1 = (char_type *)_str1;
    str2 = (char_type *)_str2;
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

template <typename char_type>
JIMI_INLINE
int char_traits<char_type>::strncmp(const char_type *_str1,
                                    const char_type *_str2,
                                    size_t _count)
{
    int equal = 0;
    char_type *str1, *str2;
    size_t n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_str1 == NULL || _str2 == NULL)
        return (_str1 != _str2);
#endif

    n = _count;
    if (n == 0)
        return 0;

    str1 = (char_type *)_str1;
    str2 = (char_type *)_str2;
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

template <typename char_type>
JIMI_INLINE
int char_traits<char_type>::streql(const char_type *_str1,
                                   const char_type *_str2)
{
    int equal = 0;
    char_type *str1, *str2;

#if CHAR_TRAITS_STRICT_CHECK
    if (_str1 == NULL || _str2 == NULL)
        return (_str1 == _str2);
#endif

    str1 = (char_type *)_str1;
    str2 = (char_type *)_str2;
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

template <typename char_type>
JIMI_INLINE
int char_traits<char_type>::strneql(const char_type *_str1,
                                           const char_type *_str2,
                                           size_t _count)
{
    int equal = 1;
    char_type *str1, *str2;
    size_t n;

#if CHAR_TRAITS_STRICT_CHECK
    if (_str1 == NULL || _str2 == NULL)
        return (_str1 == _str2);
#endif
    
    n = _count;
    if (n == 0)
        return 1;

    str1 = (char_type *)_str1;
    str2 = (char_type *)_str2;
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

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::utoa(char_type *buf, unsigned int val, const int radix)
{
    unsigned int digit_val, num_digits;
    char_type *cur;
    char digits[16];    // 实际最多只会用到10个bytes
    cur = digits;
    do {
        digit_val = val % radix;
        val /= radix;

        *cur++ = static_cast<char_type>(digit_val + '0');
    } while (val != 0);

    num_digits = cur - digits;

#if 1
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#else
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#endif
    *buf = '\0';

    return num_digits;
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::itoa(char_type *buf, int val, const int radix)
{
    if (val < 0) {
        val = -val;
        *buf++ = '-';
    }
    return char_traits<char_type>::utoa(buf, val, radix);
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::utoa_radix10(char_type *buf, unsigned int val)
{
    unsigned int digit_val, num_digits;
    char_type *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = static_cast<char_type>(digit_val + '0');
    } while (val != 0);

    num_digits = cur - digits;

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#elif 1
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#elif 0
    while (--cur >= digits)
        *buf++ = *cur;
#else
    do {
        --cur;
        if (cur < digits)
            break;
        *buf++ = *cur;
    } while (1);
#endif
    *buf = '\0';

    return num_digits;
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::itoa_radix10(char_type *buf, int val)
{
    if (val < 0) {
        val = -val;
        *buf++ = '-';
    }
    return char_traits<char_type>::utoa_radix10(buf, val);
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::utoa_radix10_fast(char_type *buf, unsigned int val, const int last)
{
    unsigned int digit_val, num_digits;
    char_type *cur, *end;
    end = buf + last;
    cur = end;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur-- = static_cast<char_type>(digit_val + '0');
    } while (val != 0);

    num_digits = end - cur;

#if 1
    do {
        ++cur;
        *buf++ = *cur;
    } while (cur != end);
#else
    cur++;
    while (cur <= end)
        *buf++ = *cur++;
#endif
    *buf = '\0';
    return num_digits;
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::itoa_radix10_fast(char_type *buf, int val, const int last)
{
    if (val < 0) {
        val = -val;
        *buf++ = '-';
    }
    return char_traits<char_type>::utoa_radix10_fast(buf, val, last);
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::ultoa_radix10(char_type *buf, unsigned long val)
{
    unsigned long digit_val, num_digits;
    char_type *cur;
    char digits[16];    // 实际最多只会用到10个bytes

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = static_cast<char_type>(digit_val + '0');
    } while (val != 0);

    num_digits = cur - digits;

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#elif 1
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#else
    while (--cur >= digits)
        *buf++ = *cur;
#endif
    *buf = '\0';

    return num_digits;
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::ltoa_radix10(char_type *buf, long val)
{
    if (val < 0) {
        val = -val;
        *buf++ = '-';
    }
    return char_traits<char_type>::ultoa_radix10(buf, val);
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::u64toa_radix10(char_type *buf, uint64_t val)
{
    unsigned int digit_val, num_digits;
    char_type *cur;
    char digits[32];    // 实际最多只会用到20个bytes

    cur = digits;
    do {
        digit_val = val % 10;
        val /= 10;

        *cur++ = static_cast<char_type>(digit_val + '0');
    } while (val != 0);

    num_digits = cur - digits;

#if 0
    do {
        --cur;
        *buf++ = *cur;
    } while (cur != digits);
#elif 1
    cur--;
    while (cur >= digits)
        *buf++ = *cur--;
#else
    while (--cur >= digits)
        *buf++ = *cur;
#endif
    *buf = '\0';

    return num_digits;
}

template<typename char_type>
JIMI_INLINE
int char_traits<char_type>::i64toa_radix10(char_type *buf, int64_t val)
{
    if (val < 0) {
        val = -val;
        *buf++ = '-';
    }
    return char_traits<char_type>::u64toa_radix10(buf, val);
}

NS_JIMI_END

#endif  /* _JIMI_LANG_CHAR_TRAITS_H_ */
