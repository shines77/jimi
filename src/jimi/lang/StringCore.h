
#ifndef _JIMI_LANG_STRING_CORE_H_
#define _JIMI_LANG_STRING_CORE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/thread/Atomic.h"
#include "jimi/lang/CharTraits.h"
#include "jimi/lang/RefCounted.h"
#include "jimi/lang/StringDetail.h"
#include "jimi/util/DumpUtils.h"
#include "jimic/string/jm_strings.h"
#include "jimic/string/jmf_strings.h"
#include "jimic/util/utils.h"

#include "jimi/log/log.h"

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(push)

#pragma warning(disable : 4995)         // 禁止不带_s函数的废弃warning信息
#pragma warning(disable : 4996)
#endif  /* _MSC_VER */

#include <string>
#include <cstdlib>

//#define JIMI_STRING_PERVERSE
//#define JIMI_STRING_CONSERVATIVE

NS_JIMI_BEGIN

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

/* Small Object Type的大小(单位是字节), 最小不得小于16字节. */
#define STRING_SMALL_BYTES      32

/**
  Medium Type的Size大小范围(单位是Word, 一个Word大小为sizeof(CharT_)),
  会自动对齐到64字节大小, 例如你设置为257, 会对齐到320 Words.
 */
#define STRING_MEDIUM_SIZE      256

/* String Null Terminator, 字符串结尾字符. */
#define STRING_NULL_CHAR        '\0'

enum StringTypeMask
{
    STRING_TYPE_SMALL       = (sizeof(size_t) == 4) ? 0x01000000 : 0x0100000000000000,
    STRING_TYPE_MEDIUM      = (sizeof(size_t) == 4) ? 0x02000000 : 0x0200000000000000,
    STRING_TYPE_LARGE       = (sizeof(size_t) == 4) ? 0x04000000 : 0x0400000000000000,
    STRING_TYPE_CONSTANT    = (sizeof(size_t) == 4) ? 0x08000000 : 0x0800000000000000,
    STRING_TYPE_MASK        = (STRING_TYPE_SMALL
        | STRING_TYPE_MEDIUM | STRING_TYPE_LARGE | STRING_TYPE_CONSTANT),
};

enum StringTypeMaskX
{
    STRING_TYPE_SMALL_X     = 0x01,
    STRING_TYPE_MEDIUM_X    = 0x02,
    STRING_TYPE_LARGE_X     = 0x04,
    STRING_TYPE_CONSTANT_X  = 0x08,
    STRING_TYPE_MASK_X      = (STRING_TYPE_SMALL_X
        | STRING_TYPE_MEDIUM_X | STRING_TYPE_LARGE_X | STRING_TYPE_CONSTANT_X),
};

#define STRING_TYPE_IS(type, mask)      ((type & mask) != 0)

#define STRING_TYPE_IS_SMALL(type)      ((type & STRING_TYPE_SMALL)     != 0)
#define STRING_TYPE_IS_MEDIUM(type)     ((type & STRING_TYPE_MEDIUM)    != 0)
#define STRING_TYPE_IS_LARGE(type)      ((type & STRING_TYPE_LARGE)     != 0)
#define STRING_TYPE_IS_CONSTANT(type)   ((type & STRING_TYPE_CONSTANT)  != 0)

#define STRING_TYPE_NOT_IS_SMALL(type)   \
    ((type & (STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT))) != 0)

#define STRING_TYPE_IS_MEDIUM_OR_LARGE(type)   \
    ((type & (STRING_TYPE_MEDIUM | STRING_TYPE_LARGE)) != 0)

////////////////////////////////////////////////////////////////////////////////

#undef STRING_CORE_CLASSES
#undef STRING_CORE

#define STRING_CORE_CLASSES    \
    class _CharT, class _RefCount

#define STRING_CORE            \
    string_core<_CharT, _RefCount>

template <class _CharT, class _RefCount = refcounted<_CharT, size_t> >
class JIMI_DLL string_core
{
public:
    // Types
    typedef _CharT                  char_type;
    typedef _RefCount               refcount_type;
    typedef size_t                  size_type;
    typedef size_t                  flag_type;
    typedef char_traits<char_type>  traits_type;

protected:
    struct small_info_t;
    struct small_t;
    struct core_data_t;
    struct medium_large_t;

    typedef struct small_info_t small_info_t;
    typedef struct small_t      small_t;
    typedef struct core_data_t  core_data_t;
    typedef struct medium_large_t medium_large_t;

public:

    // Constant
    static const char_type kNullChar    =  STRING_NULL_CHAR;

    static const flag_type kIsSmall     =  STRING_TYPE_SMALL;
    static const flag_type kIsMedium    =  STRING_TYPE_MEDIUM;
    static const flag_type kIsLarge     =  STRING_TYPE_LARGE;
    static const flag_type kIsConstant  =  STRING_TYPE_CONSTANT;
    static const flag_type kTypeMask    =  STRING_TYPE_MASK;
    static const flag_type kRefCntMask  = ~(flag_type)STRING_TYPE_MASK;

    static const flag_type kIsSmallX    =  STRING_TYPE_SMALL_X;

    static const flag_type kIsMediumOrLarge =
        STRING_TYPE_MEDIUM | STRING_TYPE_LARGE;
    static const flag_type kNotIsSmall =
        STRING_TYPE_MASK & (~STRING_TYPE_SMALL) & (~STRING_TYPE_CONSTANT);

    static const size_type kMaxMediumSizeBytes =
        JIMI_ALIGNED_TO(STRING_MEDIUM_SIZE * sizeof(char_type), 64);

    static const size_type kMaxSmallSizeBytes =
        (STRING_SMALL_BYTES >= sizeof(core_data_t)) ? STRING_SMALL_BYTES : sizeof(core_data_t);

