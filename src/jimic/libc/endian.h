
#ifndef _JIMIC_LIBC_ENDIAN_H_
#define _JIMIC_LIBC_ENDIAN_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimic/core/jimic_platform_def.h>
#include <jimic/core/jimic_def.h>

#define JMC_LITTLE_ENDIAN       0
#define JMC_BIG_ENDIAN          1

#define JMC_BYTE_ORDER          JMC_LITTLE_ENDIAN
#define JMC_FLOAT_DWORD_ORDER   JMC_LITTLE_ENDIAN

#ifdef __cplusplus
extern "C" {
#endif

extern int jmc_check_endian();

#ifdef __cplusplus
}
#endif

#include <jimic/stdio/sprintf.inl.h>

#endif  /* !_JIMIC_LIBC_ENDIAN_H_ */
