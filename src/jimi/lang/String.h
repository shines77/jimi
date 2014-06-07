
#ifndef _JIMI_LANG_STRING_H_
#define _JIMI_LANG_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimi/lang/char_traits.h>
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
class allocator
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
class normal_iterator
{
};

#define BASIC_STRING_CLASSES    class _CharT, class _Traits, class _Alloc
#define BASIC_STRING            basic_string<_CharT, _Traits, _Alloc>

//#define STRING_SSO_SIZE         (128 - 5 * sizeof(size_t))
#define STRING_SSO_SIZE         32

#define STRING_ECP_SIZE         256

typedef enum StringTypeMask
{
    STRING_TYPE_SSO     = 0x01,
    STRING_TYPE_ECP     = 0x02,
    STRING_TYPE_COW     = 0x04,
    STRING_TYPE_MASK    = 0x07,
    STRING_TYPE_MAX
} StringTypeMask;

#define TYPE_IS_SSO(flag)       ((flag & STRING_TYPE_SSO) != 0)
#define TYPE_IS_ECP(flag)       ((flag & STRING_TYPE_ECP) != 0)
#define TYPE_IS_COW(flag)       ((flag & STRING_TYPE_COW) != 0)

#define TYPE_NOT_IS_SSO(flag)   ((flag & (STRING_TYPE_ECP | STRING_TYPE_COW)) != 0)

template < class _CharT  = char,
           class _Traits = char_traits<_CharT>,
           class _Alloc  = allocator<_CharT> >
class JIMI_API basic_string
{
public:
    // Types:
    typedef _CharT                                  value_type;
    typedef typename _Traits::char_type             char_type;
    typedef _Traits                                 traits_type;
    typedef _Alloc                                  allocator_type;
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

    bool equals(const basic_string &rhs);

    bool compare(const basic_string &rhs);
    bool compare(const char_type *rhs);

    bool is_type_sso()  { return TYPE_IS_SSO(_flag); }
    bool is_type_ecp()  { return TYPE_IS_ECP(_flag); }
    bool is_type_cow()  { return TYPE_IS_COW(_flag); }

    bool is_not_sso()   { return TYPE_NOT_IS_SSO(_flag); }

    char_type *c_str() const;

protected:
    size_t calc_capacity(size_t src_len);

protected:
    char_type  *_data;
    size_type   _size;
    size_type   _capacity;
    uint32_t    _flag;
    int32_t     _refcount;
    char_type   _buf[STRING_SSO_SIZE];
};

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string()
: _data(NULL)
, _size(0)
, _capacity(0)
, _flag(0)
, _refcount(0)
{
    // init SSO buffer
    _buf[0] = '\0';
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const basic_string &src)
{
    this->_data      = src._data;
    this->_size      = src._size;
    this->_capacity  = src._capacity;
    this->_flag      = src._flag;
    this->_refcount  = src._refcount;

    // init SSO buffer
    _buf[0] = '\0';
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const char *src)
{
    size_t src_len = char_traits<char>::strlen(src);
    if (src_len < STRING_SSO_SIZE) {
        _flag = STRING_TYPE_SSO;
        _size = src_len;
        char_traits<char>::strncpy(_buf, jm_countof(_buf), src, src_len);

        _data = NULL;
        _capacity = 0;
    }
    else if (src_len < STRING_ECP_SIZE) {
        _flag = STRING_TYPE_SSO;
        _capacity = calc_capacity(src_len);
        _data = char_traits<char>::assign(_capacity);
        _size = src_len;
        if (_data)
            char_traits<char>::strncpy(_data, _capacity, src, src_len);
    }
    else {
        _flag = STRING_TYPE_COW;
        _capacity = calc_capacity(src_len);
        _data = char_traits<char>::assign(_capacity);
        _size = src_len;
        if (_data)
            char_traits<char>::strncpy(_data, _capacity, src, src_len);
    }
    _refcount = 0;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::~basic_string()
{
    destroy();
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::destroy()
{
    _refcount--;
    if (_refcount <= 0) {
        if (_data != NULL) {
            delete _data;
            _data = NULL;
        }
        _size = 0;
        _capacity = 0;
        _flag = 0;
        _refcount = 0;
    }
}

template <BASIC_STRING_CLASSES>
inline size_t BASIC_STRING::calc_capacity(size_t src_len)
{
    return src_len + 1;
}

template <BASIC_STRING_CLASSES>
inline typename BASIC_STRING::char_type *BASIC_STRING::c_str() const
{
    if (_size < STRING_SSO_SIZE)
        return (char_type *)&_buf[0];
    else
        return _data;
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
        if (flag == rhs._flag) {
            if (is_type_sso()) {
                equal = traits_type::strncmp(_buf, rhs._buf, _size);
            }
            else if (is_not_sso()) {
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
    if (is_type_sso()) {
        if (buf == rhs)
            equal = true;
        else
            equal = traits_type::strncmp(_buf, rhs, _size);
    }
    else if (is_not_sso()) {
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
