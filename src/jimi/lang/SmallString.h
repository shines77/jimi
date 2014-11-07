
#ifndef _JIMI_LANG_SMALL_STRING_H_
#define _JIMI_LANG_SMALL_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/lang/Allocator.h>
#include <jimi/lang/CharTraits.h>
#include <jimic/string/jm_strings.h>
#include <jimi/log/log.h>

#include <string>

NS_JIMI_BEGIN

template <class _CharT = char,
          size_t _Capacity = 32U,
          size_t _Alignment = 8U,
          class _Traits = char_traits<_CharT> >
class JIMI_DLL SmallString
{
public:
    // Types
    typedef _CharT                                  char_type;
    typedef typename _Traits::char_type             value_type;
    typedef _Traits                                 traits_type;
    typedef size_t                                  size_type;
    typedef size_t                                  difference_type;
    typedef char_type &                             reference;
    typedef const char_type &                       const_reference;
    typedef char_type *                             pointer;
    typedef const char_type *                       const_pointer;
#if 0
    typedef normal_iterator<pointer, basic_string>  iterator;
    typedef normal_iterator<const_pointer, basic_string>
                                                    const_iterator;
#else
    typedef char_type *                             iterator;
    typedef const char_type *                       const_iterator;
#endif
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;

    static const size_t _alignment = JIMI_MAX(JIMI_ROUND_TO_POW2(_Alignment), 4);
    static const size_t _capacity  = JIMI_MAX(JIMI_ALIGNED_TO(_Capacity, _alignment), 1);

public:
    // Contructor
    SmallString();
    SmallString(const value_type c);
    SmallString(const value_type c, size_type n);
    SmallString(const value_type *src);
    SmallString(const value_type *src, size_type size);
    SmallString(const value_type *begin, const value_type *end);
    SmallString(const SmallString &src);
    SmallString(const std::basic_string<value_type> &src);

    // Discontructor
    ~SmallString();

public:
    const value_type * c_str() const  { return _data;           }
    const value_type * data() const   { return this->c_str();   }

    const size_type size() const      { return _size;           }
    const size_type length() const    { return this->size();    }

    const size_type alignment() const { return _alignment;      }
    const size_type capacity() const  { return _capacity;       }

protected:
    void init(const value_type *src, size_type size);

private:
#if defined(JIMI_SIZE_T_SIZEOF) && (JIMI_SIZE_T_SIZEOF == 4)
    /* for aligned to 8 bytes in x86 (32bit mode) */
    size_t      _reserve;
#endif
    size_t      _size;
    value_type  _data[_capacity];
};

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString()
: _size(0)
{
    _data[0] = (value_type)'\0';
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const value_type c)
: _size(1)
{
    _data[0] = (value_type)c;
    _data[1] = (value_type)'\0';
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const value_type c, size_type n)
: _size(n)
{
    if (n < _capacity) {
        ::memset(_data, c, n * sizeof(value_type));
        _data[size] = (value_type)'\0';
        _size = size;
    }
    else {
        ::memset(_data, c, (_capacity - 1) * sizeof(value_type));
        _data[_capacity - 1] = (value_type)'\0';
        _size = _capacity - 1;
    }
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const value_type *src)
//: SmallString(src, (size_type)traits_type::length(src))
//: SmallString(src, (size_type)::jm_strlen(src))
//: SmallString(src, (size_type)jm_countof(src))
{
    size_type size = traits_type::length(src);
    init(src, size);
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const value_type *src, size_type size)
{
    init(src, size);
}

// Specialization for const value_type *, const value_type *
template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const value_type *begin, const value_type *end)
: SmallString(begin, end - begin)
{
    jimi_assert(begin <= end);
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const SmallString &src)
: SmallString(src.data(), src.size())
{
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::SmallString(const std::basic_string<value_type> &src)
: SmallString(src.data(), src.size())
{
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
SmallString<_CharT, _Capacity, _Alignment, _Traits>::~SmallString()
{
#if defined(_DEBUG) || !defined(NDEBUG)
    _size = 0;
#endif
}

template <class _CharT, size_t _Capacity, size_t _Alignment, class _Traits>
inline
void SmallString<_CharT, _Capacity, _Alignment, _Traits>::init(const value_type *src, size_type size)
{
    if (size < _capacity) {
        ::memcpy(_data, src, size * sizeof(value_type));
        _data[size] = (value_type)'\0';
        _size = size;
    }
    else {
        ::memcpy(_data, src, (_capacity - 1) * sizeof(value_type));
        _data[_capacity - 1] = (value_type)'\0';
        _size = _capacity - 1;
    }
}

#ifndef small_string
#define small_string(N)     SmallString<char, (N)>
#endif

#ifndef small_wstring
#define small_wstring(N)    SmallString<wchar_t, (N)>
#endif

NS_JIMI_END

#endif  /* !_JIMI_LANG_SMALL_STRING_H_ */
