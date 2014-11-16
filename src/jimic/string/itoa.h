
#ifndef _JIMIC_STRING_ITOA_H_
#define _JIMIC_STRING_ITOA_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_stdint.h"
#include "jimic/core/jimic_declare.h"

#ifdef __cplusplus
extern "C" {
#endif

// itoa_radix10()
JMC_INLINE_NONSTD(int)
jmc_utoa_r10(char *buf, unsigned int val);

JMC_INLINE_NONSTD(int)
jmc_itoa_r10(char *buf, int val);

// ltoa_radix10()
JMC_INLINE_NONSTD(int)
jmc_ultoa_r10(char *buf, unsigned long val);

JMC_INLINE_NONSTD(int)
jmc_ltoa_r10(char *buf, long val);

// i64toa_radix10()
JMC_INLINE_NONSTD(int)
jmc_u64toa_r10(char *buf, uint64_t val);

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10(char *buf, int64_t val);

// itoa_radix10_ex()
JMC_INLINE_NONSTD(int)
jmc_utoa_r10_ex(char *buf, size_t count, unsigned int val, unsigned int flag,
                unsigned int fill, unsigned int width, int length);

JMC_INLINE_NONSTD(int)
jmc_itoa_r10_ex(char *buf, size_t count, int val, unsigned int flag,
                unsigned int fill, unsigned int width, int length);

// i64toa_radix10_ex()
JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_ex(char *buf, size_t count, uint64_t val, unsigned int flag,
                  unsigned int fill, unsigned int field_width, int length);

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_ex(char *buf, size_t count, int64_t val, unsigned int flag,
                  unsigned int fill, unsigned int field_width, int length);

//
// for snprintf() double
//
JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_integer(char *buf, uint64_t val, int sign, unsigned int field_width);

JMC_INLINE_NONSTD(int)
jmc_i64toa_r10_integer(char *buf, int64_t val, unsigned int field_width);

JMC_INLINE_NONSTD(int)
jmc_u64toa_r10_frac(char *buf, uint64_t val, unsigned int precision);

#ifdef __cplusplus
}
#endif

#include "jimic/string/itoa.inl.h"

#endif  /* !_JIMIC_STRING_ITOA_H_ */
