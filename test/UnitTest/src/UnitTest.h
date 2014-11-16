
#ifndef _JIMI_UNITEST_H_
#define _JIMI_UNITEST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/core/jimi_def.h"

#define UNITEST_USE_NAMESPACE       0

#if defined(UNITEST_USE_NAMESPACE) && (UNITEST_USE_NAMESPACE != 0)

#define NS_UNITEST_BEGIN    namespace UnitTest {
#define NS_UNITEST_END      }

#define USING_NS_UNITEST    using namespace UnitTest;

#else

#define NS_UNITEST_BEGIN
#define NS_UNITEST_END

#define USING_NS_UNITEST

#endif  /* UNITEST_USE_NAMESPACE */

NS_UNITEST_BEGIN

// declear namespace UnitTest

NS_UNITEST_END

#endif  /* _JIMI_UNITEST_H_ */
