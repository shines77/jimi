
#ifndef _JIMI_UNITTEST_SNPRINTF_TEST_H_
#define _JIMI_UNITTEST_SNPRINTF_TEST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_platform_def.h"

/* 是否显示测试的输出结果(以便验证), 为1时显示 */
#define VSNPRINTF_DISPLAY_TEST_RESULT   1

/* 是否运行jm_snprintf()的测试, 为1时表示运行 */
#define VSNPRINTF_TEST_JM_SNPRINTF      0

#ifdef __cplusplus
extern "C" {
#endif

void String_Snprintf_Test();

void Sprintf_Preformance_Test_Integer1();

void Snprintf_Preformance_Test_Integer1();
void Snprintf_Preformance_Test_Integer2();
void Snprintf_Preformance_Test_Integer3();

void Snprintf_Preformance_Test_Double1();
void Snprintf_Preformance_Test_Double2();
void Snprintf_Preformance_Test_Double3();
void Snprintf_Preformance_Test_Double4();

void Snprintf_Preformance_Test_String1();
void Snprintf_Preformance_Test_String2();
void Snprintf_Preformance_Test_String3();
void Snprintf_Preformance_Test_String4();
void Snprintf_Preformance_Test_String5();

#ifdef __cplusplus
}
#endif

#endif  /* _JIMI_UNITTEST_SNPRINTF_TEST_H_ */
