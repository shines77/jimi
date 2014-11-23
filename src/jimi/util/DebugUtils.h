
#ifndef _JIMI_UTIL_DEBUGUTILS_H_
#define _JIMI_UTIL_DEBUGUTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_namespace.h"
#include "jimi/internal/NonCopyable.h"
#include "jimi/util/DumpUtils.h"

NS_JIMI_BEGIN

class DebugUtils : public internal::NonCopyable
{
public:
    DebugUtils() {};
    ~DebugUtils() {};

public:
    static void dump(void *p, size_t size);
};

void DebugUtils::dump(void *p, size_t size)
{
#if defined(_DEBUG) && !defined(NDEBUG)
    DumpUtils::dump(p, size);
#endif
}

NS_JIMI_END

#endif  /* _JIMI_UTIL_DEBUGUTILS_H_ */
