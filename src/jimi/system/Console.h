
#ifndef _JIMI_SYSTEM_CONSOLE_H_
#define _JIMI_SYSTEM_CONSOLE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/internal/NonCopyable.h>

#include <stdio.h>
#include <conio.h>
#include <stdarg.h>

NS_JIMI_BEGIN

class Console : public jimi::NonCopyable
{
public:
#if defined(JIMI_HAS_DEFAULTED_FUNCTIONS) && (JIMI_HAS_DEFAULTED_FUNCTIONS != 0)
    //! Allow default construction
    JIMI_CONSTEXPR Console() = default;
    ~Console() = default;
#else
    //! Allow default construction
    Console() {}
    ~Console() {}
#endif

    static void Write(const char *fmt, ...) {
        va_list arg_list;
        va_start(arg_list, fmt);
        vprintf(fmt, arg_list);
        va_end(arg_list);
    }

    static void WriteLine(const char *fmt, ...) {
        va_list arg_list;
        va_start(arg_list, fmt);
        vprintf(fmt, arg_list);
        va_end(arg_list);
        printf("\n");
    }

    static int ReadKey() {
#if 0
        printf("Press any key to continue ...\n");
#else
        printf("按任意键继续 ...\n");
#endif
        int keycode = _getch();
        printf("\n");
        return keycode;
    }
};

static class Console Console;

NS_JIMI_END

#endif  /* !_JIMI_SYSTEM_CONSOLE_H_ */
