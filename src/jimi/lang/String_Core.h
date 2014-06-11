
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
#include <jimi/lang/String_Core.h>
#include <jimic/string/jm_strings.h>

#include <string>
using namespace std;

NS_JIMI_BEGIN

/// <comment>
///
/// 标准C＋＋类string的Copy-On-Write技术（一）
///
/// Reference:
///    http://haoel.blog.51cto.com/313033/124638
///
///
/// </comment>

#define STRING_SMALL_SIZE       32

#define STRING_MEDIUM_SIZE      256

typedef enum StringTypeMask
{
    STRING_TYPE_SMALL   = (sizeof(size_t) == 4) ? 0x01000000 : 0x0100000000000000,
    STRING_TYPE_MEDIUM  = (sizeof(size_t) == 4) ? 0x02000000 : 0x0200000000000000,
    STRING_TYPE_LARGE   = (sizeof(size_t) == 4) ? 0x04000000 : 0x0400000000000000,
    STRING_TYPE_MASK    = (STRING_TYPE_SMALL
        | STRING_TYPE_MEDIUM | STRING_TYPE_LARGE),
} StringTypeMask;

typedef enum StringTypeMaskX
{
    STRING_TYPE_SMALL_X = 0x01,
    TRING_TYPE_MEDIUM_X = 0x02,
    STRING_TYPE_LARGE_X = 0x04,
    STRING_TYPE_MASK_X  = 0x07
} StringTypeMaskX;

#define TYPE_IS_SMALL(type)     ((type & STRING_TYPE_SMALL)  != 0)
#define TYPE_IS_MIDIUM(type)    ((type & STRING_TYPE_MEDIUM) != 0)
#define TYPE_IS_LARGE(type)     ((type & STRING_TYPE_LARGE)  != 0)

#define TYPE_NOT_IS_SMALL(type)   \
    ((type & (STRING_TYPE_MEDIUM | STRING_TYPE_LARGE)) != 0)

#define STRING_CORE_CLASSES    \
    class _CharT, class _RefCount
#define STRING_CORE            \
    string_core<_CharT, _RefCount>

template <class _CharT, class _RefCount = refcounted<_CharT, size_t>>
class JIMI_API string_core
{
public:
    // Types
    typedef _CharT              char_type;
    typedef _RefCount           refcount_type;
    typedef size_t              size_type;
    typedef size_t              flag_type;
    typedef char_traits<_CharT> traits_type;

    // Constant
    static const flag_type      kIsSmall  = STRING_TYPE_SMALL;
    static const flag_type      kIsMedium = STRING_TYPE_MEDIUM;
    static const flag_type      kIsLarge  = STRING_TYPE_LARGE;
    static const flag_type      kTypeMask = STRING_TYPE_MASK;
    static const flag_type      kSizeMask = ~STRING_TYPE_MASK;

    static const size_type      kMaxSmallSize  = (STRING_SMALL_SIZE - 1);
    static const size_type      kMaxMediumSize = STRING_MEDIUM_SIZE;

public:
    // Contructor
    string_core();
    string_core(const string_core &src);
    string_core(const char * const src, const size_t size);
    string_core(const wchar_t * const src, const size_t size);
    ~string_core();

    void destroy();

    void retail();
    void release();

    bool equals(const string_core &rhs) const;

    int compare(const string_core &rhs) const;
    int compare(const char_type *rhs) const;

    bool is_small() const   { return TYPE_IS_SMALL(_ml.type);  }
    bool is_medium() const  { return TYPE_IS_MIDIUM(_ml.type); }
    bool is_large() const   { return TYPE_IS_LARGE(_ml.type);  }

    bool not_is_small() const { return TYPE_NOT_IS_SMALL(_ml.type); }

    bool is_shared() const {
        return (is_large() && (refcount_type::refs(_ml.data) > 1));
    }

    flag_type get_type() const { return (_ml.type & kTypeMask); }

    const char_type *data() const { return c_str(); }
    const char_type *c_str() const;

    size_type size() const;

