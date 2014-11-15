
#ifndef _JIMIC_LIBC_ENDIAN_H_
#define _JIMIC_LIBC_ENDIAN_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* 字节顺序定义值, 0为未定义或未知, 未定义认为是小端 */
#define JIMIC_UNKNOWN_ENDIAN        0
#define JIMIC_LITTLE_ENDIAN         1
#define JIMIC_BIG_ENDIAN            2

/* 如果 JIMIC_BYTE_ORDER 或 JIMIC_FLOAT_DWORD_ORDER 未定义时, 默认认为是小端. */

/* 注意: 如果 JIMIC_BYTE_ORDER 定义为大端时, 则 JIMIC_FLOAT_DWORD_ORDER 很可能也必须定义为大端. */
/* 因为 JIMIC_BYTE_ORDER 为大端, 且 JIMIC_FLOAT_DWORD_ORDER 为小端的CPU目前应该还没有,
/* 但我们考虑了这种情况, 所以要注意检查定义是否准确. */

/* 字节顺序定义: 小端或大端, 未定义时, 默认值为小端. */
#define JIMIC_BYTE_ORDER            JIMIC_LITTLE_ENDIAN

/* 浮点数字节顺序定义: 小端或大端, 未定义时, 默认值为小端. */
#define JIMIC_FLOAT_DWORD_ORDER     JIMIC_LITTLE_ENDIAN

#ifdef __cplusplus
extern "C" {
#endif

extern int jmc_check_endian();

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_LIBC_ENDIAN_H_ */
