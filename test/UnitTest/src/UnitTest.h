
#ifndef _UNITEST_H_
#define _UNITEST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

#if 1

#define NS_UNITEST_BEGIN    namespace UnitTest {
#define NS_UNITEST_END      }

#define USING_NS_UNITEST    using namespace UnitTest

#else

#define NS_UNITEST_BEGIN
#define NS_UNITEST_END

#define USING_NS_UNITEST

#endif

#endif  /* _UNITEST_H_ */
