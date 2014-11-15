
#ifndef _JIMIC_STRING_STRING_IMPL_INL_
#define _JIMIC_STRING_STRING_IMPL_INL_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef _JIMIC_STRING_STRIGN_INL_H_
#error "Don't include the internal file <jimic/string/string.impl.inl> directly; include <jimic/string/string.h> instead."
#endif

#include "jimic/libc/ieee754.h"
#include "jimic/string/itoa.h"
#include "jimic/string/string.h"
#include "jimic/string/jm_strings.h"

#include <stdarg.h>
#include <math.h>       // for isnan(), isinf()
#include <float.h>
#include <limits.h>     // for UINT_MAX

#if defined(JMC_STRNCPY_EX_INLINE_DECLARE) && (JMC_STRNCPY_EX_INLINE_DECLARE != 0)
JMC_INLINE_NONSTD(size_t)
#else
JMC_DECLARE_NONSTD(size_t)
#endif
jmc_strncpy_ex(char *dest, size_t countOfElements, jm_const char *src, size_t count,
               unsigned int flag, unsigned int fill, unsigned int width, int length)
{
    char *end;
    size_t copy_len;
    int fill_cnt, padding;

    jimic_assert(dest != NULL);
    jimic_assert(src  != NULL);

    count = JIMIC_MIN(count, countOfElements - 1);
    copy_len = JIMIC_MIN(JIMIC_MIN((unsigned int)length, width), count);
    end = (char *)src + copy_len;

    fill_cnt = width - copy_len;
    if (fill_cnt <= 0) {
        // copy from src
        while (src < end) {
            *dest++ = *src++;
        }

        return copy_len;
    }
    else {
        // when legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)width) {
            // fill normal
            while (fill_cnt > 0) {
                *dest++ = fill;
                fill_cnt--;
            }

            // copy from src
            while (src < end) {
                *dest++ = *src++;
            }

            return width;
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // align to right, when (length < width)
                jimic_assert(length < (int)width);

                // fill right padding space
                padding = length - count;
                if (padding > 0) {
                    // fill right padding space
                    while (fill_cnt > padding) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }

                    // fill normal
                    while (fill_cnt > 0) {
                        *dest++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // fill right padding space
                    while (fill_cnt > 0) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }
                }

                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }

                return width;
            }
            else {
                // align to left, when (length < width)
                jimic_assert(length < (int)width);

                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }

                // fill normal
                padding = length - count;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *dest++ = fill;
                        padding--;
                    }
                }

                // fill left padding space
                while (fill_cnt > 0) {
                    *dest++ = ' ';
                    fill_cnt--;
                }

                return width;
            }
        }
    }
}

/* jmc_strncpy_ex_fast() 是否使用memcpy复制字符串 */
#define JMC_STRNCPY_EX_FAST_USE_MEMCPY       1

