
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
#include <jimi/lang/StringDetail.h>
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
/// Reference: 标准C＋＋类string的Copy-On-Write技术（一）
///
/// http://haoel.blog.51cto.com/313033/124638
///
/// Reference: C++ 工程实践(10)：再探std::string
///
/// http://www.cnblogs.com/Solstice/archive/2012/03/17/2403335.html
///
/// Reference: 漫步Facebook开源C++库folly(1)：string类的设计
///
/// http://www.cnblogs.com/promise6522/archive/2012/06/05/2535530.html
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

#undef STRING_CORE_CLASSES
#undef STRING_CORE

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
    static const flag_type  kIsSmall    =  STRING_TYPE_SMALL;
    static const flag_type  kIsMedium   =  STRING_TYPE_MEDIUM;
    static const flag_type  kIsLarge    =  STRING_TYPE_LARGE;
    static const flag_type  kIsConstant =  STRING_TYPE_CONSTANT;
    static const flag_type  kTypeMask   =  STRING_TYPE_MASK;
    static const flag_type  kRefCntMask = ~(flag_type)STRING_TYPE_MASK;

    static const flag_type  kIsMediumOrLarge =
        STRING_TYPE_MEDIUM | STRING_TYPE_LARGE;
    static const flag_type  kNotIsSmall      =
        STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT);

    static const size_type  kMaxMediumSizeBytes  =
        JIMI_ALIGNED_TO(STRING_MEDIUM_SIZE * sizeof(char_type), 64);

    static const size_type  kMaxSmallSizeBytes   =
        (STRING_SMALL_BYTES >= sizeof(core_data_t)) ? STRING_SMALL_BYTES : sizeof(core_data_t);

    static const size_type  kMaxSmallSize   =
        (kMaxSmallSizeBytes - sizeof(small_info_t)) / sizeof(char_type);

    static const size_type  kMaxMediumSize  =
        kMaxMediumSizeBytes / sizeof(char_type);

public:
    // Contructor
    string_core();
    string_core(const string_core &rhs);
    string_core(const size_t _capacity);
    string_core(const char_type * const src);
    string_core(const char_type * const src, const size_t size);
    string_core(const char_type c);
    string_core(const char_type c, size_type n);

    // Discontructor
    ~string_core();

