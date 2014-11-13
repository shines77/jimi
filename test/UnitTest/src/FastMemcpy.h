
#ifndef _UNITEST_FASTMEMCPY_H_
#define _UNITEST_FASTMEMCPY_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "UnitTest.h"

#include "jimi/core/jimi_def.h"
#include "jimi/log/log.h"

#ifdef __cplusplus
extern "C" {
#endif

void buffer_set_val(void *buffer, int val, size_t size);
void init_buffer_index(void *buffer, size_t size);
bool verify_buffer_val(void *buffer, size_t size, int val);

void * memcpy_mmx1(void *dest, const void *src, size_t len);
void * memcpy_mmx2(void *dest, const void *src, size_t len);
void * memcpy_mmx3(void *dest, const void *src, size_t len);
void * memcpy_mmx4(void *dest, const void *src, size_t len);

void * memcpy_sse2a(void *dest, const void *src, size_t len);

void * memcpy_sse2c(void *dest, const void *src, size_t len);
void * memcpy_sse2d(void *dest, const void *src, size_t len);
void * memcpy_sse2e(void *dest, const void *src, size_t len);

#ifdef __cplusplus
}
#endif

#endif  /* _UNITEST_FASTMEMCPY_H_ */