#if defined(JMC_STRNCPY_EX_INLINE_DECLARE) && (JMC_STRNCPY_EX_INLINE_DECLARE != 0)
JMC_INLINE_NONSTD(size_t)
#else
JMC_DECLARE_NONSTD(size_t)
#endif
jmc_strncpy_ex_fast(char *dest, size_t countOfElements, jm_const char *src, size_t count,
                    unsigned int flag, unsigned int fill, unsigned int width, int length)
{
#if defined(JMC_STRNCPY_EX_FAST_USE_MEMCPY) && (JMC_STRNCPY_EX_FAST_USE_MEMCPY != 0)
    // jmc_strncpy_ex_fast() use memcpy
#else
    char *end;
#endif
    size_t copy_len;
    int fill_cnt, padding;

    jimic_assert(dest != NULL);
    jimic_assert(src  != NULL);

    count = JIMIC_MIN(count, countOfElements - 1);
    copy_len = JIMIC_MIN(JIMIC_MIN((unsigned int)length, width), count);
#if defined(JMC_STRNCPY_EX_FAST_USE_MEMCPY) && (JMC_STRNCPY_EX_FAST_USE_MEMCPY != 0)
    // jmc_strncpy_ex_fast() use memcpy
#else
    end = (char *)src + copy_len;
#endif

    fill_cnt = width - copy_len;
    if (fill_cnt <= 0) {

#if defined(JMC_STRNCPY_EX_FAST_USE_MEMCPY) && (JMC_STRNCPY_EX_FAST_USE_MEMCPY != 0)
        // copy from src
        memcpy(dest, src, copy_len * sizeof(char));
#else
        // copy from src
        while (src < end) {
            *dest++ = *src++;
        }
#endif

        return copy_len;
    }
    else {
        // when legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)width) {
            // fill normal
            while (fill_cnt > 0) {
                *dest++ = fill;
                fill_cnt--;
            }

#if defined(JMC_STRNCPY_EX_FAST_USE_MEMCPY) && (JMC_STRNCPY_EX_FAST_USE_MEMCPY != 0)
            // copy from src
            memcpy(dest, src, copy_len * sizeof(char));
#else
            // copy from src
            while (src < end) {
                *dest++ = *src++;
            }
#endif
            return width;
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // align to right, when (length < width)
                jimic_assert(length < (int)width);

                // fill right padding space
                padding = length - count;
                if (padding > 0) {
                    // fill right padding space
                    while (fill_cnt > padding) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }

                    // fill normal
                    while (fill_cnt > 0) {
                        *dest++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // fill right padding space
                    while (fill_cnt > 0) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }
                }

#if defined(JMC_STRNCPY_EX_FAST_USE_MEMCPY) && (JMC_STRNCPY_EX_FAST_USE_MEMCPY != 0)
                // copy from src
                memcpy(dest, src, copy_len * sizeof(char));
#else
                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }
#endif
                return width;
            }
            else {
                // align to left, when (length < width)
                jimic_assert(length < (int)width);

#if defined(JMC_STRNCPY_EX_FAST_USE_MEMCPY) && (JMC_STRNCPY_EX_FAST_USE_MEMCPY != 0)
                // copy from src
                memcpy(dest, src, copy_len * sizeof(char));
                dest += copy_len;
#else
                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }
#endif
                // fill normal
                padding = length - count;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *dest++ = fill;
                        padding--;
                    }
                }

                // fill left padding space
                while (fill_cnt > 0) {
                    *dest++ = ' ';
                    fill_cnt--;
                }

                return width;
            }
        }
    }
}

/* 是否使用直接顺序写入字符串 */
#define OUT_NULL_STRING_EX_DIRECT_WRITE     1

