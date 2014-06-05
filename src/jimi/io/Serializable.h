
#ifndef _JIMI_IO_SERIALIZABLE_H_
#define _JIMI_IO_SERIALIZABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

NS_JIMI_BEGIN

class Serializable
{
public:
    Serializable() {};
    ~Serializable() {};
};

NS_JIMI_END

#endif  /* _JIMI_IO_SERIALIZABLE_H_ */
