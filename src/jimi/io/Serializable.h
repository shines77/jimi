
#ifndef _JIMI_IO_SERIALIZABLE_H_
#define _JIMI_IO_SERIALIZABLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"

namespace jimi {

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

}  /* namespace jimi */

#endif  /* _JIMI_IO_SERIALIZABLE_H_ */
