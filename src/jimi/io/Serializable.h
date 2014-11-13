
#ifndef _JIMI_IO_SERIALIZABLE_H_
#define _JIMI_IO_SERIALIZABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"

NS_JIMI_BEGIN

class Serializable
{
public:
#if defined(JIMI_HAS_DEFAULTED_FUNCTIONS) && (JIMI_HAS_DEFAULTED_FUNCTIONS != 0)
    Serializable() = default;
    ~Serializable() = default;
#else
    Serializable() {};
    ~Serializable() {};
#endif
};

NS_JIMI_END

#endif  /* _JIMI_IO_SERIALIZABLE_H_ */
