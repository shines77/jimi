
#ifndef _JIMI_IOCPSERVD_H_
#define _JIMI_IOCPSERVD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_def.h"

#define IOCPSERVD_USE_NAMESPACE     1

#if defined(IOCPSERVD_USE_NAMESPACE) && (IOCPSERVD_USE_NAMESPACE != 0)

#define NS_IOCPSERVD_BEGIN  namespace IocpServd {
#define NS_IOCPSERVD_END    }

#define USING_NS_IOCPSERVD  using namespace IocpServd;

#else

#define NS_IOCPSERVD_BEGIN
#define NS_IOCPSERVD_END

#define USING_NS_IOCPSERVD

#endif

NS_IOCPSERVD_BEGIN

// declear namespace IocpServd

NS_IOCPSERVD_END

#endif  /* _JIMI_IOCPSERVD_H_ */
