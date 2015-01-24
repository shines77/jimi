
#ifndef _JIMI_LANG_CLONEABLE_H_
#define _JIMI_LANG_CLONEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"
#include "jimi/lang/String.h"
#include "jimi/lang/CloneNotSupportedException.h"

namespace jimi {

template <class T>
class Cloneable
{
public:
#if 1
    T Clone() throw (CloneNotSupportedException) {
        //throw CloneNotSupportedException();
    }
#else
    T Clone() throw (CloneNotSupportedException) {
        T *pThis = (T *)this;
        return *pThis;
    }
#endif
};

}  /* namespace jimi */

#endif  /* _JIMI_LANG_CLONEABLE_H_ */
