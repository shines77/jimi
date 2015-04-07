
#ifndef _JIMIC_SYSTEM_GETCHAR_H_
#define _JIMIC_SYSTEM_GETCHAR_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/// <comment>
///
/// What is equivalent to getch() & getche() in Linux?
///
/// From: http://stackoverflow.com/questions/7469139/what-is-equivalent-to-getch-getche-in-linux
///
/// </comment>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__linux__)

//static void init_terminal_os(int echo);
//static void reset_terminal_os(void);

/* Read 1 character - echo defines echo mode */
int jimi_getch_term(int echo);

#endif  /* __linux__ */

/* Read 1 character without echo */
int jimi_getch(void);

/* Read 1 character with echo */
int jimi_getche(void);

/* Sleep for the platform */
void jimi_sleep(unsigned int millisec);

/* Sleep for Windows or MinGW */
void jimi_wsleep(unsigned int millisec);

#ifdef __cplusplus
}
#endif

#endif  /* !_JIMIC_SYSTEM_GET_CHAR_H_ */
