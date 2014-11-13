
#ifndef _JIMIC_SYSTEM_CONSOLE_H_
#define _JIMIC_SYSTEM_CONSOLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/core/jimic_def.h"

#ifdef __cplusplus
extern "C" {
#endif

void JIMIC_DLL jimi_cpu_warmup(int delayTime);

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_SYSTEM_CONSOLE_H_ */
