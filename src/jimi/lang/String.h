
#ifndef _JIMI_LANG_STRING_H_
#define _JIMI_LANG_STRING_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimi/lang/Allocator.h>
#include <jimi/lang/Char_Traits.h>
#include <jimi/lang/RefCounted.h>
#include <jimi/lang/StringCore.h>
#include <jimic/string/jm_strings.h>

#include <string>
#include <limits>
#if (defined(_MSC_VER) && (_MSC_VER >= 1700)) || defined(__GNUC__)
#include <functional>   // std::less_equal, std::bind2nd
#endif
using namespace std;

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
inline void pod_copy(Pod *dest, const Pod *src, size_t size) {
    jimi_assert(size >= 0);
    jimi_assert(dest >= (src + size) || (dest + size) <= src);
    ::memcpy(dest, src, size * sizeof(Pod));
}

template <class Pod>
inline void pod_copy2(Pod *dest, const Pod *src, const Pod *end) {
    jimi_assert(end >= src);
    jimi_assert(dest >= end || (dest + (end - src)) <= src);
    ::memcpy(dest, src, (end - src) * sizeof(Pod));
}

/*
 * Lightly structured memmove, simplifies copying PODs and introduces
 * some asserts
 */
template <class Pod>
inline void pod_move(Pod *dest, const Pod *src, size_t size) {
    jimi_assert(size >= 0);
    jimi_assert(dest >= (src + size) || (dest + size) <= src);
    ::memmove(dest, src, size * sizeof(Pod));
}

template <class Pod, class T>
inline void pod_fill(Pod *dest, size_t size, T c) {
    jimi_assert(dest && size > 0);
    if (sizeof(T) == 1) {   /*static*/
        ::memset(dest, c, size);
    }
    else {
        const Pod *ee = dest + (size & ~(size_t)7u);
        for (; dest != ee; dest += 8) {
            dest[0] = c;
            dest[1] = c;
            dest[2] = c;
            dest[3] = c;
            dest[4] = c;
            dest[5] = c;
            dest[6] = c;
            dest[7] = c;
        }
        // Leftovers
        const Pod *end = dest + size;
        for (; dest != end; ++dest) {
            *dest = c;
        }
    }
}

}  /* end of the namespace string_detail */

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

class JIMI_API basic_string
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
    basic_string(const value_type c);
    basic_string(const value_type c, size_type n);
    basic_string(const value_type *src);
    basic_string(const value_type *src, size_type size);
    basic_string(const value_type *begin, const value_type *end);
    basic_string(const std::string &src);

    // Discontructor
    ~basic_string();

    // Assigment operators
    basic_string &operator = (const basic_string &rhs);
    basic_string &operator = (const value_type c);
    basic_string &operator = (const value_type *str);

    // Compatibility with std::string
    basic_string &operator = (const std::string &rhs);
    std::string &toStdString() const;

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

    bool empty() const          { return size() == 0; }
    size_type length() const    { return size(); }
    size_type size() const      { return _store.size(); }
    size_type capacity() const  { return _store.capacity(); }

    void swap(basic_string &rhs) { _store.swap(rhs._store); }

    void clear() { resize(0); }

    size_type max_size() const  { return std::numeric_limits<size_type>::max(); }

    void resize(const size_type newSize, const value_type c = value_type()) {}

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
BASIC_STRING::basic_string(const std::string &src)
: _store(src.data(), src.size())
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
BASIC_STRING::~basic_string()
{
    destroy();
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
        ///*
        if (oldSize < srcSize)
            _store.expandTo(srcSize);
        else
            _store.shrinkTo(srcSize);
        //*/
        _store = rhs._store;
        jimi_assert(size() == srcSize);
        string_detail::pod_copy(begin(), rhs.begin(), rhs.size());
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
std::string &BASIC_STRING::toStdString() const
{
    return std::string(data(), size());
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
    resize(size() + n, c);
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
    // Warning! Repeated appends with short strings may actually incur
    // practically quadratic performance. Avoid that by pushing back
    // the first character (which ensures exponential growth) and then
    // appending the rest normally. Worst case the append may incur a
    // second allocation but that will be rare.
    push_back(*s++);
    _store.expandTo(oldSize + n);
    --n;
    if (n > 0) {
        ::memcpy((void *)(data() + oldSize + 1), s, n * sizeof(value_type));
    }
    _store.writeNullForce();
    //jimi_assert(size() == oldSize + n);
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
typename const BASIC_STRING::value_type &BASIC_STRING::front() const
{
    return *begin();
}

template <BASIC_STRING_CLASSES>
typename const BASIC_STRING::value_type &BASIC_STRING::back() const
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
    _store.destroy();
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

template <BASIC_STRING_CLASSES>
inline void swap(BASIC_STRING &lhs, BASIC_STRING &rhs)
{
    lhs.swap(rhs);
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

typedef basic_string<char> string;
typedef basic_string<char> String;

NS_JIMI_END

#endif  /* _JIMI_LANG_STRING_H_ */
