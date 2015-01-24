
#ifndef _JIMI_IO_IOEXCEPTION_H_
#define _JIMI_IO_IOEXCEPTION_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"
#include "jimi/lang/Exception.h"

namespace jimi {

class IOException : public Exception
{
public:
    IOException() {};
    ~IOException() {};
};

}  /* namespace jimi */

#endif  /* _JIMI_IO_IOEXCEPTION_H_ */
