
#ifndef _JIMIC_STRING_DTOS_H_
#define _JIMIC_STRING_DTOS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/stdint.h"
#include "jimic/basic/declare.h"

#ifdef __cplusplus
extern "C" {
#endif

JMC_DECLARE_NONSTD(int)
jmc_get_fexponent(float * JMC_RESTRICT pval);

JMC_DECLARE_NONSTD(int)
jmc_get_dexponent(double * JMC_RESTRICT pval);

// jmc_ftos()
JMC_DECLARE_NONSTD(int)
jmc_adjustf_to_bin32(float * JMC_RESTRICT pval, int exponent);

JMC_DECLARE_NONSTD(int)
jmc_ftos(char * JMC_RESTRICT buf, float val, int field_width, int precision);

JMC_DECLARE_NONSTD(int)
jmc_ftos_ex(char * JMC_RESTRICT buf, size_t count, float val, unsigned int flag,
            unsigned int fill, int field_width, int precision);

// for jmc_dtos()
JMC_DECLARE_NONSTD(int)
jmc_adjustd_to_bin64(double * JMC_RESTRICT pval, int exponent);

JMC_DECLARE_NONSTD(int)
jmc_dtos(char * JMC_RESTRICT buf, double val, int field_width, int precision);

JMC_DECLARE_NONSTD(int)
jmc_dtos_ex(char * JMC_RESTRICT buf, size_t count, double val, unsigned int flag,
            unsigned int fill, int field_width, int precision);

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_STRING_DTOS_H_ */
