
#ifndef _UNITEST_H_
#define _UNITEST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

#define UNITEST_USE_NAMESPACE   1

#if UNITEST_USE_NAMESPACE

#define NS_UNITEST_BEGIN    namespace UnitTest {
#define NS_UNITEST_END      }

#define USING_NS_UNITEST    using namespace UnitTest

#else

#define NS_UNITEST_BEGIN
#define NS_UNITEST_END

#define USING_NS_UNITEST

#endif  /* UNITEST_USE_NAMESPACE */

#endif  /* _UNITEST_H_ */
