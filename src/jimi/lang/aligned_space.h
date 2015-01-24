
#ifndef _JIMI_LANG_ALIGNED_SPACE_H_
#define _JIMI_LANG_ALIGNED_SPACE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/core/jimi_machine.h"
#include "jimi/internal/pointer_cast.h"

namespace jimi {

//! Block of space aligned sufficiently to construct an array T with N elements.
/** The elements are not constructed or destroyed by this class.
    @ingroup memory_allocation */
template<typename T, size_t N>
class aligned_space
{
private:
    typedef JIMI_TypeWithAlignmentAtLeastAsStrict(T) element_type;
    element_type array[(sizeof(T) * N + sizeof(element_type) - 1) / sizeof(element_type)];

public:
    //! Pointer to beginning of array
    T * begin() { return internal::punned_cast<T *>(this); }

    //! Pointer to one past last element in array.
    T * end()   { return begin() + N; }
};

}  /* namespace jimi */

#endif  /* !_JIMI_LANG_ALIGNED_SPACE_H_ */
