
#ifndef _JIMI_DEF_H_
#define _JIMI_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>
#include <jimi/platform/jimi_compiler_config.h>

#if JIMI_IS_WINDOWS
    #include <jimi/platform/jimi_targetver.h>
#endif

#include <jimi/core/win32/jimi_config.h>
#include <jimi/core/jimi_export.h>

#if (JIMI_TARGET_COMPILER == JIMI_COMPILER_MSVC)
    #include <jimi/core/vs/stdint.h>
    #define snprintf _snprintf
#else
    #include <stdint.h>
#endif

#include <assert.h>

#define JIMI_ASSERT_TRUE(predicate)                   assert(!(predicate))
#define JIMI_ASSERT_FALSE(predicate)                  assert(!!(predicate))
#define JIMI_ASSERT(predicate)                        JIMI_ASSERT_FALSE

#define JIMI_ASSERT_EX_TRUE(predicate, comment)       assert(!(predicate))
#define JIMI_ASSERT_EX_FALSE(predicate, comment)      assert(!!(predicate))
#define JIMI_ASSERT_EX(predicate, comment)            JIMI_ASSERT_EX_FALSE

#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

#define JIMI_WINAPI             __CLR_OR_STD_CALL

#include <iostream>
#include <string>
using namespace std;

#define USING_NS_JIMI           using namespace jimi;
#define USING_NS_JIMI_CORE      using namespace jimi::core;
#define USING_NS_JIMI_SYSTEM    using namespace jimi::system;
#define USING_NS_JIMI_IOCP      using namespace jimi::iocp;
#define USING_NS_JIMI_FS        using namespace jimi::fs;
#define USING_NS_JIMI_IO        using namespace jimi::io;

#define USING_NS_JIMI_SUB(NS)   using namespace jimi::##NS##;

// namespace jimi
#define NS_JIMI_BEGIN           namespace jimi {
#define NS_JIMI_END             }

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

#define JIMI_MIN(a, b)          ((a) < (b) ? (a) : (b))
#define JIMI_MAX(a, b)          ((a) > (b) ? (a) : (b))

// for precompiled macro to string
#define JIMI_STRING_ESCAPE(x)   #x
#define JIMI_TO_STRING(x)       JIMI_STRING_ESCAPE(x)

#define JIMI_MAKE_STRING(x)     { #x, JIMI_STRING_ESCAPE(x) }

typedef struct JIMI_MACRO_T
{
    const char *name;
    const char *value;
} JIMI_MACRO_T;

// for exported func
#if _MSC_VER >= 1400
    #define JIMI_EXPORTED_FUNC      __cdecl
    #define JIMI_EXPORTED_METHOD    __thiscall
#else
    #define JIMI_EXPORTED_FUNC
    #define JIMI_EXPORTED_METHOD
#endif

#if _MSC_VER || __INTEL_COMPILER
#define JIMI_NOINLINE(decl)     __declspec(noinline) decl
#elif __GNUC__
#define JIMI_NOINLINE(decl)     decl __attribute__ ((noinline))
#else
#define JIMI_NOINLINE(decl)     decl
#endif

#include <cstddef>      /* Need size_t and ptrdiff_t */

// warning C4290: 忽略 C++ 异常规范，但指示函数不是 __declspec(nothrow)
#pragma warning(disable : 4290)

/**
 * for jm_countof helper
 */
#if defined(_M_MRX000) || defined(_M_ALPHA) || defined(_M_PPC) || defined(_M_IA64) || defined(_M_AMD64)
#define ALIGNMENT_MACHINE
#define UNALIGNED   __unaligned
#if defined(_WIN64)
#define UNALIGNED64 __unaligned
#else
#define UNALIGNED64
#endif
#else
#undef ALIGNMENT_MACHINE
#ifndef UNALIGNED
#define UNALIGNED
#endif
#ifndef UNALIGNED64
#define UNALIGNED64
#endif
#endif

/**
 * jm_countof helper
 */
#if !defined(jm_countof)

#if !defined(__cplusplus)

#define jm_countof(_Array)      (sizeof(_Array) / sizeof(_Array[0]))

#else

extern "C++"
{
template <typename _CountofType, size_t _SizeOfArray>
char (*__jm_countof_helper(UNALIGNED _CountofType (&_Array)[_SizeOfArray]))[_SizeOfArray];

#define jm_countof(_Array)      sizeof(*__jm_countof_helper(_Array))
}

#endif  /* !defined(__cplusplus) */

#endif  /* !defined(jm_countof) */

#define ITT_SYNC_CREATE(obj, type, name)

NS_JIMI_BEGIN

/**
 * @cond INTERNAL
 * @brief Identifiers declared inside namespace internal should never be used directly by client code.
 */
namespace internal {

//! Base class for types that should not be assigned.
class no_assign {
    // Deny assignment
    void operator= (const no_assign &);
public:
#if __GNUC__
    //! Explicitly define default construction, because otherwise gcc issues gratuitous warning.
    no_assign() {}
#endif /* __GNUC__ */
};

//! Base class for types that should not be copied or assigned.
class no_copy: no_assign {
    //! Deny copy construction
    no_copy(const no_copy &);
public:
    //! Allow default construction
    no_copy() {}
};

} // internal
//! @endcond

NS_JIMI_END

#endif  /* _JIMI_DEF_H_ */
