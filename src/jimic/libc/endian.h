
#ifndef _JIMIC_LIBC_ENDIAN_H_
#define _JIMIC_LIBC_ENDIAN_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* �ֽ�˳����ֵ, 0Ϊδ�����δ֪, δ������Ϊ��С�� */
#define JIMIC_UNKNOWN_ENDIAN        0
#define JIMIC_LITTLE_ENDIAN         1
#define JIMIC_BIG_ENDIAN            2

/* ��� JIMIC_BYTE_ORDER �� JIMIC_FLOAT_DWORD_ORDER δ����ʱ, Ĭ����Ϊ��С��. */

/* �ֽ�˳����: С�˻���, δ����ʱ, Ĭ��ֵΪС��. */
#define JIMIC_BYTE_ORDER            JIMIC_LITTLE_ENDIAN

/* �������ֽ�˳����: С�˻���, δ����ʱ, Ĭ��ֵΪС��. */
#define JIMIC_FLOAT_DWORD_ORDER     JIMIC_LITTLE_ENDIAN

/* ע��: ��� JIMIC_BYTE_ORDER ����Ϊ���ʱ, �� JIMIC_FLOAT_DWORD_ORDER �ܿ���Ҳ���붨��Ϊ���. */
/*       ��Ϊ JIMIC_BYTE_ORDER Ϊ���, �� JIMIC_FLOAT_DWORD_ORDER ΪС�˵�CPUĿǰӦ�ò�����, */
/*       �����ǿ������������, ���ȷʵ��������, ������ע�͵����漸��. */

#if JIMIC_BYTE_ORDER == JIMIC_BIG_ENDIAN
  #undef  JIMIC_FLOAT_DWORD_ORDER
  #define JIMIC_FLOAT_DWORD_ORDER   JIMIC_BIG_ENDIAN
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int jmc_check_endian();

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_LIBC_ENDIAN_H_ */
