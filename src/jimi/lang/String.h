
#ifndef _JIMI_LANG_STRING_H_
#define _JIMI_LANG_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_def.h"
#include "jimi/lang/Allocator.h"
#include "jimi/lang/CharTraits.h"
#include "jimi/lang/RefCounted.h"
#include "jimi/lang/StringCore.h"
#include "jimi/lang/StringDetail.h"
#include "jimi/util/DumpUtils.h"
#include "jimic/string/jm_strings.h"
#include "jimi/log/log.h"

#include <vadefs.h>
#include <string>
#include <limits>

#if (defined(_MSC_VER) && (_MSC_VER >= 1700)) || defined(__GNUC__)
#include <functional>   // For std::less_equal, std::bind2nd
#endif

namespace jimi {

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

template <class _PtrT, class _T>
class JIMI_DLL normal_iterator
{
};

#undef BASIC_STRING_CLASSES
#undef BASIC_STRING

#define BASIC_STRING_CLASSES    \
    class _CharT, class _Traits, class _Alloc, class _Storage

#define BASIC_STRING            \
    basic_string<_CharT, _Traits, _Alloc, _Storage>

template <class _CharT   = char,
          class _Traits  = char_traits<_CharT>,
          class _Alloc   = allocator<_CharT>,
          class _Storage = string_core<_CharT> >

class JIMI_DLL basic_string
{
public:
    // Types
    typedef _CharT                                  char_type;
    typedef typename _Traits::char_type             value_type;
    typedef _Traits                                 traits_type;
    typedef _Alloc                                  allocator_type;
    typedef _Storage                                storage_type;
    typedef typename _Alloc::size_type              size_type;
    typedef typename _Alloc::difference_type        difference_type;
    typedef typename _Alloc::reference              reference;
    typedef typename _Alloc::const_reference        const_reference;
    typedef typename _Alloc::pointer                pointer;
    typedef typename _Alloc::const_pointer          const_pointer;
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

public:
    // Contructor
    basic_string();
    basic_string(const basic_string &src);
    basic_string(const size_t _capacity);
    basic_string(const value_type c);
    basic_string(const value_type c, size_type n);
    basic_string(const value_type *src);
    basic_string(const value_type *src, size_type size);
    basic_string(const value_type *begin, const value_type *end);
    basic_string(const std::string &src);

#if defined(JIMI_HAS_CXX11_MOVE_FUNCTIONS) && (JIMI_HAS_CXX11_MOVE_FUNCTIONS != 0)
    // Move constructor
    basic_string(basic_string && goner)
        : _store(std::move(goner._store)) {
        // do nothing!
    }

    // Move assignment
    basic_string & operator = (basic_string && goner) {
        if (JIMI_UNLIKELY(&goner == this)) {
            // Compatibility with std::basic_string<>,
            // C++11 21.4.2 [string.cons] / 23 requires self-move-assignment support.
            return *this;
        }
        // No need of this anymore
        this->~basic_string();
        // Move the goner into this
        new (&_store) string_core<_CharT>(std::move(goner._store));
        return *this;
    }
#endif

    // Discontructor
    ~basic_string();

    void dump();

    // Assigment operators
    basic_string &operator = (const basic_string &rhs);
    basic_string &operator = (const value_type c);
    basic_string &operator = (const value_type *str);

    // Compatibility with std::string
    basic_string &operator = (const std::string &rhs);

protected:
    // Assigment functions
    basic_string &assign(const basic_string &str);
    basic_string &assign(const value_type c);
    basic_string &assign(const value_type c, size_type n);
    basic_string &assign(const value_type *str);
    basic_string &assign(const value_type *str, const size_type size);

public:
    // Append operators
    basic_string &append(const basic_string &str);
    basic_string &append(const value_type c);
    basic_string &append(const value_type c, size_type n);
    basic_string &append(const value_type *s);
    basic_string &append(const value_type *s, size_type n);

    basic_string &append(const int n);
    basic_string &append(const unsigned int u);
    basic_string &append(const long l);
    basic_string &append(const unsigned long ul);
    basic_string &append(const int64_t i64);   
    basic_string &append(const uint64_t u64);
    basic_string &append(const float f);
    basic_string &append(const double d);

    basic_string &append_hex(const uint32_t hex32, const bool isUpper = true);
    basic_string &append_hex(const uint64_t hex64, const bool isUpper = true);

    basic_string &append_format_c(const value_type *fmt, ...);
    basic_string &format_c(const value_type *fmt, const value_type *args, ...);

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)
    template<typename ...Args>
    basic_string &format(const value_type *fmt, Args const ... args);

    template<typename ...Args>
    basic_string &append_format(Args const & ... args);
#else
    basic_string &format(const value_type *fmt, ...) { return *this; }

    basic_string &append_format(...) { return *this; }
#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */

private:
#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)
    template<typename T, typename ...Args>
    void append_format_next(const T & value);

    template<typename T, typename ...Args>
    void append_format_next(const T & value, Args const ... args);
#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */

public:
    storage_type &getStorage() { return _store; }

    void push_back(const value_type c);         // primitive
    void pop_back();

    // C++11 21.4.6 modifiers:
    // Concatenation
    basic_string &operator += (const basic_string &rhs);
    basic_string &operator += (const value_type c);
    basic_string &operator += (const value_type *str);
    basic_string &operator += (value_type *str);

#if 0
    // Equality
    bool operator == (const basic_string &rhs) const;
    bool operator == (const value_type c) const;
    bool operator == (const value_type *str) const;
    bool operator == (value_type *str) const;

    // Comparison
    bool operator <  (const basic_string &rhs) const;
    bool operator <= (const basic_string &rhs) const;
    bool operator >  (const basic_string &rhs) const;
    bool operator >= (const basic_string &rhs) const;

    // Inequality
    bool operator != (const basic_string &rhs) const;
    bool operator != (const value_type c) const;
    bool operator != (const value_type *str) const;
    bool operator != (value_type *str) const;

#endif