    // swap below doesn't test whether &rhs == this (and instead
    // potentially does extra work) on the premise that the rarity of
    // that situation actually makes the check more expensive than is
    // worth.
    void swap(string_core &rhs);

    size_type capacity() const;
    
protected:
    size_t calc_capacity(size_t src_len);

private:
    // Disabled
    string_core & operator = (const string_core &rhs) {
        flag_type type = rhs.get_type();
        if (type == kIsSmall)
            small_clone(_small, rhs._small);
        else
            ml_clone(_ml, rhs._ml);
    }

    static void small_clone(struct small_t &dest, const struct small_t &src) {
        dest.info.lastShort = src._small.info.lastShort;
        traits_type::strncpy_u(dest.buf, src._small.buf, src._small.info.size);
        dest.buf[src._small.info.size] = '\0';
    }

    static void ml_clone(struct medium_large &dest, const struct medium_large &src) {
        dest.data = src.data;
        dest.size = src.size;
        dest.capacity = src.capacity;
        dest.type = src.type;
    }

private:
    struct small_size_t {
        union {
            struct {
                unsigned char size;
                unsigned char type;
            };
            unsigned short    lastShort;
        };
    };

    struct small_t {
        /* small object buffer */
        union {
            struct {
                unsigned char dummy[(STRING_SMALL_SIZE - sizeof(small_size_t)) / sizeof(char)];
                small_size_t  info;
            };
            char_type buf[(STRING_SMALL_SIZE - sizeof(small_size_t)) / sizeof(char_type)];
        };
    };

    /* 这个结构只是为了动态设置buf的size而存在的 */
    struct core_data_t {
        char_type  *data;
        size_type   size;
        size_type   capacity;
        flag_type   type;
    };

    struct medium_large {
        union {
            struct {
                /* small object buffer (这里只是占位用, 未使用) */
                unsigned char dummy[(STRING_SMALL_SIZE - sizeof(core_data_t)) / sizeof(char)];

                /* 后面的定义必须和core_data的结构一致 */
                char_type *data;
                size_type  size;
                size_type  capacity;
                flag_type  type;
            };

            char_type buf[(STRING_SMALL_SIZE - sizeof(core_data_t)) / sizeof(char_type)];
        };
    };

private:

