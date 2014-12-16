
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

#define jimi_mm_pause       _mm_pause

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
    InterlockedCompareExchange((volatile uint32_t *)destPtr,    \
                            (uint32_t)(newValue), (uint32_t)(oldValue))
#define jimi_bool_compare_and_swap32(destPtr, oldValue, newValue)       \
    (InterlockedCompareExchange((volatile uint32_t *)destPtr,           \
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

template <typename T, uint32_t Capcity = 16U>
class JIMI_DLL RingQueue
{
public:
    typedef uint32_t    size_type;
    typedef uint32_t    index_type;
    typedef T *         value_type;
    typedef T *         pointer;
    typedef const T *   const_pointer;
    typedef T &         reference;
    typedef const T &   const_reference;

public:
    static const size_type  kCapcity = (size_type)JIMI_MAX(JIMI_ROUND_TO_POW2(Capcity), 2);
    static const index_type kMask    = (index_type)(kCapcity - 1);

public:
    RingQueue(bool bFillQueue = false, bool bInitHead = false);
    ~RingQueue();

public:
    void dump_info();
    void dump_detail();

    index_type mask() const      { return kMask;    };
    size_type capcity() const    { return kCapcity; };
    size_type length() const     { return sizes();  };
    size_type sizes() const;

    void init(bool bFillQueue = false, bool bInitHead = false);

    int push(T * item);
    T * pop();

private:
    RingQueueHead   info;
    value_type      queue[kCapcity];
};

template <typename T, uint32_t Capcity>
RingQueue<T, Capcity>::RingQueue(bool bFillQueue/* = false */, bool bInitHead/* = false */)
{
    init(bInitHead, bFillQueue);
}

template <typename T, uint32_t Capcity>
RingQueue<T, Capcity>::~RingQueue()
{
    // Do nothing!
}

template <typename T, uint32_t Capcity>
inline
void RingQueue<T, Capcity>::init(bool bFillQueue/* = false */, bool bInitHead/* = false */)
{
    if (!bInitHead) {
        info.p.head = 0;
        info.p.tail = 0;
        info.c.head = 0;
        info.c.tail = 0;
    }
    else {
        memset((void *)&info, 0, sizeof(info));
    }

    if (bFillQueue)
        memset((void *)&queue[0], 0, sizeof(queue[0]) * kCapcity);
}

template <typename T, uint32_t Capcity>
void RingQueue<T, Capcity>::dump_info()
{
    ReleaseUtils::dump(&info, sizeof(info));
}

template <typename T, uint32_t Capcity>
void RingQueue<T, Capcity>::dump_detail()
{
    printf("---------------------------------------------------------\n");
    printf("ringQueue.p.head = %u\nringQueue.p.tail = %u\n\n", info.p.head, info.p.tail);
    printf("ringQueue.c.head = %u\nringQueue.c.tail = %u\n",   info.c.head, info.c.tail);
    printf("---------------------------------------------------------\n\n");
}

template <typename T, uint32_t Capcity>
inline
typename RingQueue<T, Capcity>::size_type
RingQueue<T, Capcity>::sizes() const
{
    index_type head, tail;

    head = info.p.head;
    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((info.p.tail != head)))
        jimi_mm_pause();

    tail = info.c.head;
    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((info.c.tail != tail)))
        jimi_mm_pause();

    return (size_type)(head - tail);
}

template <typename T, uint32_t Capcity>
inline
int RingQueue<T, Capcity>::push(value_type item)
{
    index_type head, tail, next;
    int ok;

    do {
        head = info.p.head;
        tail = info.c.tail;
        if ((head - tail) > kMask)
            return -1;
        next = head + 1;
        ok = jimi_bool_compare_and_swap32(&info.p.head, head, next);
    } while (!ok);

    queue[head & kMask] = item;
    
    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((info.p.tail != head)))
        jimi_mm_pause();

    info.p.tail = next;

    return 0;
}

template <typename T, uint32_t Capcity>
inline
typename RingQueue<T, Capcity>::value_type
RingQueue<T, Capcity>::pop()
{
    index_type head, tail, next;
    value_type item;
    int ok;

    do {
        head = info.c.head;
        tail = info.p.tail;
        //if ((tail - head) < 1U)
        if (tail == head)
            return (value_type)NULL;
        next = head + 1;
        ok = jimi_bool_compare_and_swap32(&info.c.head, head, next);
    } while (!ok);

    item = queue[head & kMask];

    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((info.c.tail != head)))
        jimi_mm_pause();

    info.c.tail = next;

    return item;
}

}  /* namespace jimi */

#undef JIMI_CACHELINE_SIZE

#endif  /* _JIMI_UTIL_RINGQUEUE_H_ */
