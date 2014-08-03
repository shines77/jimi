
#ifndef _JIMIC_STRING_JMC_STRINGS_H_
#define _JIMIC_STRING_JMC_STRINGS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_def.h>
#include <jimic/core/jimic_def.h>

#ifndef JMC_DTOS_INLINE_DECLARE
#define JMC_DTOS_INLINE_DECLARE         0
#endif

#ifdef __cplusplus
extern "C" {
#endif

// itoa_radix10()
JMC_INLINE_NONSTD(int)
jmc_utoa_radix10(jm_char *buf, unsigned int val);

JMC_INLINE_NONSTD(int)
jmc_itoa_radix10(jm_char *buf, int val);

// ltoa_radix10()
JMC_INLINE_NONSTD(int)
jmc_ultoa_radix10(jm_char *buf, unsigned long val);

JMC_INLINE_NONSTD(int)
jmc_ltoa_radix10(jm_char *buf, long val);

// i64toa_radix10()
JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10(jm_char *buf, uint64_t val);

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10(jm_char *buf, int64_t val);

// itoa_radix10_ex()
JMC_INLINE_NONSTD(int)
jmc_utoa_radix10_ex(jm_char *buf, size_t count, unsigned int val, unsigned int flag,
                    unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(int)
jmc_itoa_radix10_ex(jm_char *buf, size_t count, int val, unsigned int flag,
                    unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10_ex(jm_char *buf, size_t count, uint64_t val, unsigned int flag,
                      unsigned int fill, unsigned int filed_width, int length);

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10_ex(jm_char *buf, size_t count, int64_t val, unsigned int flag,
                      unsigned int fill, unsigned int filed_width, int length);

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10_for_integer_part(jm_char *buf, uint64_t val, int sign,
                                    unsigned int filed_width);

JMC_INLINE_NONSTD(int)
jmc_i64toa_radix10_for_integer_part(jm_char *buf, int64_t val,
                                    unsigned int filed_width);

JMC_INLINE_NONSTD(int)
jmc_u64toa_radix10_for_frac_part(jm_char *buf, uint64_t val,
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

// ftos(), dtos()
JMC_INLINE_NONSTD(int)
jmc_ftos(jm_char *buf, float val, unsigned int filed_width, int precision);

JMC_INLINE_NONSTD(int)
jmc_ftos_ex(jm_char *buf, size_t count, float val, unsigned int flag,
            unsigned int fill, unsigned int filed_width, int precision);

#if defined(JMC_DTOS_INLINE_DECLARE) && (JMC_DTOS_INLINE_DECLARE != 0)

JMC_INLINE_NONSTD(int)
jmc_dtos(jm_char *buf, double val, int filed_width, int precision);

JMC_INLINE_NONSTD(int)
jmc_dtos_ex(jm_char *buf, size_t count, double val, unsigned int flag,
            unsigned int fill, int filed_width, int precision);

JMC_INLINE_NONSTD(int)
jmc_dtos_ex2(jm_char *buf, size_t count, double val, unsigned int flag,
             unsigned int fill, int filed_width, int precision);

#else  /* !JMC_DTOS_INLINE_DECLARE */

JMC_DECLARE_NONSTD(int)
jmc_dtos(jm_char *buf, double val, int filed_width, int precision);

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex(jm_char *buf, size_t count, double val, unsigned int flag,
            unsigned int fill, int filed_width, int precision);

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex2(jm_char *buf, size_t count, double val, unsigned int flag,
             unsigned int fill, int filed_width, int precision);

#endif  /* JMC_DTOS_INLINE_DECLARE */

// to_hex()
JMC_INLINE_NONSTD(int)
jmc_ptohex(jm_char *buf, void *p);

JMC_INLINE_NONSTD(int)
jmc_uitohex(jm_char *buf, uint32_t val, int isUpper);

JMC_INLINE_NONSTD(int)
jmc_ui64tohex(jm_char *buf, uint64_t val);

// strings
JMC_INLINE_NONSTD(size_t)
jmc_strlen(jm_char *str);

JMC_INLINE_NONSTD(size_t)
jmc_strcpy(jm_char *dest, JM_CONST jm_char *src);

// strncpy()
JMC_INLINE_NONSTD(size_t)
jmc_strncpy(jm_char *dest, size_t countOfElements, JM_CONST jm_char *src, size_t count);

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_fast(jm_char *dest, size_t countOfElements, JM_CONST jm_char *src, size_t count);

JMC_INLINE_NONSTD(size_t)
jmc_strncpy_ex(jm_char *dest, size_t countOfElements, JM_CONST jm_char *src, size_t count,
               unsigned int flag, unsigned int fill, unsigned int width, int length);

#ifdef __cplusplus
}
#endif

#include <jimic/string/jmc_strings.inl.h>

#endif  /* !_JIMIC_STRING_JMC_STRINGS_H_ */
