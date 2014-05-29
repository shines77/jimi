
#ifndef _JIMI_LOG_DEF_H_
#define _JIMI_LOG_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <assert.h>

#include <iostream>
#include <string>
using namespace std;

#include <jimi/core/jimi_def.h>

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
#define NS_JIMI_LOG(XX)                 jimi::log::XX

#define USING_NS_JIMI_LOG               using namespace jimi::log;

#define USING_NS_JIMI_LOG_SUB(NS)       using namespace jimi::log::##NS##;

// namespace jimi::log::internal
#define NS_JIMI_LOG_INTERNAL_BEGIN      namespace internal {
#define NS_JIMI_LOG_INTERNAL_END        }

// namespace jimi::log::core
#define NS_JIMI_LOG_CORE_BEGIN          namespace core {
#define NS_JIMI_LOG_CORE_END            }

// namespace jimi::log::system
#define NS_JIMI_LOG_SYSTEM_BEGIN        namespace system {
#define NS_JIMI_LOG_SYSTEM_END          }

// namespace jimi::log::util
#define NS_JIMI_LOG_UTIL_BEGIN          namespace util {
#define NS_JIMI_LOG_UTIL_END            }

#define JIMI_LOG_MIN(a, b)          ((a) < (b) ? (a) : (b))
#define JIMI_LOG_MAX(a, b)          ((a) > (b) ? (a) : (b))

#endif  /* _JIMI_LOG_DEF_H_ */
