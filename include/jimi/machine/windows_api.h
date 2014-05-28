
#ifndef _JIMI_MACHINE_WINDOWS_API_H_
#define _JIMI_MACHINE_WINDOWS_API_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/platform/jimi_platform_config.h>

#if JIMI_IS_WINDOWS

#include <jimi/platform/jimi_targetver.h>

#if _XBOX

#define NONET
#define NOD3D
#include <xtl.h>

#else // Assume "usual" Windows

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#endif // _XBOX

#if _WIN32_WINNT < 0x0600
// The following Windows API function is declared explicitly;
// otherwise it fails to compile by VS2005.
#if !defined(WINBASEAPI) || (_WIN32_WINNT < 0x0501 && _MSC_VER == 1400)
    #define IOCP_WINBASEAPI     extern "C"
#else
    #define IOCP_WINBASEAPI     WINBASEAPI
#endif

IOCP_WINBASEAPI BOOL WINAPI TryEnterCriticalSection(LPCRITICAL_SECTION);
IOCP_WINBASEAPI BOOL WINAPI InitializeCriticalSectionAndSpinCount(LPCRITICAL_SECTION, DWORD);

// Overloading WINBASEAPI macro and using local functions missing in Windows XP/2003
#define InitializeCriticalSectionEx     inlineInitializeCriticalSectionEx
#define CreateSemaphoreEx               inlineCreateSemaphoreEx
#define CreateEventEx                   inlineCreateEventEx

inline BOOL WINAPI inlineInitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD)
{
    return InitializeCriticalSectionAndSpinCount(lpCriticalSection, dwSpinCount);
}

inline HANDLE WINAPI inlineCreateSemaphoreEx(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCTSTR lpName, DWORD, DWORD)
{
    return CreateSemaphore(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
}

inline HANDLE WINAPI inlineCreateEventEx(LPSECURITY_ATTRIBUTES lpEventAttributes, LPCTSTR lpName, DWORD dwFlags, DWORD)
{
    BOOL manual_reset = (dwFlags & 0x00000001) ? TRUE : FALSE; // CREATE_EVENT_MANUAL_RESET
    BOOL initial_set  = (dwFlags & 0x00000002) ? TRUE : FALSE; // CREATE_EVENT_INITIAL_SET
    return CreateEvent(lpEventAttributes, manual_reset, initial_set, lpName);
}
#endif // _WIN32_WINNT < 0x0600

#if defined(RTL_SRWLOCK_INIT)
  #ifndef IOCP_USE_SRWLOCK
    // TODO: turn it on when bug 1952 will be fixed
    #define IOCP_USE_SRWLOCK    0
  #endif
#endif // RTL_SRWLOCK_INIT

#else
    #error jimi/machine/windows_api.h should only be used for Windows based platforms
#endif // JIMI_IS_WINDOWS

#endif  /* _JIMI_MACHINE_WINDOWS_API_H_ */
