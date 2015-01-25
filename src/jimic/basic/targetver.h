
#ifndef _JIMIC_BASIC_TARGETVER_H_
#define _JIMIC_BASIC_TARGETVER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_WIN32) || defined(__MINGW32__) || defined(__CYGWIN__) \
 || defined(__ICL) || defined(__INTEL_COMPILER)

// ���º궨��Ҫ������ƽ̨��Ҫ������ƽ̨
// �Ǿ�������Ӧ�ó������蹦�ܵ� Windows��Internet Explorer �Ȳ�Ʒ��
// ����汾��ͨ����ָ���汾�����Ͱ汾��ƽ̨���������п��õĹ��ܣ������
// ����������

// �������Ҫ��Ե�������ָ���汾��ƽ̨�����޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER                  // ָ��Ҫ������ƽ̨�� Windows XP��
#define WINVER 0x0501           // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

/***
 ** NOTE:
 **  _WIN32_WINNT defined as 0x0502(Windows 2003 server) for InterlockedExchangeAdd64()
 **  Same for other functions like InterlockedXXXXX64().
 **/

#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows XP��
#define _WIN32_WINNT 0x0502     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif

#ifndef _WIN32_WINDOWS          // ָ��Ҫ������ƽ̨�� Windows 98��
#define _WIN32_WINDOWS 0x0410   // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif

#ifndef _WIN32_IE               // ָ��Ҫ������ƽ̨�� Internet Explorer 6.0��
#define _WIN32_IE 0x0600        // ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif

#endif  /* _WIN32 || __MINGW32__ || __CYGWIN__ || __ICL || __INTEL_COMPILER */

#endif  /* _JIMIC_BASIC_TARGETVER_H_ */
