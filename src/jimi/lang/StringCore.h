
#ifndef _JIMI_LANG_STRING_CORE_H_
#define _JIMI_LANG_STRING_CORE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimi/thread/Atomic.h>
#include <jimi/lang/Char_Traits.h>
#include <jimi/lang/RefCounted.h>
#include <jimi/lang/StringCore.h>
#include <jimic/string/jm_strings.h>
#include <jimic/string/jmf_strings.h>

#if _MSC_VER
#pragma warning(push)

#pragma warning(disable : 4995)         // 禁止不带_s函数的废弃warning信息
#pragma warning(disable : 4996)
#endif  /* _MSC_VER */

#include <string>
using namespace std;

//#define JIMI_STRING_PERVERSE
//#define JIMI_STRING_CONSERVATIVE

#define JIMI_ALIGNED_TO(x, alignment)   \
    ((x + ((alignment) - 1)) & ~size_t((alignment) - 1))

NS_JIMI_BEGIN

/// <comment>
///
/// 标准C＋＋类string的Copy-On-Write技术（一）
///
/// Reference:
///    http://haoel.blog.51cto.com/313033/124638
///
/// 漫步Facebook开源C++库folly(1)：string类的设计
///
/// Reference:
///    http://www.cnblogs.com/promise6522/archive/2012/06/05/2535530.html
///
/// </comment>

#define STRING_SMALL_BYTES      32

#define STRING_MEDIUM_SIZE      256

#define STRING_NULL_CHAR        '\0'

typedef enum StringTypeMask
{
    STRING_TYPE_SMALL       = (sizeof(size_t) == 4) ? 0x01000000 : 0x0100000000000000,
    STRING_TYPE_MEDIUM      = (sizeof(size_t) == 4) ? 0x02000000 : 0x0200000000000000,
    STRING_TYPE_LARGE       = (sizeof(size_t) == 4) ? 0x04000000 : 0x0400000000000000,
    STRING_TYPE_CONSTANT    = (sizeof(size_t) == 4) ? 0x08000000 : 0x0800000000000000,
    STRING_TYPE_MASK        = (STRING_TYPE_SMALL
        | STRING_TYPE_MEDIUM | STRING_TYPE_LARGE | STRING_TYPE_CONSTANT),
} StringTypeMask;

typedef enum StringTypeMaskX
{
    STRING_TYPE_SMALL_X     = 0x01,
    STRING_TYPE_MEDIUM_X    = 0x02,
    STRING_TYPE_LARGE_X     = 0x04,
    STRING_TYPE_CONSTANT_X  = 0x08,
    STRING_TYPE_MASK_X      = (STRING_TYPE_SMALL_X
        | STRING_TYPE_MEDIUM_X | STRING_TYPE_LARGE_X | STRING_TYPE_CONSTANT_X),
} StringTypeMaskX;

#define STRING_TYPE_IS(type, mask)      ((type & mask) != 0)

#define STRING_TYPE_IS_SMALL(type)      ((type & STRING_TYPE_SMALL)     != 0)
#define STRING_TYPE_IS_MEDIUM(type)     ((type & STRING_TYPE_MEDIUM)    != 0)
#define STRING_TYPE_IS_LARGE(type)      ((type & STRING_TYPE_LARGE)     != 0)
#define STRING_TYPE_IS_CONSTANT(type)   ((type & STRING_TYPE_CONSTANT)  != 0)

#define STRING_TYPE_NOT_IS_SMALL(type)   \
    ((type & (STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT))) != 0)

#define STRING_TYPE_IS_MEDIUM_OR_LARGE(type)   \
    ((type & (STRING_TYPE_MEDIUM | STRING_TYPE_LARGE)) != 0)

#define STRING_CORE_CLASSES    \
    class _CharT, class _RefCount
#define STRING_CORE            \
    string_core<_CharT, _RefCount>

