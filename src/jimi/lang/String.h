
#ifndef _JIMI_LANG_STRING_H_
#define _JIMI_LANG_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
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

std::string str;

template <class _CharT>
class JIMI_API allocator
{
public:
    typedef size_t          size_type;
    typedef size_t          difference_type;
    typedef _CharT &        reference;
    typedef const _CharT &  const_reference;
    typedef _CharT *        pointer;
    typedef const _CharT *  const_pointer;
};

template <class _PtrT, class _T>
class JIMI_API normal_iterator
{
};

namespace string_detail {

/*
 * Lightly structured memcpy, simplifies copying PODs and introduces
 * some asserts. Unfortunately using this function may cause
 * measurable overhead (presumably because it adjusts from a begin/end
 * convention to a pointer/size convention, so it does some extra
 * arithmetic even though the caller might have done the inverse
 * adaptation outside).
 */
template <class Pod>
inline void pod_copy(const Pod *b, const Pod *e, Pod *d) {
    jimi_assert(e >= b);
    jimi_assert(d >= e || d + (e - b) <= b);
    ::memcpy(d, b, (e - b) * sizeof(Pod));
}

}  /* end of the namespace string_detail */

#define BASIC_STRING_CLASSES    \
    class _CharT, class _Traits, class _Alloc, class _RefCount, class _Storage
#define BASIC_STRING            \
    basic_string<_CharT, _Traits, _Alloc, _RefCount, _Storage>

template <class _CharT  = char,
          class _Traits = char_traits<_CharT>,
          class _Alloc  = allocator<_CharT>,
          class _RefCount = refcounted<_CharT, int32_t>,
          class _Storage = string_core<_CharT, _RefCount>>

class JIMI_API basic_string
{
public:
    // Types:
    typedef _CharT                                  value_type;
    typedef typename _Traits::char_type             char_type;
    typedef _Traits                                 traits_type;
    typedef _Alloc                                  allocator_type;
    typedef _Storage                                storage_type;
    typedef typename _Alloc::size_type              size_type;
    typedef typename _Alloc::difference_type        difference_type;
    typedef typename _Alloc::reference              reference;
    typedef typename _Alloc::const_reference        const_reference;
    typedef typename _Alloc::pointer                pointer;
    typedef typename _Alloc::const_pointer          const_pointer;
    typedef normal_iterator<pointer, basic_string>  iterator;
    typedef normal_iterator<const_pointer, basic_string>
                                                    const_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;

public:
    // Contructor
    basic_string();
    basic_string(const basic_string &src);
    basic_string(const char *src);
    basic_string(const wchar_t *src);
    ~basic_string();

    void destroy();

    void retail();
    void release();

    bool equals(const basic_string &rhs);

    bool compare(const basic_string &rhs);
    bool compare(const char_type *rhs);

    char_type *data() const { return c_str(); }
    char_type *c_str() const;

protected:
    size_t calc_capacity(size_t src_len);

protected:
    char_type  *_data;
    size_type   _size;
    size_type   _capacity;
    uint32_t    _type;
    int32_t     _refcount;
    char_type   _buf[STRING_SMALL_SIZE];

    storage_type _store;
};

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string()
: _data(NULL)
, _size(0)
, _capacity(0)
, _type(0)
, _refcount(1)
{
    // init sso buffer
    (*(uint32_t *)(&_buf[0])) = 0;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const basic_string &src)
{
    _size = src._size;
    _type = src._type;
    /* eager copy */
    if (_store.is_medium()) {
        jimi_assert(src._capacity == STRING_MEDIUM_SIZE);
        _capacity = STRING_MEDIUM_SIZE;
        _refcount = 1;
        (*(uint32_t *)(&_buf[0])) = 0;
        _data = char_traits<char>::assign(STRING_MEDIUM_SIZE);
        char_traits<char>::strlcpy(_data, STRING_MEDIUM_SIZE, src._data, src._size);
    }
    /* copy-on-write */
    else if (_store.is_large()) {
        jimi_assert(src._refcount >= 0);
        _data     = src._data;
        _capacity = src._capacity;
        _refcount = 1;
        (*(uint32_t *)(&_buf[0])) = 0;
        const_cast<basic_string &>(src).retail();
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
            sLog.error("basic_string(const basic_string &src): _data = %08X, _size = %d.", _data, _size);
    }

    // init SSO buffer
    _buf[0] = '\0';
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const char *src)
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

