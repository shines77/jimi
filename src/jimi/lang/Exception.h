
#ifndef _JIMI_LANG_EXCEPTION_H_
#define _JIMI_LANG_EXCEPTION_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/lang/Throwable.h>
#include <jimi/lang/String.h>

NS_JIMI_BEGIN

class Exception : public Throwable
{
public:
    Exception() : Throwable() {};
    Exception(String s) : Throwable(s) {};
    ~Exception() {};
};

NS_JIMI_END

#endif  /* _JIMI_LANG_EXCEPTION_H_ */
