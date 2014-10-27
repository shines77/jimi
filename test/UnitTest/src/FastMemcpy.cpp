
#include "FastMemcpy.h"

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>

// 一个QWORD几个字节
#define SIZE_OF_QWORD       8
// 一个Block块中有多少个字节
#define CACHE_BLOCK_SIZE    (16384 * 1)

#define USE_SOFT_PREFETCH   0

// prefetcht0, prefetcht1, prefetchnta
#define PREFETCH_INST       prefetchnta
#define PREFETCH_SZIE       ((17 * 8 + 4) * 8)

static const BOOL m_bIsMMX  = TRUE;
static const BOOL m_bIsSSE  = TRUE;
static const BOOL m_bIsSSE2 = TRUE;
static const BOOL m_bIsSSE3 = TRUE;

void buffer_set_val(void *buffer, int val, size_t size)
{
    memset(buffer, val, size);
}

void init_buffer_index(void *buffer, size_t size)
{
    unsigned int i;
    unsigned char *buf = (unsigned char *)buffer;
    for (i = 0; i < size; ++i) {
        *buf = (unsigned char)(i & 0xff);
        buf++;
    }
}

bool verify_buffer_val(void *buffer, size_t size, int val)
{
    int i;
    bool verify_ok = true;
    unsigned char *buf = (unsigned char *)buffer;
    for (i = 0; i < (int)size; ++i) {
        if (*buf != (unsigned char)val) {
            verify_ok = false;
            break;
        }
    }
    return verify_ok;
}

#if defined(_WIN64)

