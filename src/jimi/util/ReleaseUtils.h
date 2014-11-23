
#ifndef _JIMI_UTIL_RELEASEUTILS_H_
#define _JIMI_UTIL_RELEASEUTILS_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_namespace.h"
#include "jimi/internal/NonCopyable.h"
#include "jimi/util/DumpUtils.h"

NS_JIMI_BEGIN

class ReleaseUtils : public internal::NonCopyable
{
public:
    ReleaseUtils() {};
    ~ReleaseUtils() {};

public:
    static void dump(void *p, size_t size);
};

void ReleaseUtils::dump(void *p, size_t size)
{
    DumpUtils::dump(p, size);
}

NS_JIMI_END

#endif  /* _JIMI_UTIL_RELEASEUTILS_H_ */