template <class _CharT, class _RefCount = refcounted<_CharT, size_t> >
class JIMI_API string_core
{
public:
    // Types
    typedef _CharT              char_type;
    typedef _RefCount           refcount_type;
    typedef size_t              size_type;
    typedef size_t              flag_type;
    typedef char_traits<_CharT> traits_type;

    struct small_info_t;
    struct small_t;
    struct core_data_t;
    struct medium_large;

    typedef struct small_info_t small_info_t;
    typedef struct small_t      small_t;
    typedef struct core_data_t  core_data_t;
    typedef struct medium_large medium_large;

    // Constant
    static const flag_type      kIsSmall    =  STRING_TYPE_SMALL;
    static const flag_type      kIsMedium   =  STRING_TYPE_MEDIUM;
    static const flag_type      kIsLarge    =  STRING_TYPE_LARGE;
    static const flag_type      kIsConstant =  STRING_TYPE_CONSTANT;
    static const flag_type      kTypeMask   =  STRING_TYPE_MASK;
    static const flag_type      kSizeMask   = ~STRING_TYPE_MASK;

    static const flag_type      kIsMediumOrLargeMask = STRING_TYPE_MEDIUM | STRING_TYPE_LARGE;
    static const flag_type      kNotIsSmallMask      = STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT);

    static const size_type      kMaxMediumSizeBytes  = JIMI_ALIGNED_TO(STRING_MEDIUM_SIZE * sizeof(char_type), 64);

    static const size_type      kMaxSmallSize   = (STRING_SMALL_BYTES - sizeof(small_info_t)) / sizeof(char_type);
    static const size_type      kMaxMediumSize  = kMaxMediumSizeBytes / sizeof(char_type);

public:
    // Contructor
    string_core();
    string_core(const string_core &rhs);
    string_core(const char_type * const src);
    string_core(const char_type * const src, const size_t size);
    ~string_core();

    void destroy();

    void retail();
    void release();

    bool equals(const string_core &rhs) const;

    int compare(const string_core &rhs) const;
    int compare(const char_type *rhs) const;

    flag_type getType() const       { return (_ml.type & kTypeMask); }
    flag_type getTypeValue() const  { return _ml.type;               }
    flag_type getTypeMask() const   { return kTypeMask;              }

    bool is_small() const       { return STRING_TYPE_IS_SMALL(_ml.type);    }
    bool is_medium() const      { return STRING_TYPE_IS_MEDIUM(_ml.type);   }
    bool is_large() const       { return STRING_TYPE_IS_LARGE(_ml.type);    }
    bool is_constant() const    { return STRING_TYPE_IS_CONSTANT(_ml.type); }

    bool is_medium_or_large() const { return STRING_TYPE_IS_MEDIUM_OR_LARGE(_ml.type); }

    bool not_is_small() const { return STRING_TYPE_NOT_IS_SMALL(_ml.type); }

    bool is_shared() const {
        return (is_large() && (refcount_type::refs(_ml.data) > 1));
    }

    static bool is_small(const flag_type type)      { return STRING_TYPE_IS_SMALL(type);    }
    static bool is_medium(const flag_type type)     { return STRING_TYPE_IS_MEDIUM(type);   }
    static bool is_large(const flag_type type)      { return STRING_TYPE_IS_LARGE(type);    }
    static bool is_constant(const flag_type type)   { return STRING_TYPE_IS_CONSTANT(type); }

    static bool is_medium_or_large(const flag_type type) { return STRING_TYPE_IS_MEDIUM_OR_LARGE(type); }

    static bool not_is_small(const flag_type type) { return STRING_TYPE_NOT_IS_SMALL(type); }

    static bool is_shared(const medium_large &ml) {
        return (is_large(ml.type) && (refcount_type::refs(ml.data) > 1));
    }

    const char_type *data() const { return c_str(); }
    const char_type *c_str() const;

    char_type *mutable_data();

    size_type size() const;
    size_type capacity() const;

    // swap below doesn't test whether &rhs == this (and instead
    // potentially does extra work) on the premise that the rarity of
    // that situation actually makes the check more expensive than is
    // worth.
    void swap(string_core &rhs);

    // Don't disabled
    string_core & operator = (const string_core &rhs);

    void reserve(size_type minCapacity);

    void reserveTo(size_type minCapacity);
    void small_reserveTo(size_type minCapacity);
    void medium_reserveTo(size_type minCapacity);
    void large_reserveTo(size_type minCapacity);

    void expandTo(const size_type newSize);
    void shrinkTo(const size_type newSize);

    void push_back(const char_type c);

    void writeNull();

