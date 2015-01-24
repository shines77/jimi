
#ifndef _JIMI_INTERNAL_NONASSIGNABLE_H_
#define _JIMI_INTERNAL_NONASSIGNABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"

namespace jimi {

namespace internal {    // protection from unintended ADL

class NonAssignable
{
public:
#ifdef __GNUC__
    //! Explicitly define default construction, because otherwise gcc issues gratuitous warning.
    NonAssignable() {}
#endif  /* __GNUC__ */

#if defined(JIMI_HAS_DELETED_FUNCTIONS) && (JIMI_HAS_DELETED_FUNCTIONS != 0)
    //! Deny assignment operator
    NonCopyable & operator =(const NonCopyable &) = delete;
#else
private:
    //! Deny assignment operator
    NonAssignable & operator =(const NonAssignable &);
#endif
};

}  /* namespace of internal */

typedef internal::NonAssignable NonAssignable;

}  /* namespace jimi */

#endif  /* !_JIMI_INTERNAL_NONASSIGNABLE_H_ */
