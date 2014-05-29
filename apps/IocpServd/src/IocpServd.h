
#ifndef _IOCPSERVD_H_
#define _IOCPSERVD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

#if 1

#define NS_IOCPSERVD_BEGIN  namespace IocpServd {
#define NS_IOCPSERVD_END    }

#define USING_NS_IOCPSERVD  using namespace IocpServd

#else

#define NS_IOCPSERVD_BEGIN
#define NS_IOCPSERVD_END

#define USING_NS_IOCPSERVD

#endif

#endif  /* _IOCPSERVD_H_ */
