
#ifndef _JIMI_LANG_CLONEABLE_H_
#define _JIMI_LANG_CLONEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/lang/String.h"
#include "jimi/lang/CloneNotSupportedException.h"

NS_JIMI_BEGIN

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

NS_JIMI_END

#endif  /* _JIMI_LANG_CLONEABLE_H_ */