#if defined(JMC_STRNCPY_EX_INLINE_DECLARE) && (JMC_STRNCPY_EX_INLINE_DECLARE != 0)
JMC_INLINE_NONSTD(size_t)
#else
JMC_DECLARE_NONSTD(size_t)
#endif
jmc_out_null_string_ex(char *dest, size_t countOfElements, unsigned int flag,
                       unsigned int fill, unsigned int width, int length)
{
#if !defined(OUT_NULL_STRING_EX_DIRECT_WRITE) || (OUT_NULL_STRING_EX_DIRECT_WRITE == 0)
    char *end;
    char *src;
#endif  /* OUT_NULL_STRING_EX_DIRECT_WRITE */
    size_t count;
    size_t copy_len;
    int fill_cnt, padding;

    jimic_assert(dest != NULL);

    count = JIMIC_MIN(6, countOfElements - 1);
    copy_len = JIMIC_MIN(JIMIC_MIN((unsigned int)length, width), count);

    fill_cnt = width - copy_len;
    if (fill_cnt <= 0) {
        /* Write "(null)" if there's space.  */
        if (copy_len >= (int)sizeof(FMT_NULL_STRING) - 1) {
            *(dest + 0)  = '(';
            *(dest + 1)  = 'n';
            *(dest + 2)  = 'u';
            *(dest + 3)  = 'l';
            *(dest + 4)  = 'l';
            *(dest + 5)  = ')';
            *(dest + 6)  = '\0';
            return 6;
        }
        else {
            *dest = '\0';
            return 0;
        }
    }
    else {
#if !defined(OUT_NULL_STRING_EX_DIRECT_WRITE) || (OUT_NULL_STRING_EX_DIRECT_WRITE == 0)
        src = FMT_NULL_STRING;
        end = (char *)src + copy_len;
#endif  /* OUT_NULL_STRING_EX_DIRECT_WRITE */

        // when legnth <= 0 || legnth >= witdh, align to right or left is same
        if (length <= 0 || length >= (int)width) {
            // fill normal
            while (fill_cnt > 0) {
                *dest++ = fill;
                fill_cnt--;
            }

#if defined(OUT_NULL_STRING_EX_DIRECT_WRITE) && (OUT_NULL_STRING_EX_DIRECT_WRITE != 0)
            // copy from "(null)"
            *(dest + 0)  = '(';
            *(dest + 1)  = 'n';
            *(dest + 2)  = 'u';
            *(dest + 3)  = 'l';
            *(dest + 4)  = 'l';
            *(dest + 5)  = ')';
            *(dest + 6)  = '\0';
#else
            // copy from src
            while (src < end) {
                *dest++ = *src++;
            }
#endif  /* OUT_NULL_STRING_EX_DIRECT_WRITE */
            return width;
        }
        else {
            if ((flag & FMT_ALIGN_LEFT) == 0) {
                // align to right, when (length < width)
                jimic_assert(length < (int)width);

                // fill right padding space
                padding = length - count;
                if (padding > 0) {
                    // fill right padding space
                    while (fill_cnt > padding) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }

                    // fill normal
                    while (fill_cnt > 0) {
                        *dest++ = fill;
                        fill_cnt--;
                    }
                }
                else {
                    // fill right padding space
                    while (fill_cnt > 0) {
                        *dest++ = ' ';
                        fill_cnt--;
                    }
                }

#if defined(OUT_NULL_STRING_EX_DIRECT_WRITE) && (OUT_NULL_STRING_EX_DIRECT_WRITE != 0)
                // copy from "(null)"
                *(dest + 0) = '(';
                *(dest + 1) = 'n';
                *(dest + 2) = 'u';
                *(dest + 3) = 'l';
                *(dest + 4) = 'l';
                *(dest + 5) = ')';
                *(dest + 6) = '\0';
#else
                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }
#endif  /* OUT_NULL_STRING_EX_DIRECT_WRITE */

                return width;
            }
            else {
                // align to left, when (length < width)
                jimic_assert(length < (int)width);

#if defined(OUT_NULL_STRING_EX_DIRECT_WRITE) && (OUT_NULL_STRING_EX_DIRECT_WRITE != 0)
                // copy from "(null)"
                *(dest + 0) = '(';
                *(dest + 1) = 'n';
                *(dest + 2) = 'u';
                *(dest + 3) = 'l';
                *(dest + 4) = 'l';
                *(dest + 5) = ')';
                *(dest + 6) = '\0';
                dest += 6;
#else
                // copy from src
                while (src < end) {
                    *dest++ = *src++;
                }
#endif  /* OUT_NULL_STRING_EX_DIRECT_WRITE */

                // fill normal
                padding = length - count;
                if (padding > 0) {
                    fill_cnt -= padding;
                    while (padding > 0) {
                        *dest++ = fill;
                        padding--;
                    }
                }

                // fill left padding space
                while (fill_cnt > 0) {
                    *dest++ = ' ';
                    fill_cnt--;
                }

                return width;
            }
        }
    }
}

#endif  /* !_JIMIC_STRING_STRING_IMPL_INL_ */
