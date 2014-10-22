
#ifndef _JIMIC_LIBC_ENDIAN_H_
#define _JIMIC_LIBC_ENDIAN_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define JIMIC_LITTLE_ENDIAN         0
#define JIMIC_BIG_ENDIAN            1

#define JIMIC_BYTE_ORDER            JIMIC_LITTLE_ENDIAN
#define JIMIC_FLOAT_DWORD_ORDER     JIMIC_LITTLE_ENDIAN

#ifdef __cplusplus
extern "C" {
#endif

extern int jmc_check_endian();

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_LIBC_ENDIAN_H_ */
