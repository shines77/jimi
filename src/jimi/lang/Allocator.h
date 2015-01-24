
#ifndef _JIMI_LANG_ALLOCATOR_H_
#define _JIMI_LANG_ALLOCATOR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_def.h"

namespace jimi {

template <class _CharT>
class JIMI_DLL allocator
{
public:
    typedef size_t          size_type;
    typedef size_t          difference_type;
    typedef _CharT &        reference;
    typedef const _CharT &  const_reference;
    typedef _CharT *        pointer;
    typedef const _CharT *  const_pointer;
};

}  /* namespace jimi */

#endif  /* _JIMI_LANG_ALLOCATOR_H_ */
