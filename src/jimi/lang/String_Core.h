
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

//#define STRING_SMALL_SIZE     (128 - 4 * sizeof(size_t))
#define STRING_SMALL_SIZE       32

#define STRING_MEDIUM_SIZE      256

typedef enum StringTypeMask
{
    STRING_TYPE_SMALL   = 0x01,
    STRING_TYPE_MEDIUM  = 0x02,
    STRING_TYPE_LARGE   = 0x04,
    STRING_TYPE_REF     = 0x08,
    STRING_TYPE_MASK    = 0x0F,
    STRING_TYPE_MAX
} StringTypeMask;

#define TYPE_IS_SMALL(type)     ((type & STRING_TYPE_SMALL) != 0)
#define TYPE_IS_MIDIUM(type)    ((type & STRING_TYPE_MEDIUM) != 0)
#define TYPE_IS_LARGE(type)     ((type & STRING_TYPE_LARGE) != 0)
#define TYPE_IS_REF(type)       ((type & STRING_TYPE_REF) != 0)

#define TYPE_NOT_IS_SMALL(type)   \
    ((type & (STRING_TYPE_MEDIUM | STRING_TYPE_LARGE | STRING_TYPE_REF)) != 0)

#define STRING_CORE_CLASSES    \
    class _CharT, class _RefCount
#define STRING_CORE            \
    string_core<_CharT, _RefCount>

template <class _CharT, class _RefCount = refcounted<_CharT, int32_t>>
class JIMI_API string_core
{
public:
    // Types:
    typedef _CharT      char_type;
    typedef _RefCount   refcount_type;
    typedef size_t      size_type;
    typedef uint32_t    flag_type;

public:
    // Contructor
    string_core();
    string_core(const string_core &src);
    string_core(const char *src);
    string_core(const wchar_t *src);
    ~string_core();

    void destroy();

    void retail();
    void release();

    bool is_small()  { return TYPE_IS_SMALL(_type);  }
    bool is_medium() { return TYPE_IS_MIDIUM(_type); }
    bool is_large()  { return TYPE_IS_LARGE(_type);  }
    bool is_ref()    { return TYPE_IS_REF(_type);    }

    bool is_not_small() { return TYPE_NOT_IS_SMALL(_type); }

private:
    typedef struct core_data {
        char_type  *_data;
        size_type   _size;
        size_type   _capacity;
        flag_type   _type;
    } core_data;

public:
    char_type  *_data;
    size_type   _size;
    size_type   _capacity;
    flag_type   _type;
    char_type   _buf[STRING_SMALL_SIZE - sizeof(core_data)];
    int32_t     _refcount;
};

template <STRING_CORE_CLASSES>
STRING_CORE::string_core()
: _data(NULL)
, _size(0)
, _capacity(0)
, _type(0)
, _refcount(1)
{
    // init sso buffer
    (*(uint32_t *)(&_buf[0])) = 0;
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const string_core &src)
{
    jimi_assert(&src != this);

    _size = src._size;
    _type = src._type;
    /* eager copy */
    if (is_medium()) {
        jimi_assert(src._capacity == STRING_MEDIUM_SIZE);
        _capacity = STRING_MEDIUM_SIZE;
        _refcount = 1;
        // init sso buffer
        (*(uint32_t *)(&_buf[0])) = 0;
        _data = char_traits<char>::assign(STRING_MEDIUM_SIZE);
        char_traits<char>::strlcpy(_data, STRING_MEDIUM_SIZE, src._data, src._size);
    }
    /* copy-on-write */
    else if (is_large()) {
        jimi_assert(src._refcount >= 0);
        _data     = src._data;
        _capacity = src._capacity;
        _refcount = 1;
        // init sso buffer
        (*(uint32_t *)(&_buf[0])) = 0;
        const_cast<string_core &>(src).retail();
    }
    /* is_small or unknown type */
    else {
        _data = NULL;
        _capacity = 0;
        _refcount = 1;
        jimi_assert(src._size < STRING_SMALL_SIZE);
        if (_size < STRING_SMALL_SIZE)
            char_traits<char>::strlcpy(&_buf[0], STRING_SMALL_SIZE, &src._buf[0], _size);
        else
            sLog.error("string_core(const string_core &src): _data = %08X, _size = %d.", _data, _size);
    }
}

template <STRING_CORE_CLASSES>
STRING_CORE::string_core(const char *src)
{
    size_t src_len = char_traits<char>::strlen(src);
    if (src_len < STRING_SMALL_SIZE) {
        _data = NULL;       
        _size = src_len;     
        _capacity = 0;
        _type = STRING_TYPE_SMALL;
        char_traits<char>::strncpy(_buf, jm_countof(_buf), src, src_len);
    }
    /* eager copy */
    else if (src_len < STRING_MEDIUM_SIZE) {
        _size = src_len;
        _capacity = calc_capacity(src_len);
        _type = STRING_TYPE_MEDIUM;
        _data = char_traits<char>::assign(_capacity);
        if (_data)
            char_traits<char>::strncpy(_data, _capacity, src, src_len);
    }
    /* copy-on-write or unknown type */
    else {
        _capacity = calc_capacity(src_len);
        _data = char_traits<char>::assign(_capacity);
        _size = src_len;
        _type = STRING_TYPE_LARGE;
        if (_data)
            char_traits<char>::strncpy(_data, _capacity, src, src_len);
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
    if (_store.is_large())
        _type = STRING_TYPE_REF;
}

template <STRING_CORE_CLASSES>
inline void STRING_CORE::release()
{
    --_refcount;
    if (_refcount <= 0 && !is_ref()) {
        if (_data != NULL) {
            delete _data;
            _data = NULL;
        }
        _size = 0;
        _capacity = 0;
        _type = 0;
        _refcount = 0;
    }
}

NS_JIMI_END

#endif  /* _JIMI_LANG_STRING_CORE_H_ */
