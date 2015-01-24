
#ifndef _JIMIC_ASSERT_H_
#define _JIMIC_ASSERT_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimic/basic/jimic_config.h"
#include "jimic/basic/jimic_declare.h"

#include <assert.h>

#ifndef JIMIC_EXPORTED_FUNC
#if _MSC_VER >= 1400
#define JIMIC_EXPORTED_FUNC   __cdecl
#else
#define JIMIC_EXPORTED_FUNC
#endif
#endif  /* JIMIC_EXPORTED_FUNC */

//! Type for an assertion handler
typedef void (*jimic_assertion_handler_type)(const char * filename, int line,
                                             const char * expression, const char * comment);

#if JIMIC_USE_ASSERT

//! Assert that x is true.
/** If x is false, print assertion failure message.  
    If the comment argument is not NULL, it is printed as part of the failure message.  
    The comment argument has no other effect. */
#define JIMIC_ASSERT_MARCO(predicate, message) \
    ((predicate) ? ((void)0) : jimic_assertion_failure(__FILE__, __LINE__, #predicate, message))

#ifndef JIMIC_ASSERT
#define JIMIC_ASSERT_TRUE(predicate)                JIMIC_ASSERT_MARCO(!(predicate),  NULL)
#define JIMIC_ASSERT_FALSE(predicate)               JIMIC_ASSERT_MARCO(!!(predicate), NULL)
#define JIMIC_ASSERT(predicate)                     JIMIC_ASSERT_FALSE(predicate)
#endif

#ifndef JIMIC_ASSERT_EX
#define JIMIC_ASSERT_EX_TRUE(predicate, comment)    JIMIC_ASSERT_MARCO(!(predicate),  comment)
#define JIMIC_ASSERT_EX_FALSE(predicate, comment)   JIMIC_ASSERT_MARCO(!!(predicate), comment)
#define JIMIC_ASSERT_EX(predicate, comment)         JIMIC_ASSERT_EX_FALSE(predicate, comment)
#endif

#else  /* !JIMIC_USE_ASSERT */

//! No-op version of JIMIC_ASSERT.
#define JIMIC_ASSERT_TRUE(predicate)                ((void)0)
#define JIMIC_ASSERT_FALSE(predicate)               ((void)0)
#define JIMIC_ASSERT(predicate)                     JIMIC_ASSERT_FALSE(predicate)

//! "Extended" version is useful to suppress warnings if a variable is only used with an assert
#define JIMIC_ASSERT_EX_TRUE(predicate, comment)    ((void)0)
#define JIMIC_ASSERT_EX_FALSE(predicate, comment)   ((void)0)
#define JIMIC_ASSERT_EX(predicate, comment)         JIMIC_ASSERT_EX_FALSE(predicate, comment)

#endif  /* !JIMIC_USE_ASSERT */

#ifndef jimic_assert
#define jimic_assert_true           JIMIC_ASSERT_TRUE
#define jimic_assert_false          JIMIC_ASSERT_FALSE
#define jimic_assert                JIMIC_ASSERT
#endif

#ifndef jimic_assert_ex
#define jimic_assert_ex_true        JIMIC_ASSERT_EX_TRUE
#define jimic_assert_ex_false       JIMIC_ASSERT_EX_FALSE
#define jimic_assert_ex             JIMIC_ASSERT_EX
#endif

#ifdef __cplusplus
extern "C" {
#endif

    //! Set assertion handler and return previous value of it.
    jimic_assertion_handler_type JIMIC_EXPORTED_FUNC
    set_c_assertion_handler(jimic_assertion_handler_type new_handler);

    //! Process an assertion failure.
    /** Normally called from JIMIC_ASSERT macro.
        If assertion handler is null, print message for assertion failure and abort.
        Otherwise call the assertion handler. */
    void JIMIC_EXPORTED_FUNC jimic_assertion_failure(const char * filename, int line,
                                                     const char * expression, const char * comment);

#if !JIMIC_MALLOC_BUILD
    //! Report a runtime warning.
    void JIMIC_EXPORTED_FUNC jimic_runtime_warning(const char * format, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif  /* _JIMIC_ASSERT_H_ */
