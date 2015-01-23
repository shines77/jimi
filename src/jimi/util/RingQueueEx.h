
#ifndef _JIMI_UTIL_RINGQUEUEEX_H_
#define _JIMI_UTIL_RINGQUEUEEX_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/util/RingQueue.h"
#include "jimi/util/DumpUtils.h"
#include "jimi/log/log.h"

#ifdef _MSC_VER
#include <intrin.h>     // For _ReadWriteBarrier(), InterlockedCompareExchange()
#endif
#include <emmintrin.h>

#ifndef JIMI_CACHELINE_SIZE
#define JIMI_CACHELINE_SIZE     64
#endif

namespace jimi {

///////////////////////////////////////////////////////////////////
// class SmallRingQueue<T, Capcity>
///////////////////////////////////////////////////////////////////

template <typename T, uint32_t Capcity = 16U>
class JIMI_DLL SmallRingQueue
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
    SmallRingQueue(bool bFillQueue = false, bool bInitHead = false);
    ~SmallRingQueue();

public:
    void dump_info();
    void dump_detail();

    index_type mask() const      { return kMask;    };
    size_type capcity() const    { return kCapcity; };
    size_type length() const     { return sizes();  };
    size_type sizes() const;

    int push(T * item);
    T * pop();

protected:
    void init(bool bFillQueue = false, bool bInitHead = false);

private:
    RingQueueHead   info;
    value_type      queue[kCapcity];
};

template <typename T, uint32_t Capcity>
SmallRingQueue<T, Capcity>::SmallRingQueue(bool bFillQueue/* = false */, bool bInitHead/* = false */)
{
    init(bFillQueue, bInitHead);
}

template <typename T, uint32_t Capcity>
SmallRingQueue<T, Capcity>::~SmallRingQueue()
{
    // Do nothing!
}

template <typename T, uint32_t Capcity>
inline
void SmallRingQueue<T, Capcity>::init(bool bFillQueue/* = false */, bool bInitHead/* = false */)
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
void SmallRingQueue<T, Capcity>::dump_info()
{
    ReleaseUtils::dump(&info, sizeof(info));
}

template <typename T, uint32_t Capcity>
void SmallRingQueue<T, Capcity>::dump_detail()
{
#if 0
    printf("---------------------------------------------------------\n");
    printf("smallRingQueue.p.head = %u\nsmallRingQueue.p.tail = %u\n\n", info.p.head, info.p.tail);
    printf("smallRingQueue.c.head = %u\nsmallRingQueue.c.tail = %u\n",   info.c.head, info.c.tail);
    printf("---------------------------------------------------------\n\n");
#else
    printf("SmallRingQueue: p(%u, %u), c(%u, %u)\n",
            info.p.head, info.p.tail,
            info.c.head, info.c.tail);
#endif
}

template <typename T, uint32_t Capcity>
inline
typename SmallRingQueue<T, Capcity>::size_type
SmallRingQueue<T, Capcity>::sizes() const
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
int SmallRingQueue<T, Capcity>::push(T * item)
{
    index_type head, tail, next;
    bool ok;

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
T * SmallRingQueue<T, Capcity>::pop()
{
    index_type head, tail, next;
    value_type item;
    bool ok;

    do {
        head = info.c.head;
        tail = info.p.tail;
        //if ((tail - head) < 1U)
        //if (tail == head)
        if (head >= tail && (tail - head) <= kMask)
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

///////////////////////////////////////////////////////////////////
// class RingQueue<T, Capcity>
///////////////////////////////////////////////////////////////////

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

    int push(T * item);
    T * pop();

protected:
    void init(bool bFillQueue = false, bool bInitHead = false);

private:
    RingQueueHead   info;
    value_type *    queue;
};

template <typename T, uint32_t Capcity>
RingQueue<T, Capcity>::RingQueue(bool bFillQueue/* = false */, bool bInitHead/* = false */)
{
    init(bFillQueue, bInitHead);
}

template <typename T, uint32_t Capcity>
RingQueue<T, Capcity>::~RingQueue()
{
    if (queue) {
        delete [] queue;
        queue = NULL;
    }
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

    queue = (value_type *)new T[kCapcity];
    if (queue != NULL && bFillQueue)
        memset((void *)queue, 0, sizeof(value_type) * kCapcity);
}

template <typename T, uint32_t Capcity>
void RingQueue<T, Capcity>::dump_info()
{
    ReleaseUtils::dump(&info, sizeof(info));
}

template <typename T, uint32_t Capcity>
void RingQueue<T, Capcity>::dump_detail()
{
#if 0
    printf("---------------------------------------------------------\n");
    printf("ringQueue.p.head = %u\nringQueue.p.tail = %u\n\n", info.p.head, info.p.tail);
    printf("ringQueue.c.head = %u\nringQueue.c.tail = %u\n",   info.c.head, info.c.tail);
    printf("---------------------------------------------------------\n\n");
#else
    printf("ringQueue: p(%u, %u), c(%u, %u)\n",
            info.p.head, info.p.tail,
            info.c.head, info.c.tail);
#endif
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
int RingQueue<T, Capcity>::push(T *item)
{
    index_type head, tail, next;
    bool ok;

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

    Jimi_ReadWriteBarrier();
    info.p.tail = next;

    return 0;
}

template <typename T, uint32_t Capcity>
inline
T * RingQueue<T, Capcity>::pop()
{
    index_type head, tail, next;
    value_type item;
    bool ok;

    do {
        head = info.c.head;
        tail = info.p.tail;
        //if ((tail - head) < 1U)
        //if (tail == head)
        if (head >= tail && (tail - head) <= kMask)
            return (value_type)NULL;
        next = head + 1;
        ok = jimi_bool_compare_and_swap32(&info.c.head, head, next);
    } while (!ok);

    item = queue[head & kMask];

    Jimi_ReadWriteBarrier();

    while (jimi_unlikely((info.c.tail != head)))
        jimi_mm_pause();


    Jimi_ReadWriteBarrier();
    info.c.tail = next;

    return item;
}

}  /* namespace jimi */

#undef JIMI_CACHELINE_SIZE

#endif  /* _JIMI_UTIL_RINGQUEUEEX_H_ */
