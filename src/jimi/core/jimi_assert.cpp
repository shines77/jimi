
#include "jimi/core/jimi_assert.h"

#include "jimic/string/jm_strings.h"

// include headers for required function declarations
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if _MSC_VER
#include <crtdbg.h>
#ifndef JIMI_USE_DBGBREAK_DLG
#define JIMI_USE_DBGBREAK_DLG   JIMI_USE_DEBUG
#endif
#endif

namespace jimi {

//! Type for an assertion handler
typedef void (*assertion_handler_type)(const char * filename, int line,
                                       const char * expression, const char * comment);

static assertion_handler_type assertion_handler;

assertion_handler_type JIMI_EXPORTED_FUNC
set_assertion_handler(assertion_handler_type new_handler) {
    assertion_handler_type old_handler = assertion_handler;
    assertion_handler = new_handler;
    return old_handler;
}

void JIMI_EXPORTED_FUNC assertion_failure(const char * filename, int line,
                                          const char * expression, const char * comment) {
    if (assertion_handler_type assert_handler = assertion_handler) {
        (*assert_handler)(filename, line, expression, comment);
    }
    else {
        static bool already_failed;
        if (!already_failed) {
            already_failed = true;
            fprintf(stderr, "Assertion %s failed on line %d of file %s\n", expression, line, filename);
            if (comment)
                fprintf(stderr, "Detailed description: %s\n", comment);
#if JIMI_USE_DBGBREAK_DLG
            if (1 == _CrtDbgReport(_CRT_ASSERT, filename, line, "jimi_shared_debug.dll",
                "%s\r\n%s", expression, comment ? comment : ""))
                _CrtDbgBreak();
#else
            fflush(stderr);
            abort();
#endif
        }
    }
}

#if JIMI_USE_ASSERT

#if !JIMI_MALLOC_BUILD
namespace internal {
    //! Report a runtime warning.
    void JIMI_EXPORTED_FUNC runtime_warning(const char * format, ...)
    {
        char str[1024];
        memset(str, 0, 1024);
        va_list args;
        va_start(args, format);
        jm_vsnprintf(str, 1024, 1024 - 1, format, args);
        va_end(args);
        fprintf(stderr, "Jimi Warning: %s\n", str);
    }
}  // namespace internal
#endif  /* JIMI_MALLOC_BUILD */

#endif  /* !JIMI_USE_ASSERT */

namespace internal {
    //! Report a debug error.
    void JIMI_EXPORTED_FUNC handle_perror(int error_code, const char *error_info)
    {
        //
    }
}

}  /* namespace jimi */
