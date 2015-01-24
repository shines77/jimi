
#ifndef _JIMIC_MATH_INT_LOG10_H_
#define _JIMIC_MATH_INT_LOG10_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_sys(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_comp(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_v2(uint32_t val);

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint_log10_v3(uint32_t val);

#if defined(_WIN64) || defined(_M_X64)

JMC_DECLARE_NONSTD(unsigned int)
jmc_uint64_log10(uint64_t val);

#endif

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_MATH_INT_LOG10_H_ */