void * memcpy_mmx1(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_mmx2(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_mmx3(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_mmx4(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_sse2a(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_sse2c(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_sse2d(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

void * memcpy_sse2e(void *dest, const void *src, size_t len)
{
    return memcpy(dest, src, len);
}

#else /* !_WIN64 */

//
// 用MMX指令实现的内存拷贝函数，相比于memcpy函数可以提高50%的效率
//
void * memcpy_mmx1(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    //修改此值有可能实现更高的速度
    //int n = (int)((len / CACHE_BLOCK_SIZE) * CACHE_BLOCK_SIZE);
    int m = (int)(len & (CACHE_BLOCK_SIZE - 1));
    int n = (int)(len - m);
    if (n > 0) {
        __asm {
            push edi
            push esi
            push eax
            push ecx
            push ebx
            push edx

            // 下面先拷贝整数个块
            mov esi, src
            mov edi, dest
            mov ecx, n                          // 要拷贝多少个bytes
            lea esi, [esi + ecx]
            lea edi, [edi + ecx]
            neg ecx

            align 16
MainLoop:
            mov eax, CACHE_BLOCK_SIZE / 128     // 一次预读128个字节

            align 16
PrefetchLoop:
            mov ebx, [esi + ecx]                // 预读一条cache line
            mov edx, [esi + ecx + 64]           // 预读下一条cache line
            add ecx, 128
            dec eax
            jnz PrefetchLoop

            sub ecx, CACHE_BLOCK_SIZE
            mov eax, CACHE_BLOCK_SIZE / 64      // 一次拷贝64个字节

            align 16
WriteLoop:
            movq    mm0, qword ptr[esi + ecx     ]
            movq    mm1, qword ptr[esi + ecx +  8]
            movq    mm2, qword ptr[esi + ecx + 16]
            movq    mm3, qword ptr[esi + ecx + 24]
            movq    mm4, qword ptr[esi + ecx + 32]
            movq    mm5, qword ptr[esi + ecx + 40]
            movq    mm6, qword ptr[esi + ecx + 48]
            movq    mm7, qword ptr[esi + ecx + 56]

            movntq  qword ptr[edi + ecx     ], mm0
            movntq  qword ptr[edi + ecx +  8], mm1
            movntq  qword ptr[edi + ecx + 16], mm2
            movntq  qword ptr[edi + ecx + 24], mm3
            movntq  qword ptr[edi + ecx + 32], mm4
            movntq  qword ptr[edi + ecx + 40], mm5
            movntq  qword ptr[edi + ecx + 48], mm6
            movntq  qword ptr[edi + ecx + 56], mm7

            add ecx, 64
            dec eax
            jnz WriteLoop

            or ecx, ecx
            jnz MainLoop

            pop edx
            pop ebx
            pop ecx
            pop eax
            pop esi
            pop edi

            sfence
            emms
        }
    }

    // 拷贝不足一个块的数据
    if (m > 0) {
        memcpy((char*)dest + len - m, (char*)src + len - m, m);
    }
#endif

    return dest;
}

void * memcpy_mmx2(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    //int n = (int)((len / CACHE_BLOCK_SIZE) * CACHE_BLOCK_SIZE);
    int n = (int)(len - (len & (CACHE_BLOCK_SIZE - 1)));
    // 剩余的不对齐CACHE_BLOCK_SIZE但对齐64byte的数据
    int m = ((len - n) / SIZE_OF_QWORD) * SIZE_OF_QWORD;
    // 最后剩余的不对齐64byte的字节数
    int nLeftBytes = len - n - m;
    if (n > 0) {
        __asm {
            push edi
            push esi
            push eax
            push ecx
            //push ebx
            //push edx

            // 下面先拷贝整数个块
            mov esi, src                ; source array
            mov edi, dest               ; destination array
            mov ecx, n                  ; number of QWORDS (8 bytes) assumes len / CACHE_BLOCK_SIZE is an integer

            lea esi, [esi + ecx]        ; end of source
            lea edi, [edi + ecx]        ; end of destination
            neg ecx                     ; use a negative offset as a combo pointer-and-loop-counter

            align 16
MainLoop:
#if 0
            mov eax, CACHE_BLOCK_SIZE / 128 ; note: .prefetchloop is unrolled 2X
            add ecx, CACHE_BLOCK_SIZE       ; move up to end of block

            align 16
PrefetchLoop:
            mov ebx, [esi + ecx - 64]       ; read one address in this cache line...
            mov edx, [esi + ecx - 128]      ; ... and one in the previous line
            sub ecx, 128                    ; 16 x 8 bytes = 2 x 64-byte cache lines
            dec eax
            jnz PrefetchLoop
#endif

            mov eax, CACHE_BLOCK_SIZE / 64

            align 16
WriteLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]  ; fetch ahead by 1024 bytes
#endif
            movq mm0, qword ptr [esi + ecx     ]
            movq mm1, qword ptr [esi + ecx +  8]
            movq mm2, qword ptr [esi + ecx + 16]
            movq mm3, qword ptr [esi + ecx + 24]
            movq mm4, qword ptr [esi + ecx + 32]
            movq mm5, qword ptr [esi + ecx + 40]
            movq mm6, qword ptr [esi + ecx + 48]
            movq mm7, qword ptr [esi + ecx + 56]

            movntq qword ptr [edi + ecx     ], mm0
            movntq qword ptr [edi + ecx +  8], mm1
            movntq qword ptr [edi + ecx + 16], mm2
            movntq qword ptr [edi + ecx + 24], mm3
            movntq qword ptr [edi + ecx + 32], mm4
            movntq qword ptr [edi + ecx + 40], mm5
            movntq qword ptr [edi + ecx + 48], mm6
            movntq qword ptr [edi + ecx + 56], mm7

            sub ecx, -64
            dec eax
            jnz WriteLoop

            or ecx, ecx                         ; assumes integer number of cacheblocks
            jnz MainLoop

            //pop edx
            //pop ebx
            pop ecx
            pop eax
            pop esi
            pop edi
        }
    }

    if (m > 0) {
        __asm {
            push edi
            push esi
            push ecx

            mov esi, src
            mov edi, dest
            mov ecx, m
            lea esi, [esi + ecx]
            lea edi, [edi + ecx]
            neg ecx

            align 16
CopyLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]       // 预读
#endif
            movq mm0, qword ptr [esi + ecx     ]
            movq mm1, qword ptr [esi + ecx +  8]
            movq mm2, qword ptr [esi + ecx + 16]
            movq mm3, qword ptr [esi + ecx + 24]
            movq mm4, qword ptr [esi + ecx + 32]
            movq mm5, qword ptr [esi + ecx + 40]
            movq mm6, qword ptr [esi + ecx + 48]
            movq mm7, qword ptr [esi + ecx + 56]

            movntq qword ptr [edi + ecx     ], mm0
            movntq qword ptr [edi + ecx +  8], mm1
            movntq qword ptr [edi + ecx + 16], mm2
            movntq qword ptr [edi + ecx + 24], mm3
            movntq qword ptr [edi + ecx + 32], mm4
            movntq qword ptr [edi + ecx + 40], mm5
            movntq qword ptr [edi + ecx + 48], mm6
            movntq qword ptr [edi + ecx + 56], mm7

            sub ecx, -64
            jnz CopyLoop

            pop ecx
            pop esi
            pop edi
        }
    }

    __asm {
        sfence
        emms
    }

    // 拷贝剩余的字节数据
    if (nLeftBytes > 0)
        memcpy((char*)dest + len - nLeftBytes, (char*)src + len - nLeftBytes, nLeftBytes);
#endif

    return dest;
}

void * memcpy_mmx3(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    //int n = (int)((len / CACHE_BLOCK_SIZE) * CACHE_BLOCK_SIZE);
    int n = (int)(len - (len & (CACHE_BLOCK_SIZE - 1)));
    // 剩余的不对齐CACHE_BLOCK_SIZE但对齐64byte的数据
    int m = ((len - n) / SIZE_OF_QWORD) * SIZE_OF_QWORD;
    // 最后剩余的不对齐64byte的字节数
    int nLeftBytes = len - n - m;
    if (n > 0) {
        __asm {
            push edi
            push esi
            push eax
            push ecx
            //push ebx
            //push edx

            // 下面先拷贝整数个块
            mov esi, src                ; source array
            mov edi, dest               ; destination array
            mov ecx, n                  ; number of QWORDS (8 bytes) assumes len / CACHE_BLOCK_SIZE is an integer

            lea esi, [esi + ecx]        ; end of source
            lea edi, [edi + ecx]        ; end of destination
            neg ecx                     ; use a negative offset as a combo pointer-and-loop-counter

            align 16
MainLoop:
#if 0
            mov eax, CACHE_BLOCK_SIZE / 128 ; note: .prefetchloop is unrolled 2X
            add ecx, CACHE_BLOCK_SIZE       ; move up to end of block

            align 16
PrefetchLoop:
            mov ebx, [esi + ecx - 64]       ; read one address in this cache line...
            mov edx, [esi + ecx - 128]      ; ... and one in the previous line
            sub ecx, 128                    ; 16 x 8 bytes = 2 x 64-byte cache lines
            dec eax
            jnz PrefetchLoop
#endif

            mov eax, CACHE_BLOCK_SIZE / 64

            align 16
WriteLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]  ; fetch ahead by 1024 bytes
#endif
            movq xmm0, qword ptr [esi + ecx     ]
            movq xmm1, qword ptr [esi + ecx +  8]
            movq xmm2, qword ptr [esi + ecx + 16]
            movq xmm3, qword ptr [esi + ecx + 24]

            movq qword ptr [edi + ecx     ], xmm0
            movq qword ptr [edi + ecx +  8], xmm1
            movq qword ptr [edi + ecx + 16], xmm2
            movq qword ptr [edi + ecx + 24], xmm3

            movq xmm4, qword ptr [esi + ecx + 32]
            movq xmm5, qword ptr [esi + ecx + 40]
            movq xmm6, qword ptr [esi + ecx + 48]
            movq xmm7, qword ptr [esi + ecx + 56]

            movq qword ptr [edi + ecx + 32], xmm4
            movq qword ptr [edi + ecx + 40], xmm5
            movq qword ptr [edi + ecx + 48], xmm6
            movq qword ptr [edi + ecx + 56], xmm7

            sub ecx, -64
            dec eax
            jnz WriteLoop

            or ecx, ecx                         ; assumes integer number of cacheblocks
            jnz MainLoop

            //pop edx
            //pop ebx
            pop ecx
            pop eax
            pop esi
            pop edi
        }
    }

    if (m > 0) {
        __asm {
            push edi
            push esi
            push ecx

            mov esi, src
            mov edi, dest
            mov ecx, m
            lea esi, [esi + ecx]
            lea edi, [edi + ecx]
            neg ecx

            align 16
CopyLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]       // 预读
#endif
            movq xmm0, qword ptr [esi + ecx     ]
            movq xmm1, qword ptr [esi + ecx +  8]
            movq xmm2, qword ptr [esi + ecx + 16]
            movq xmm3, qword ptr [esi + ecx + 24]

            movq qword ptr [edi + ecx     ], xmm0
            movq qword ptr [edi + ecx +  8], xmm1
            movq qword ptr [edi + ecx + 16], xmm2
            movq qword ptr [edi + ecx + 24], xmm3

            movq xmm4, qword ptr [esi + ecx + 32]
            movq xmm5, qword ptr [esi + ecx + 40]
            movq xmm6, qword ptr [esi + ecx + 48]
            movq xmm7, qword ptr [esi + ecx + 56]

            movq qword ptr [edi + ecx + 32], xmm4
            movq qword ptr [edi + ecx + 40], xmm5
            movq qword ptr [edi + ecx + 48], xmm6
            movq qword ptr [edi + ecx + 56], xmm7

            sub ecx, -64
            jnz CopyLoop

            pop ecx
            pop esi
            pop edi
        }
    }

    __asm {
        //sfence
        //emms
    }

    // 拷贝剩余的字节数据
    if (nLeftBytes > 0)
        memcpy((char*)dest + len - nLeftBytes, (char*)src + len - nLeftBytes, nLeftBytes);

#endif
    return dest;
}

void * memcpy_mmx4(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    if (len <= (1024 * 1024)) {
        __asm {
            mov esi, src
            mov edi, dest
            mov ecx, len
            shr ecx, 6

            align 16
MainLoop:
#if 1
            // movdqa, movaps, movntdq
            movdqa  xmm0, xmmword ptr [esi     ]
            movdqa  xmm1, xmmword ptr [esi + 16]
            movdqa  xmm2, xmmword ptr [esi + 32]
            movdqa  xmm3, xmmword ptr [esi + 48]

            movdqa  xmmword ptr [edi     ], xmm0
            movdqa  xmmword ptr [edi + 16], xmm1
            movdqa  xmmword ptr [edi + 32], xmm2
            movdqa  xmmword ptr [edi + 48], xmm3
#elif 0
            // movdqa, movaps, movntdq
            movdqa  xmm0, xmmword ptr [esi     ]
            movdqa  xmm1, xmmword ptr [esi + 16]
            movdqa  xmm2, xmmword ptr [esi + 32]
            movdqa  xmm3, xmmword ptr [esi + 48]

            movntdq  xmmword ptr [edi     ], xmm0
            movntdq  xmmword ptr [edi + 16], xmm1
            movntdq  xmmword ptr [edi + 32], xmm2
            movntdq  xmmword ptr [edi + 48], xmm3
#elif 0
            movq    xmm0, qword ptr [esi     ]
            movq    xmm1, qword ptr [esi +  8]
            movq    xmm2, qword ptr [esi + 16]
            movq    xmm3, qword ptr [esi + 24]

            movq    qword ptr [edi     ], xmm0
            movq    qword ptr [edi +  8], xmm1
            movq    qword ptr [edi + 16], xmm2
            movq    qword ptr [edi + 24], xmm3

            movq    xmm4, qword ptr [esi + 32]
            movq    xmm5, qword ptr [esi + 40]
            movq    xmm6, qword ptr [esi + 48]
            movq    xmm7, qword ptr [esi + 56]

            movq    qword ptr [edi + 32], xmm4
            movq    qword ptr [edi + 40], xmm5
            movq    qword ptr [edi + 48], xmm6
            movq    qword ptr [edi + 56], xmm7
#else
            movq    mm0, qword ptr [esi     ]
            movq    mm1, qword ptr [esi +  8]
            movq    mm2, qword ptr [esi + 16]
            movq    mm3, qword ptr [esi + 24]

            movq    qword ptr [edi     ], mm0
            movq    qword ptr [edi +  8], mm1
            movq    qword ptr [edi + 16], mm2
            movq    qword ptr [edi + 24], mm3

            movq    mm4, qword ptr [esi + 32]
            movq    mm5, qword ptr [esi + 40]
            movq    mm6, qword ptr [esi + 48]
            movq    mm7, qword ptr [esi + 56]

            movq    qword ptr [edi + 32], mm4
            movq    qword ptr [edi + 40], mm5
            movq    qword ptr [edi + 48], mm6
            movq    qword ptr [edi + 56], mm7
#endif
            sub esi, -64
            sub edi, -64
            dec ecx
            jnz MainLoop

            //sfence
            //emms
        }
    }
    else {

        __asm {
            mov esi, src
            mov edi, dest
            mov ecx, len
            shr ecx, 6

            align 16
MainLoop_NTQ:

            movq    mm0, qword ptr [esi     ]
            movq    mm1, qword ptr [esi +  8]
            movq    mm2, qword ptr [esi + 16]
            movq    mm3, qword ptr [esi + 24]
            movq    mm4, qword ptr [esi + 32]
            movq    mm5, qword ptr [esi + 40]
            movq    mm6, qword ptr [esi + 48]
            movq    mm7, qword ptr [esi + 56]

            movntq  qword ptr [edi     ], mm0
            movntq  qword ptr [edi +  8], mm1
            movntq  qword ptr [edi + 16], mm2
            movntq  qword ptr [edi + 24], mm3
            movntq  qword ptr [edi + 32], mm4
            movntq  qword ptr [edi + 40], mm5
            movntq  qword ptr [edi + 48], mm6
            movntq  qword ptr [edi + 56], mm7

            sub esi, -64
            sub edi, -64
            dec ecx
            jnz MainLoop_NTQ

            sfence                  ; flush write buffer
            emms
        }
    }
#endif

    return dest;
}

void * memcpy_sse2a(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    //int n = (int)((len / CACHE_BLOCK_SIZE) * CACHE_BLOCK_SIZE);
    size_t len_n = (len & (CACHE_BLOCK_SIZE - 1));
    int n = (int)(len - len_n);
    // 剩余的不对齐CACHE_BLOCK_SIZE但对齐64byte的数据
    int m = (int)(len_n - (len_n & (SIZE_OF_QWORD - 1)));
    // 最后剩余的不对齐64byte的字节数
    int nLeftBytes = len - n - m;
    if (n > 0) {
        __asm {
            push edi
            push esi
            push eax
            push ecx
            //push ebx
            //push edx

            // 下面先拷贝整数个块
            mov esi, src                ; source array
            mov edi, dest               ; destination array
            mov ecx, n                  ; number of QWORDS (8 bytes) assumes len / CACHE_BLOCK_SIZE is an integer

            lea esi, [esi + ecx]        ; end of source
            lea edi, [edi + ecx]        ; end of destination
            neg ecx                     ; use a negative offset as a combo pointer-and-loop-counter

            align 16
MainLoop:
#if 0
            mov eax, CACHE_BLOCK_SIZE / 128 ; note: .prefetchloop is unrolled 2X
            add ecx, CACHE_BLOCK_SIZE       ; move up to end of block

            align 16
PrefetchLoop:
            mov ebx, [esi + ecx - 64]       ; read one address in this cache line...
            mov edx, [esi + ecx - 128]      ; ... and one in the previous line
            sub ecx, 128                    ; 16 x 8 bytes = 2 x 64-byte cache lines
            dec eax
            jnz PrefetchLoop
#endif

            mov eax, CACHE_BLOCK_SIZE / 64

            align 16
WriteLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]   ; fetch ahead by 1024 bytes
#endif

            movdqa xmm0, xmmword ptr [esi + ecx     ]
            movdqa xmm1, xmmword ptr [esi + ecx + 16]
            movdqa xmm2, xmmword ptr [esi + ecx + 32]
            movdqa xmm3, xmmword ptr [esi + ecx + 48]

            movntdq xmmword ptr [edi + ecx     ], xmm0
            movntdq xmmword ptr [edi + ecx + 16], xmm1
            movntdq xmmword ptr [edi + ecx + 32], xmm2
            movntdq xmmword ptr [edi + ecx + 48], xmm3

            sub ecx, -64
            dec eax
            jnz WriteLoop

            or ecx, ecx                     ; assumes integer number of cacheblocks
            jnz MainLoop

            //pop edx
            //pop ebx
            pop ecx
            pop eax
            pop esi
            pop edi
        }
    }

    if (m > 0) {
        __asm {
            push edi
            push esi
            push ecx

            mov esi, src
            mov edi, dest
            mov ecx, m
            lea esi, [esi + ecx]
            lea edi, [edi + ecx]
            neg ecx

            align 16
CopyLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]           // 预读
#endif

            movdqa xmm0, xmmword ptr [esi + ecx     ]
            movdqa xmm1, xmmword ptr [esi + ecx + 16]
            movdqa xmm2, xmmword ptr [esi + ecx + 32]
            movdqa xmm3, xmmword ptr [esi + ecx + 48]

            movntdq xmmword ptr [edi + ecx     ], xmm0
            movntdq xmmword ptr [edi + ecx + 16], xmm1
            movntdq xmmword ptr [edi + ecx + 32], xmm2
            movntdq xmmword ptr [edi + ecx + 48], xmm3

            sub ecx, -64
            jnz CopyLoop

            pop ecx
            pop esi
            pop edi
        }
    }

    __asm {
        sfence
    }

    // 拷贝剩余的字节数据
    if (nLeftBytes > 0)
        memcpy((char*)dest + len - nLeftBytes, (char*)src + len - nLeftBytes, nLeftBytes);

#endif
    return dest;
}

void * memcpy_sse2c(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE || !m_bIsSSE2)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    __asm {

//ac_memcpy_sse2:

        /*
        push ebx
        push ecx
        push edi
        push esi
        //*/

        mov edx, dest		; dest
        mov eax, src		; src
        mov ecx, len	    ; len

        //align 16
//entry64_sse2:
        shr ecx, 6			;  / 64
        jz rest_sse2

        align 16
mainloop_sse2:

#if USE_SOFT_PREFETCH
        PREFETCH_INST [eax + PREFETCH_SZIE]     ; fetch ahead by 1024 bytes
        //prefetchnta   [eax]
        //prefetchnta [eax + 32]
#endif
        movdqa xmm0, xmmword ptr [eax     ]
        movdqa xmm1, xmmword ptr [eax + 16]
        movdqa xmm2, xmmword ptr [eax + 32]
        movdqa xmm3, xmmword ptr [eax + 48]

        movntdq xmmword ptr [edx     ], xmm0
        movntdq xmmword ptr [edx + 16], xmm1
        movntdq xmmword ptr [edx + 32], xmm2
        movntdq xmmword ptr [edx + 48], xmm3

        sub eax, -64
        sub edx, -64
        dec ecx
        jg mainloop_sse2

rest_sse2:
        /*
        prefetchnta [eax]
        mov esi, eax
        mov edi, ebx
        std
        rep movsb
        //*/

//_exit_sse2:
        sfence
        //emms

        //xor eax, eax		; exit

        /*
        pop esi
        pop edi
        pop ecx
        pop ebx
        //*/
    }

#endif
    return dest;
}