public:
    // Append operators
    void append(const string_core &str);
    void append(const char_type c);
    void append(const char_type c, size_type n);
    void append(const char_type *s);
    void append(const char_type *s, size_type n);

    void append(const int n);
    void append(const unsigned int n);

    void destroy();

    void retain();
    void release();

    bool equals(const string_core &rhs) const;

    int compare(const string_core &rhs) const;
    int compare(const char_type *rhs) const;

    flag_type getType() const       { return (_ml.type & kTypeMask); }
    flag_type getTypeValue() const  { return _ml.type;               }
    flag_type getTypeMask() const   { return kTypeMask;              }

    flag_type getRefCount() const   { return (_ml.type & kRefCntMask); }
    flag_type getRefCntMask() const { return kRefCntMask;              }

    bool is_small() const       { return STRING_TYPE_IS_SMALL(_ml.type);    }
    bool is_medium() const      { return STRING_TYPE_IS_MEDIUM(_ml.type);   }
    bool is_large() const       { return STRING_TYPE_IS_LARGE(_ml.type);    }
    bool is_constant() const    { return STRING_TYPE_IS_CONSTANT(_ml.type); }

    bool is_medium_or_large() const { return STRING_TYPE_IS_MEDIUM_OR_LARGE(_ml.type); }

    bool not_is_small() const { return STRING_TYPE_NOT_IS_SMALL(_ml.type); }

    bool is_shared() const {
        return (is_large() && (refcount_type::refs(_ml.data) > 1));
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
    void reallocate(size_type newSize);
    void resize(size_type newSize);

    void reserveTo(size_type minCapacity);
    void reserveTo_from_small(size_type minCapacity);
    void reserveTo_from_medium(size_type minCapacity);
    void reserveTo_from_large(size_type minCapacity);

    void expandTo(const size_type newSize);
    void shrinkTo(const size_type newSize);

    char_type *expand_noinit(const size_type delta);
    char_type *expandTo_noinit(const size_type newSize);
    void expand_and_copy(const char_type *s, size_type n);

    void push_back(const char_type c);

    void writeNull();
    void writeNull_small();
    void writeNull_ml();
    void writeNullForce();

protected:
    size_t calc_capacity(const size_t minCapacity);

private:
    void small_clone(small_t &dest, const small_t &src) {
#if 1
        // 优化版本, 如果内存地址对齐4/8字节, 一次拷贝4/8字节
        traits_type::strncpy_align4_unsafe(dest.buf, src.buf, src.info.size);
        dest.info.lastShort = src.info.lastShort;
        dest.buf[src.info.size] = '\0';
#else
        // 慢速版本
        traits_type::strncpy_unsafe(dest.buf, src.buf, src.info.size);
        dest.info.lastShort = src.info.lastShort;
        dest.buf[src.info.size] = '\0';
#endif
    }

    void ml_clone(medium_large &dest, const medium_large &src) {
        dest.data = src.data;
        dest.size = src.size;
        dest.capacity = src.capacity;
        dest.type = src.type;
    }

    void small_swap(small_t &dest, small_t &src) {
        small_t t;
        small_clone(t, dest);
        small_clone(dest, src);
        small_clone(src, t);
    }

    void ml_swap(medium_large &dest, medium_large &src) {
        medium_large t;
        ml_clone(t, dest);
        ml_clone(dest, src);
        ml_clone(src, t);
    }

protected:
    /* 使用lastShort有时候可以快一点 */
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
                char dummy[(kMaxSmallSizeBytes - sizeof(small_info_t)) / sizeof(char)];
                /* size and type */
                small_info_t  info;
            };
            char_type buf[(kMaxSmallSizeBytes - sizeof(small_info_t)) / sizeof(char_type)];
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
                char dummy[(kMaxSmallSizeBytes - sizeof(core_data_t)) / sizeof(char)];

                /* 后面的定义必须和core_data的结构一致 */
                char_type *data;
                size_type  size;
                size_type  capacity;
                flag_type  type;
            };

            char_type buf[(kMaxSmallSizeBytes - sizeof(core_data_t)) / sizeof(char_type)];
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
    (*(size_t *)(&_small.buf[0])) = (size_t)0;
    _ml.type = STRING_TYPE_SMALL;
    //_ml.size = 0;
    //_ml.capacity = 0;
#else
    _ml.data = NULL;
    _ml.size = 0;
    _ml.capacity = 0;
    _ml.type = 0;

    // init sso buffer
    (*(uint32_t *)(&_ml.buf[0])) = (uint32_t)0;
