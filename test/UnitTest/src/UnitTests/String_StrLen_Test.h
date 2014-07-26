
#ifndef _JIMI_UNITTEST_STRING_STRLEN_TEST_H_
#define _JIMI_UNITTEST_STRING_STRLEN_TEST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_def.h>
#include <jimi/core/jimi_def.h>

#ifdef __cplusplus
extern "C" {
#endif

int get_bytes_display(char *buf, size_t buf_size, uint32_t size);

int get_bytes_display64(char *buf, size_t buf_size, uint64_t size);

void String_StrLen_Test();

#ifdef __cplusplus
}
#endif

#endif  /* _JIMI_UNITTEST_STRING_STRLEN_TEST_H_ */
