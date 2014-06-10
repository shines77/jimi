
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
/// ��׼C������string��Copy-On-Write������һ��
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
    STRING_TYPE_SMALL   = (sizeof(size_t) == 4) ? 0x20000000u : 0x2000000000000000u,
    STRING_TYPE_MEDIUM  = (sizeof(size_t) == 4) ? 0x40000000u : 0x4000000000000000u,
    STRING_TYPE_LARGE   = (sizeof(size_t) == 4) ? 0x80000000u : 0x8000000000000000u,
    STRING_TYPE_MASK    = (STRING_TYPE_SMALL
        | STRING_TYPE_MEDIUM | STRING_TYPE_LARGE),
    STRING_TYPE_SMALL_X = 0x01,
    TRING_TYPE_MEDIUM_X = 0x02,
    STRING_TYPE_LARGE_X = 0x04,
    STRING_TYPE_MASK_X  = 0x07
} StringTypeMask;

#define TYPE_IS_SMALL(type)     ((type & STRING_TYPE_SMALL)  != 0)
#define TYPE_IS_MIDIUM(type)    ((type & STRING_TYPE_MEDIUM) != 0)
#define TYPE_IS_LARGE(type)     ((type & STRING_TYPE_LARGE)  != 0)

#define TYPE_NOT_IS_SMALL(type)   \
    ((type & (STRING_TYPE_MEDIUM | STRING_TYPE_LARGE)) != 0)

#define STRING_CORE_CLASSES    \
    class _CharT, class _RefCount
#define STRING_CORE            \
    string_core<_CharT, _RefCount>

template <class _CharT, class _RefCount = refcounted<_CharT, int32_t>>
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

    flag_type get_type() const { return (_ml.type & kTypeMask); }

    const char_type *data() const { return c_str(); }
    const char_type *c_str() const;

    size_type size() const;

protected:
    size_t calc_capacity(size_t src_len);

private:
    /* ����ṹֻ��Ϊ�˶�̬����buf��size�����ڵ� */
    struct core_data_t {
        char_type  *data;
        size_type   size;
        size_type   capacity;
        flag_type   type;
    };

    struct medium_large {
        union {
            struct {
                /* small object buffer (����ֻ��ռλ��, δʹ��) */
                char       dummy[(STRING_SMALL_SIZE - sizeof(core_data_t)) / sizeof(char)];

                /* ����Ķ�������core_data�Ľṹһ�� */
                char_type *data;
                size_type  size;
                size_type  capacity;
                flag_type  type;
            };

            char_type buf[(STRING_SMALL_SIZE - sizeof(core_data_t)) / sizeof(char_type)];
        };
    };

    struct small_size_t {
        unsigned char size : 5;
        unsigned char type : 3;
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

protected:
    /* ����һ��union����, ��small_t���ͺ�medium_large���͹�����ͬһ���ڴ�ṹ */
    union {
        /* mutable���η������const���ε�, ��ʹ����const���ι��ĳ�Ա������, */
        /* Ҳ���Ըı�mutable���ι��ĳ�Ա���� */
        mutable small_t      _small;
        mutable medium_large _ml;
    };
    int32_t _refcount;
};

template <STRING_CORE_CLASSES>
STRING_CORE::string_core()
{
    _ml.data = NULL;
    _ml.size = 0;
    _ml.capacity = 0;
    _ml.type = 0;
    _refcount = 1;

    // init sso buffer
    (*(uint32_t *)(&_ml.buf[0])) = 0;
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const string_core &src)
{
    jimi_assert(&src != this);
    /* small object */
    if (is_small()) {
        _small.info.type = src._small.info.type;
        _small.info.size = src._small.info.size;
        _refcount = 1;
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
    else if (is_medium()) {
        _ml.size = src._ml.size;
        jimi_assert(src._ml.capacity == STRING_MEDIUM_SIZE);
        _ml.capacity = STRING_MEDIUM_SIZE;
        _ml.type = src._ml.type;
        _refcount = 1;
        // init sso buffer
        (*(uint32_t *)(&_ml.buf[0])) = 0;
        _ml.data = char_traits<char>::assign(STRING_MEDIUM_SIZE);
        char_traits<char>::strlcpy(_ml.data, STRING_MEDIUM_SIZE, src._ml.data, src._ml.size);
    }
    /* copy-on-write */
    else if (is_large()) {
        jimi_assert(src._refcount >= 0);
        _ml.data     = src._ml.data;
        _ml.capacity = src._ml.capacity;
        _refcount = 1;
        // init sso buffer
        (*(uint32_t *)(&_ml.buf[0])) = 0;
        const_cast<string_core &>(src).retail();
    }
    /* unknown type */
    else {
        sLog.error("string_core(const string_core &src): type = 0x%04X, data = %08X, size = %d.", get_type(), _ml.data, _ml.size);
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
    _refcount = 1;
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
    _refcount = 1;
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
    ++_refcount;
    if (is_large())
        _ml.type = STRING_TYPE_LARGE;
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::release()
{
    --_refcount;
    if (_refcount <= 0) {
        if (not_is_small()) {
            if (_ml.data != NULL) {
                delete _ml.data;
                _ml.data = NULL;
            }
        }
        _ml.size = 0;
        _ml.capacity = 0;
        _ml.type = 0;
        _refcount = 0;
    }
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
inline typename STRING_CORE::size_type STRING_CORE::size() const
{
    return _ml.size;
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
        if (_ml.type == rhs._ml.type) {
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
        sLog.info("string_core::compare(const string_core &rhs), size = %d, rhs.size = %d", size(), rhs.size());
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