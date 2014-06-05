
#ifndef _JIMI_LANG_AUTOCLOSEABLE_H_
#define _JIMI_LANG_AUTOCLOSEABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/lang/Exception.h>

NS_JIMI_BEGIN

class AutoCloseable
{
private:
    void Close() throw (Exception) {
        //throw Exception();
    }
};

NS_JIMI_END

#endif  /* _JIMI_LANG_AUTOCLOSEABLE_H_ */
