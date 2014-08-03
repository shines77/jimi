
#include <jimic/system/get_char.h>

#include <stdio.h>

#if defined(__MINGW32__)
#include <conio.h>
#include <unistd.h>
#elif defined(__linux__)
#include <termios.h>
#include <unistd.h>
#endif  /* __MINGW32__ */

#if defined(_MSC_VER)
#include <conio.h>
#include <windows.h>
#endif  /* _MSC_VER */

#if defined(__MINGW32__)

/* Read 1 character without echo */
int jimi_getch(void)
{
    return _getch();
}

/* Read 1 character with echo */
int jimi_getche(void)
{
    int ch = _getch();
    printf("%d", ch);
    return ch;
}

void jimi_sleep(int millisec)
{
    usleep(millisec * 1000);
}

#elif defined(__linux__)

/// <comment>
///
/// What is equivalent to getch() & getche() in Linux?
///
/// From: http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
///
/// </comment>

static struct termios s_term_old, s_term_new;

/* Initialize new terminal i/o settings */
void init_terminal_os(int echo)
{
    tcgetattr(0, &s_term_old);                  /* grab old terminal i/o settings */
    s_term_new = s_term_old;                    /* make new settings same as old settings */
    s_term_new.c_lflag &= ~ICANON;              /* disable buffered i/o */
    s_term_new.c_lflag &= echo ? ECHO : ~ECHO;  /* set echo mode */
    tcsetattr(0, TCSANOW, &s_term_new);         /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void reset_terminal_os(void)
{
    tcsetattr(0, TCSANOW, &s_term_old);
}

/* Read 1 character - echo defines echo mode */
int jimi_getch_term(int echo)
{
    int ch;
    init_terminal_os(echo);
    ch = getchar();
    reset_terminal_os();
    return ch;
}

/* Read 1 character without echo */
int jimi_getch(void)
{
    return jimi_getch_term(0);
}

/* Read 1 character with echo */
int jimi_getche(void)
{
    return jimi_getch_term(1);
}

void jimi_sleep(int millisec)
{
    usleep(millisec * 1000);
}

#elif defined(_MSC_VER)

/* Read 1 character without echo */
int jimi_getch(void)
{
    return _getch();
}

/* Read 1 character with echo */
int jimi_getche(void)
{
    int ch = _getch();
    printf("%d", ch);
    return ch;
}

void jimi_sleep(int millisec)
{
    Sleep(millisec);
}

#else  /* other unknown os */

/* Read 1 character without echo */
int jimi_getch(void)
{
    return (int)-1;
}

/* Read 1 character with echo */
int jimi_getche(void)
{
    return (int)-1;
}

void jimi_sleep(int millisec)
{
    // Do nothing !!
    volatile int sum = 0;
    int i, j;
    for (i = 0; i < 50000; ++i) {
        sum += i;
        for (j = 2000; j >= 0; --j) {
            sum -= j;
        }
    }
}

#endif  /* __linux__ */
