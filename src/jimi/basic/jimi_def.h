
#ifndef _JIMI_DEF_H_
#define _JIMI_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/jimi_platform_def.h"
#include "jimi/basic/jimi_compiler_def.h"

#if JIMI_IS_WINDOWS
    #include "jimi/basic/win32/jimi_targetver.h"
#endif

#include "jimi/basic/jimi_config.h"
#include "jimi/basic/jimi_export.h"

#include "jimi/basic/jimi_stdint.h"
#include "jimi/basic/jimi_common.h"

#ifndef __has_feature
#define __has_feature(X)        (0)
#endif

// Is noexcept supported?
#if defined(__clang__) && __has_feature(cxx_noexcept) || \
    defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
    defined(_MSC_FULL_VER) && (_MSC_FULL_VER >= 180021114 && _MSC_FULL_VER >= 190021114)
    // support noexcept
    #define NOEXCEPT        noexcept
#else  /* ! Is noexcept supported? */

    #if defined(_MSC_VER)
      #include <yvals.h>
      #ifndef NOEXCEPT
        #define NOEXCEPT    _NOEXCEPT
        //#define NOEXCEPT
      #endif
    #endif  /* _MSC_VER */

#endif  /* Is noexcept supported? */

/**
 * define the name of jimi namespaces
 */
#include "jimi/basic/jimi_namespace.h"

/**
 * define jimic function declare type and inline defines
 */
#include "jimi/basic/jimi_declare.h"

/**
 * for assert defines
 */
#include "jimi/basic/jimi_assert.h"

/**
 * for jimi_winapi
 */
#if defined(JIMI_IS_MSVC)
#ifndef __CLR_OR_STD_CALL
#if defined (_M_CEE) || defined (MRTDLL)
#define __CLR_OR_STD_CALL   __clrcall
#else  /* defined (_M_CEE) || defined (MRTDLL) */
#define __CLR_OR_STD_CALL   __stdcall
#endif  /* defined (_M_CEE) || defined (MRTDLL) */
#endif  /* __CLR_OR_STD_CALL */

#define JIMI_WINAPI             __CLR_OR_STD_CALL
#else
#define JIMI_WINAPI
#endif  /* JIMI_IS_MSVC */

/* for static_assert() under VisualStudio 2010 */
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#ifndef static_assert
#define static_assert(expr, msg)
#endif
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef JIMI_MIN
#define JIMI_MIN(a, b)          ((a) < (b) ? (a) : (b))
#endif

#ifndef JIMI_MAX
#define JIMI_MAX(a, b)          ((a) > (b) ? (a) : (b))
#endif

#if defined(_WIN64) || defined(_M_X64)
#define JIMI_SIZE_T_SIZEOF      8
#else
#define JIMI_SIZE_T_SIZEOF      4
#endif

/**
 * macro for address aligned to n bytes
 */
#define JIMI_ALIGNED_TO(n, alignment)   \
    (((n) + ((alignment) - 1)) & ~(size_t)((alignment) - 1))

#define JIMI_ADDR_ALIGNED_TO(p, alignment)   \
    ((void *)((((size_t)(void *)(p)) + ((alignment) - 1)) & ~(size_t)((alignment) - 1)))

#define JIMI_ADDR_ALIGNED_DOWNTO(p, alignment)   \
    ((void *)(((size_t)(void *)(p)) & ~(size_t)((alignment) - 1)))

#define JIMI_PVOID_INC(p, n)    ((void *)((size_t)(p) + 1))
#define JIMI_PVOID_DEC(p, n)    ((void *)((size_t)(p) - 1))

#define JIMI_PVOID_ADD(p, n)    ((void *)((size_t)(p) + (n)))
#define JIMI_PVOID_SUB(p, n)    ((void *)((size_t)(p) - (n)))

/**
 * macro for round to power of 2
 */
#define jimi_b2(x)              (        (x) | (        (x) >>  1))
#define jimi_b4(x)              ( jimi_b2(x) | ( jimi_b2(x) >>  2))
#define jimi_b8(x)              ( jimi_b4(x) | ( jimi_b4(x) >>  4))
#define jimi_b16(x)             ( jimi_b8(x) | ( jimi_b8(x) >>  8))  
#define jimi_b32(x)             (jimi_b16(x) | (jimi_b16(x) >> 16))
#define jimi_b64(x)             (jimi_b32(x) | (jimi_b32(x) >> 32))

#define jimi_next_power_of_2(x)     (jimi_b32((x) - 1) + 1)
#define jimi_next_power_of_2_64(x)  (jimi_b64((x) - 1) + 1)

#if defined(JIMI_SIZE_T_SIZEOF) && (JIMI_SIZE_T_SIZEOF == 8)
#define JIMI_ROUND_TO_POW2(N)   jimi_next_power_of_2_64(N)
#else
#define JIMI_ROUND_TO_POW2(N)   jimi_next_power_of_2(N)
#endif

namespace jimi {

template <typename value_type>
inline value_type jimi_min(value_type a, value_type b)
{
    return (a < b ? a : b);
}

template <typename value_type>
inline value_type jimi_max(value_type a, value_type b)
{
    return (a > b ? a : b);
}

}  /* namespace jimi */

/**
 * for c++ const_cast
 */
#define CONST_CAST(_Val, _TypeVal, _TypeNew) \
    reinterpret_cast<_TypeNew>(const_cast<_TypeVal>(_Val))

#define CONST_CAST_CONST(_Val, _TypeVal, _TypeNew) \
    const_cast<const _TypeNew>(reinterpret_cast<_TypeNew>(const_cast<_TypeVal>(_Val)))

/**
 * for precompiled macro to string
 */
#define JIMI_STRING_ESCAPE(x)   #x
#define JIMI_TO_STRING(x)       JIMI_STRING_ESCAPE(x)

#define JIMI_MAKE_STRING(x)     { #x, JIMI_STRING_ESCAPE(x) }

typedef struct JIMI_MACRO_T
{
    const char *name;
    const char *value;
} JIMI_MACRO_T;

/* Need size_t and ptrdiff_t */
#include <cstddef>

//#include <iostream>
//#include <string>

//using namespace std;

#if defined(_MSC_VER) && _MSC_VER >= 1400
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(disable: 4996)
#endif  /* _MSC_VER */

// warning C4290: 忽略 C++ 异常规范，但指示函数不是 __declspec(nothrow)
#pragma warning(disable : 4290)

/**
 * for tbb's itune support
 */
#define ITT_SYNC_CREATE(obj, type, name)

namespace jimi {

/**
 * @cond INTERNAL
 * @brief Identifiers declared inside namespace internal should never be used directly by client code.
 */
namespace internal {

//! Base class for types that should not be assigned.
class no_assign {
    //! Deny assignment
    void operator = (const no_assign &);
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

}  /* namespace jimi */

#endif  /* _JIMI_DEF_H_ */