    static const size_type kMaxSmallSize =
        (kMaxSmallSizeBytes - sizeof(small_info_t)) / sizeof(char_type);

    static const size_type kMaxMediumSize =
        kMaxMediumSizeBytes / sizeof(char_type);

    /* small_t.info.size 相对于 medium_large_t.core.type 的偏移值(单位为字节) */
    static const size_type kSmallSizeOffset = (size_type)jmc_abs_ex((intptr_t)(
                                    (unsigned char *)&(((small_t *)0)->info.size)
                                  - (unsigned char *)&(((medium_large_t *)0)->core.type)), size_type);

     /* small_t.info.type 相对于 medium_large_t.core.type 的偏移值(单位为字节) */
    static const size_type kSmallTypeOffset = (size_type)jmc_abs_ex((intptr_t)(
                                    (unsigned char *)&(((small_t *)0)->info.type)
                                  - (unsigned char *)&(((medium_large_t *)0)->core.type)), size_type);

    /* small_t.info.size 相对于 medium_large_t.core.type 的偏移值(单位为bit) */
    static const size_type kSmallSizeOffsetBits = kSmallSizeOffset * sizeof(unsigned char) * 8;

     /* small_t.info.type 相对于 medium_large_t.core.type 的偏移值(单位为bit) */
    static const size_type kSmallTypeOffsetBits = kSmallTypeOffset * sizeof(unsigned char) * 8;

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

    void dump();

public:
    // Append operators
    void append(const string_core &str);
    void append(const char_type c);
    void append(const char_type c, size_type n);
    void append(const char_type *s);
    void append(const char_type *s, size_type n);

    void append(const int n);
    void append(const unsigned int u);
    void append(const long l);
    void append(const unsigned long ul);
    void append(const int64_t i64);   
    void append(const uint64_t u64);
    void append(const float f);
    void append(const double d);

    void append_hex(const uint32_t hex32, const bool isUpper = true);
    void append_hex(const uint64_t hex64, const bool isUpper = true);

    void destroy();

    void retain();
    void release();

    bool equals(const string_core &rhs) const;

    int compare(const string_core &rhs) const;
    int compare(const char_type *rhs) const;

    flag_type getType() const       { return (_ml.core.type & kTypeMask); }
    flag_type getTypeValue() const  { return _ml.core.type;               }
    flag_type getTypeMask() const   { return kTypeMask;              }

    flag_type getRefCount() const   { return (_ml.core.type & kRefCntMask); }
    flag_type getRefCntMask() const { return kRefCntMask;              }

    bool is_small() const       { return STRING_TYPE_IS_SMALL(_ml.core.type);    }
    bool is_medium() const      { return STRING_TYPE_IS_MEDIUM(_ml.core.type);   }
    bool is_large() const       { return STRING_TYPE_IS_LARGE(_ml.core.type);    }
    bool is_constant() const    { return STRING_TYPE_IS_CONSTANT(_ml.core.type); }

    bool is_medium_or_large() const { return STRING_TYPE_IS_MEDIUM_OR_LARGE(_ml.core.type); }

    bool not_is_small() const { return STRING_TYPE_NOT_IS_SMALL(_ml.core.type); }

    bool is_shared() const {
        return (is_large() && (refcount_type::refs(_ml.core.data) > 1));
    }

    const char_type *data() const { return c_str(); }
    const char_type *data_small() const { return c_str_small(); }
    const char_type *data_ml() const { return c_str_ml(); }

    const char_type *c_str() const;
    const char_type *c_str_small() const;
    const char_type *c_str_ml() const;

    char_type *mutable_data();

    size_type size() const;
    size_type size_small() const;
    size_type size_ml() const;

    size_type capacity() const;
    const size_type capacity_small() const;
    size_type capacity_ml() const;

    // Swap below doesn't test whether &rhs == this (and instead
    // potentially does extra work) on the premise that the rarity of
    // that situation actually makes the check more expensive than is
    // worth.
    void swap(string_core &rhs);

    // Stole from rhs, and clear rhs
    void stole(string_core &rhs);

#if defined(JIMI_HAS_CXX11_MOVE_FUNCTIONS) && (JIMI_HAS_CXX11_MOVE_FUNCTIONS != 0)
    string_core(string_core && goner) {
        //if (goner.getType() == kIsSmall) {
        if (goner.is_small()) {
            // Just copy, leave the goner in peace
            new (this) string_core(goner._small.buf, goner._small.info.size);
        }
        else {
#if 1
            // Take goner's guts
            _ml.core = goner._ml.core;

            // 把 goner 清零为 kIsSmall 类型
            *(size_t *)(&goner._small.buf[0]) = (size_t)0;
            goner._ml.core.type = kIsSmall;
#else
            // Take goner's guts
            _ml = goner._ml;

            // Clean goner's carcass
            goner._ml.core.data = NULL;
            goner._ml.core.size = 0;
            goner._ml.core.capacity = 0;
#endif
        }
    }
#endif

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
        traits_type::strncpy_align4_unsafe(dest.buf, src.buf, src.info.size + 1);
        //dest.info.lastShort = src.info.lastShort;
        dest.info.size = src.info.size;
        dest.info.type = src.info.type;
        //dest.buf[src.info.size] = '\0';
#else
        // 慢速版本
        traits_type::strncpy_unsafe(dest.buf, src.buf, src.info.size);
        dest.info.lastShort = src.info.lastShort;
        dest.buf[src.info.size] = '\0';
#endif
    }

    void ml_core_clone(core_data_t &dest, const core_data_t &src) {
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

    void ml_swap(medium_large_t &dest, medium_large_t &src) {
        core_data_t t;
        ml_core_clone(t, dest.core);
        ml_core_clone(dest.core, src.core);
        ml_core_clone(src.core, t);
    }

    void small_stole(small_t &dest, small_t &src) {
        // 复制 src 到 dest
        small_clone(dest, src);
        // 把src清零为 kIsSmall 类型
        *(size_t *)(&src._small.buf[0]) = (size_t)0;
        src._ml.core.type = kIsSmall;
    }

    void ml_stole(medium_large_t &dest, medium_large_t &src) {
        // 复制 src 到 dest
        ml_core_clone(dest.core, src.core);

#if 1
        if (src.is_small()) {
            // 如果src已经是 kIsSmall 类型, 则不用管
            // do nothing !!
        }
        else {
            // 如果src不是 kIsSmall 类型
#if 1
            // 把src清零为 kIsSmall 类型
            *(size_t *)(&src._small.buf[0]) = (size_t)0;
            src._ml.core.type = kIsSmall;
#else
            // Clean src's carcass
            src._ml.core.data = NULL;
            src._ml.core.size = 0;
            src._ml.core.capacity = 0;
#endif
        }
#else
        // 把src清零为 kIsSmall 类型
        *(size_t *)(&src._small.buf[0]) = (size_t)0;
        src._ml.core.type = kIsSmall;
#endif
    }

protected:
    /* 使用lastShort有时候可能会快一点 */
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
                small_info_t info;
            };
            char_type buf[(kMaxSmallSizeBytes - sizeof(small_info_t)) / sizeof(char_type)];
        };
    };

    /* medium_large_t 的有效核心数据 */
    struct core_data_t {
        char_type  *data;
        size_type   size;
        size_type   capacity;
        flag_type   type;
    };

    /* medium 和 large 共享一样的结构 */
    struct medium_large_t {
        union {
            struct {
                /* (dummy只是占位用, 未使用) */
                char dummy[(kMaxSmallSizeBytes - sizeof(core_data_t)) / sizeof(char)];

                /* medium_large_t的有效数据 */
                core_data_t core;
            };
            char_type buf[(kMaxSmallSizeBytes - sizeof(core_data_t)) / sizeof(char_type)];
        };
    };

