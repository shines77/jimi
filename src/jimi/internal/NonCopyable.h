
#ifndef _JIMI_INTERNAL_NONCOPYABLE_H_
#define _JIMI_INTERNAL_NONCOPYABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"
#include "jimi/internal/NonAssignable.h"

namespace jimi {

//
// C++ Idioms - Non Copyable Objects
// http://dev-faqs.blogspot.com/2010/07/c-idioms-non-copyable.html
//

namespace internal {    // protection from unintended ADL

class NonCopyable
{
protected:
#if defined(JIMI_HAS_DEFAULTED_FUNCTIONS) && (JIMI_HAS_DEFAULTED_FUNCTIONS != 0)
    //! Allow default construction
    JIMI_CONSTEXPR NonCopyable() = default;
    ~NonCopyable() = default;
#else
    //! Allow default construction
    NonCopyable() {}
    ~NonCopyable() {}
#endif

#if defined(JIMI_HAS_DELETED_FUNCTIONS) && (JIMI_HAS_DELETED_FUNCTIONS != 0)
    //! Deny copy construction
    NonCopyable(const NonCopyable &) = delete;
    //! Deny assignment operator
    NonCopyable & operator =(const NonCopyable &) = delete;
#else
private:
    //! Deny copy construction
    NonCopyable(const NonCopyable &);
    //! Deny assignment operator
    NonCopyable & operator =(const NonCopyable &);
#endif
};

}  /* namespace of internal */

typedef internal::NonCopyable NonCopyable;

}  /* namespace jimi */

#endif  /* !_JIMI_INTERNAL_NONCOPYABLE_H_ */