template <BASIC_STRING_CLASSES>
BASIC_STRING::~basic_string()
{
    destroy();
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::destroy()
{
    release();
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::retail()
{
    ++_refcount;
    if (_store.is_large())
        _type = STRING_TYPE_REF;
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::release()
{
    --_refcount;
    if (_refcount <= 0 && !_store.is_ref()) {
        if (_data != NULL) {
            delete _data;
            _data = NULL;
        }
        _size = 0;
        _capacity = 0;
        _type = 0;
        _refcount = 0;
    }
    return _refcount;
}

template <BASIC_STRING_CLASSES>
inline size_t BASIC_STRING::calc_capacity(size_t src_len)
{
    return src_len + 1;
}

template <BASIC_STRING_CLASSES>
inline typename BASIC_STRING::char_type *BASIC_STRING::c_str() const
{
    if (_size < STRING_SMALL_SIZE)
        return (char_type *)&_store._buf[0];
    else
        return _store._data;
}

template <BASIC_STRING_CLASSES>
inline bool BASIC_STRING::equals(const BASIC_STRING &rhs)
{
    bool equal = false;
    if (this == &rhs)
        equal = true;
    return equal;
}

template <BASIC_STRING_CLASSES>
bool BASIC_STRING::compare(const BASIC_STRING &rhs)
{
    if (this == &rhs)
        return true;

    bool equal = false;
    char_type *rhs_data;

    if (size == rhs._size) {
        if (flag == rhs._type) {
            if (_store.is_small()) {
                equal = traits_type::strncmp(_buf, rhs._buf, _size);
            }
            else if (_store.is_not_small()) {
                rhs_data = rhs._data;
                if (_data == rhs_data)
                    equal = true;
                else
                    equal = traits_type::strncmp(_data, rhs_data, _size);
            }
            else {
                // Unknown string type
                sLog.error("BASIC_STRING::compare(const BASIC_STRING &rhs) - Error: Unknown string type.");
            }
        }
        else {
            //
        }
    }
    else {
        // Is not same size
        sLog.info("BASIC_STRING::compare(const BASIC_STRING &rhs), size = %d, rhs.size = %d", size, rhs.size);
    }
    return equal;
}

template <BASIC_STRING_CLASSES>
bool BASIC_STRING::compare(const char_type *rhs)
{
    bool equal = false;
    if (_store.is_small()) {
        if (buf == rhs)
            equal = true;
        else
            equal = traits_type::strncmp(_buf, rhs, _size);
    }
    else if (_store.is_not_small()) {
        if (data == rhs)
            equal = true;
        else
            equal = traits_type::strncmp(_data, rhs, _size);
    }
    else {
        // Unknown string type
        sLog.error("BASIC_STRING::compare(const char_type *rhs) - Error: Unknown string type.");
    }
    return equal;
}

template <BASIC_STRING_CLASSES>
inline bool operator == (const BASIC_STRING &lhs, const BASIC_STRING &rhs)
{
    return lhs.compare(rhs);
}

template <BASIC_STRING_CLASSES>
inline bool operator == (const BASIC_STRING &lhs, const _CharT *rhs)
{
    return lhs.compare(rhs);
}

template <BASIC_STRING_CLASSES>
inline bool operator == (const _CharT *lhs, const BASIC_STRING &rhs)
{
    return rhs.compare(lhs);
}

typedef basic_string<char> String;

NS_JIMI_END

#endif  /* _JIMI_LANG_STRING_H_ */