#endif
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const string_core &rhs)
{
    jimi_assert(&rhs != this);
    flag_type rhs_type = rhs.getType();
    /* small string optimized */
    if (rhs_type == kIsSmall) {
#if 0
        _small.info.type = rhs._small.info.type;
        _small.info.size = rhs._small.info.size;
#else
        _small.info.lastShort = rhs._small.info.lastShort;
#endif
        jimi_assert(rhs._small.info.size < kMaxSmallSize);
        if (_small.info.size < kMaxSmallSize) {
            //traits_type::strncpy(&_small.buf[0], kMaxSmallSize, &rhs._small.buf[0], _small.info.size);
            string_detail::pod_copy(&_small.buf[0], &rhs._small.buf[0], _small.info.size + 1);
            // No need for writeNull() here, we copied one extra element just above.
        }
        else {
#ifdef _DEBUG
            sLog.error("string_core(const string_core &src): data = %s, type = 0x%04X, size = %d.",
                &_small.buf[0], _small.info.type, _small.info.size);
#endif
        }
        jimi_assert(getType() == kIsSmall && this->size() == rhs.size());
    }
    /* eager copy */
    else if (rhs_type == kIsMedium) {
        // Medium strings are copied eagerly. Don't forget to allocate
        // one extra Char for the null terminator.
        jimi_assert(rhs._ml.capacity == kMaxMediumSize - 1);
        char_type *newData = traits_type::mem_alloc(kMaxMediumSize);
        //traits_type::strncpy(newData, kMaxMediumSize, rhs._ml.data, rhs._ml.size);
        string_detail::pod_copy(newData, rhs._ml.data, rhs._ml.size + 1);
        // No need for writeNull() here, we copied one extra element just above.
        _ml.data     = newData;
        _ml.size     = rhs._ml.size;
        _ml.capacity = kMaxMediumSize - 1;
        _ml.type     = STRING_TYPE_MEDIUM;

        jimi_assert(getType() == kIsMedium);
    }
    /* copy-on-write */
    else if (rhs_type == kIsLarge) {
        // Large strings are just refcounted
        _ml.data     = rhs._ml.data;
        _ml.size     = rhs._ml.size;
        _ml.capacity = rhs._ml.capacity;
        _ml.type     = STRING_TYPE_LARGE;

        refcount_type::retain(_ml.data);
        jimi_assert(getType() == kIsLarge && size() == rhs.size());
    }
    /* constant string data */
    else if (rhs_type == kIsConstant) {
        // It's a constant string data
        _ml.data     = rhs._ml.data;
        _ml.size     = rhs._ml.size;
        _ml.capacity = rhs._ml.capacity;
        _ml.type     = STRING_TYPE_CONSTANT;
    }
    /* unknown type */
    else {
        sLog.error("string_core(const string_core &src): type = 0x%04X, data = %08X, size() = %d.", getType(), _ml.data, _ml.size);
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const size_t _capacity)
{
    if (_capacity < kMaxSmallSize) {
        // Small: small string optimized
        (*(size_t *)(&_small.buf[0])) = (size_t)0;
        _ml.type = STRING_TYPE_SMALL;
    }
    else if (_capacity < kMaxMediumSize) {
        // Medium: eager copy
        size_type capacitySize = calc_capacity(_capacity);
        char_type *newData = traits_type::mem_alloc(capacitySize);
        newData[0] = '\0';
        _ml.data = newData;
        _ml.size = 0;
        _ml.capacity = _capacity;
        _ml.type = STRING_TYPE_MEDIUM;
    }
    else {
        // Large: copy-on-write
        size_type effectiveCapacity = calc_capacity(_capacity);
        refcount_type *newRC = refcount_type::create(&effectiveCapacity);
        char_type *newData = newRC->data();
        newData[0] = '\0';
        _ml.data = newData;
        _ml.size = 0;
        _ml.capacity = _capacity;
        _ml.type = STRING_TYPE_LARGE;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type c)
{
    _small.buf[0] = c;
    _small.buf[1] = '\0';
    _small.info.size = 1;
    _small.info.type = STRING_TYPE_SMALL_X;
    //_ml.type = STRING_TYPE_SMALL;
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type c, size_type n)
{
    if (_capacity < kMaxSmallSize) {
        // Small: small string optimized
        ::memset((void *)&_small.buf[0], c, n * sizeof(char_type));
        _small.buf[n] = '\0';
        _small.info.size = n;
        _small.info.type = STRING_TYPE_SMALL_X;
    }
    else if (_capacity < kMaxMediumSize) {
        // Medium: eager copy
        size_type capacitySize = calc_capacity(_capacity);
        char_type *newData = traits_type::mem_alloc(capacitySize);
        ::memset((void *)newData, c, n * sizeof(char_type));
        newData[n] = '\0';
        _ml.data = newData;
        _ml.size = 0;
        _ml.capacity = _capacity;
        _ml.type = STRING_TYPE_MEDIUM;
    }
    else {
        // Large: copy-on-write
        size_type effectiveCapacity = calc_capacity(_capacity);
        refcount_type *newRC = refcount_type::create(&effectiveCapacity);
        char_type *newData = newRC->data();
        ::memset((void *)newData, c, n * sizeof(char_type));
        newData[n] = '\0';
        _ml.data = newData;
        _ml.size = 0;
        _ml.capacity = _capacity;
        _ml.type = STRING_TYPE_LARGE;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type * const src)
: string_core(src, traits_type::length(src))
//: string_core(src, ::jm_strlen(src))
{
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
#elif 0
        traits_type::strncpy_unsafe(_small.buf, src, src_len);
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
void STRING_CORE::destroy()
{
    flag_type type = getType();
    if (type == kIsMedium) {
        if (_ml.data != NULL) {
            ::free(_ml.data);
            _ml.data = NULL;
        }
    }
    else if (type == kIsLarge) {
        if (_ml.data != NULL) {
            refcount_type::release(_ml.data);
            _ml.data = NULL;
        }
    }
}

template <STRING_CORE_CLASSES>
void STRING_CORE::retain()
{
    flag_type type = getType();
    if (type == kIsLarge)
        refcount_type::retain(_ml.data);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::release()
{
    flag_type type = getType();
    if (type == kIsMedium) {
        if (_ml.data != NULL) {
            ::free(_ml.data);
            _ml.data = NULL;
        }
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
    }
    else if (type == kIsLarge) {
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
STRING_CORE & STRING_CORE::operator = (const string_core &rhs) {
    flag_type type = rhs.getType();
    if (type == kIsSmall)
        small_clone(_small, rhs._small);
    else {
        ml_clone(_ml, rhs._ml);
        if (type == kIsLarge)
            refcount_type::retain(_ml.data);
    }
    return *this;
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const string_core &str)
{
#if defined(_DEBUG) || !defined(NDEBUG)
    size_type nDesiredSize = size() + str.size();
#endif
    append(str.data(), str.size());
    jimi_assert(size() == nDesiredSize);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const char_type c)
{
    push_back(c);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const char_type c, size_type n)
{
    resize(size() + n, c);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const char_type *s)
{
    append(s, traits_type::length(s));
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const char_type *s, size_type n)
{
    if (JIMI_UNLIKELY(!n)) {
        // Unlikely but must be done
        return;
    }

    size_type oldSize = size();
    const char_type *oldData = data();
    // Check for aliasing (rare). We could use "<=" here but in theory
    // those do not work for pointers unless the pointers point to
    // elements in the same array. For that reason we use
    // std::less_equal, which is guaranteed to offer a total order
    // over pointers. See discussion at http://goo.gl/Cy2ya for more
    // info.
    std::less_equal<const char_type *> le;
    if (JIMI_UNLIKELY(le(oldData, s) && !le(oldData + oldSize, s))) {
        jimi_assert(le(s + n, oldData + oldSize));
        const size_type last = s - oldData;
        reserve(oldSize + n);
        // Restore the source
        s = data() + last;
    }
#if 0
    // Warning! Repeated appends with short strings may actually incur
    // practically quadratic performance. Avoid that by pushing back
    // the first character (which ensures exponential growth) and then
    // appending the rest normally. Worst case the append may incur a
    // second allocation but that will be rare.
    push_back(*s++);
    expandTo(oldSize + n);
    --n;
    if (n > 0) {
        ::memcpy((void *)(data() + oldSize + 1), s, n * sizeof(char_type));
    }
    writeNullForce();
#else
    expand_and_copy(s, n);
#endif
    jimi_assert(size() == oldSize + n);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::push_back(const char_type c)
{
    jimi_assert(capacity() >= size());
    size_t _size;
    flag_type type = getType();
    if (type == kIsSmall) {
        _size = _small.info.size;
        if (_size < kMaxSmallSize - 1) {
            _small.info.size = _size + 1;
            _small.buf[_size] = c;
            _small.buf[_size + 1] = '\0';
            return;
        }
        reserve(kMaxMediumSize - 1);
    }
    else {
        _size = _ml.size;
        if (_size == capacity()) {          // always true for isShared()
            //reserve(_size * 3 / 2 + 1);     // ensures not shared
            reserve(_size * 2 + 2);         // ensures not shared
        }
    }
    jimi_assert(!is_shared());
    jimi_assert(capacity() >= (_size + 1));
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.size = _size + 1;
    _ml.data[_size] = c;
    _ml.data[_size + 1] = '\0';
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const int n)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    const int delta = 11;
    int len;
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
            //_small.info.size = newSize;
#if 0
            len = traits_type::itoa_radix10_fast(_small.buf + oldSize, n, delta);
#elif 1
            len = traits_type::itoa_radix10(_small.buf + oldSize, n);
            //len = traits_type::itoa(_small.buf + oldSize, n, 10);
#else
            _itoa(n, _small.buf + oldSize, 10);
            len = jm_strlen(_small.buf + oldSize);
#endif
            _small.info.size = oldSize + len;
            jimi_assert(size() == oldSize + len);
            return;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.size;
        newSize = oldSize + delta;
        if (newSize > capacity())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    //_ml.size = newSize;
#if 0
    len = traits_type::itoa_radix10_fast(_ml.data + oldSize, n, delta);
#elif 1
    len = traits_type::itoa_radix10(_ml.data + oldSize, n);
    //len = traits_type::itoa(_ml.data + oldSize, n, 10);
#else
    _itoa(n, _ml.data + oldSize, 10);
    len = jm_strlen(_ml.data + oldSize);
#endif
    _ml.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const unsigned int n)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    const int delta = 11;
    int len;
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
            //_small.info.size = newSize;
#if 0
            len = jm_uitoa_radix10_fast(_small.buf + oldSize, n, delta);
#elif 1
            len = jm_utoa_radix10(_small.buf + oldSize, n);
            //len = jm_utoa(_small.buf + oldSize, n, 10);
#else
            _itoa(n, _small.buf + oldSize, 10);
            len = jm_strlen(_small.buf + oldSize);
#endif
            _small.info.size = oldSize + len;
            jimi_assert(size() == oldSize + len);
            return;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.size;
        newSize = oldSize + delta;
        if (newSize > capacity())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    //_ml.size = newSize;
#if 0
    len = jm_utoa_radix10_fast(_ml.data + oldSize, n, delta);
#elif 1
    len = jm_utoa_radix10(_ml.data + oldSize, n);
    //len = jm_utoa(_ml.data + oldSize, n, 10);
#else
    _itoa(n, _ml.data + oldSize, 10);
    len = jm_strlen(_ml.data + oldSize);
#endif
    _ml.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

// swap below doesn't test whether &rhs == this (and instead
// potentially does extra work) on the premise that the rarity of
// that situation actually makes the check more expensive than is
// worth.

template <STRING_CORE_CLASSES>
void STRING_CORE::swap(STRING_CORE &rhs)
{
#if 1
    // 在不同的type下, _ml的有些数据是不必复制的
    flag_type type = rhs.getType();
    if (type == kIsSmall)
        small_swap(_small, rhs._small);
    else
        ml_swap(_ml, rhs._ml);
#else
    // 完全直接复制_ml, 有些复制可能是多余的
    const medium_large t = _ml;
    _ml = rhs._ml;
    rhs._ml = t;
#endif
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::size() const
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
const typename STRING_CORE::char_type *STRING_CORE::c_str() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return &_small.buf[0];
    else
        return _ml.data;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::char_type *STRING_CORE::mutable_data()
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
size_t STRING_CORE::calc_capacity(const size_t minCapacity)
{
    return (minCapacity + 1);
}

template <STRING_CORE_CLASSES>
bool STRING_CORE::equals(const STRING_CORE &rhs) const
{
    return (&rhs == this);
}

template <STRING_CORE_CLASSES>
int STRING_CORE::compare(const STRING_CORE &rhs) const
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
#ifndef _DEBUG
                // Unknown string type
                sLog.error("string_core::compare(const string_core &rhs) - Error: Unknown string type.");
#endif
            }
        }
        else {
            //
        }
    }
    else {
#ifndef _DEBUG
        // Is not same size
        sLog.info("string_core::compare(const string_core &rhs), size() = %d, rhs.size() = %d", size(), rhs.size());
#endif
    }
    return equal;
}

template <STRING_CORE_CLASSES>
int STRING_CORE::compare(const char_type *rhs) const
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
#ifdef _DEBUG
        // Unknown string type
        sLog.error("STRING_CORE::compare(const char_type *rhs) - Error: Unknown string type.");
#endif
    }
    return equal;
}

template <STRING_CORE_CLASSES>
int operator == (const STRING_CORE &lhs, const STRING_CORE &rhs)
{
    return lhs.compare(rhs);
}

template <STRING_CORE_CLASSES>
int operator == (const STRING_CORE &lhs, const _CharT *rhs)
{
    return lhs.compare(rhs);
}

template <STRING_CORE_CLASSES>
int operator == (const _CharT *lhs, const STRING_CORE &rhs)
{
    return rhs.compare(lhs);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::reserveTo_from_small(size_t minCapacity)
{
    jimi_assert(getType() == kIsSmall);
    if (minCapacity >= kMaxMediumSize) {
        // large
        refcount_type *newRC = refcount_type::create(&minCapacity);
        size_type size = _small.info.size;
        jimi_assert(newRC != NULL);
        string_detail::pod_copy(newRC->data(), _small.buf, size);
        _ml.data = newRC->data();
        _ml.size = size;
        _ml.capacity = minCapacity;
        _ml.type = STRING_TYPE_LARGE;
    }
    else if (minCapacity >= kMaxSmallSize) {
        // medium
        // Don't forget to allocate one extra Char for the terminating null
        size_type allocSizeBytes = calc_capacity(minCapacity);
        char_type *data = static_cast<char_type *>(::malloc(allocSizeBytes));
        size_type size = _small.info.size;
        jimi_assert(data != NULL);
        string_detail::pod_copy(data, _small.buf, size);
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
void STRING_CORE::reserveTo_from_medium(size_t minCapacity)
{
    // String is not shared
    if (minCapacity <= _ml.capacity) {
        return; // nothing to do, there's enough room
    }
    if (minCapacity < kMaxMediumSize) {
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
void STRING_CORE::reserveTo_from_large(size_t minCapacity)
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
void STRING_CORE::expandTo(const size_t newSize)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    size_t type = getType();
    if (type == kIsLarge) {
        //size_type _capacity = _ml.capacity;
        if (newSize > _ml.capacity)
            reserveTo_from_large(newSize);
    }
    else if (type == kIsMedium) {
        //size_type _capacity = kMaxMediumSize - 1;
        if (newSize >= kMaxMediumSize)
            reserveTo_from_medium(newSize);
    }
    else if (type == kIsSmall) {
        //size_type _capacity = kMaxSmallSize - 1;
        if (newSize < kMaxSmallSize) {
            _small.info.size = newSize;
            //_small.buf[newSize] = STRING_NULL_CHAR;
            return;
        }
        reserveTo_from_small(newSize);
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
typename STRING_CORE::char_type *STRING_CORE::expand_noinit(const size_type delta)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
            _small.info.size = newSize;
            writeNull_small();
            return _small.buf + oldSize;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.size;
        newSize = oldSize + delta;
        if (newSize > capacity())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.size = newSize;
    writeNull_ml();
    jimi_assert(size() == newSize);
    return _ml.data + oldSize;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::char_type *STRING_CORE::expandTo_noinit(const size_type newSize)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    size_type oldSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        if (newSize < kMaxSmallSize) {
            _small.info.size = newSize;
            writeNull_small();
            return _small.buf + oldSize;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.size;
        if (newSize > capacity())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.size = newSize;
    writeNull_ml();
    jimi_assert(size() == newSize);
    return _ml.data + oldSize;
}

template <STRING_CORE_CLASSES>
void STRING_CORE::expand_and_copy(const char_type *s, size_type n)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + n;
        if (newSize < kMaxSmallSize) {
            ::memcpy((void *)(_small.buf + oldSize), s, n * sizeof(char_type));
            _small.info.size = newSize;
            writeNull_small();
            return;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.size;
        newSize = oldSize + n;
        if (newSize > capacity())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    ::memcpy((void *)(_ml.data + oldSize), s, n * sizeof(char_type));
    _ml.size = newSize;
    writeNull_ml();
    jimi_assert(size() == newSize);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::shrinkTo(const size_t newSize)
{
    flag_type type = getType();
    if (type == kIsLarge) {
        jimi_assert(newSize <= _ml.size);
        if (refcount_type::refs(_ml.data) == 1) {
            // Medium strings and unique large strings need no special handling.
            jimi_assert(newSize <= _ml.size);
            _ml.size = newSize;
            writeNull_ml();
        }
        else {
            // Shared large string, must make unique. This is because of the
            // durn terminator must be written, which may trample the shared
            // data.
            if (newSize != _ml.size)
                string_core(_ml.data, newSize).swap(*this);
            // No need to write the terminator.
            return;
        }
    }
    else if (type == kIsMedium) {
        // Medium strings and unique large strings need no special handling.
        jimi_assert(newSize <= _ml.size);
        _ml.size = newSize;
        writeNull_ml();
    }
    else if (type == kIsSmall) {
        // Check for underflow
        jimi_assert(newSize <= _small.info.size);
        _small.info.size = newSize;
        writeNull_small();
    }
}

template <STRING_CORE_CLASSES>
void STRING_CORE::reserveTo(size_t newCapacity)
{
    size_t type = getType();
    if (type == kIsLarge) {
        reserveTo_from_large(newCapacity);
    }
    else if (type == kIsMedium) {
        reserveTo_from_medium(newCapacity);
    }
    else if (type == kIsSmall) {
        reserveTo_from_small(newCapacity);
    }
    else {
        // small
        // Nothing to do, everything stays put
    }
}

template <STRING_CORE_CLASSES>
void STRING_CORE::reserve(size_t newCapacity)
{
    flag_type type = getType();
    if (type == kIsLarge) {
        // Ensure unique
        if (refcount_type::refs(_ml.data) > 1) {
            // We must make it unique regardless; in-place reallocation is
            // useless if the string is shared. In order to not surprise
            // people, reserve the new block at current capacity or
            // more. That way, a string's capacity never shrinks after a
            // call to reserve.
            newCapacity = jimi_max(newCapacity, _ml.capacity);
            refcount_type * newRC = refcount_type::create(&newCapacity);
            string_detail::pod_copy(newRC->data(), _ml.data, _ml.size + 1);
            // Done with the old data. No need to call writeTerminator(),
            // we have + 1 above.
            refcount_type::release(_ml.data);
            _ml.data = newRC->data();
            _ml.capacity = newCapacity;
            //_ml.type = kIsLarge;
            // size and type remains unchanged
        }
        else {
            // String is not shared, so let's try to realloc (if needed)
            if (newCapacity > _ml.capacity) {
                // Asking for more memory
                refcount_type * newRC =
                    refcount_type::reallocate(_ml.data, _ml.size, _ml.capacity, newCapacity);
                _ml.data = newRC->data();
                _ml.capacity = newCapacity;
                writeNull_ml();
            }
            jimi_assert(capacity() >= newCapacity);
        }
    }
    else if (type == kIsMedium) {
        // String is not shared
        if (newCapacity <= _ml.capacity)
            return;  // nothing to do, there's enough room

        if (newCapacity < kMaxMediumSize) {
            // Keep the string at medium size. Don't forget to allocate
            // one extra Char for the terminating null.
            size_t capacityBytes = calc_capacity((newCapacity + 1)) * sizeof(char_type);
            _ml.data = static_cast<char_type *>(
                smartRealloc(
                    _ml.data,
                    _ml.size * sizeof(char_type),
                    (_ml.capacity + 1) * sizeof(char_type),
                    capacityBytes));
            writeNull_ml();
            _ml.capacity = newCapacity;
            _ml.type = STRING_TYPE_MEDIUM;
        }
        else {
            // Conversion from medium to large string
            string_core nascent;
            // Will recurse to another branch of this function
            nascent.reserve(newCapacity);
            nascent._ml.size = _ml.size;
            string_detail::pod_copy(nascent._ml.data, _ml.data, _ml.size);
            nascent.swap(*this);
            writeNull_ml();
            jimi_assert(capacity() >= newCapacity);
        }
    }
    else if (type == kIsSmall) {
        jimi_assert(getType() == kIsSmall);
        if (newCapacity >= kMaxMediumSize) {
            // large
            refcount_type * newRC = refcount_type::create(&newCapacity);
            size_type size = _small.info.size;
            string_detail::pod_copy(newRC->data(), _small.buf, size + 1);
            // No need for writeTerminator(), we wrote it above with + 1.
            _ml.data = newRC->data();
            _ml.size = size;
            _ml.capacity = newCapacity;
            _ml.type = STRING_TYPE_LARGE;
            jimi_assert(capacity() >= newCapacity);
        }
        else if (newCapacity >= kMaxSmallSize) {
            // medium
            // Don't forget to allocate one extra Char for the terminating null
            size_type allocSizeBytes =
                calc_capacity(newCapacity + 1) * sizeof(char_type);
            char_type *data = static_cast<char_type *>(::malloc(allocSizeBytes));
            size_type size = _small.info.size;
            string_detail::pod_copy(data, _small.buf, size + 1);
            // No need for writeTerminator(), we wrote it above with + 1.
            _ml.data = data;
            _ml.size = size;
            _ml.capacity = newCapacity;
            _ml.type = STRING_TYPE_MEDIUM;
        }
        else {
            // small
            // Nothing to do, everything stays put
        }
    }
    else {
        // Other type
    }
    jimi_assert(capacity() >= newCapacity);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::reallocate(size_t newSize)
{
    jimi_assert(false);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::resize(size_t newSize)
{
    jimi_assert(false);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::writeNull()
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

template <STRING_CORE_CLASSES>
void STRING_CORE::writeNull_small()
{
    jimi_assert(getType() == kIsSmall);
    const size_type size = _small.info.size;
    jimi_assert(size < kMaxSmallSize);
    //if (size < kMaxSmallSize)
        _small.buf[size] = STRING_NULL_CHAR;
}

template <STRING_CORE_CLASSES>
void STRING_CORE::writeNull_ml()
{
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.data[_ml.size] = STRING_NULL_CHAR;
}

template <STRING_CORE_CLASSES>
void STRING_CORE::writeNullForce()
{
    if (getType() == kIsSmall) {
        const size_type size = _small.info.size;
        jimi_assert(size < kMaxSmallSize);
        //if (size < kMaxSmallSize)
            _small.buf[size] = STRING_NULL_CHAR;
    }
    else {
        _ml.data[_ml.size] = STRING_NULL_CHAR;
    }
}

#undef STRING_CORE_CLASSES
#undef STRING_CORE

NS_JIMI_END

#if _MSC_VER
#pragma warning(pop)                    // 恢复warning状态
#endif

#endif  /* _JIMI_LANG_STRING_CORE_H_ */
