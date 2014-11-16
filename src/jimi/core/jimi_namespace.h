
#ifndef _JIMI_NAMESPACE_H_
#define _JIMI_NAMESPACE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#define USING_NS_JIMI           using namespace jimi;

// namespace jimi
#define NS_JIMI_BEGIN           namespace jimi {
#define NS_JIMI_END             }

#define USING_NS_JIMI_CORE      using namespace jimi::core;
#define USING_NS_JIMI_SYSTEM    using namespace jimi::system;
#define USING_NS_JIMI_IOCP      using namespace jimi::iocp;
#define USING_NS_JIMI_FS        using namespace jimi::fs;
#define USING_NS_JIMI_IO        using namespace jimi::io;

#define USING_NS_JIMI_MT        using namespace jimi::mt;
#define USING_NS_JIMI_STL       using namespace jimi::stl;

#define USING_NS_JIMI_SUB(NS)   using namespace jimi::NS;

// namespace jimi::internal
#define NS_JIMI_INTERNAL_BEGIN  namespace internal {
#define NS_JIMI_INTERNAL_END    }

// namespace jimi::core
#define NS_JIMI_CORE_BEGIN      namespace core {
#define NS_JIMI_CORE_END        }

// namespace jimi::system
#define NS_JIMI_SYSTEM_BEGIN    namespace system {
#define NS_JIMI_SYSTEM_END      }

// namespace jimi::ui
#define NS_JIMI_UI_BEGIN        namespace ui {
#define NS_JIMI_UI_END          }

// namespace jimi::util
#define NS_JIMI_UTIL_BEGIN      namespace util {
#define NS_JIMI_UTIL_END        }

// namespace jimi::iocp
#define NS_JIMI_IOCP_BEGIN      namespace iocp {
#define NS_JIMI_IOCP_END        }

// namespace jimi::fs
#define NS_JIMI_FS_BEGIN        namespace fs {
#define NS_JIMI_FS_END          }

// namespace jimi::io
#define NS_JIMI_IO_BEGIN        namespace io {
#define NS_JIMI_IO_END          }

// namespace jimi::mt
#define NS_JIMI_MT_BEGIN        namespace mt {
#define NS_JIMI_MT_END          }

// namespace jimi::stl
#define NS_JIMI_STL_BEGIN       namespace stl {
#define NS_JIMI_STL_END         }

#endif  /* _JIMI_NAMESPACE_H_ */
