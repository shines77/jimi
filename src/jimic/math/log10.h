
#ifndef _JIMIC_MATH_LOG10_H_
#define _JIMIC_MATH_LOG10_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_def.h"

#ifdef __cplusplus
extern "C" {
#endif

JMC_DECLARE_NONSTD(int)
ieee754_log10_crt_1(double val);

JMC_DECLARE_NONSTD(int)
ieee754_log10_crt_2(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v0(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v1(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v2(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast_v3(double val);

JMC_DECLARE_NONSTD(int)
jmc_log10_fast64(double val);

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_MATH_LOG10_H_ */
