
#ifndef _JIMIC_STRING_STRING_H_
#define _JIMIC_STRING_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/jimic_stdint.h"
#include "jimic/basic/jimic_declare.h"

#ifndef JMC_STRNCPY_EX_INLINE_DECLARE
#define JMC_STRNCPY_EX_INLINE_DECLARE       0
#endif

#ifdef __cplusplus
extern "C" {
#endif

// isnan(), is inf() for float
JMC_INLINE_NONSTD(int)
jmc_isnan_f(float val);

JMC_INLINE_NONSTD(int)
jmc_isinf_f(float val);

JMC_INLINE_NONSTD(int)
jmc_is_nan_or_inf_f(float val);

JMC_INLINE_NONSTD(int)
jmc_ftest(float val);

// is_nan(), is inf() for double
JMC_INLINE_NONSTD(int)
jmc_isnan_d(double val);

JMC_INLINE_NONSTD(int)
jmc_isinf_d(double val);

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

#include "jimic/string/string.inl.h"

#endif  /* !_JIMIC_STRING_STRING_H_ */
