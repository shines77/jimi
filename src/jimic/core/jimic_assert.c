
#include <jimic/core/jimic_def.h>
#include <jimic/core/jimic_assert.h>
#include <jimic/string/jm_strings.h>

// include headers for required function declarations
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if _MSC_VER
#include <crtdbg.h>
#ifndef JIMIC_USE_DBGBREAK_DLG
#define JIMIC_USE_DBGBREAK_DLG   JIMIC_USE_DEBUG
#endif
#endif

//! Type for an assertion handler
typedef void (*jimic_assertion_handler_type)(const char * filename, int line,
                                             const char * expression, const char * comment);

static jimic_assertion_handler_type jimic_assertion_handler;

jimic_assertion_handler_type JIMIC_EXPORTED_FUNC
set_c_assertion_handler(jimic_assertion_handler_type new_handler) {
    jimic_assertion_handler_type old_handler = jimic_assertion_handler;
    jimic_assertion_handler = new_handler;
    return old_handler;
}

void JIMIC_EXPORTED_FUNC jimic_assertion_failure(const char * filename, int line,
                                                 const char * expression, const char * comment) {
    jimic_assertion_handler_type assert_handler;
    static int already_failed;
    if (assert_handler = jimic_assertion_handler) {
        (*jimic_assertion_handler)(filename, line, expression, comment);
    }
    else {
        if (!already_failed) {
            already_failed = TRUE;
            fprintf(stderr, "Assertion %s failed on line %d of file %s\n", expression, line, filename);
            if (comment)
                fprintf(stderr, "Detailed description: %s\n", comment);
#if JIMIC_USE_DBGBREAK_DLG
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

#if JIMIC_USE_ASSERT

#if !JIMI_MALLOC_BUILD
    //! Report a runtime warning.
    void JIMI_EXPORTED_FUNC jimic_runtime_warning(const char * format, ...)
    {
        char str[1024];
        memset(str, 0, 1024);
        va_list args;
        va_start(args, format);
        jm_vsnprintf(str, 1024, 1024 - 1, format, args);
        va_end(args);
        fprintf(stderr, "Jimi Warning: %s\n", str);
    }
#endif

#endif /* JIMIC_USE_ASSERT */
