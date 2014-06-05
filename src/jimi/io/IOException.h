
#ifndef _JIMI_IO_IOEXCEPTION_H_
#define _JIMI_IO_IOEXCEPTION_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/lang/Exception.h>

NS_JIMI_BEGIN

class IOException : public Exception
{
public:
    IOException() {};
    ~IOException() {};
};

NS_JIMI_END

#endif  /* _JIMI_IO_IOEXCEPTION_H_ */
