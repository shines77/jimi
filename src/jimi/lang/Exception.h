
#ifndef _JIMI_LANG_EXCEPTION_H_
#define _JIMI_LANG_EXCEPTION_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"
#include "jimi/lang/Throwable.h"
#include "jimi/lang/String.h"

namespace jimi {

class Exception : public Throwable
{
public:
    Exception() : Throwable() {};
    Exception(jimi::string s) : Throwable(s) {};
    Exception(std::string s) : Throwable(s) {};
    ~Exception() {};
};

}  /* namespace jimi */

#endif  /* _JIMI_LANG_EXCEPTION_H_ */