protected:
    size_t calc_capacity(size_t src_len);

private:

    void small_clone(small_t &dest, const small_t &src) {
        dest.info.lastShort = src.info.lastShort;
        traits_type::strncpy_unsafe(dest.buf, src.buf, src.info.size);
        dest.buf[src.info.size] = '\0';
    }

    void ml_clone(medium_large &dest, const medium_large &src) {
        dest.data = src.data;
        dest.size = src.size;
        dest.capacity = src.capacity;
        dest.type = src.type;
    }

private:
    /* lastShort 有时候可以提供效率 */
    struct small_info_t {
        union {
            struct {
                unsigned char size;
                unsigned char type;
            };
            unsigned short    lastShort;
        };
    };

    /* small string optimized buffer */
    struct small_t {
        union {
            struct {
                /* (dummy只是占位用, 未使用) */
                unsigned char dummy[(STRING_SMALL_BYTES - sizeof(small_info_t)) / sizeof(char)];
                /* size and type */
                small_info_t  info;
            };
            char_type buf[(STRING_SMALL_BYTES - sizeof(small_info_t)) / sizeof(char_type)];
        };
    };

    /* 这个结构只是为了动态设置buf的size而存在的 */
    struct core_data_t {
        char_type  *data;
        size_type   size;
        size_type   capacity;
        flag_type   type;
    };

    /* medium 和 large 共享一样的结构 */
    struct medium_large {
        union {
            struct {
                /* (dummy只是占位用, 未使用) */
                unsigned char dummy[(STRING_SMALL_BYTES - sizeof(core_data_t)) / sizeof(char)];

                /* 后面的定义必须和core_data的结构一致 */
                char_type *data;
                size_type  size;
                size_type  capacity;
                flag_type  type;
            };

            char_type buf[(STRING_SMALL_BYTES - sizeof(core_data_t)) / sizeof(char_type)];
        };
    };

private:
    /* 这是一个union联合, 即small_t类型和medium_large类型共享于同一个内存结构 */
    union {
        /* mutable修饰符是针对const修饰的, 即使是在被const修饰过的成员函数里, */
        /* 也可以改变被mutable修饰过的成员变量 */
        /* Reference: http://blog.csdn.net/wuliming_sc/article/details/3717017 */
        mutable small_t      _small;
        mutable medium_large _ml;
    };
};

