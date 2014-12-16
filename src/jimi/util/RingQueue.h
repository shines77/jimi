
#ifndef _JIMI_UTIL_RINGQUEUE_H_
#define _JIMI_UTIL_RINGQUEUE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/util/DumpUtils.h"
#include "jimi/log/log.h"

#ifdef _MSC_VER
#include <intrin.h>     // For _ReadWriteBarrier(), InterlockedCompareExchange()
#endif
#include <emmintrin.h>

#ifndef JIMI_CACHELINE_SIZE
#define JIMI_CACHELINE_SIZE     64
#endif

#ifndef jimi_mm_pause
#define jimi_mm_pause       _mm_pause
#endif

#if defined(_MSC_VER)

#ifndef jimi_likely
#define jimi_likely(x)      (x)
#endif

#ifndef jimi_unlikely
#define jimi_unlikely(x)    (x)
#endif

///
/// _ReadWriteBarrier
///
/// See: http://msdn.microsoft.com/en-us/library/f20w0x5e%28VS.80%29.aspx
///
/// See: http://en.wikipedia.org/wiki/Memory_ordering
///
#define Jimi_ReadWriteBarrier()  _ReadWriteBarrier();

#else  /* !_MSC_VER */

#ifndef jimi_likely
#define jimi_likely(x)      __builtin_expect((x), 1)
#endif

#ifndef jimi_unlikely
#define jimi_unlikely(x)    __builtin_expect((x), 0)
#endif

///
/// See: http://en.wikipedia.org/wiki/Memory_ordering
///
/// See: http://bbs.csdn.net/topics/310025520
///

#define Jimi_ReadWriteBarrier()     asm volatile ("":::"memory");
//#define Jimi_ReadWriteBarrier()     __asm__ __volatile__ ("":::"memory");

#endif  /* _MSC_VER */

#if defined(_MSC_VER)
#define jimi_compare_and_swap32(destPtr, oldValue, newValue)    \
    InterlockedCompareExchange((volatile LONG *)destPtr,    \
                            (uint32_t)(newValue), (uint32_t)(oldValue))
#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    (InterlockedCompareExchange((volatile LONG *)destPtr,           \
                            (uint32_t)(newValue), (uint32_t)(oldValue)) \
                                == (uint32_t)(oldValue))
#elif defined(__linux__) || defined(__cygwin__) || defined(__MINGW__) || defined(__MINGW32__)
#define jimi_compare_and_swap32(destPtr, oldValue, newValue)    \
    __sync_compare_and_swap((volatile uint32_t *)destPtr,       \
                            (uint32_t)(oldValue), (uint32_t)(newValue))
#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)   \
    __sync_bool_compare_and_swap((volatile uint32_t *)destPtr,      \
                            (uint32_t)(oldValue), (uint32_t)(newValue))
#else
#define jimi_compare_and_swap32(destPtr, oldValue, newValue)        \
    __internal_compare_and_swap32((volatile uint32_t *)(destPtr),   \
                                (uint32_t)(oldValue), (uint32_t)(newValue))
#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    __internal_bool_compare_and_swap32((volatile uint32_t *)(destPtr),  \
                                (uint32_t)(oldValue), (uint32_t)(newValue))
#endif  /* _MSC_VER */

static inline
uint32_t __internal_compare_and_swap32(volatile uint32_t *destPtr,
                                       uint32_t oldValue,
                                       uint32_t newValue)
{
    uint32_t origValue = *destPtr;
    if (*destPtr == oldValue) {
        *destPtr = newValue;
    }
    return origValue;
}

static inline
uint32_t __internal_bool_compare_and_swap32(volatile uint32_t *destPtr,
                                            uint32_t oldValue,
                                            uint32_t newValue)
{
    if (*destPtr == oldValue) {
        *destPtr = newValue;
        return 1;
    }
    else
        return 0;
}

namespace jimi {

struct RingQueueInfo
{
    volatile uint32_t head;
    volatile uint32_t tail;
};

typedef struct RingQueueInfo RingQueueInfo;

struct RingQueueHead
{
    RingQueueInfo p;
    char padding1[JIMI_CACHELINE_SIZE - sizeof(RingQueueInfo)];

