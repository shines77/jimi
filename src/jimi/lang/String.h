
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

    void destroy();

    void retail();
    void release();

    bool equals(const basic_string &rhs) const;

    bool compare(const basic_string &rhs) const;
    bool compare(const char_type *rhs) const;

    const storage_type &getStorage() const { return _store; }

    char_type *data() const  { return _store.c_str(); }
    char_type *c_str() const { return _store.c_str(); }
    size_type size() const   { return (size_type)_store.size();  }

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
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const char *src, size_t size)
: _store(src, size)
{
}

template <BASIC_STRING_CLASSES>
BASIC_STRING::basic_string(const wchar_t *src)
: _store(src, ::jm_strlen(src))
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
    release();
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
inline bool BASIC_STRING::equals(const BASIC_STRING &rhs) const
{
    if (&rhs == this)
        return true;
    else
        return _store.equals(rhs._store);
}

template <BASIC_STRING_CLASSES>
bool BASIC_STRING::compare(const BASIC_STRING &rhs) const
{
    if (&rhs == this)
        return true;
    else
        return _store.compare(rhs._store);
}

template <BASIC_STRING_CLASSES>
bool BASIC_STRING::compare(const char_type *rhs) const
{
    return _store.compare(rhs);
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
