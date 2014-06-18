
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
using namespace std;

NS_JIMI_BEGIN

/// <comment>
///
/// 标准C＋＋类string的Copy-On-Write技术（一）
///
/// Reference:
///    http://haoel.blog.51cto.com/313033/124638
///
/// 漫步Facebook开源C++库folly(1)：string类的设计
///
/// Reference:
///    http://www.cnblogs.com/promise6522/archive/2012/06/05/2535530.html
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
inline void pod_copy(const Pod *b, const Pod *e, Pod *d) {
    jimi_assert(e >= b);
    jimi_assert(d >= e || d + (e - b) <= b);
    ::memcpy(d, b, (e - b) * sizeof(Pod));
}

}  /* end of the namespace string_detail */

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
    typedef normal_iterator<pointer, basic_string>  iterator;
    typedef normal_iterator<const_pointer, basic_string>
                                                    const_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;
    typedef std::reverse_iterator<iterator>         reverse_iterator;

public:
    // Contructor
    basic_string();
    basic_string(const basic_string &src);
    basic_string(const std::string &src);
    basic_string(const char *src);
    basic_string(const char *src, size_t size);
    basic_string(const wchar_t *src);
    basic_string(const wchar_t *src, size_t size);
    ~basic_string();

    void retail();
    void release();

    basic_string &operator = (const basic_string &lhs);

    bool equals(const basic_string &rhs) const;

    int compare(const basic_string &rhs) const;
    int compare(const value_type *rhs) const;

    //const storage_type &getStorage() const { return _store; }

    const value_type *data() const  { return _store.c_str(); }
    const value_type *c_str() const { return _store.c_str(); }
    size_type size() const { return (size_type)_store.size(); }

    void swap(basic_string &rhs) { _store.swap(rhs._store); }
    size_type capacity() const   { return (size_type)_store.capacity(); }

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
BASIC_STRING::basic_string(const char *src)
: _store(src, ::jm_strlen(src))
//: _store(src)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const char *src, size_t size)
: _store(src, size)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const wchar_t *src)
: _store(src, ::jm_wcslen(src))
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const wchar_t *src, size_t size)
: _store(src, size)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::~basic_string()
{
    destroy();
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::destroy()
{
    _store.destroy();
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::retail()
{
    _store.retail();
}

template <BASIC_STRING_CLASSES>
inline void BASIC_STRING::release()
{
    _store.release();
}

template <BASIC_STRING_CLASSES>
BASIC_STRING &BASIC_STRING::operator = (const BASIC_STRING &lhs) {
    if (&lhs == this)
        return *this;

    const size_type oldSize = size();
    const size_type srcSize = lhs.size();
    if (capacity() >= srcSize && !_store.is_shared()) {
        // great, just copy the contents
        ///*
        if (oldSize < srcSize)
            _store.expandTo(srcSize);
        else
            _store.shrinkTo(srcSize);
        //*/
        _store = lhs._store;
        jimi_assert(size() == srcSize);
        //string_detail::pod_copy(lhs.begin(), lhs.end(), begin());
        //_store.writeNull();
    }
    else {
        // need to reallocate, so we may as well create a brand new string
        basic_string(lhs).swap(*this);
    }
    return *this;
}

template <BASIC_STRING_CLASSES>
inline bool BASIC_STRING::equals(const BASIC_STRING &rhs) const
{
    if (&rhs == this)
        return true;
    else
        return _store.equals(rhs._store);
}

template <BASIC_STRING_CLASSES>
inline int BASIC_STRING::compare(const BASIC_STRING &rhs) const
{
    if (&rhs == this)
        return 0;
    else
        return _store.compare(rhs._store);
}

template <BASIC_STRING_CLASSES>
inline int BASIC_STRING::compare(const value_type *rhs) const
{
    return _store.compare(rhs);
}

template <BASIC_STRING_CLASSES>
inline void swap(BASIC_STRING &lhs, BASIC_STRING &rhs)
{
    lhs.swap(rhs);
}

template <BASIC_STRING_CLASSES>
inline bool operator == (const BASIC_STRING &lhs, const BASIC_STRING &rhs)
{
    return (lhs.compare(rhs) == 0);
}

template <BASIC_STRING_CLASSES>
inline bool operator == (const BASIC_STRING &lhs, const _CharT *rhs)
{
    return (lhs.compare(rhs) == 0);
}

template <BASIC_STRING_CLASSES>
inline bool operator == (const _CharT *lhs, const BASIC_STRING &rhs)
{
    return (rhs.compare(lhs) == 0);
}

typedef basic_string<char> String;

NS_JIMI_END

#endif  /* _JIMI_LANG_STRING_H_ */
