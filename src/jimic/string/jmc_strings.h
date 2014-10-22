
#ifndef _JIMIC_STRING_JMC_STRINGS_H_
#define _JIMIC_STRING_JMC_STRINGS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_def.h>

#ifndef JMC_STRNCPY_EX_INLINE_DECLARE
#define JMC_STRNCPY_EX_INLINE_DECLARE       0
#endif

#ifdef __cplusplus
extern "C" {
#endif

JMC_DECLARE_NONSTD(int)
jmc_log10(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast1(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast2(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_64(double val);

// itoa_radix10()
JMC_INLINE_NONSTD(int)
jmc_utoa_radix10(char *buf, unsigned int val);

JMC_INLINE_NONSTD(int)
jmc_itoa_radix10(char *buf, int val);

// ltoa_radix10()
JMC_INLINE_NONSTD(int)
jmc_ultoa_radix10(char *buf, unsigned long val);

JMC_INLINE_NONSTD(int)
jmc_ltoa_radix10(char *buf, long val);

// i64toa_radix10()
JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10(char *buf, uint64_t val);

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10(char *buf, int64_t val);

// itoa_radix10_ex()
JMC_INLINE_NONSTD(int)
jmc_utoa_radix10_ex(char *buf, size_t count, unsigned int val, unsigned int flag,
                    unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(int)
jmc_itoa_radix10_ex(char *buf, size_t count, int val, unsigned int flag,
                    unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10_ex(char *buf, size_t count, uint64_t val, unsigned int flag,
                      unsigned int fill, unsigned int filed_width, int length);

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10_ex(char *buf, size_t count, int64_t val, unsigned int flag,
                      unsigned int fill, unsigned int filed_width, int length);

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10_for_integer_part(char *buf, uint64_t val, int sign,
                                    unsigned int filed_width);

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10_for_integer_part(char *buf, int64_t val,
                                    unsigned int filed_width);

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10_for_frac_part(char *buf, uint64_t val,
                                 unsigned int precision);

// is_nan(), is inf() for float
JMC_INLINE_NONSTD(int)
jmc_is_nan_f(float val);

JMC_INLINE_NONSTD(int)
jmc_is_inf_f(float val);

JMC_INLINE_NONSTD(int)
jmc_is_nan_or_inf_f(float val);

JMC_INLINE_NONSTD(int)
jmc_ftest(float val);

// is_nan(), is inf() for double
JMC_INLINE_NONSTD(int)
jmc_is_nan_d(double val);

JMC_INLINE_NONSTD(int)
jmc_is_inf_d(double val);

JMC_INLINE_NONSTD(int)
jmc_is_nan_or_inf_d(double val);

JMC_INLINE_NONSTD(int)
jmc_dtest(double val);

// to_hex()
JMC_INLINE_NONSTD(int)
jmc_ptohex(char *buf, void *p);

JMC_INLINE_NONSTD(int)
jmc_uitohex(char *buf, uint32_t val, int isUpper);

JMC_INLINE_NONSTD(int)
jmc_ui64tohex(char *buf, uint64_t val);

// strings
JMC_INLINE_NONSTD(size_t)
jmc_strlen(char *str);

JMC_INLINE_NONSTD(size_t)
jmc_strcpy(char *dest, jm_const char *src);

// strncpy()
JMC_INLINE_NONSTD(size_t)
jmc_strncpy(char *dest, size_t countOfElements, jm_const char *src, size_t count);

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_null(char *dest, size_t countOfElements, jm_const char *src, size_t count);

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_fast(char *dest, size_t countOfElements, jm_const char *src, size_t count);

JMC_INLINE_NONSTD(size_t)
jmc_out_null_string(char *dest, size_t countOfElements);

/* JMC_STRNCPY_EX_INLINE_DECLARE */
#if defined(JMC_STRNCPY_EX_INLINE_DECLARE) && (JMC_STRNCPY_EX_INLINE_DECLARE != 0)

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_ex(char *dest, size_t countOfElements, jm_const char *src, size_t count,
               unsigned int flag, unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_ex_fast(char *dest, size_t countOfElements, jm_const char *src, size_t count,
                    unsigned int flag, unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(size_t)
jmc_out_null_string_ex(char *dest, size_t countOfElements,
                       unsigned int flag, unsigned int fill, unsigned int width, int length);

#else  /* !JMC_STRNCPY_EX_INLINE_DECLARE */

JMC_DECLARE_NONSTD(size_t)
jmc_strncpy_ex(char *dest, size_t countOfElements, jm_const char *src, size_t count,
               unsigned int flag, unsigned int fill, unsigned int width, int length);

JMC_DECLARE_NONSTD(size_t)
jmc_strncpy_ex_fast(char *dest, size_t countOfElements, jm_const char *src, size_t count,
                    unsigned int flag, unsigned int fill, unsigned int width, int length);

JMC_DECLARE_NONSTD(size_t)
jmc_out_null_string_ex(char *dest, size_t countOfElements,
                       unsigned int flag, unsigned int fill, unsigned int width, int length);

#endif  /* JMC_STRNCPY_EX_INLINE_DECLARE */

#ifdef __cplusplus
}
#endif

#include <jimic/string/jmc_strings.inl.h>

#endif  /* !_JIMIC_STRING_JMC_STRINGS_H_ */