    RingQueueInfo c;
    char padding2[JIMI_CACHELINE_SIZE - sizeof(RingQueueInfo)];
};

typedef struct RingQueueHead RingQueueHead;

///////////////////////////////////////////////////////////////////
// class SmallRingQueueCore<Capcity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capcity>
class JIMI_DLL SmallRingQueueCore
{
public:
    typedef uint32_t    size_type;
    typedef T *         item_type;

public:
    static const size_type  kCapcityCore = (size_type)JIMI_MAX(JIMI_ROUND_TO_POW2(Capcity), 2);
    static const bool kIsAllocOnHeap     = false;

public:
    RingQueueHead   info;
    item_type       queue[kCapcityCore];
};

///////////////////////////////////////////////////////////////////
// class RingQueueCore<Capcity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capcity>
class JIMI_DLL RingQueueCore
{
public:
    typedef T *     item_type;

public:
    static const bool kIsAllocOnHeap = true;

public:
    RingQueueHead   info;
    item_type *     queue;
};

///////////////////////////////////////////////////////////////////
// class RingQueueBase<T, Capcity, CoreTy>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capcity = 16U, typename CoreTy = RingQueueCore>
class JIMI_DLL RingQueueBase
{
public:
    typedef uint32_t                    size_type;
    typedef uint32_t                    index_type;
    typedef T *                         value_type;
    typedef typename CoreTy::item_type  item_type;
    typedef CoreTy                      core_type;
    typedef T *                         pointer;
    typedef const T *                   const_pointer;
    typedef T &                         reference;
    typedef const T &                   const_reference;

public:
    static const size_type  kCapcity = (size_type)JIMI_MAX(JIMI_ROUND_TO_POW2(Capcity), 2);
    static const index_type kMask    = (index_type)(kCapcity - 1);

public:
    RingQueueBase(bool bInitHead = false);
    ~RingQueueBase();

public:
    void dump_info();
    void dump_detail();

    index_type mask() const      { return kMask;    };
    size_type capcity() const    { return kCapcity; };
    size_type length() const     { return sizes();  };
    size_type sizes() const;

    void init(bool bInitHead = false);

    int push(T * item);
    T * pop();

protected:
    core_type core;
};

template <typename T, uint32_t Capcity, typename CoreTy>
RingQueueBase<T, Capcity, CoreTy>::RingQueueBase(bool bInitHead  /* = false */)
{
    //printf("RingQueueBase::RingQueueBase();\n\n");

    init(bInitHead);
}

template <typename T, uint32_t Capcity, typename CoreTy>
RingQueueBase<T, Capcity, CoreTy>::~RingQueueBase()
{
    // Do nothing!
}

template <typename T, uint32_t Capcity, typename CoreTy>
inline
void RingQueueBase<T, Capcity, CoreTy>::init(bool bInitHead /* = false */)
{
    //printf("RingQueueBase::init();\n\n");

    if (!bInitHead) {
        core.info.p.head = 0;
        core.info.p.tail = 0;
        core.info.c.head = 0;
        core.info.c.tail = 0;
    }
    else {
        memset((void *)&core.info, 0, sizeof(core.info));
    }
}

template <typename T, uint32_t Capcity, typename CoreTy>
void RingQueueBase<T, Capcity, CoreTy>::dump_info()
{
    ReleaseUtils::dump(&core.info, sizeof(core.info));
}

template <typename T, uint32_t Capcity, typename CoreTy>
void RingQueueBase<T, Capcity, CoreTy>::dump_detail()
{
#if 0
    printf("---------------------------------------------------------\n");
    printf("RingQueueBase.p.head = %u\nRingQueueBase.p.tail = %u\n\n", core.info.p.head, core.info.p.tail);
    printf("RingQueueBase.c.head = %u\nRingQueueBase.c.tail = %u\n",   core.info.c.head, core.info.c.tail);
    printf("---------------------------------------------------------\n\n");
#else
    printf("RingQueueBase: p(%u, %u), c(%u, %u)\n",
           core.info.p.head, core.info.p.tail,
            core.info.c.head, core.info.c.tail);
#endif
}

template <typename T, uint32_t Capcity, typename CoreTy>
inline
typename RingQueueBase<T, Capcity, CoreTy>::size_type
RingQueueBase<T, Capcity, CoreTy>::sizes() const
{
    index_type head, tail;

    head = core.info.p.head;
    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((core.info.p.tail != head)))
        jimi_mm_pause();

    tail = core.info.c.head;
    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((core.info.c.tail != tail)))
        jimi_mm_pause();

    return (size_type)(head - tail);
}

template <typename T, uint32_t Capcity, typename CoreTy>
inline
int RingQueueBase<T, Capcity, CoreTy>::push(T * item)
{
    index_type head, tail, next;
    int ok;

    do {
        head = core.info.p.head;
        tail = core.info.c.tail;
        if ((head - tail) > kMask)
            return -1;
        next = head + 1;
        ok = jimi_bool_compare_and_swap32(&core.info.p.head, head, next);
    } while (!ok);

    core.queue[head & kMask] = item;
    
    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((core.info.p.tail != head)))
        jimi_mm_pause();

    core.info.p.tail = next;

    return 0;
}

