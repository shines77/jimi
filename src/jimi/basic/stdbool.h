
#ifndef _JIMI_BASIC_STDBOOL_H_
#define _JIMI_BASIC_STDBOOL_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1700)
#include "jimi/basic/msvc/stdbool.h"
#else
#include <stdbool.h>
#endif  /* _MSC_VER */

#endif  /* _JIMI_BASIC_STDBOOL_H_ */