void * memcpy_sse2d(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE || !m_bIsSSE2)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    __asm {

//ac_memcpy_sse2:

        ///*
        push edx
        push ecx
        push edi
        push esi
        //*/

        mov edx, dest		; dest
        mov eax, src		; src
        mov ecx, len	    ; len

        //align 16
//entry64_sse2:
        shr ecx, 6			;  / 64
        jz rest_sse2

        align 16
mainloop_sse2:

#if USE_SOFT_PREFETCH
        PREFETCH_INST [eax + PREFETCH_SZIE]     ; fetch ahead by 1024 bytes
        //prefetchnta   [eax]
        //prefetchnta [eax + 32]
#endif
        // movdqa, movaps
        movaps xmm0, xmmword ptr [eax     ]
        movaps xmm1, xmmword ptr [eax + 16]
        movaps xmm2, xmmword ptr [eax + 32]
        movaps xmm3, xmmword ptr [eax + 48]

        sub eax, -64

        movaps xmmword ptr [edx     ], xmm0
        movaps xmmword ptr [edx + 16], xmm1
        movaps xmmword ptr [edx + 32], xmm2
        movaps xmmword ptr [edx + 48], xmm3
        
        sub edx, -64
        dec ecx
        jg mainloop_sse2

rest_sse2:
        /*
        prefetchnta [eax]
        mov esi, eax
        mov edi, ebx
        std
        rep movsb
        //*/

//_exit_sse2:
        //sfence
        //emms

        //xor eax, eax		; exit

        //*
        pop esi
        pop edi
        pop ecx
        pop edx
        //*/
    }