template <typename T, uint32_t Capcity, typename CoreTy>
inline
T * RingQueueBase<T, Capcity, CoreTy>::pop()
{
    index_type head, tail, next;
    value_type item;
    int ok;

    do {
        head = core.info.c.head;
        tail = core.info.p.tail;
        //if ((tail - head) < 1U)
        if (tail == head)
            return (value_type)NULL;
        next = head + 1;
        ok = jimi_bool_compare_and_swap32(&core.info.c.head, head, next);
    } while (!ok);

    item = core.queue[head & kMask];

    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((core.info.c.tail != head)))
        jimi_mm_pause();

    core.info.c.tail = next;

    return item;
}

///////////////////////////////////////////////////////////////////
// class SmallRingQueue2<T, Capcity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capcity = 16U>
class JIMI_DLL SmallRingQueue2 : public RingQueueBase<T, Capcity, SmallRingQueueCore<T, Capcity>>
{
public:
    SmallRingQueue2(bool bFillQueue = false, bool bInitHead = false);
    ~SmallRingQueue2();

public:
    void dump_detail();

protected:
    void init_queue(bool bFillQueue = false);
};

template <typename T, uint32_t Capcity>
SmallRingQueue2<T, Capcity>::SmallRingQueue2(bool bFillQueue /* = false */,
                                             bool bInitHead  /* = false */)
: RingQueueBase(bInitHead)
{
    //printf("SmallRingQueue2::SmallRingQueue2();\n\n");

    init_queue(bFillQueue);
}

template <typename T, uint32_t Capcity>
SmallRingQueue2<T, Capcity>::~SmallRingQueue2()
{
    // Do nothing!
}

template <typename T, uint32_t Capcity>
inline
void SmallRingQueue2<T, Capcity>::init_queue(bool bFillQueue /* = false */)
{
    //printf("SmallRingQueue2::init_queue();\n\n");

    if (bFillQueue) {
        memset((void *)core.queue, 0, sizeof(value_type) * kCapcity);
    }
}

template <typename T, uint32_t Capcity>
void SmallRingQueue2<T, Capcity>::dump_detail()
{
#if 0
    printf("---------------------------------------------------------\n");
    printf("SmallRingQueue2.p.head = %u\nSmallRingQueue2.p.tail = %u\n\n", core.info.p.head, core.info.p.tail);
    printf("SmallRingQueue2.c.head = %u\nSmallRingQueue2.c.tail = %u\n",   core.info.c.head, core.info.c.tail);
    printf("---------------------------------------------------------\n\n");
#else
    printf("SmallRingQueue2: p(%u, %u), c(%u, %u)\n",
            core.info.p.head, core.info.p.tail,
            core.info.c.head, core.info.c.tail);
#endif
}

///////////////////////////////////////////////////////////////////
// class RingQueue2<T, Capcity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capcity = 16U>
class JIMI_DLL RingQueue2 : public RingQueueBase<T, Capcity, RingQueueCore<T, Capcity>>
{
public:
    RingQueue2(bool bFillQueue = false, bool bInitHead = false);
    ~RingQueue2();

public:
    void dump_detail();

protected:
    void init_queue(bool bFillQueue = false);
};

template <typename T, uint32_t Capcity>
RingQueue2<T, Capcity>::RingQueue2(bool bFillQueue /* = false */,
                                   bool bInitHead  /* = false */)
: RingQueueBase(bInitHead)
{
    //printf("RingQueue2::RingQueue2();\n\n");

    init_queue(bFillQueue);
}

template <typename T, uint32_t Capcity>
RingQueue2<T, Capcity>::~RingQueue2()
{
    // If the queue is allocated on system heap, release them.
    if (core_type::kIsAllocOnHeap) {
        delete [] core.queue;
        core.queue = NULL;
    }
}

template <typename T, uint32_t Capcity>
inline
void RingQueue2<T, Capcity>::init_queue(bool bFillQueue /* = false */)
{
    //printf("RingQueue2::init_queue();\n\n");

    value_type *newData = (value_type *)new T[kCapcity];
    if (newData != NULL) {
        core.queue = newData;
        if (bFillQueue) {
            memset((void *)core.queue, 0, sizeof(value_type) * kCapcity);
        }
    }
}

template <typename T, uint32_t Capcity>
void RingQueue2<T, Capcity>::dump_detail()
{
#if 0
    printf("---------------------------------------------------------\n");
    printf("RingQueue2.p.head = %u\nRingQueue2.p.tail = %u\n\n", core.info.p.head, core.info.p.tail);
    printf("RingQueue2.c.head = %u\nRingQueue2.c.tail = %u\n",   core.info.c.head, core.info.c.tail);
    printf("---------------------------------------------------------\n\n");
#else
    printf("RingQueue2: p(%u, %u), c(%u, %u)\n",
            core.info.p.head, core.info.p.tail,
            core.info.c.head, core.info.c.tail);
#endif
}

}  /* namespace jimi */

#undef JIMI_CACHELINE_SIZE

#endif  /* _JIMI_UTIL_RINGQUEUE_H_ */
