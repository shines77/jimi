
#ifndef _JIMI_BASIC_ASSERT_H_
#define _JIMI_BASIC_ASSERT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/config.h"
#include "jimi/basic/declare.h"

#include <stddef.h>
#include <assert.h>

#ifndef JIMI_EXPORTED_FUNC
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#define JIMI_EXPORTED_FUNC   __cdecl
#else
#define JIMI_EXPORTED_FUNC
#endif
#endif  /* JIMI_EXPORTED_FUNC */

#if JIMI_USE_ASSERT

//! Assert that x is true.
/** If x is false, print assertion failure message.  
    If the comment argument is not NULL, it is printed as part of the failure message.  
    The comment argument has no other effect. */
#define JIMI_ASSERT_NS(predicate, message) \
    ((predicate) ? ((void)0) : jimi::assertion_failure(__FILE__, __LINE__, #predicate, message))

#ifndef JIMI_ASSERT
#define JIMI_ASSERT_TRUE(predicate)                 JIMI_ASSERT_NS(!(predicate),  NULL)
#define JIMI_ASSERT_FALSE(predicate)                JIMI_ASSERT_NS(!!(predicate), NULL)
#define JIMI_ASSERT(predicate)                      JIMI_ASSERT_FALSE(predicate)
#endif

#ifndef JIMI_ASSERT_EX
#define JIMI_ASSERT_EX_TRUE(predicate, comment)     JIMI_ASSERT_NS(!(predicate),  comment)
#define JIMI_ASSERT_EX_FALSE(predicate, comment)    JIMI_ASSERT_NS(!!(predicate), comment)
#define JIMI_ASSERT_EX(predicate, comment)          JIMI_ASSERT_EX_FALSE(predicate, comment)
#endif

#else  /* !JIMI_USE_ASSERT */

//! No-op version of JIMI_ASSERT.
#define JIMI_ASSERT_TRUE(predicate)                 ((void)0)
#define JIMI_ASSERT_FALSE(predicate)                ((void)0)
#define JIMI_ASSERT(predicate)                      JIMI_ASSERT_FALSE(predicate)

//! "Extended" version is useful to suppress warnings if a variable is only used with an assert
#define JIMI_ASSERT_EX_TRUE(predicate, comment)     ((void)0)
#define JIMI_ASSERT_EX_FALSE(predicate, comment)    ((void)0)
#define JIMI_ASSERT_EX(predicate, comment)          JIMI_ASSERT_EX_FALSE(predicate, comment)

#endif  /* !JIMI_USE_ASSERT */

#ifndef jimi_assert
#define jimi_assert_true            JIMI_ASSERT_TRUE
#define jimi_assert_false           JIMI_ASSERT_FALSE
#define jimi_assert                 JIMI_ASSERT
#endif

#ifndef jimi_assert_ex
#define jimi_assert_ex_true         JIMI_ASSERT_EX_TRUE
#define jimi_assert_ex_false        JIMI_ASSERT_EX_FALSE
#define jimi_assert_ex              JIMI_ASSERT_EX
#endif

namespace jimi {

    //! Type for an assertion handler
    typedef void (*assertion_handler_type)(const char * filename, int line,
                                           const char * expression, const char * comment);

    //! Set assertion handler and return previous value of it.
    assertion_handler_type
    JIMI_EXPORTED_FUNC set_assertion_handler(assertion_handler_type new_handler);

    //! Process an assertion failure.
    /** Normally called from JIMI_ASSERT macro.
        If assertion handler is null, print message for assertion failure and abort.
        Otherwise call the assertion handler. */
    void JIMI_EXPORTED_FUNC assertion_failure(const char * filename, int line,
                                              const char * expression, const char * comment);

#if !JIMI_MALLOC_BUILD
namespace internal {
    //! Report a runtime warning.
    void JIMI_EXPORTED_FUNC runtime_warning(const char * format, ...);
}
#endif  /* !JIMI_MALLOC_BUILD */

namespace internal {
    //! Report a debug error.
    void JIMI_EXPORTED_FUNC handle_perror(int error_code, const char *error_info);
}

}  /* namespace jimi */

#endif  /* !_JIMI_BASIC_ASSERT_H_ */