private:
    /* 这是一个union联合, 即small_t类型和medium_large_t类型共享于同一个内存结构 */
    union {
        /* mutable修饰符是针对const修饰的, 即使是在被const修饰过的成员函数里, */
        /* 也可以改变被mutable修饰过的成员变量 */
        /* Reference: http://blog.csdn.net/wuliming_sc/article/details/3717017 */
        mutable small_t         _small;
        mutable medium_large_t  _ml;
    };
};

template <STRING_CORE_CLASSES>
STRING_CORE::string_core()
{
#if 1
    if (sizeof(char_type) <= sizeof(size_t) && kMaxSmallSizeBytes > sizeof(size_t))
        (*(size_t *)(&_small.buf[0])) = (size_t)0;
    else
        (*(char_type *)(&_small.buf[0])) = (char_type)0;

    _ml.core.type = kIsSmall;
    //_ml.core.size = 0;
    //_ml.core.capacity = 0;
#else
    _ml.core.data = NULL;
    _ml.core.size = 0;
    _ml.core.capacity = 0;
    _ml.core.type = 0;

    // init sso buffer
    (*(uint32_t *)(&_small.buf[0])) = (uint32_t)0;
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
            jmLog.error("string_core(const string_core &src): data = %s, type = 0x%04X, size = %d.",
                &_small.buf[0], _small.info.type, _small.info.size);
#endif
        }
        jimi_assert(getType() == kIsSmall && this->size() == rhs.size());
    }
    /* eager copy */
    else if (rhs_type == kIsMedium) {
        // Medium strings are copied eagerly. Don't forget to allocate
        // one extra Char for the null terminator.
        //jimi_assert(rhs._ml.core.capacity == kMaxMediumSize - 1);
        jimi_assert(rhs._ml.core.capacity <= kMaxMediumSize - 1);
        char_type *newData = traits_type::mem_alloc(kMaxMediumSize);
        //traits_type::strncpy(newData, kMaxMediumSize, rhs._ml.core.data, rhs._ml.core.size);
        string_detail::pod_copy(newData, rhs._ml.core.data, rhs._ml.core.size + 1);
        // No need for writeNull() here, we copied one extra element just above.
        _ml.core.data     = newData;
        _ml.core.size     = rhs._ml.core.size;
        _ml.core.capacity = kMaxMediumSize - 1;
        _ml.core.type     = kIsMedium;

        jimi_assert(getType() == kIsMedium);
    }
    /* copy-on-write */
    else if (rhs_type == kIsLarge) {
        // Large strings are just refcounted
        _ml.core.data     = rhs._ml.core.data;
        _ml.core.size     = rhs._ml.core.size;
        _ml.core.capacity = rhs._ml.core.capacity;
        _ml.core.type     = kIsLarge;

        refcount_type::retain(_ml.core.data);
        jimi_assert(getType() == kIsLarge && size() == rhs.size());
    }
    /* constant string data */
    else if (rhs_type == kIsConstant) {
        // It's a constant string data
        _ml.core.data     = rhs._ml.core.data;
        _ml.core.size     = rhs._ml.core.size;
        _ml.core.capacity = rhs._ml.core.capacity;
        _ml.core.type     = kIsConstant;
    }
    /* unknown type */
    else {
        jmLog.error("string_core(const string_core &src): type = 0x%04X, data = %08X, size() = %d.", getType(), _ml.core.data, _ml.core.size);
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const size_t _capacity)
{
    if (_capacity < kMaxSmallSize) {
        // Small: small string optimized
        (*(size_t *)(&_small.buf[0])) = (size_t)0;
        //_small.info.size = 0;
        //_small.info.type = kIsSmallX;
        _ml.core.type = kIsSmall;
    }
    else if (_capacity < kMaxMediumSize) {
        // Medium: eager copy
        size_type capacitySize = calc_capacity(_capacity);
        char_type *newData = traits_type::mem_alloc(capacitySize);
        newData[0] = '\0';
        _ml.core.data = newData;
        _ml.core.size = 0;
        _ml.core.capacity = capacitySize - 1;
        _ml.core.type = kIsMedium;
    }
    else {
        // Large: copy-on-write
        size_type effectiveCapacity = calc_capacity(_capacity);
        refcount_type *newRC = refcount_type::create(&effectiveCapacity);
        char_type *newData = newRC->data();
        newData[0] = '\0';
        _ml.core.data = newData;
        _ml.core.size = 0;
        _ml.core.capacity = effectiveCapacity;
        _ml.core.type = kIsLarge;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type c)
{
#if 1
    if (sizeof(char_type) <= sizeof(size_t) && kMaxSmallSizeBytes > sizeof(size_t))
        (*(size_t *)(&_small.buf[0])) = (size_t)c;
    else
        (*(char_type *)(&_small.buf[0])) = (char_type)c;
#else
    _small.buf[0] = c;
    _small.buf[1] = '\0';
#endif
#if 0
    _small.info.size = 1;
    _small.info.type = kIsSmallX;
#else
    _ml.core.type = kIsSmall | (1 << kSmallSizeOffsetBits);
#endif
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char_type c, size_type n)
{
    if (n < kMaxSmallSize) {
        // Small: small string optimized
        if (sizeof(size_type) == sizeof(char))
            ::memset((void *)&_small.buf[0], c, n * sizeof(char_type));
        else
            string_detail::pod_fill(_small.buf, c, n);
        _small.buf[n] = '\0';
        _small.info.size = n;
        _small.info.type = kIsSmallX;
    }
    else if (n < kMaxMediumSize) {
        // Medium: eager copy
        size_type capacitySize = calc_capacity(n);
        char_type *newData = traits_type::mem_alloc(capacitySize);
        if (sizeof(size_type) == sizeof(char))
            ::memset((void *)newData, c, n * sizeof(char_type));
        else
            string_detail::pod_fill(newData, c, n);
        newData[n] = '\0';
        _ml.core.data = newData;
        _ml.core.size = 0;
        _ml.core.capacity = capacitySize - 1;
        _ml.core.type = kIsMedium;
    }
    else {
        // Large: copy-on-write
        size_type effectiveCapacity = calc_capacity(n);
        refcount_type *newRC = refcount_type::create(&effectiveCapacity);
        char_type *newData = newRC->data();
        if (sizeof(size_type) == sizeof(char))
            ::memset((void *)newData, c, n * sizeof(char_type));
        else
            string_detail::pod_fill(newData, c, n);
        newData[n] = '\0';
        _ml.core.data = newData;
        _ml.core.size = 0;
        _ml.core.capacity = effectiveCapacity;
        _ml.core.type = kIsLarge;
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
        _small.info.type = kIsSmallX;
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
            //traits_type::strncpy(_ml.core.data, _ml.core.capacity, src, src_len);
            //traits_type::strncpy_unsafe(_ml.core.data, src, src_len);
            string_detail::pod_copy(newData, src, src_len);
            newData[src_len] = '\0';
        }
#endif
        _ml.core.data = newData;
        _ml.core.size = size;
        _ml.core.capacity = allocSize - 1;
        _ml.core.type = kIsMedium;
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
        _ml.core.data     = newData;
        _ml.core.size     = size;
        _ml.core.capacity = effectiveCapacity;
        _ml.core.type     = kIsLarge;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::~string_core()
{
    destroy();
}

template <STRING_CORE_CLASSES>
void STRING_CORE::dump()
{
    size_t size = sizeof(*this);
    ReleaseUtils::dump2((void *)this, size, false);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::destroy()
{
    flag_type type = getType();
    if (type == kIsMedium) {
        if (_ml.core.data != NULL) {
            ::free(_ml.core.data);
            _ml.core.data = NULL;
        }
    }
    else if (type == kIsLarge) {
        if (_ml.core.data != NULL) {
            refcount_type::release(_ml.core.data);
            _ml.core.data = NULL;
        }
    }
}

template <STRING_CORE_CLASSES>
void STRING_CORE::retain()
{
    flag_type type = getType();
    if (type == kIsLarge)
        refcount_type::retain(_ml.core.data);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::release()
{
    flag_type type = getType();
    if (type == kIsMedium) {
        if (_ml.core.data != NULL) {
            ::free(_ml.core.data);
            _ml.core.data = NULL;
        }
        _ml.core.size = 0;
        _ml.core.capacity = 0;
        _ml.core.type = 0;
    }
    else if (type == kIsLarge) {
        if (_ml.core.data != NULL) {
            refcount_type::release(_ml.core.data);
            _ml.core.data = NULL;
        }
        _ml.core.size = 0;
        _ml.core.capacity = 0;
        _ml.core.type = 0;
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE & STRING_CORE::operator = (const string_core &rhs) {
    flag_type type = rhs.getType();
    if (type == kIsSmall)
        small_clone(_small, rhs._small);
    else {
        ml_core_clone(_ml.core, rhs._ml.core);
        if (type == kIsLarge)
            refcount_type::retain(_ml.core.data);
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
    jimi_assert(capacity() >= this->size());
    size_t size;
    flag_type type = getType();
    if (type == kIsSmall) {
        size = _small.info.size;
        if (size < kMaxSmallSize - 1) {
            _small.info.size = size + 1;
            _small.buf[size] = c;
            _small.buf[size + 1] = '\0';
            return;
        }
        reserve(kMaxMediumSize - 1);
    }
    else {
        size = _ml.core.size;
        if (size == capacity_ml()) {        // always true for isShared()
            //reserve(_size * 3 / 2 + 1);   // ensures not shared
            reserve(size * 2 + 2);          // ensures not shared
        }
    }
    jimi_assert(!is_shared());
    jimi_assert(capacity() >= (size + 1));
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.core.size = size + 1;
    _ml.core.data[size] = c;
    _ml.core.data[size + 1] = '\0';
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
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
#if 0
    len = traits_type::itoa_radix10_fast(_ml.core.data + oldSize, n, delta);
#elif 1
    len = traits_type::itoa_radix10(_ml.core.data + oldSize, n);
    //len = traits_type::itoa(_ml.core.data + oldSize, n, 10);
#else
    _itoa(n, _ml.core.data + oldSize, 10);
    len = jm_strlen(_ml.core.data + oldSize);
#endif
    _ml.core.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const unsigned int n)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    const int delta = 10;
    int len;
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
#if 1
            len = traits_type::utoa_radix10(_small.buf + oldSize, n);
            //len = traits_type::utoa(_small.buf + oldSize, n, 10);
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
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
#if 0
    len = traits_type::utoa_radix10_fast(_ml.core.data + oldSize, n, delta);
#elif 1
    len = traits_type::utoa_radix10(_ml.core.data + oldSize, n);
    //len = traits_type::utoa(_ml.core.data + oldSize, n, 10);
#else
    _itoa(n, _ml.core.data + oldSize, 10);
    len = jm_strlen(_ml.core.data + oldSize);
#endif
    _ml.core.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const long n)
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
#if 0
            len = traits_type::itoa_radix10_fast(_small.buf + oldSize, n, delta);
#elif 1
            len = traits_type::ltoa_radix10(_small.buf + oldSize, n);
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
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
#if 0
    len = traits_type::itoa_radix10_fast(_ml.core.data + oldSize, n, delta);
#elif 1
    len = traits_type::ltoa_radix10(_ml.core.data + oldSize, n);
    //len = traits_type::itoa(_ml.core.data + oldSize, n, 10);
#else
    _itoa(n, _ml.core.data + oldSize, 10);
    len = jm_strlen(_ml.core.data + oldSize);
#endif
    _ml.core.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const unsigned long n)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    const int delta = 10;
    int len;
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
#if 0
            len = traits_type::utoa_radix10_fast(_small.buf + oldSize, n, delta);
#elif 1
            len = traits_type::ultoa_radix10(_small.buf + oldSize, n);
            //len = traits_type::utoa(_small.buf + oldSize, n, 10);
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
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
#if 0
    len = traits_type::utoa_radix10_fast(_ml.core.data + oldSize, n, delta);
#elif 1
    len = traits_type::ultoa_radix10(_ml.core.data + oldSize, n);
    //len = traits_type::utoa(_ml.core.data + oldSize, n, 10);
#else
    _itoa(n, _ml.core.data + oldSize, 10);
    len = jm_strlen(_ml.core.data + oldSize);
#endif
    _ml.core.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const int64_t i64)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    const int delta = 21;
    int len;
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
#if 1
            len = traits_type::i64toa_radix10(_small.buf + oldSize, i64);
            //len = traits_type::i64toa(_small.buf + oldSize, i64, 10);
#else
            _i64toa(i64, _small.buf + oldSize, 10);
            len = jm_strlen(_small.buf + oldSize);
#endif
            _small.info.size = oldSize + len;
            jimi_assert(size() == oldSize + len);
            return;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
#if 1
    len = traits_type::i64toa_radix10(_ml.core.data + oldSize, i64);
#else
    _i64toa(i64, _ml.core.data + oldSize, 10);
    len = jm_strlen(_ml.core.data + oldSize);
#endif
    _ml.core.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const uint64_t u64)
{
    // Strategy is simple: make room, then change size
    jimi_assert(capacity() >= size());
    const int delta = 20;
    int len;
    size_type oldSize, newSize;
    flag_type type = getType();
    if (type == kIsSmall) {
        oldSize = _small.info.size;
        newSize = oldSize + delta;
        if (newSize < kMaxSmallSize) {
#if 1
            len = traits_type::u64toa_radix10(_small.buf + oldSize, u64);
            //len = traits_type::u64toa(_small.buf + oldSize, u64, 10);
#else
            _u64toa(u64, _small.buf + oldSize, 10);
            len = jm_strlen(_small.buf + oldSize);
#endif
            _small.info.size = oldSize + len;
            jimi_assert(size() == oldSize + len);
            return;
        }
        reserve(newSize);
    }
    else {
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
#if 1
    len = traits_type::u64toa_radix10(_ml.core.data + oldSize, u64);
#else
    _u64toa(u64, _ml.core.data + oldSize, 10);
    len = jm_strlen(_ml.core.data + oldSize);
#endif
    _ml.core.size = oldSize + len;
    jimi_assert(size() == oldSize + len);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const float f)
{
    //
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append(const double d)
{
    //
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append_hex(const uint32_t hex32, const bool isUpper /* = true */)
{
    //
}

template <STRING_CORE_CLASSES>
void STRING_CORE::append_hex(const uint64_t hex64, const bool isUpper /* = true */)
{
    //
}

// swap below doesn't test whether &rhs == this (and instead
// potentially does extra work) on the premise that the rarity of
// that situation actually makes the check more expensive than is
// worth.

template <STRING_CORE_CLASSES>
void STRING_CORE::swap(STRING_CORE &rhs)
{
    // 因为在内部, 我们要使用swap(rhs)的地方, 都能保证(this != &rhs), 所以这个判断是多余的
#if 0
    // same object, do nothing
    if (this == &rhs)
        return;
#endif

#if 0
    // 在不同的type下, _ml的有些数据是不必复制的
#if 0
    flag_type type = getType();
    flag_type rhs_type = rhs.getType();
    if ((type == kIsSmall) && (rhs_type == kIsSmall)) {
        small_swap(_small, rhs._small);
    }
    else if ((type != kIsSmall) && (rhs_type != kIsSmall)) {
        ml_swap(_ml, rhs._ml);
    }
#else
    flag_type type = _ml.core.type;
    flag_type rhs_type = rhs._ml.core.type;
    if (((type | rhs_type) & kIsSmall) == 0) {
        ml_swap(_ml, rhs._ml);
    }
    else if (((type & rhs_type) & kIsSmall) != 0) {
        small_swap(_small, rhs._small);
    }
#endif
    else {
        const medium_large_t t = _ml;
        _ml = rhs._ml;
        rhs._ml = t;
    }
#elif 1
    // 完全直接复制_ml, 有些复制可能是多余的
    const medium_large_t t = _ml;
    _ml = rhs._ml;
    rhs._ml = t;
#else
    medium_large_t t;
    memcpy((void *)&t,       (void *)&_ml,      sizeof(medium_large_t));
    memcpy((void *)&_ml,     (void *)&rhs._ml,  sizeof(medium_large_t));
    memcpy((void *)&rhs._ml, (void *)&t,        sizeof(medium_large_t));
#endif
}

template <STRING_CORE_CLASSES>
void STRING_CORE::stole(STRING_CORE &rhs)
{
#if 1
    // 在不同的type下, _ml的复制是不同的
#if 0
    flag_type type = getType();
    flag_type rhs_type = rhs.getType();
    if ((type == kIsSmall) && (rhs_type == kIsSmall)) {
        small_stole(_small, rhs._small);
    }
    else if ((type != kIsSmall) && (rhs_type != kIsSmall)) {
        ml_stole(_ml, rhs._ml);
    }
#else
    flag_type type = _ml.core.type;
    flag_type rhs_type = rhs._ml.core.type;
    if (((type | rhs_type) & kIsSmall) == 0) {
        ml_stole(_ml, rhs._ml);
    }
    else if (((type & rhs_type) & kIsSmall) != 0) {
        small_stole(_small, rhs._small);
    }
#endif
    else {
        // 复制rhs._ml到_ml, 然后清零rhs._ml
        _ml = rhs._ml;

        // 清零rhs._ml为 kIsSmall 类型
        *(size_t *)(rhs._small.buf[0]) = (size_t)0;
        rhs._ml.core.type = kIsSmall;
    }
#elif 1
    // 复制rhs._ml到_ml, 然后清零rhs._ml
    _ml = rhs._ml;

    // 清零rhs._ml为 kIsSmall 类型
    *(size_t *)(rhs._small.buf[0]) = (size_t)0;
    rhs._ml.core.type = kIsSmall;
#else
    // 复制rhs._ml到_ml, 然后清零rhs._ml
    memcpy((void *)&_ml, (void *)&rhs._ml, sizeof(medium_large_t));

    // 清零rhs._ml为 kIsSmall 类型
    *(size_t *)(rhs._small.buf[0]) = (size_t)0;
    rhs._ml.core.type = kIsSmall;
#endif
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::size() const
{
    if (is_small())
        return _small.info.size;
    else
        return _ml.core.size;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::size_small() const
{
    return _small.info.size;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::size_ml() const
{
    return _ml.core.size;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::capacity() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return kMaxSmallSize - 1;
    /*
    else if (type == kIsMedium)
        return kMaxMediumSize - 1;
    //*/
    else
        return _ml.core.capacity;
}

template <STRING_CORE_CLASSES>
const typename STRING_CORE::size_type STRING_CORE::capacity_small() const
{
    return kMaxSmallSize - 1;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::size_type STRING_CORE::capacity_ml() const
{
    return _ml.core.capacity;
}

template <STRING_CORE_CLASSES>
const typename STRING_CORE::char_type *STRING_CORE::c_str() const
{
    flag_type type = getType();
    if (type == kIsSmall)
        return &_small.buf[0];
    else
        return _ml.core.data;
}

template <STRING_CORE_CLASSES>
const typename STRING_CORE::char_type *STRING_CORE::c_str_small() const
{
    return &_small.buf[0];
}

template <STRING_CORE_CLASSES>
const typename STRING_CORE::char_type *STRING_CORE::c_str_ml() const
{
    return _ml.core.data;
}

template <STRING_CORE_CLASSES>
typename STRING_CORE::char_type *STRING_CORE::mutable_data()
{
    flag_type type = getType();
    if (type == kIsSmall)
        return &_small.buf[0];

    if (type == kIsLarge && refcount_type::refs(_ml.core.data) > 1) {
        // Ensure unique.
        size_t effectiveCapacity = _ml.core.capacity;
        refcount_type *newRC = refcount_type::create(&effectiveCapacity);
        char_type *newData = newRC->data();
        // If this fails, someone placed the wrong capacity in an jimi_string.
        jimi_assert(effectiveCapacity >= _ml.core.capacity);
        string_detail::pod_copy(newData, _ml.core.data, _ml.core.size + 1);
        refcount_type::release(_ml.core.data);
        _ml.core.data = newData;
        // No need to call writeNull(), we have + 1 above.
    }
    return _ml.core.data;
}

template <STRING_CORE_CLASSES>
size_t STRING_CORE::calc_capacity(const size_t minCapacity)
{
    size_t trueCapacitySize = minCapacity + 1;
    // Round to 64 bytes
    size_t newCapacity = (trueCapacitySize + (64 - 1)) & (~((size_t)64 - 1));
    return newCapacity;
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

    if (_ml.core.size == rhs._ml.core.size) {
        if (getType() == rhs.getType()) {
            if (is_small()) {
                equal = traits_type::strncmp(&_small.buf[0], &rhs._small.buf[0], _small.info.size);
            }
            else if (not_is_small()) {
                rhs_data = rhs._ml.core.data;
                if (_ml.core.data == rhs_data)
                    equal = 0;
                else
                    equal = traits_type::strncmp(_ml.core.data, rhs_data, _ml.core.size);
            }
            else {
#ifndef _DEBUG
                // Unknown string type
                jmLog.error("string_core::compare(const string_core &rhs) - Error: Unknown string type.");
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
        jmLog.info("string_core::compare(const string_core &rhs), size() = %d, rhs.size() = %d", size(), rhs.size());
#endif
    }
    return equal;
}

template <STRING_CORE_CLASSES>
int STRING_CORE::compare(const char_type *rhs) const
{
    int equal = -1;
    if (is_small()) {
        if (_small.buf == rhs)
            equal = 0;
        else
            equal = traits_type::strncmp(_small.buf, rhs, _small.info.size);
    }
    else if (not_is_small()) {
        if (_ml.core.data == rhs)
            equal = 0;
        else
            equal = traits_type::strncmp(_ml.core.data, rhs, _ml.core.size);
    }
    else {
#ifdef _DEBUG
        // Unknown string type
        jmLog.error("STRING_CORE::compare(const char_type *rhs) - Error: Unknown string type.");
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
        refcount_type * newRC = refcount_type::create(&minCapacity);
        size_type size = _small.info.size;
        jimi_assert(newRC != NULL);
        string_detail::pod_copy(newRC->data(), _small.buf, size + 1);
        // No need for writeTerminator(), we wrote it above with + 1.
        _ml.core.data = newRC->data();
        _ml.core.size = size;
        _ml.core.capacity = minCapacity;
        _ml.core.type = kIsLarge;
        jimi_assert(capacity() >= minCapacity);
    }
    else if (minCapacity >= kMaxSmallSize) {
        // medium
        // Don't forget to allocate one extra Char for the terminating null
        size_type newCapacitySize = calc_capacity(minCapacity + 1);
        size_type allocSizeBytes = newCapacitySize * sizeof(char_type);
        char_type *data = static_cast<char_type *>(::malloc(allocSizeBytes));
        size_type size = _small.info.size;
        jimi_assert(data != NULL);
        string_detail::pod_copy(data, _small.buf, size + 1);
        // No need for writeTerminator(), we wrote it above with + 1.
        _ml.core.data = data;
        _ml.core.size = size;
        _ml.core.capacity = newCapacitySize - 1;
        _ml.core.type = kIsMedium;
        jimi_assert(capacity() >= minCapacity);
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
    if (minCapacity <= _ml.core.capacity) {
        return; // nothing to do, there's enough room
    }
    if (minCapacity < kMaxMediumSize) {
        // Keep the string at medium size. Don't forget to allocate
        // one extra Char for the terminating null.
        size_type newCapacitySize = calc_capacity(minCapacity + 1);
        size_t capacityBytes = newCapacitySize * sizeof(char_type);
        _ml.core.data = static_cast<char_type *>(
            smartRealloc(_ml.core.data, _ml.core.size * sizeof(char_type),
                (_ml.core.capacity + 1) * sizeof(char_type),
                capacityBytes));
        writeNull_ml();
        _ml.core.capacity = newCapacitySize - 1;
        _ml.core.type = kIsMedium;
    }
    else {
        // Conversion from medium to large string
        string_core nascent;
        // Will recurse to another branch of this function
        nascent.reserve(minCapacity);
        nascent._ml.core.size = _ml.core.size;
        string_detail::pod_copy(nascent._ml.core.data, _ml.core.data, _ml.core.size + 1);
        // Done with the old data. No need to call writeNull(),
        nascent.swap(*this);
        //writeNull_ml();
        jimi_assert(capacity() >= minCapacity);
    }
}

template <STRING_CORE_CLASSES>
void STRING_CORE::reserveTo_from_large(size_t minCapacity)
{
    jimi_assert(getType() == kIsLarge);
    // Ensure unique
    if (refcount_type::refs(_ml.core.data) > 1) {
        // We must make it unique regardless; in-place reallocation is
        // useless if the string is shared. In order to not surprise
        // people, reserve the new block at current capacity or
        // more. That way, a string's capacity never shrinks after a
        // call to reserve.
        minCapacity = jimi_max(minCapacity, _ml.core.capacity);
        refcount_type * newRC = refcount_type::create(&minCapacity);
        char_type * newData = newRC->data();
        string_detail::pod_copy(newData, _ml.core.data, _ml.core.size + 1);
        // Done with the old data. No need to call writeNull(),
        // we have + 1 above.
        refcount_type::release(_ml.core.data);
        _ml.core.data = newData;
        _ml.core.capacity = minCapacity;
        //_ml.core.type = kIsLarge;
        // size and type remains unchanged
    }
    else {
        // String is not shared, so let's try to realloc (if needed)
        if (minCapacity > _ml.core.capacity) {
            // Asking for more memory
            refcount_type *newRC =
                refcount_type::reallocate(_ml.core.data, _ml.core.size, _ml.core.capacity, minCapacity);
            _ml.core.data = newRC->data();
            _ml.core.capacity = minCapacity;
            //_ml.core.type = kIsLarge;
            writeNull_ml();
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
        //size_type _capacity = _ml.core.capacity;
        if (newSize > _ml.core.capacity)
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
            //_small.buf[newSize] = kNullChar;
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

    _ml.core.size = newSize;
    //_ml.core.data[newSize] = kNullChar;

    jimi_assert(size() == newSize);
}

template <STRING_CORE_CLASSES>
void STRING_CORE::shrinkTo(const size_t newSize)
{
    flag_type type = getType();
    if (type == kIsLarge) {
        jimi_assert(newSize <= _ml.core.size);
        if (refcount_type::refs(_ml.core.data) == 1) {
            // Medium strings and unique large strings need no special handling.
            jimi_assert(newSize <= _ml.core.size);
            _ml.core.size = newSize;
            writeNull_ml();
        }
        else {
            // Shared large string, must make unique. This is because of the
            // durn terminator must be written, which may trample the shared
            // data.
            if (newSize != _ml.core.size)
                string_core(_ml.core.data, newSize).swap(*this);
            // No need to write the terminator.
            return;
        }
    }
    else if (type == kIsMedium) {
        // Medium strings and unique large strings need no special handling.
        jimi_assert(newSize <= _ml.core.size);
        _ml.core.size = newSize;
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
        oldSize = _ml.core.size;
        newSize = oldSize + delta;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.core.size = newSize;
    writeNull_ml();
    jimi_assert(size() == newSize);
    return _ml.core.data + oldSize;
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
        oldSize = _ml.core.size;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.core.size = newSize;
    writeNull_ml();
    jimi_assert(size() == newSize);
    return _ml.core.data + oldSize;
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
        oldSize = _ml.core.size;
        newSize = oldSize + n;
        if (newSize > capacity_ml())
            reserve(newSize);
    }
    jimi_assert(capacity() >= newSize);
    // Category can't be small - we took care of that above
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    ::memcpy((void *)(_ml.core.data + oldSize), s, n * sizeof(char_type));
    _ml.core.size = newSize;
    writeNull_ml();
    jimi_assert(size() == newSize);
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
        if (refcount_type::refs(_ml.core.data) > 1) {
            // We must make it unique regardless; in-place reallocation is
            // useless if the string is shared. In order to not surprise
            // people, reserve the new block at current capacity or
            // more. That way, a string's capacity never shrinks after a
            // call to reserve.
            newCapacity = jimi_max(newCapacity, _ml.core.capacity);
            refcount_type * newRC = refcount_type::create(&newCapacity);
            char_type * newData = newRC->data();
            string_detail::pod_copy(newData, _ml.core.data, _ml.core.size + 1);
            // Done with the old data. No need to call writeTerminator(),
            // we have + 1 above.
            refcount_type::release(_ml.core.data);
            _ml.core.data = newData;
            _ml.core.capacity = newCapacity;
            //_ml.core.type = kIsLarge;
            // size and type remains unchanged
        }
        else {
            // String is not shared, so let's try to realloc (if needed)
            if (newCapacity > _ml.core.capacity) {
                // Asking for more memory
                refcount_type * newRC =
                    refcount_type::reallocate(_ml.core.data, _ml.core.size, _ml.core.capacity, newCapacity);
                _ml.core.data = newRC->data();
                _ml.core.capacity = newCapacity;
                writeNull_ml();
            }
            jimi_assert(capacity() >= newCapacity);
        }
    }
    else if (type == kIsMedium) {
        // String is not shared
        if (newCapacity <= _ml.core.capacity)
            return;  // nothing to do, there's enough room

        if (newCapacity < kMaxMediumSize) {
            // Keep the string at medium size. Don't forget to allocate
            // one extra Char for the terminating null.
            size_type newCapacitySize = calc_capacity(newCapacity + 1);
            size_t capacityBytes = newCapacitySize * sizeof(char_type);
            _ml.core.data = static_cast<char_type *>(
                smartRealloc(
                    _ml.core.data,
                    _ml.core.size * sizeof(char_type),
                    (_ml.core.capacity + 1) * sizeof(char_type),
                    capacityBytes));
            writeNull_ml();
            _ml.core.capacity = newCapacitySize - 1;
            _ml.core.type = kIsMedium;
        }
        else {
            // Conversion from medium to large string
            string_core nascent;
            // Will recurse to another branch of this function
            nascent.reserve(newCapacity);
            nascent._ml.core.size = _ml.core.size;
            string_detail::pod_copy(nascent._ml.core.data, _ml.core.data, _ml.core.size);
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
            jimi_assert(newRC != NULL);
            string_detail::pod_copy(newRC->data(), _small.buf, size + 1);
            // No need for writeTerminator(), we wrote it above with + 1.
            _ml.core.data = newRC->data();
            _ml.core.size = size;
            _ml.core.capacity = newCapacity;
            _ml.core.type = kIsLarge;
            jimi_assert(capacity() >= newCapacity);
        }
        else if (newCapacity >= kMaxSmallSize) {
            // medium
            // Don't forget to allocate one extra Char for the terminating null
            size_type newCapacitySize = calc_capacity(newCapacity + 1);
            size_type allocSizeBytes = newCapacitySize * sizeof(char_type);
            char_type *data = static_cast<char_type *>(::malloc(allocSizeBytes));
            size_type size = _small.info.size;
            jimi_assert(data != NULL);
            string_detail::pod_copy(data, _small.buf, size + 1);
            // No need for writeTerminator(), we wrote it above with + 1.
            _ml.core.data = data;
            _ml.core.size = size;
            _ml.core.capacity = newCapacitySize - 1;
            _ml.core.type = kIsMedium;
            jimi_assert(capacity() >= newCapacity);
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
            _small.buf[_size] = kNullChar;
    }
    else {
        _ml.core.data[_ml.core.size] = kNullChar;
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
        _small.buf[size] = kNullChar;
}

template <STRING_CORE_CLASSES>
void STRING_CORE::writeNull_ml()
{
    jimi_assert(getType() == kIsMedium || getType() == kIsLarge);
    _ml.core.data[_ml.core.size] = kNullChar;
}

template <STRING_CORE_CLASSES>
void STRING_CORE::writeNullForce()
{
    if (getType() == kIsSmall) {
        const size_type size = _small.info.size;
        jimi_assert(size < kMaxSmallSize);
        //if (size < kMaxSmallSize)
            _small.buf[size] = kNullChar;
    }
    else {
        _ml.core.data[_ml.core.size] = kNullChar;
    }
}

#undef STRING_CORE_CLASSES
#undef STRING_CORE

NS_JIMI_END

#if defined(_MSC_VER) && (_MSC_VER != 0)
#pragma warning(pop)                    // 恢复warning状态
#endif

#endif  /* _JIMI_LANG_STRING_CORE_H_ */
