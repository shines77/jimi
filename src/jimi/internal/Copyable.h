
#ifndef _JIMI_INTERNAL_COPYABLE_H_
#define _JIMI_INTERNAL_COPYABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

NS_JIMI_BEGIN

namespace internal {    // protection from unintended ADL

class Copyable
{
public:
#if defined(JIMI_HAS_DEFAULTED_FUNCTIONS) && (JIMI_HAS_DEFAULTED_FUNCTIONS != 0)
    //! Allow default construction
    JIMI_CONSTEXPR Copyable() = default;
    ~Copyable() = default;
#else
    //! Allow default construction
    Copyable() {}
    ~Copyable() {}
#endif

    // Allow the copy constructor
    Copyable(const Copyable &);
    // Allow the assignment operator
    Copyable & operator =(const Copyable &);
};

}  /* namespace of internal */

typedef internal::Copyable Copyable;

NS_JIMI_END

#endif  /* !_JIMI_INTERNAL_COPYABLE_H_ */
