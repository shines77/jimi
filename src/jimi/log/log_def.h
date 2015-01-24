
#ifndef _JIMI_LOG_DEF_H_
#define _JIMI_LOG_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <assert.h>

#include <iostream>
#include <string>

#include "jimi/basic/jimi_def.h"

#define JIMI_LOG_ASSERT_TRUE            JIMI_ASSERT_TRUE
#define JIMI_LOG_ASSERT_FALSE           JIMI_ASSERT_FALSE
#define JIMI_LOG_ASSERT                 JIMI_LOG_ASSERT_FALSE

#define JIMI_LOG_ASSERT_EX_TRUE         JIMI_ASSERT_EX_TRUE
#define JIMI_LOG_ASSERT_EX_FALSE        JIMI_ASSERT_EX_FALSE
#define JIMI_LOG_ASSERT_EX              JIMI_LOG_ASSERT_EX_FALSE

#define JIMI_LOG_DLL                    JIMI_DLL

// namespace iocp
#define NS_JIMI_LOG_BEGIN               namespace log {
#define NS_JIMI_LOG_END                 }

#define NS_JIMI_LOG_NAME                jimi::log
#define NS_JIMI_LOG(NS)                 jimi::NS

#define USING_NS_JIMI_LOG               using namespace jimi::log;

#define USING_NS_JIMI_LOG_SUB(NS)       using namespace jimi::NS;

#define JIMI_LOG_MIN(a, b)              ((a) < (b) ? (a) : (b))
#define JIMI_LOG_MAX(a, b)              ((a) > (b) ? (a) : (b))

#endif  /* _JIMI_LOG_DEF_H_ */