#endif

    return dest;
}

void * memcpy_sse2e(void *dest, const void *src, size_t len)
{
    if (!m_bIsMMX || !m_bIsSSE)
        return NULL;

#if !defined(JIMI_MSVC_CLANG) || (JIMI_MSVC_CLANG == 0)

    //int n = (int)((len / CACHE_BLOCK_SIZE) * CACHE_BLOCK_SIZE);
    int n = (int)(len - (len & (CACHE_BLOCK_SIZE - 1)));
    // 剩余的不对齐CACHE_BLOCK_SIZE但对齐64byte的数据
    int m = ((len - n) / SIZE_OF_QWORD) * SIZE_OF_QWORD;
    // 最后剩余的不对齐64byte的字节数
    int nLeftBytes = len - n - m;
    if (n > 0) {
        __asm {
            push edi
            push esi
            push eax
            push ecx
            //push ebx
            //push edx

            // 下面先拷贝整数个块
            mov esi, src                ; source array
            mov edi, dest               ; destination array
            mov ecx, n                  ; number of QWORDS (8 bytes) assumes len / CACHE_BLOCK_SIZE is an integer

            lea esi, [esi + ecx]        ; end of source
            lea edi, [edi + ecx]        ; end of destination
            neg ecx                     ; use a negative offset as a combo pointer-and-loop-counter

            align 16
MainLoop:
#if 0
            mov eax, CACHE_BLOCK_SIZE / 128 ; note: .prefetchloop is unrolled 2X
            add ecx, CACHE_BLOCK_SIZE       ; move up to end of block

            align 16
PrefetchLoop:
            mov ebx, [esi + ecx - 64]       ; read one address in this cache line...
            mov edx, [esi + ecx - 128]      ; ... and one in the previous line
            sub ecx, 128                    ; 16 x 8 bytes = 2 x 64-byte cache lines
            dec eax
            jnz PrefetchLoop
#endif

            mov eax, CACHE_BLOCK_SIZE / 64

            align 16
WriteLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]  ; fetch ahead by 1024 bytes
#endif
            // movdqa, movaps, movntdq
            movdqa xmm0, xmmword ptr [esi + ecx     ]
            movdqa xmm1, xmmword ptr [esi + ecx + 16]
            movdqa xmm2, xmmword ptr [esi + ecx + 32]
            movdqa xmm3, xmmword ptr [esi + ecx + 48]

            movntdq xmmword ptr [edi + ecx     ], xmm0
            movntdq xmmword ptr [edi + ecx + 16], xmm1
            movntdq xmmword ptr [edi + ecx + 32], xmm2
            movntdq xmmword ptr [edi + ecx + 48], xmm3

            sub ecx, -64
            dec eax
            jnz WriteLoop

            or ecx, ecx                         ; assumes integer number of cacheblocks
            jnz MainLoop

            //pop edx
            //pop ebx
            pop ecx
            pop eax
            pop esi
            pop edi
        }
    }

    if (m > 0) {
        __asm {
            push edi
            push esi
            push ecx

            mov esi, src
            mov edi, dest
            mov ecx, m
            lea esi, [esi + ecx]
            lea edi, [edi + ecx]
            neg ecx

            align 16
CopyLoop:

#if USE_SOFT_PREFETCH
            PREFETCH_INST [esi + ecx + PREFETCH_SZIE]       // 预读
#endif
            // movdqa, movaps, movntdq
            movdqa xmm0, xmmword ptr [esi + ecx     ]
            movdqa xmm1, xmmword ptr [esi + ecx + 16]
            movdqa xmm2, xmmword ptr [esi + ecx + 32]
            movdqa xmm3, xmmword ptr [esi + ecx + 48]

            movntdq xmmword ptr [edi + ecx     ], xmm0
            movntdq xmmword ptr [edi + ecx + 16], xmm1
            movntdq xmmword ptr [edi + ecx + 32], xmm2
            movntdq xmmword ptr [edi + ecx + 48], xmm3

            sub ecx, -64
            jnz CopyLoop

            pop ecx
            pop esi
            pop edi
        }
    }

    __asm {
        sfence
    }

    // 拷贝剩余的字节数据
    if (nLeftBytes > 0)
        memcpy((char*)dest + len - nLeftBytes, (char*)src + len - nLeftBytes, nLeftBytes);

#endif
    return dest;
}

#endif /* _WIN64 */