    // C++11 21.4.3 iterators:
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    // Added by C++11
    // C++11 21.4.5, element access:
    const value_type &front() const;
    const value_type &back() const;

    value_type &front();
    value_type &back();

    // C++11 21.4.5 element access:
    const_reference operator [](size_type pos) const;
    reference operator [](size_type pos);

    void retain();
    void release();

    bool equals(const basic_string &rhs) const;

    int compare(const basic_string &rhs) const;
    int compare(const value_type *rhs) const;

    //const storage_type &getStorage() const { return _store; }

    const value_type *c_str() const { return _store.c_str(); }
    const value_type *data() const  { return c_str(); }
    const value_type *toString() const { return c_str(); }

    std::string toStdString() const;

    bool empty() const          { return size() == 0; }
    size_type length() const    { return size(); }
    size_type size() const      { return _store.size(); }
    size_type capacity() const  { return _store.capacity(); }

    // 为了提高效率, 允许 swap() 对象本身
    void swap(basic_string &rhs) { _store.swap(rhs._store); }
    // 但是 stole() 自己是不允许的..., 否则会把自己偷空...
    void stole(basic_string &rhs) { if (this != &rhs) _store.stole(rhs._store); }

    void clear() { resize(0); }

    size_type max_size() const { return std::numeric_limits<size_type>::max(); }

    void reserve(const size_type newCapacity);
    void reallocate(const size_type newSize);
    void resize(const size_type newSize, const value_type c = value_type(), bool needInit = true);

private:
    void destroy();

protected:
    storage_type _store;
};

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string()
: _store()
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const basic_string &src)
: _store(src._store)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const size_t _capacity)
: _store(_capacity)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const std::string &src)
: _store(src.data(), src.size())
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const value_type c)
: _store(c)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const value_type c, size_type n)
: _store(c, n)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const value_type *src)
: _store(src, traits_type::length(src))
//: _store(src, ::jm_strlen(src))
//: _store(src)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const value_type *src, size_type size)
: _store(src, size)
{
}

