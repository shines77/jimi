
#ifndef _JIMI_LANG_REFCOUNTED_H_
#define _JIMI_LANG_REFCOUNTED_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/thread/Atomic.h"

NS_JIMI_BEGIN

void *smartRealloc(void *memory, size_t cur_size, size_t cur_capacity, size_t new_size)
{
    return ::realloc(memory, new_size);
}

template <class _CharT,
          class _ValueT,
          class _Atomic = non_atomic<_ValueT> >
class JIMI_DLL refcounted
{
public:
    // Types
    typedef _CharT                          char_type;
    typedef typename _Atomic::value_type    value_type;
    typedef _Atomic                         atomic_type;

public:
    // Contructor
    refcounted() : _refcount(1) { }
    refcounted(const value_type &value) { _refcount = value; }
    ~refcounted() { /*--_refcount;*/ _refcount.fetch_sub(1, 0); }

    static refcounted *fromData(char_type *p) {
        return static_cast<refcounted *>(
            static_cast<void *>(
                static_cast<unsigned char *>(static_cast<void *>(p))
                    - sizeof(atomic_type)));
    }

    static value_type refs(char_type *p) {
        return fromData(p)->_refcount.load(0);
    }

    static void retain(char_type *p)  {
        //++_refcount;
        fromData(p)->_refcount.fetch_add(1, 0);
    }

    static void release(char_type *p) {
        const refcounted *refobj = fromData(p);
        //--_refcount;
        value_type oldcnt = refobj->_refcount.fetch_sub(1, 0);
        value_type refval = refobj->_refcount.load(0);
        if (refval == 0) {
            ::free((void *)refobj);
        }
    }

    static refcounted *create(size_t *size) {
        // Don't forget to allocate one extra Char for the terminating
        // null. In this case, however, one Char is already part of the
        // struct.
        const size_t allocSizeBytes = sizeof(refcounted) + (*size) * sizeof(char_type) + 1;
        refcounted *result = static_cast<refcounted *>(::malloc(allocSizeBytes));
        result->_refcount.store(1, 0);
        *size = (allocSizeBytes - sizeof(refcounted) - 1) / sizeof(char_type);
        return result;
    }

    static refcounted *create(const char_type *data, size_t *size) {
        const size_t effectiveSize = *size;
        refcounted *result = create(size);
        string_detail::pod_copy(result->_data, data, effectiveSize);
        return result;
    }

    static refcounted *reallocate(char_type * const data,
                                  const size_t currentSize,
                                  const size_t currentCapacity,
                                  const size_t newCapacity) {
        jimi_assert(newCapacity > 0 && newCapacity > currentSize);
        const refcounted *refobj = fromData(data);
        jimi_assert(refobj->_refcount.load(0) == 1);
        // Don't forget to allocate one extra Char for the terminating
        // null. In this case, however, one Char is already part of the
        // struct.
        refcounted *result = static_cast<refcounted *>(
            ::jimi::smartRealloc((void *)refobj,
                sizeof(refcounted) + currentSize * sizeof(char_type),
                sizeof(refcounted) + currentCapacity * sizeof(char_type),
                sizeof(refcounted) + newCapacity * sizeof(char_type)));
        jimi_assert(result->_refcount.load(0) == 1);
        return result;
    }

    char_type *data() { return &_data[0]; }

private:
    mutable atomic_type _refcount;
    char_type           _data[1];
};

NS_JIMI_END

#endif  /* _JIMI_LANG_REFCOUNTED_H_ */