    /* 这是一个union联合, 即small_t类型和medium_large类型共享于同一个内存结构 */
    union {
        /* mutable修饰符是针对const修饰的, 即使是在const修饰过的成员函数里, */
        /* 也可以改变mutable修饰过的成员变量 */
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
STRING_CORE::string_core(const string_core &src)
{
    jimi_assert(&src != this);
    flag_type type = src.get_type();
    /* small object */
    if (type == kIsSmall) {
        //_small.info.type = src._small.info.type;
        //_small.info.size = src._small.info.size;
        _small.info.lastShort = src._small.info.lastShort;
        jimi_assert(src._small.info.size < STRING_SMALL_SIZE);
        if (_small.info.size < STRING_SMALL_SIZE) {
            char_traits<char>::strlcpy(&_small.buf[0], STRING_SMALL_SIZE, &src._small.buf[0], _small.info.size);
        }
        else {
            sLog.error("string_core(const string_core &src): data = %s, type = 0x%04X, size = %d.",
                &_small.buf[0], _small.info.type, _small.info.size);
        }
    }
    /* eager copy */
    else if (type == kIsMedium) {
        jimi_assert(src._ml.capacity == STRING_MEDIUM_SIZE);
        _ml.size = src._ml.size;
        _ml.capacity = STRING_MEDIUM_SIZE;
        _ml.type = src._ml.type;
        _ml.data = char_traits<char>::assign(STRING_MEDIUM_SIZE);
        char_traits<char>::strlcpy(_ml.data, STRING_MEDIUM_SIZE, src._ml.data, src._ml.size);
    }
    /* copy-on-write */
    else if (type == kIsLarge) {
        _ml.data     = src._ml.data;
        _ml.capacity = src._ml.capacity;
        const_cast<string_core &>(src).retail();
    }
    /* unknown type */
    else {
        sLog.error("string_core(const string_core &src): type = 0x%04X, data = %08X, size() = %d.", get_type(), _ml.data, _ml.size);
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char * const src, const size_t size)
{
    size_t src_len = size;
    if (src_len < STRING_SMALL_SIZE) {
        _small.info.type = STRING_TYPE_SMALL_X;
        _small.info.size = src_len;     
        char_traits<char>::strncpy_u(_small.buf, src, src_len);
        _small.buf[src_len] = '\0';
    }
    /* eager copy */
    else if (src_len < STRING_MEDIUM_SIZE) {
        _ml.size = src_len;
        _ml.capacity = calc_capacity(src_len);
        _ml.type = STRING_TYPE_MEDIUM;
        _ml.data = char_traits<char>::assign(_ml.capacity);
        if (_ml.data) {
            char_traits<char>::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
    /* copy-on-write or unknown type */
    else {
        _ml.capacity = calc_capacity(src_len);
        _ml.data = char_traits<char>::assign(_ml.capacity);
        _ml.size = src_len;
        _ml.type = STRING_TYPE_LARGE;
        if (_ml.data) {
            char_traits<char>::strncpy(_ml.data, _ml.capacity, src, src_len);
            _ml.data[src_len] = '\0';
        }
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const wchar_t * const src, const size_t size)
{
    size_t src_len = size;
    if (src_len < STRING_SMALL_SIZE) {
        _small.info.size = src_len;     
        _small.info.type = STRING_TYPE_SMALL;
        char_traits<wchar_t>::strncpy(_ml.buf, jm_countof(_ml.buf), src, src_len);
    }
    /* eager copy */
    else if (src_len < STRING_MEDIUM_SIZE) {
        _ml.size = src_len;
        _ml.capacity = calc_capacity(src_len);
        _ml.type = STRING_TYPE_MEDIUM;
        _ml.data = char_traits<wchar_t>::assign(_ml.capacity);
        if (_ml.data)
            char_traits<wchar_t>::strncpy(_ml.data, _ml.capacity, src, src_len);
    }
    /* copy-on-write or unknown type */
    else {
        _ml.capacity = calc_capacity(src_len);
        _ml.data = char_traits<wchar_t>::assign(_ml.capacity);
        _ml.size = src_len;
        _ml.type = STRING_TYPE_LARGE;
        if (_ml.data)
            char_traits<wchar_t>::strncpy(_ml.data, _ml.capacity, src, src_len);
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
    release();
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::retail()
{
    if (is_large())
        _ml.type = STRING_TYPE_LARGE;
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
        refcount_type::release(_ml.data);
        _ml.data = NULL;
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
    }
}

// swap below doesn't test whether &rhs == this (and instead
// potentially does extra work) on the premise that the rarity of
// that situation actually makes the check more expensive than is
// worth.

template <STRING_CORE_CLASSES>
inline void STRING_CORE::swap(STRING_CORE &rhs)
{
#if 0
        // 在不同的type下, _ml的有些数据是不必复制的
        flag_type type = rhs.get_type();
        if (type == kIsSmall) {
            const small_t t;
            small_clone(t, _small);
            small_clone(_small, rhs._small);
            small_clone(rhs._small, t);
        }
        else {
            const medium_large t;
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
    if (is_small())
        return kMaxSmallSize - 1;
    else if (is_medium())
        return kMaxMediumSize - 1;
    else
        return _ml.capacity;
}

template <STRING_CORE_CLASSES>
inline const typename STRING_CORE::char_type *STRING_CORE::c_str() const
{
    flag_type type = get_type();
    if (type == kIsSmall)
        return &_small.buf[0];
    else
        return _ml.data;
}

template <STRING_CORE_CLASSES>
inline size_t STRING_CORE::calc_capacity(size_t src_len)
{
    return src_len + 1;
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
        if (get_type() == rhs.get_type()) {
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

NS_JIMI_END

#endif  /* _JIMI_LANG_STRING_CORE_H_ */