// Specialization for const value_type *, const value_type *
template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const value_type *begin, const value_type *end)
: store_(begin, end - begin)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::~basic_string()
{
    destroy();
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::dump()
{
    size_t size = sizeof(*this);
    ReleaseUtils::dump((void *)this, size, false);
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::assign(const BASIC_STRING &str)
{
    if (&str == this)
        return *this;
    return assign(str.data(), str.size());
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::assign(const value_type c)
{
    _store.assign(c);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::assign(const value_type c, size_type n)
{
    _store.assign(c, n);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::assign(const value_type *s)
{
    return assign(s, traits_type::length(s));
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::assign(const value_type *s, const size_type n)
{
    size_type _size = size();
    if (_size >= n) {
        std::copy(s, s + n, begin());
        resize(n);
        jimi_assert(size() == n);
        _store.writeNullForce();
    }
    else {
        const value_type * const s_end = s + _size;
        std::copy(s, s_end, begin());
        append(s_end, n - _size);
        jimi_assert(size() == n);
    }
    jimi_assert(size() == n);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator = (const BASIC_STRING &rhs)
{
    if (&rhs == this)
        return *this;

    const size_type oldSize = size();
    const size_type srcSize = rhs.size();
    if (capacity() >= srcSize && !_store.is_shared()) {
        // great, just copy the contents
        if (oldSize < srcSize)
            _store.expandTo(srcSize);
        else
            _store.shrinkTo(srcSize);
        //_store = rhs._store;
        //_store._ml = rhs._store._ml;
        jimi_assert(size() == srcSize);
        jimi_assert(begin() != rhs.begin());
        string_detail::pod_copy(begin(), rhs.begin(), srcSize + 1);
        //_store.writeNull();
    }
    else {
        // need to reallocate, so we may as well create a brand new string
        basic_string(rhs).swap(*this);
    }
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator = (const value_type *str)
{
    return assign(str);
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator = (const value_type c)
{
    return assign(c);
}

// Compatibility with std::string
template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator = (const std::string &rhs)
{
    return assign(rhs.data(), rhs.size());
}

// Compatibility with std::string
template <BASIC_STRING_CLASSES>
std::string BASIC_STRING::toStdString() const
{
    return std::string(data(), size());
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const basic_string &str)
{
#if defined(_DEBUG) || !defined(NDEBUG)
    size_type nDesiredSize = size() + str.size();
#endif
    append(str.data(), str.size());
    jimi_assert(size() == nDesiredSize);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const value_type c)
{
    push_back(c);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const value_type c, size_type n)
{
#if 0
    resize(size() + n, c);
#else
    char_type *oldDataEnd = _store.expand_noinit(n);
    string_detail::pod_fill(oldDataEnd, c, n);
    // No need to call writeNull(), we have done it within expand_noinit().
#endif
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const value_type *s)
{
    return append(s, traits_type::length(s));
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const value_type *s, size_type n)
{
    if (JIMI_UNLIKELY(!n)) {
        // Unlikely but must be done
        return *this;
    }

    size_type oldSize = size();
    const value_type *oldData = data();
    // Check for aliasing (rare). We could use "<=" here but in theory
    // those do not work for pointers unless the pointers point to
    // elements in the same array. For that reason we use
    // std::less_equal, which is guaranteed to offer a total order
    // over pointers. See discussion at http://goo.gl/Cy2ya for more
    // info.
    std::less_equal<const value_type *> le;
    if (JIMI_UNLIKELY(le(oldData, s) && !le(oldData + oldSize, s))) {
        jimi_assert(le(s + n, oldData + oldSize));
        const size_type offset = s - oldData;
        _store.reserve(oldSize + n);
        // Restore the source
        s = data() + offset;
    }
#if 0
    // Warning! Repeated appends with short strings may actually incur
    // practically quadratic performance. Avoid that by pushing back
    // the first character (which ensures exponential growth) and then
    // appending the rest normally. Worst case the append may incur a
    // second allocation but that will be rare.
    push_back(*s++);
    _store.expandTo_noinit(oldSize + n);
    --n;
    if (n > 0) {
        ::memcpy((void *)(data() + oldSize + 1), s, n * sizeof(value_type));
    }
    _store.writeNullForce();
#else
    _store.expand_and_copy(s, n);
    //::memcpy((void *)(data() + oldSize), s, n * sizeof(value_type));
    //_store.writeNull();
#endif
    jimi_assert(size() == oldSize + n);
    return *this;
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::push_back(const value_type c)
{
    _store.push_back(c);
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::pop_back()
{
    jimi_assert(!empty());
    _store.shrinkTo(size() - 1);
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const int n)
{
    _store.append(n);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const unsigned int u)
{
    _store.append(u);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const long l)
{
    _store.append(l);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const unsigned long ul)
{
    _store.append(ul);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const int64_t i64)
{
    _store.append(i64);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const uint64_t u64)
{
    _store.append(u64);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const float f)
{
    _store.append(f);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append(const double d)
{
    _store.append(d);
    return *this;
}

JIMI_FORCEINLINE
int hex_to_string(const char *buf, uint32_t hex32)
{
    jimi_assert(buf != 0);
    return 10;
}

JIMI_FORCEINLINE
int hex_to_string(const char *buf, uint64_t hex64)
{
    jimi_assert(buf != 0);
    return 18;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append_hex(uint32_t hex32,
                                       const bool isUpper /* = true */)
{
    _store.append_hex(hex32, isUpper);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::append_hex(uint64_t hex64,
                                       const bool isUpper /* = true */)
{
    _store.append_hex(hex64, isUpper);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator += (const basic_string &rhs)
{
    return append(str);
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator += (const value_type c)
{
    push_back(c);
    return *this;
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator += (const value_type *str)
{
    return append(s);
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator += (value_type *str)
{
    return append(s);
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::reserve(const size_type newCapacity)
{
    _store.reserve(newCapacity);
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::reallocate(const size_type newSize)
{
    size_type oldSize = this->size();
    if (newSize <= oldSize) {
        _store.shrinkTo(newSize);
    }
    else {
        size_type capacity = this->capacity();
        jimi_assert(capacity >= oldSize);
        if (newSize <= capacity)
            _store.expandTo_noinit(newSize);
        else
            _store.reallocate(newSize);
    }
    jimi_assert(this->size() == newSize);
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::resize(const size_type newSize, const value_type c /* = value_type() */,
                          bool needInit /* = true */)
{
    size_type oldSize = size();
    if (newSize <= oldSize) {
        _store.shrinkTo(newSize);
    }
    else {
        size_type capacity = this->capacity();
        jimi_assert(capacity >= oldSize);
        value_type *oldDataEnd = _store.expandTo_noinit(newSize);
        if (needInit) {
            string_detail::pod_fill(oldDataEnd, c, newSize - oldSize);
            // No need to call writeNull(), we have done it within expandTo_noinit().
            //_store.writeNullForce();
        }
    }
    jimi_assert(size() == newSize);
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::iterator BASIC_STRING::begin()
{
    return _store.mutable_data();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_iterator BASIC_STRING::begin() const
{
    return _store.data();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_iterator BASIC_STRING::cbegin() const
{
    return _store.mutable_data() + _store.size();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::iterator BASIC_STRING::end()
{
    return _store.mutable_data();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_iterator BASIC_STRING::end() const
{
    return _store.data() + _store.size();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_iterator BASIC_STRING::cend() const
{
    return end();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::reverse_iterator BASIC_STRING::rbegin()
{
    return reverse_iterator(end());
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_reverse_iterator BASIC_STRING::rbegin() const
{
    return const_reverse_iterator(end());
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_reverse_iterator BASIC_STRING::crbegin() const
{
    return rbegin();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::reverse_iterator BASIC_STRING::rend()
{
    return reverse_iterator(begin());
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_reverse_iterator BASIC_STRING::rend() const
{
    return const_reverse_iterator(begin());
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_reverse_iterator BASIC_STRING::crend() const
{
    return rend();
}

// Added by C++11
// C++11 21.4.5, element access:
template <BASIC_STRING_CLASSES>
const typename BASIC_STRING::value_type &BASIC_STRING::front() const
{
    return *begin();
}

template <BASIC_STRING_CLASSES>
const typename BASIC_STRING::value_type &BASIC_STRING::back() const
{
    jimi_assert(!empty());
    // Should be begin()[size() - 1], but that branches twice
    return *(end() - 1);
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::value_type &BASIC_STRING::front()
{
    return *begin();
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::value_type &BASIC_STRING::back()
{
    jimi_assert(!empty());
    // Should be begin()[size() - 1], but that branches twice
    return *(end() - 1);
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::const_reference BASIC_STRING::operator [](size_type pos) const
{
    return *(c_str() + pos);
}

template <BASIC_STRING_CLASSES>
typename BASIC_STRING::reference BASIC_STRING::operator [](size_type pos)
{
#if 0
    if (pos == size()) {
        // Just call c_str() to make sure '\0' is present
        c_str();
    }
#endif
    return *(begin() + pos);
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::destroy()
{
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::retain()
{
    _store.retain();
}

template <BASIC_STRING_CLASSES>
void BASIC_STRING::release()
{
    _store.release();
}

template <BASIC_STRING_CLASSES>
bool BASIC_STRING::equals(const BASIC_STRING &rhs) const
{
    if (&rhs == this)
        return true;
    else
        return _store.equals(rhs._store);
}

template <BASIC_STRING_CLASSES>
int BASIC_STRING::compare(const BASIC_STRING &rhs) const
{
    if (&rhs == this)
        return 0;
    else
        return _store.compare(rhs._store);
}

template <BASIC_STRING_CLASSES>
int BASIC_STRING::compare(const value_type *rhs) const
{
    return _store.compare(rhs);
}

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)

#if 1

template <typename StringType, typename T, typename ... Args>
inline void format_next_args(StringType * arg_list, const T & value)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
    }
}

template <typename StringType, typename T, typename ... Args>
inline void format_next_args(StringType * arg_list, const T & value, Args const ... args)
{
    jimi_assert(arg_list != NULL);
    if (arg_list) {
        new (arg_list)StringType();
        arg_list->append(value);
        arg_list++;
        format_next_args(arg_list, args...);
    }
}

#else

template <typename StringType, typename T, typename ... Args>
inline void format_next_args(const StringType * arg_list, size_t & index, const T & value)
{
    StringType * arg_value = arg_list + index;
    jimi_assert(arg_value != NULL);
    if (arg_value) {
        new (arg_value)jimi::string();
        arg_value->append(value);
    }
}

template <typename StringType, typename T, typename ... Args>
inline void format_next_args(const StringType * arg_list, size_t & index, const T & value, Args const ... args)
{
    StringType * arg_value = arg_list + index;
    jimi_assert(arg_value != NULL);
    if (arg_value) {
        new (arg_value)jimi::string();
        arg_value->append(value);
        index++;
        format_next_args(arg_list, index, args...);
    }
}

#endif

#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */

/* format() 是否使用栈上分配 arg_list 内存? */
#undef  FORMAT_USE_ALLOCA_ONSTACK
#define FORMAT_USE_ALLOCA_ONSTACK       0

#if defined(JIMI_HAS_CXX11_VARIADIC_TEMPLATES) && (JIMI_HAS_CXX11_VARIADIC_TEMPLATES != 0)

template <BASIC_STRING_CLASSES>
template <typename ...Args>
#if defined(FORMAT_USE_ALLOCA_ONSTACK) && (FORMAT_USE_ALLOCA_ONSTACK != 0)
JIMI_INLINE
#else
JIMI_FORCEINLINE
#endif
BASIC_STRING &BASIC_STRING::format(const value_type *fmt, Args const ... args)
{
    int delta = 0;
    size_t index;
    value_type c;
    value_type *cur, *first = NULL;
    basic_string *arg_list, *cur_arg = NULL;
#ifdef _DEBUG
    size_t oldSize = size();
#endif

    jimi_assert(fmt != NULL);
#if 1
    if (fmt == NULL)
        return *this;
#endif

    const size_t max_args = sizeof...(args);
#if defined(FORMAT_USE_ALLOCA_ONSTACK) && (FORMAT_USE_ALLOCA_ONSTACK != 0)
    arg_list = (basic_string *)alloca(max_args * sizeof(basic_string));
#else
    arg_list = (basic_string *)malloc(max_args * sizeof(basic_string));
#endif

    format_next_args(arg_list, args...);

    index = 0;
    cur = const_cast<value_type *>(fmt);
    while ((c = *cur++) != '\0') {
        if (c == static_cast<value_type>('{')) {
#if 0
            // the number string first pos
            first = cur - 1;
#endif
            c = *cur++;
            if (c == static_cast<value_type>('{')) {
                append(c);
            }
            else if (c >= static_cast<value_type>('0') && c <= static_cast<value_type>('9')) {
                index = c - '0';
                while ((c = *cur++) != '\0') {
                    if (c == static_cast<value_type>('}')) {
                        // the end of number
                        if (index < max_args)
                            append(*(arg_list + index));
                        break;
                    }
                    else if (c >= static_cast<value_type>('0') && c <= static_cast<value_type>('9')) {
                        index = index * 10 + (c - static_cast<value_type>('0'));
                    }
#if 1
                    else {
                        // get a error number
                        break;
                    }
#else
                    else if (c == static_cast<value_type>(' ')) {
                        // do nothing!
                    }
                    else {
                        // get a error number, if not found "}" behind 16 bytes, append as a string
                        while ((c = *cur++) != '\0') {
                            if (c == static_cast<value_type>('}')) {
                                append(first, cur - first + 1);
                                break;
                            }
                            else {
                                if ((cur - first) >= 15) {
                                    append(first, cur - first + 1);
                                    break;
                                }
                            }
                        }
                        break;
                    }
#endif
                }
            }
        }
        else {
            append(c);
        }
    }

#if defined(FORMAT_USE_ALLOCA_ONSTACK) && (FORMAT_USE_ALLOCA_ONSTACK != 0)
    cur_arg = arg_list;
    for (size_t i = 0; i < max_args; ++i) {
        cur_arg->~basic_string();
        cur_arg++;
    }
#else
    if (arg_list) {
        free(arg_list);
    }
#endif

#ifdef _DEBUG
    delta = size() - oldSize;
#endif
    return *this;
}

template <BASIC_STRING_CLASSES>
template <typename T, typename ... Args>
JIMI_INLINE
void BASIC_STRING::append_format_next(const T & value)
{
    append(value);
}

template <BASIC_STRING_CLASSES>
template <typename T, typename ... Args>
JIMI_INLINE
void BASIC_STRING::append_format_next(const T & value, Args const ... args)
{
    append(value);
    append_format_next(args...);
}

template <BASIC_STRING_CLASSES>
template <typename ...Args>
JIMI_FORCEINLINE
BASIC_STRING &BASIC_STRING::append_format(Args const & ... args)
{
    int delta = 0;
#ifdef _DEBUG
    size_t oldSize = size();
#endif

    append_format_next(args...);

#ifdef _DEBUG
    delta = size() - oldSize;
#endif
    return *this;
}

#endif  /* JIMI_HAS_CXX11_VARIADIC_TEMPLATES */

#define USE_STRING_ARRAY            0
#define USE_PLACEMENT_NEW           1

#if defined(USE_STRING_ARRAY) && (USE_STRING_ARRAY != 0)

template <BASIC_STRING_CLASSES>
JIMI_INLINE
BASIC_STRING &BASIC_STRING::format_c(const value_type *format, const value_type *args, ...)
{
    int delta = 0;
    int offset = 0;
    int index, first;
    value_type c;
    size_t oldSize;
    va_list arg_list;
    int guess_delta = 0;
    basic_string strArgList[4];
    oldSize = size();

    if (fmt == NULL || args == NULL)
        return *this;

    //size_t args_len = jm_strlen(args);
    //jimi::string strArgList[args_len / 3 + 1];

    index = 0;
    va_start(arg_list, args);
    while ((c = args[offset++]) != '\0') {
        if (c == '%') {
            c = args[offset++];
            if (c >= 'a' && c <= 'z') {
                if (c == 'd') {
                    // int
                    int n = static_cast<int>(va_arg(arg_list, int));
                    guess_delta += 11;
                    strArgList[index++].append(n);
                }
                else if (c == 's') {
                    // string
                    char * s = static_cast<char *>(va_arg(arg_list, char *));
                    size_t len = jm_strlen(s);
                    guess_delta += len;
                    strArgList[index++].append(s, len);
                }
                else if (c == 'f') {
                    // float or double
                    double d = static_cast<double>(va_arg(arg_list, double));
                    guess_delta += 20;
                    strArgList[index++].append(d);
                }
                else if (c == 'x') {
                    // hex(uint32_t)
                    uint32_t hex = static_cast<uint32_t>(va_arg(arg_list, uint32_t));
                    guess_delta += 10;
                    int hex_len = strArgList[index++].append_hex(hex);
                }
                else if (c == 'c') {
                    // char
                    char c = static_cast<char>(va_arg(arg_list, char));
                    guess_delta += 4;
                    strArgList[index++].append(c);
                }
                else if (c == 'u') {
                    // unsigned int
                    uint32_t u = static_cast<uint32_t>(va_arg(arg_list, uint32_t));
                    guess_delta += 10;
                    strArgList[index++].append(u);
                }
                else if (c == 'l') {
                    // long
                    long l = static_cast<long>(va_arg(arg_list, long));
                    guess_delta += 11;
                    strArgList[index++].append(l);
                }
                else if (c == 'i') {
                    // __int64(long long)
                    int64_t i64 = static_cast<int64_t>(va_arg(arg_list, int64_t));
                    guess_delta += 21;
                    strArgList[index++].append(i64);
                }
#if 0
                else if (c == 'g') {
                    // float
                    float f = static_cast<float>(va_arg(arg_list, float));
                    guess_delta += 20;
                    strArgList[index++].append(f);
                }
                else if (c == 'b') {
                    // bool
                    bool b = static_cast<bool>((va_arg(arg_list, int) != 0));
                    guess_delta += 1;
                    strArgList[index++].append(b);
                }
                else if (c == 'p') {
                    // pointer(void *)
                    void * p = static_cast<void *>(va_arg(arg_list, void *));
                    if (sizeof(void *) == 8)
                        guess_delta += 18;
                    else
                        guess_delta += 10;
                    strArgList[index++].append(p);
                }
#endif
                else {
                    // unknown format
                }
            }
            else if (c == 'X') {
                // hex(uint64_t)
                uint64_t hex = static_cast<uint64_t>(va_arg(arg_list, uint64_t));
                guess_delta += 18;
                int hex_len = strArgList[index++].append_hex(hex);
            }
        }
    }
    va_end(arg_list);

    int maxArgs = index;
    offset = 0;
    while ((c = format[offset++]) != '\0') {
        if (c == '{') {
            // number string first pos
            first = offset - 1;
            c = format[offset++];
            if (c == '{') {
                append(c);
            }
            else if (c >= '0' && c <= '9') {
                index = c - '0';
                while ((c = format[offset++]) != '\0') {
                    if (c == '}') {
                        // end of one number
                        if (index < maxArgs)
                            append(strArgList[index]);
                        break;
                    }
                    else if (c >= '0' && c <= '9') {
                        index = index * 10 + (c - '0');
                    }
#if 1
                    else {
                        // Get a error num
                        break;
                    }
#else
                    else if (c == ' ') {
                        // do nothing!
                    }
                    else {
                        // Get a error num, if not found "}" behind 16 bytes, append as a string
                        while ((c = format[offset++]) != '\0') {
                            if (c == '}') {
                                append(format + first, offset - first + 1);
                                break;
                            }
                            else {
                                if ((offset - first) >= 15) {
                                    append(format + first, offset - first + 1);
                                    break;
                                }
                            }
                        }
                        break;
                    }
#endif
                }
            }
        }
        else {
            append(c);
        }
    }

    delta = size() - oldSize;
    return *this;
}

#elif defined(USE_PLACEMENT_NEW) && (USE_PLACEMENT_NEW != 0)

template <BASIC_STRING_CLASSES>
JIMI_INLINE
BASIC_STRING &BASIC_STRING::format_c(const value_type *fmt, const value_type *args, ...)
{
    int delta = 0;
    int offset = 0;
    int index, first;
    value_type c;
    size_t oldSize;
    va_list arg_list;
    int guess_delta = 0;
    basic_string *strArgList, *strArg;
    oldSize = size();

    if (fmt == NULL || args == NULL)
        return *this;

    size_t args_len = jm_strlen(args);
    //strArgList = (basic_string *)malloc((args_len / 3 + 1) * sizeof(basic_string));
    strArgList = (basic_string *)alloca((args_len / 3 + 1) * sizeof(basic_string));
    strArg = strArgList;

    index = 0;
    va_start(arg_list, args);
    while ((c = args[offset++]) != '\0') {
        if (c == '%') {
            c = args[offset++];
            if (c >= 'a' && c <= 'z') {
                if (c == 'd') {
                    // int
                    int n = static_cast<int>(va_arg(arg_list, int));
                    guess_delta += 11;
                    new (strArg)basic_string();
                    strArg->append(n);
                    strArg++;
                    index++;
                }
                else if (c == 's') {
                    // string
                    char * s = static_cast<char *>(va_arg(arg_list, char *));
                    size_t len = jm_strlen(s);
                    guess_delta += len;
                    new (strArg)basic_string(s, len);
                    //strArg->append(s, len);
                    strArg++;
                    index++;
                }
                else if (c == 'f') {
                    // float or double
                    double d = static_cast<double>(va_arg(arg_list, double));
                    guess_delta += 20;
                    strArgList[index++].append(d);
                }
                else if (c == 'x') {
                    // hex(uint32_t)
                    uint32_t hex = static_cast<uint32_t>(va_arg(arg_list, uint32_t));
                    guess_delta += 10;
                    strArgList[index++].append_hex(hex);
                }
                else if (c == 'c') {
                    // char
                    char c = static_cast<char>(va_arg(arg_list, int));
                    guess_delta += 4;
                    strArgList[index++].append(c);
                }
                else if (c == 'u') {
                    // unsigned int
                    uint32_t u = static_cast<uint32_t>(va_arg(arg_list, uint32_t));
                    guess_delta += 10;
                    strArgList[index++].append(u);
                }
                else if (c == 'l') {
                    // long
                    long l = static_cast<long>(va_arg(arg_list, long));
                    guess_delta += 11;
                    strArgList[index++].append(l);
                }
                else if (c == 'i') {
                    // __int64(long long)
                    int64_t i64 = static_cast<int64_t>(va_arg(arg_list, int64_t));
                    guess_delta += 21;
                    strArgList[index++].append(i64);
                }
#if 0
                else if (c == 'g') {
                    // float
                    float f = static_cast<float>(va_arg(arg_list, double));
                    guess_delta += 20;
                    strArgList[index++].append_float(f);
                }
                else if (c == 'b') {
                    // bool
                    bool b = static_cast<bool>((va_arg(arg_list, int) != 0));
                    guess_delta += 1;
                    strArgList[index++].append(b);
                }
                else if (c == 'p') {
                    // pointer(void *)
                    void * p = static_cast<void *>(va_arg(arg_list, void *));
                    if (sizeof(void *) == 8)
                        guess_delta += 18;
                    else
                        guess_delta += 10;
                    strArgList[index++].append(p);
                }
#endif
                else {
                    // unknown format
                }
            }
            else if (c == 'X') {
                // hex(uint64_t)
                uint64_t hex = static_cast<uint64_t>(va_arg(arg_list, uint64_t));
                guess_delta += 18;
                strArgList[index++].append_hex(hex);
            }
        }
    }
    va_end(arg_list);

    int max_args = index;
    offset = 0;
    while ((c = fmt[offset++]) != '\0') {
        if (c == '{') {
            // number string first pos
            first = offset - 1;
            c = fmt[offset++];
            if (c == '{') {
                append(c);
            }
            else if (c >= '0' && c <= '9') {
                index = c - '0';
                while ((c = fmt[offset++]) != '\0') {
                    if (c == '}') {
                        // end of one number
                        if (index < max_args)
                            append(*(strArgList + index));
                        break;
                    }
                    else if (c >= '0' && c <= '9') {
                        index = index * 10 + (c - '0');
                    }
#if 1
                    else {
                        // Get a error num
                        break;
                    }
#else
                    else if (c == ' ') {
                        // do nothing!
                    }
                    else {
                        // Get a error num, if not found "}" behind 16 bytes, append as a string
                        while ((c = fmt[offset++]) != '\0') {
                            if (c == '}') {
                                append(fmt + first, offset - first + 1);
                                break;
                            }
                            else {
                                if ((offset - first) >= 15) {
                                    append(fmt + first, offset - first + 1);
                                    break;
                                }
                            }
                        }
                        break;
                    }
#endif
                }
            }
        }
        else {
            append(c);
        }
    }

    strArg = strArgList;
    for (int i = 0; i < max_args; ++i) {
        strArg->~basic_string();
        strArg++;
    }

#if 0
    if (strArgList) {
        free(strArgList);
    }
#endif

    delta = size() - oldSize;
    return *this;
}

#else  /* !USE_STRING_ARRAY */

template <BASIC_STRING_CLASSES>
FORCEINLINE
BASIC_STRING &BASIC_STRING::format_c(const value_type *fmt, const value_type *args, ...)
{
    int delta = 0;
    int offset = 0;
    value_type c;
    size_t oldSize;
    va_list arg_list;
    int guess_delta = 0;
#if defined(USE_STRING_ARRAY) && (USE_STRING_ARRAY != 0)
    std::vector<basic_string> strArgList;
    basic_string strArg;
#eles
    basic_string *strArgList;
#endif
    oldSize = size();

    if (fmt == NULL || args == NULL)
        return *this;

    size_t args_len = jm_strlen(args);
#if defined(USE_STRING_ARRAY) && (USE_STRING_ARRAY != 0)
    strArgList.reserve(args_len / 3 + 1);
    //strArg.reserve(28);
#else
    strArgList = new basic_string[args_len / 3 + 1];
#endif

    va_start(arg_list, args);
    while ((c = args[offset++]) != '\0') {
        if (c == '%') {
            c = args[offset++];
            if (c >= 'a' && c <= 'z') {
                strArg.clear();
                if (c == 'd') {
                    // int
                    int n = static_cast<int>(va_arg(arg_list, int));
                    guess_delta += 11;
                    strArgList.push_back(strArg.append(n));
                }
                else if (c == 's') {
                    // string
                    char * s = static_cast<char *>(va_arg(arg_list, char *));
                    size_t len = jm_strlen(s);
                    guess_delta += len;
                    strArgList.push_back(strArg.append(s, len));
                }
                else if (c == 'f') {
                    // float or double
                    double d = static_cast<double>(va_arg(arg_list, double));
                    guess_delta += 20;
                    strArgList.push_back(strArg.append(d));
                }
                else if (c == 'x') {
                    // hex(uint32_t)
                    uint32_t hex = static_cast<uint32_t>(va_arg(arg_list, uint32_t));
                    guess_delta += 10;
                    strArg.append_hex(hex);
                    strArgList.push_back(strArg);
                }
                else if (c == 'c') {
                    // char
                    char c = static_cast<char>(va_arg(arg_list, int));
                    guess_delta += 4;
                    strArgList.push_back(strArg.append(c));
                }
                else if (c == 'u') {
                    // unsigned int
                    uint32_t u = static_cast<uint32_t>(va_arg(arg_list, uint32_t));
                    guess_delta += 10;
                    strArgList.push_back(strArg.append(u));
                }
                else if (c == 'l') {
                    // long
                    long l = static_cast<long>(va_arg(arg_list, long));
                    guess_delta += 11;
                    strArgList.push_back(strArg.append(l));
                }
                else if (c == 'i') {
                    // __int64(long long)
                    int64_t i64 = static_cast<int64_t>(va_arg(arg_list, int64_t));
                    guess_delta += 21;
                    strArgList.push_back(strArg.append(i64));
                }
#if 0
                else if (c == 'g') {
                    // float
                    float f = static_cast<float>(va_arg(arg_list, double));
                    guess_delta += 20;
                    strArgList.push_back(strArg.append_float(f));
                }
                else if (c == 'b') {
                    // bool
                    bool b = static_cast<bool>((va_arg(arg_list, int) != 0));
                    guess_delta += 1;
                    strArgList.push_back(strArg.append(b));
                }
#endif
                else {
                    // unknown format
                }
            }
            else if (c == 'X') {
                // hex(uint64_t)
                strArg.clear();
                uint64_t hex = static_cast<uint64_t>(va_arg(arg_list, uint64_t));
                guess_delta += 18;
                strArg.append_hex(hex);
                strArgList.push_back(strArg);
            }
        }
    }
    va_end(arg_list);

    int index, first;
    int maxArgs = strArgList.size();
    offset = 0;
    while ((c = fmt[offset++]) != '\0') {
        if (c == '{') {
            // number string first pos
            first = offset - 1;
            c = fmt[offset++];
            if (c == '{') {
                append(c);
            }
            else if (c >= '0' && c <= '9') {
                index = c - '0';
                while ((c = fmt[offset++]) != '\0') {
                    if (c == '}') {
                        // end of one number
                        if (index < maxArgs)
                            append(strArgList[index]);
                        break;
                    }
                    else if (c >= '0' && c <= '9') {
                        index = index * 10 + (c - '0');
                    }
#if 1
                    else {
                        // Get a error num
                        break;
                    }
#else
                    else if (c == ' ') {
                        // do nothing!
                    }
                    else {
                        // Get a error num, if not found "}" behind 16 bytes, append as a string
                        while ((c = fmt[offset++]) != '\0') {
                            if (c == '}') {
                                append(fmt + first, offset - first + 1);
                                break;
                            }
                            else {
                                if ((offset - first) >= 15) {
                                    append(fmt + first, offset - first + 1);
                                    break;
                                }
                            }
                        }
                        break;
                    }
#endif
                }
            }
        }
        else {
            append(c);
        }
    }

    delta = size() - oldSize;
    return *this;
}

#endif  /* !USE_STRING_ARRAY */

/**
  basic_string::append_format_c(const value_type *fmt, ...);
 */

#if 0
template <BASIC_STRING_CLASSES>
FORCEINLINE
BASIC_STRING &BASIC_STRING::append_format_c(const value_type *fmt, ...)
{
    int delta = 0;
    int offset = 0;
    value_type c;
    size_t oldSize;
    va_list arg_list;
    char *arg_ptr;
    int guess_delta = 0;
    if (fmt == NULL)
        return *this;

    oldSize = size();
    va_start(arg_list, fmt);
    arg_ptr = (char *)arg_list;
    while ((c = fmt[offset++]) != '\0') {
        if (c == '%') {
            c = fmt[offset++];
            switch (c) {
                case 'b': {
                    // bool
                    bool b = static_cast<bool>((va_arg(arg_ptr, int) != 0));
                    guess_delta += 1;
                    append(b);
                    break;
                }
                case 'c': {
                    // char
                    char c = static_cast<char>(va_arg(arg_ptr, int));
                    guess_delta += 4;
                    append(c);
                    break;
                }
                case 'd': {
                    // int
                    int n = static_cast<int>(va_arg(arg_ptr, int));
                    guess_delta += 11;
                    append(n);
                    break;
                }
                case 'f': {
                    // float or double
                    double d = static_cast<double>(va_arg(arg_ptr, double));
                    guess_delta += 20;
                    append(d);
                    break;
                }
                case 'g': {
                    // float
                    float f = static_cast<float>(va_arg(arg_ptr, double));
                    guess_delta += 20;
                    append_float(f);
                    break;
                }
                case 'i': {
                    // __int64(long long)
                    int64_t i64 = static_cast<int64_t>(va_arg(arg_ptr, int64_t));
                    guess_delta += 21;
                    append(i64);
                    break;
                }
                case 'l': {
                    // long
                    long l = static_cast<long>(va_arg(arg_ptr, long));
                    guess_delta += 11;
                    append(l);
                    break;
                }
                case 's': {
                    // string
                    char * s = static_cast<char *>(va_arg(arg_ptr, char *));
                    size_t len = jm_strlen(s);
                    guess_delta += len;
                    append(s, len);
                    break;
                }
                case 'u': {
                    // unsigned int
                    uint32_t u = static_cast<uint32_t>(va_arg(arg_ptr, uint32_t));
                    guess_delta += 10;
                    append(u);
                    break;
                }
                case 'x': {
                    // hex(uint32_t)
                    uint32_t hex = static_cast<uint32_t>(va_arg(arg_ptr, uint32_t));
                    guess_delta += 10;
                    append_hex(hex);
                    break;
                }
                default: {
                    if (c == '%') {
                        // 实现 %%
                        append('%');
                    }
                    else if (c == 'X') {
                        // hex(uint64_t)
                        uint64_t hex = static_cast<uint64_t>(va_arg(arg_ptr, uint64_t));
                        guess_delta += 18;
                        append_hex(hex);
                    }
                    break;
                }
            }
        }
        else {
            append(c);
        }
    }
    va_end(arg_list);
    //printf("guess_delta = %d\n\n", guess_delta);
    delta = size() - oldSize;
    return *this;
}
#else
template <BASIC_STRING_CLASSES>
FORCEINLINE
BASIC_STRING &BASIC_STRING::append_format_c(const value_type *fmt, ...)
{
    int delta = 0;
    int offset = 0;
    value_type c;
    va_list arg_list;
    char *arg_ptr;
    int guess_delta = 0;

    if (fmt == NULL)
        return *this;

    va_start(arg_list, fmt);
    arg_ptr = (char *)arg_list;
    while ((c = fmt[offset++]) != '\0') {
        if (c == '%') {
            c = fmt[offset++];
            if (c >= 'a' && c <= 'z') {
#if 1
                if (c == 'd') {
                    // int
                    int n = static_cast<int>(va_arg(arg_ptr, int));
                    guess_delta += 11;
                    append(n);
                }
                else if (c == 's') {
                    // string
                    char * s = static_cast<char *>(va_arg(arg_ptr, char *));
                    size_t len = jm_strlen(s);
                    guess_delta += len;
                    append(s, len);
                }
                else if (c == 'f') {
                    // float or double
                    double d = static_cast<double>(va_arg(arg_ptr, double));
                    guess_delta += 20;
                    append(d);
                }
                else if (c == 'x') {
                    // hex(uint32_t)
                    uint32_t hex32 = static_cast<uint32_t>(va_arg(arg_ptr, uint32_t));
                    guess_delta += 10;
                    append_hex(hex32);
                }
                else if (c == 'c') {
                    // char
                    char c = static_cast<char>(va_arg(arg_ptr, int));
                    guess_delta += 4;
                    append(c);
                }
                else if (c == 'u') {
                    // unsigned int
                    uint32_t u = static_cast<unsigned int>(va_arg(arg_ptr, unsigned int));
                    guess_delta += 10;
                    append(u);
                }
                else if (c == 'l') {
                    // long
                    long l = static_cast<long>(va_arg(arg_ptr, long));
                    guess_delta += 11;
                    append(l);
                }
                else if (c == 'i') {
                    // __int64(long long)
                    int64_t i64 = static_cast<int64_t>(va_arg(arg_ptr, int64_t));
                    guess_delta += 21;
                    append(i64);
                }
                else if (c == 'g') {
                    // double
                    float f = static_cast<float>(va_arg(arg_ptr, double));
                    guess_delta += 20;
                    append(f);
                }
                else if (c == 'b') {
                    // bool
                    bool b = static_cast<bool>((va_arg(arg_ptr, int) != 0));
                    guess_delta += 1;
                    append(b);
                }
                else {
                    // unknown format
                }
#else
                if (c == 'b') {
                    // bool
                    bool b = static_cast<bool>((va_arg(arg_ptr, int) != 0));
                    guess_delta += 1;
                    append(b);
                }
                else if (c == 'c') {
                    // char
                    char c = static_cast<char>(va_arg(arg_ptr, int));
                    guess_delta += 4;
                    append(c);
                }
                else if (c == 'd') {
                    // int
                    int n = static_cast<int>(va_arg(arg_ptr, int));
                    guess_delta += 11;
                    append(n);
                }
                else if (c == 'f') {
                    // float or double
                    double d = static_cast<double>(va_arg(arg_ptr, double));
                    guess_delta += 20;
                    append(d);
                }
                else if (c == 'g') {
                    // float
                    float f = static_cast<float>(va_arg(arg_ptr, double));
                    guess_delta += 20;
                    append(f);
                }
                else if (c == 'i') {
                    // __int64(long long)
                    int64_t i64 = static_cast<int64_t>(va_arg(arg_ptr, int64_t));
                    guess_delta += 21;
                    append(i64);
                }
                else if (c == 'l') {
                    // long
                    long l = static_cast<long>(va_arg(arg_ptr, long));
                    guess_delta += 11;
                    append(l);
                }
                else if (c == 's') {
                    // string
                    char * s = static_cast<char *>(va_arg(arg_ptr, char *));
                    size_t len = jm_strlen(s);
                    guess_delta += len;
                    append(s, len);
                }
                else if (c == 'u') {
                    // unsigned int
                    uint32_t u = static_cast<uint32_t>(va_arg(arg_ptr, uint32_t));
                    guess_delta += 10;
                    append(u);
                }
                else if (c == 'x') {
                    // hex(uint32_t)
                    uint32_t hex = static_cast<uint32_t>(va_arg(arg_ptr, uint32_t));
                    guess_delta += 10;
                    append_hex(hex);
                }
                else {
                    // unknown format
                }
#endif
            }
            else if (c == '%') {
                // 实现 %%
                append('%');
            }
            else if (c == 'X') {
                // hex(uint64_t)
                uint64_t hex64 = static_cast<uint64_t>(va_arg(arg_ptr, uint64_t));
                guess_delta += 18;
                append_hex(hex64);
            }
        }
        else {
            append(c);
        }
    }
    va_end(arg_list);
    return *this;
}
#endif

template <BASIC_STRING_CLASSES>
inline void swap(BASIC_STRING &lhs, BASIC_STRING &rhs)
{
    lhs.swap(rhs);
}

template <BASIC_STRING_CLASSES>
inline void stole(BASIC_STRING &lhs, BASIC_STRING &rhs)
{
    lhs.stole(rhs);
}

template <BASIC_STRING_CLASSES>
bool operator == (const BASIC_STRING &lhs, const BASIC_STRING &rhs)
{
    return (lhs.compare(rhs) == 0);
}

template <BASIC_STRING_CLASSES>
bool operator == (const BASIC_STRING &lhs, const _CharT *rhs)
{
    return (lhs.compare(rhs) == 0);
}

template <BASIC_STRING_CLASSES>
bool operator == (const _CharT *lhs, const BASIC_STRING &rhs)
{
    return (rhs.compare(lhs) == 0);
}

#undef BASIC_STRING_CLASSES
#undef BASIC_STRING

typedef basic_string<char>      string;
typedef basic_string<wchar_t>   wstring;

typedef basic_string<char>      String;
typedef basic_string<wchar_t>   WString;

}  /* namespace jimi */

#endif  /* _JIMI_LANG_STRING_H_ */