template <STRING_CORE_CLASSES>
STRING_CORE::string_core()
{
#if 1
    _ml.type = STRING_TYPE_SMALL;
    (*(size_t *)(&_small.buf[0])) = 0;
#else
    _ml.data = NULL;
    _ml.size = 0;
    _ml.capacity = 0;
    _ml.type = 0;

    // init sso buffer
    (*(size_t *)(&_ml.buf[0])) = 0;
#endif
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const string_core &rhs)
{
    jimi_assert(&rhs != this);
    flag_type type = rhs.getType();
    /* small string optimized */
    if (type == kIsSmall) {
#if 0
        _small.info.type = rhs._small.info.type;
        _small.info.size = rhs._small.info.size;
#else
        _small.info.lastShort = rhs._small.info.lastShort;
#endif
        jimi_assert(rhs._small.info.size < kMaxSmallSize);
        if (_small.info.size < kMaxSmallSize) {
            //traits_type::strncpy(&_small.buf[0], kMaxSmallSize, &rhs._small.buf[0], _small.info.size);
            //traits_type::strncpy_unsafe(&_small.buf[0], &rhs._small.buf[0], _small.info.size);
            string_detail::pod_copy(&_small.buf[0], &rhs._small.buf[0], _small.info.size);
        }
        else {
            sLog.error("string_core(const string_core &src): data = %s, type = 0x%04X, size = %d.",
                &_small.buf[0], _small.info.type, _small.info.size);
        }
        jimi_assert(getType() == kIsSmall && this->size() == rhs.size());
    }
    /* eager copy */
    else if (type == kIsMedium) {
        // Medium strings are copied eagerly. Don't forget to allocate
        // one extra Char for the null terminator.
        jimi_assert(rhs._ml.capacity == kMaxMediumSize - 1);
        char_type *newData = traits_type::mem_alloc(kMaxMediumSize);
        //traits_type::strncpy(_ml.data, kMaxMediumSize, rhs._ml.data, rhs._ml.size);
        //traits_type::strncpy_unsafe(newData, rhs._ml.data, rhs._ml.size);
        string_detail::pod_copy(newData, rhs._ml.data, rhs._ml.size + 1);
        // No need for writeNull() here, we copied one extra
        // element just above.
        _ml.data     = newData;
        _ml.size     = rhs._ml.size;
        _ml.capacity = kMaxMediumSize - 1;
        _ml.type     = STRING_TYPE_MEDIUM;

        jimi_assert(getType() == kIsMedium);
    }
    /* copy-on-write */
    else if (type == kIsLarge) {
        // Large strings are just refcounted
        _ml.data     = rhs._ml.data;
        _ml.size     = rhs._ml.size;
        _ml.capacity = rhs._ml.capacity;
        _ml.type     = STRING_TYPE_LARGE;

        refcount_type::retail(_ml.data);
        //const_cast<string_core &>(rhs).retail();
        jimi_assert(getType() == kIsLarge && size() == rhs.size());
    }
    /* constant string data */
    else if (type == kIsConstant) {
        // It's a constant string data
    }
    /* unknown type */
    else {
        sLog.error("string_core(const string_core &src): type = 0x%04X, data = %08X, size() = %d.", getType(), _ml.data, _ml.size);
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type * const src)
: string_core(src, ::jm_strlen(src))
{
#if 1
    // Do nothing!!!
    //return;
#else

#if 0
    size_t src_len = ::strnlen(src, kMaxMediumSize);
#else
    size_t src_len = ::strlen(src);
#endif
    /* small string optimized */
    if (src_len < kMaxSmallSize) {
        _small.info.type = STRING_TYPE_SMALL_X;
        _small.info.size = src_len;
#if 1
        traits_type::strncpy_unsafe(_small.buf, src, src_len);
        _small.buf[src_len] = '\0';
#else
        ::strcpy(_small.buf, src);
#endif
    }
#if 1
    /* eager copy */
    else if (src_len < kMaxMediumSize) {
        _ml.size = src_len;
        _ml.capacity = calc_capacity(src_len);
        _ml.type = STRING_TYPE_MEDIUM;
        _ml.data = traits_type::mem_alloc(_ml.capacity);
        if (_ml.data) {
            traits_type::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
    /* copy-on-write or unknown type */
    else {
        size_t src_left;
        src_left = ::strlen(src + src_len);
        src_len += src_left;
        _ml.capacity = calc_capacity(src_len);
        _ml.data = traits_type::mem_alloc(_ml.capacity);
        _ml.size = src_len;
        _ml.type = STRING_TYPE_LARGE;
        if (_ml.data) {
            traits_type::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
#endif

#endif
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type * const src, const size_t size)
{
    /* small string optimized */
    if (size < kMaxSmallSize) {
        _small.info.size = (unsigned char)size;
        _small.info.type = STRING_TYPE_SMALL_X;
#if 1
        //traits_type::strncpy_unsafe(_small.buf, src, src_len);
        string_detail::pod_copy(_small.buf, src, size);
        _small.buf[size] = '\0';
#else
        ::strcpy(_small.buf, src);
#endif
    }
    /* eager copy */
    else if (size < kMaxMediumSize) {
        // Medium strings are allocated normally. Don't forget to
        // allocate one extra Char for the terminating null.
        size_type allocSize = calc_capacity(size);
        char_type *newData = traits_type::mem_alloc(allocSize);
#if 1
        string_detail::pod_copy(newData, src, size);
        newData[size] = '\0';
#else
        if (newData) {
            //traits_type::strncpy(_ml.data, _ml.capacity, src, src_len);
            //traits_type::strncpy_unsafe(_ml.data, src, src_len);
            string_detail::pod_copy(newData, src, src_len);
            newData[src_len] = '\0';
        }
#endif
        _ml.data = newData;
        _ml.size = size;
        _ml.capacity = allocSize - 1;
        _ml.type = STRING_TYPE_MEDIUM;
    }
    /* copy-on-write or unknown type */
    else {
        // Large strings are allocated differently
        size_type effectiveCapacity = calc_capacity(size);
        refcount_type *newRC = refcount_type::create(src, &effectiveCapacity);
        char_type *newData = newRC->data();
#if 1
        newData[size] = '\0';
#else
        if (newData) {
            //traits_type::strncpy(newData, effectiveCapacity, src, src_len);
            //traits_type::strncpy_unsafe(newData, src, src_len);
            string_detail::pod_copy(newData, src, src_len);
            newData[src_len] = '\0';
        }
#endif
        _ml.data     = newData;
        _ml.size     = size;
        _ml.capacity = effectiveCapacity;
        _ml.type     = STRING_TYPE_LARGE;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::~string_core()
{
    destroy();
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::destroy()
{
    if (is_medium()) {
        if (_ml.data != NULL) {
            delete _ml.data;
            _ml.data = NULL;
        }
    }
    else if (is_large()) {
        if (_ml.data != NULL) {
            refcount_type::release(_ml.data);
            _ml.data = NULL;
        }
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::retail()
{
    if (is_large())
        _ml.type = STRING_TYPE_LARGE;
    refcount_type::retail(_ml.data);
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::release()
{
    if (is_medium()) {
        if (_ml.data != NULL) {
            delete _ml.data;
            _ml.data = NULL;
        }
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
    }
    else if (is_large()) {
        if (_ml.data != NULL) {
            refcount_type::release(_ml.data);
            _ml.data = NULL;
        }
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE & STRING_CORE::operator = (const STRING_CORE &rhs) {
    flag_type type = rhs.getType();
    if (type == kIsSmall)
        small_clone(_small, rhs._small);
    else
        ml_clone(_ml, rhs._ml);
    return *this;
}

// swap below doesn't test whether &rhs == this (and instead
// potentially does extra work) on the premise that the rarity of
// that situation actually makes the check more expensive than is
// worth.

template <STRING_CORE_CLASSES>
inline void STRING_CORE::swap(STRING_CORE &rhs)
{
#if 1
        // 在不同的type下, _ml的有些数据是不必复制的
        flag_type type = rhs.getType();
        if (type == kIsSmall) {
            small_t t;
            small_clone(t, _small);
            small_clone(_small, rhs._small);
            small_clone(rhs._small, t);
        }
        else {
            medium_large t;
            ml_clone(t, _ml);
            ml_clone(_ml, rhs._ml);
            ml_clone(rhs._ml, t);
        }
#else
        // 完全直接复制_ml, 有些复制可能是多余的
        const medium_large t = _ml;
        _ml = rhs._ml;
        rhs._ml = t;
#endif
}

template <STRING_CORE_CLASSES>
inline typename STRING_CORE::size_type STRING_CORE::size() const
{
    if (is_small())
        return _small.info.size;
    else
        return _ml.size;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::capacity() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return kMaxSmallSize - 1;
    else if (type == kIsMedium)
        return kMaxMediumSize - 1;
    else
        return _ml.capacity;
}

template <STRING_CORE_CLASSES>
inline const typename STRING_CORE::char_type *STRING_CORE::c_str() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return &_small.buf[0];
    else
        return _ml.data;
}

template <STRING_CORE_CLASSES>
inline typename STRING_CORE::char_type *STRING_CORE::mutable_data()
{
    flag_type type = getType();
    if (type == kIsSmall)
        return &_small.buf[0];

    if (type == kIsLarge && refcount_type::refs(_ml.data) > 1) {
        // Ensure unique.
        size_t effectiveCapacity = _ml.capacity;
        refcount_type *newRC = refcount_type::create(&effectiveCapacity);
        char_type *newData = newRC->data();
        // If this fails, someone placed the wrong capacity in an jimi_string.
        jimi_assert(effectiveCapacity >= _ml.capacity);
        string_detail::pod_copy(newData, _ml.data, _ml.size + 1);
        refcount_type::release(_ml.data);
        _ml.data = newData;
        // No need to call writeNull(), we have + 1 above.
    }
    return _ml.data;
}

template <STRING_CORE_CLASSES>
inline size_t STRING_CORE::calc_capacity(size_t src_len)
{
    return (src_len + 1);
}

template <STRING_CORE_CLASSES>
inline bool STRING_CORE::equals(const STRING_CORE &rhs) const
{
    return (&rhs == this);
}

template <STRING_CORE_CLASSES>
inline int STRING_CORE::compare(const STRING_CORE &rhs) const
{
    if (&rhs == this)
        return 0;

    int equal = -1;
    char_type *rhs_data;

    if (_ml.size == rhs._ml.size) {
        if (getType() == rhs.getType()) {
            if (is_small()) {
                equal = traits_type::strncmp(&_ml.buf[0], &rhs._ml.buf[0], _ml.size);
            }
            else if (not_is_small()) {
                rhs_data = rhs._ml.data;
                if (_ml.data == rhs_data)
                    equal = 0;
                else
                    equal = traits_type::strncmp(_ml.data, rhs_data, _ml.size);
            }
            else {
                // Unknown string type
                sLog.error("string_core::compare(const string_core &rhs) - Error: Unknown string type.");
            }
        }
        else {
            //
        }
    }
    else {
        // Is not same size
        sLog.info("string_core::compare(const string_core &rhs), size() = %d, rhs.size() = %d", size(), rhs.size());
    }
    return equal;
}

template <STRING_CORE_CLASSES>
inline int STRING_CORE::compare(const char_type *rhs) const
{
    int equal = -1;
    if (is_small()) {
        if (_ml.buf == rhs)
            equal = 0;
        else
            equal = traits_type::strncmp(_ml.buf, rhs, _ml.size);
    }
    else if (not_is_small()) {
        if (_ml.data == rhs)
            equal = 0;
        else
            equal = traits_type::strncmp(_ml.data, rhs, _ml.size);
    }
    else {
        // Unknown string type
        sLog.error("STRING_CORE::compare(const char_type *rhs) - Error: Unknown string type.");
    }
    return equal;
}

template <STRING_CORE_CLASSES>
inline int operator == (const STRING_CORE &lhs, const STRING_CORE &rhs)
{
    return lhs.compare(rhs);
}

template <STRING_CORE_CLASSES>
inline int operator == (const STRING_CORE &lhs, const _CharT *rhs)
{
    return lhs.compare(rhs);
}

template <STRING_CORE_CLASSES>
inline int operator == (const _CharT *lhs, const STRING_CORE &rhs)
{
    return rhs.compare(lhs);
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::reserve(size_t minCapacity)
{
    size_t type = getType();
    if (type == kIsLarge) {
        //
    }
    else if (type == kIsMedium) {
        //
    }
    else if (type == kIsSmall) {
        //
    }
    else {
        //
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::large_reserveTo(size_t minCapacity)
{
    jimi_assert(getType() == kIsLarge);
    // Ensure unique
    if (refcount_type::refs(_ml.data) > 1) {
        // We must make it unique regardless; in-place reallocation is
        // useless if the string is shared. In order to not surprise
        // people, reserve the new block at current capacity or
        // more. That way, a string's capacity never shrinks after a
        // call to reserve.
        minCapacity = std::max(minCapacity, _ml.capacity);
        refcount_type *newRC = refcount_type::create(&minCapacity);
        char_type *newData = newRC->data();
        string_detail::pod_copy(newData, _ml.data, _ml.size + 1);
        // Done with the old data. No need to call writeNull(),
        // we have + 1 above.
        refcount_type::release(_ml.data);
        _ml.data = newData;
        _ml.capacity = minCapacity;
        _ml.type = STRING_TYPE_LARGE;
        // size remains unchanged
    }
    else {
        // String is not shared, so let's try to realloc (if needed)
        if (minCapacity > _ml.capacity) {
            // Asking for more memory
            refcount_type *newRC =
                refcount_type::reallocate(_ml.data, _ml.size, _ml.capacity, minCapacity);
            _ml.data = newRC->data();
            _ml.capacity = minCapacity;
            _ml.type = STRING_TYPE_LARGE;
            writeNull();
        }
        jimi_assert(capacity() >= minCapacity);
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::medium_reserveTo(size_t minCapacity)
{
    // String is not shared
    if (minCapacity <= _ml.capacity) {
        return; // nothing to do, there's enough room
    }
    if (minCapacity <= kMaxMediumSize) {
        // Keep the string at medium size. Don't forget to allocate
        // one extra Char for the terminating null.
        size_t capacityBytes = (minCapacity + 1) * sizeof(char_type);
        _ml.data = static_cast<char_type *>(
            smartRealloc(_ml.data, _ml.size * sizeof(char_type),
                (_ml.capacity + 1) * sizeof(char_type),
                capacityBytes));
        writeNull();
        _ml.capacity = (capacityBytes / sizeof(char_type) - 1);
        _ml.type = STRING_TYPE_MEDIUM;
    } else {
        // Conversion from medium to large string
        string_core nascent;
        // Will recurse to another branch of this function
        nascent.reserve(minCapacity);
        nascent._ml.size = _ml.size;
        string_detail::pod_copy(nascent._ml.data, _ml.data, _ml.size);
        nascent.swap(*this);
        writeNull();
        jimi_assert(capacity() >= minCapacity);
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::small_reserveTo(size_t minCapacity)
{
    jimi_assert(getType() == kIsSmall);
    if (minCapacity > kMaxMediumSize) {
        // large
        refcount_type *newRC = refcount_type::create(&minCapacity);
        size_type size = _small.info.size;
        jimi_assert(newRC != NULL);
        //string_detail::pod_copy(newRC->data(), _small.buf, size);
        _ml.data = newRC->data();
        _ml.size = size;
        _ml.capacity = minCapacity;
        _ml.type = STRING_TYPE_LARGE;
    }
    else if (minCapacity > kMaxSmallSize) {
        // medium
        // Don't forget to allocate one extra Char for the terminating null
        size_type allocSizeBytes = calc_capacity(minCapacity);
        char_type *data = static_cast<char_type *>(::malloc(allocSizeBytes));
        size_type size = _small.info.size;
        jimi_assert(data != NULL);
        //string_detail::pod_copy(data, _small.buf, size);
        _ml.data = data;
        _ml.size = size;
        _ml.capacity = (allocSizeBytes / sizeof(char_type) - 1);
        _ml.type = STRING_TYPE_MEDIUM;
    }
    else {
        // small
        // Nothing to do, everything stays put
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::reserveTo(size_t minCapacity)
{
    size_t type = getType();
    if (type == kIsLarge) {
        large_reserveTo(minCapacity);
    }
    else if (type == kIsMedium) {
        medium_reserveTo(minCapacity);
    }
    else if (type == kIsSmall) {
        small_reserveTo(minCapacity);
    }
    else {
        // small
        // Nothing to do, everything stays put
    }
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::expandTo(const size_t newSize)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    size_t type = getType();
    if (type == kIsLarge) {
        //size_type _capacity = _ml.capacity;
        if (newSize > _ml.capacity)
            large_reserveTo(newSize);
    }
    else if (type == kIsMedium) {
        //size_type _capacity = kMaxMediumSize - 1;
        if (newSize > kMaxMediumSize - 1)
            medium_reserveTo(newSize);
    }
    else if (type == kIsSmall) {
        //size_type _capacity = kMaxSmallSize - 1;
        if (newSize <= kMaxSmallSize - 1) {
            _small.info.size = newSize;
            //_small.buf[newSize] = STRING_NULL_CHAR;
            return;
        }
        small_reserveTo(newSize);
    }
    /* is constant string or unknown type */
    else {
        return;
    }

    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);

    _ml.size = newSize;
    //_ml.data[newSize] = STRING_NULL_CHAR;

    jimi_assert(size() == newSize);
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::shrinkTo(const size_t newSize)
{
    flag_type type = getType();
    if (type == kIsSmall) {
        // Check for underflow
        jimi_assert(newSize <= _small.info.size);
        _small.info.size = newSize;
    }
    else if (type == kIsMedium || refcount_type::refs(_ml.data) == 1) {
        // Medium strings and unique large strings need no special
        // handling.
        jimi_assert(newSize <= _ml.size);
        _ml.size = newSize;
    }
    else if (type == kIsLarge) {
        jimi_assert(newSize <= _ml.size);
        // Shared large string, must make unique. This is because of the
        // durn terminator must be written, which may trample the shared
        // data.
        if (newSize != _ml.size)
            string_core(_ml.data, newSize).swap(*this);
        // No need to write the terminator.
        return;
    }
    writeNull();
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::push_back(const char_type c)
{
    jimi_assert(capacity() >= size());
    size_t sz;
    flag_type type = getType();
    if (type == kIsSmall) {
        sz = _small.info.size;
        if (sz < kMaxSmallSize - 1) {
            _small.info.size = sz + 1;
            _small.buf[sz] = c;
            _small.buf[sz + 1] = '\0';
            return;
        }
        reserve(kMaxMediumSize);
    }
    else {
        sz = _ml.size;
        if (sz == capacity()) {         // always true for isShared()
            reserve(sz * 3 / 2 + 1);    // ensures not shared
        }
    }
    jimi_assert(!is_shared());
    jimi_assert(capacity() >= (sz + 1));
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.size = sz + 1;
    _ml.data[sz] = c;
    _ml.data[sz + 1] = '\0';
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::writeNull()
{
#if defined(JIMI_STRING_PERVERSE) || defined(JIMI_STRING_CONSERVATIVE)
    if (getType() == kIsSmall) {
        const size_type _size = _small.info.size;
        if (_size < kMaxSmallSize)
            _small.buf[_size] = STRING_NULL_CHAR;
    }
    else {
        _ml.data[_ml.size] = STRING_NULL_CHAR;
    }
#endif
}

NS_JIMI_END

#if _MSC_VER
#pragma warning(pop)                    // 恢复warning状态
#endif

#endif  /* _JIMI_LANG_STRING_CORE_H_ */
