
#ifndef _JIMI_THREAD_THREAD_DEF_H_
#define _JIMI_THREAD_THREAD_DEF_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"

#define HANDLE_IS_VALID(X)      ((X) != NULL && (X) != INVALID_HANDLE_VALUE)

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

#endif  /* _WIN32 */

namespace jimi {

#if defined(_WIN32)
typedef HANDLE jm_handle_t;
#else
typedef void * jm_handle_t;
#endif  /* _WIN32 */

/* For return value. */
typedef struct ErrorCode
{
    static const uint32_t kErrorSuccess = ERROR_SUCCESS;
    static const uint32_t kNoError      = NO_ERROR;
} ErrorCode;

/* For WaitForObject() */
typedef struct Timeout
{
    static const uint32_t kInfinite = INFINITE;
    static const uint32_t kIgnore   = IGNORE;

    /* The waiting time when Thread::Abort() or Thread::Terminate()
     * terminate the thread, the default value is 5000 ms. */
    static const uint32_t kAbortWaitTime = 5000;
} Timeout;

/* For class Event() */
typedef struct WaitFor
{
    static const uint32_t kSignalled    = WAIT_OBJECT_0;
    static const uint32_t kTimeout      = WAIT_TIMEOUT;
    static const uint32_t kAbandoned    = WAIT_ABANDONED;       // For Mutex
    static const uint32_t kIoCompletion = WAIT_IO_COMPLETION;
    static const uint32_t kWaitObject_0 = WAIT_OBJECT_0;
    static const uint32_t kAbandoned_0  = WAIT_ABANDONED_0;
    static const uint32_t kFailed       = WAIT_FAILED;
} WaitFor;

typedef struct SignalState
{
    static const uint32_t kUndefined    = (uint32_t)-1;
    static const uint32_t kSignalled    = TRUE;
    static const uint32_t kNoSignalled  = FALSE;
} SignalState;

typedef struct EventResetMode
{
    static const uint32_t kUndefined    = (uint32_t)-1;
    static const uint32_t kAutoReset    = FALSE;
    static const uint32_t kManualReset  = TRUE;
} EventResetMode;

/* For class ThreadBase() */
typedef struct ThreadVerifySign
{
    static const uint32_t kVerifySign       = 'JIMI';
} ThreadVerifySign;

typedef struct ThreadInitFlag
{
    static const uint32_t kCreateDefault    = 0;
    static const uint32_t kCreateSuspended  = CREATE_SUSPENDED;
} ThreadInitFlag;

typedef struct ThreadPriority
{
    static const uint32_t kLowest       = THREAD_PRIORITY_LOWEST;
    static const uint32_t kBelowNormal  = THREAD_PRIORITY_BELOW_NORMAL;
    static const uint32_t kNormal       = THREAD_PRIORITY_NORMAL;
    static const uint32_t kAboveNormal  = THREAD_PRIORITY_ABOVE_NORMAL;
    static const uint32_t kHighest      = THREAD_PRIORITY_HIGHEST;

    static const uint32_t kTimeCritical = THREAD_PRIORITY_TIME_CRITICAL;
    static const uint32_t kIdle         = THREAD_PRIORITY_IDLE;
} ThreadPriority;

typedef struct ThreadBackgroundMode
{
    static const uint32_t kBegin        = THREAD_MODE_BACKGROUND_BEGIN;
    static const uint32_t kEnd          = THREAD_MODE_BACKGROUND_END;
} ThreadBackgroundMode;

/* For class ThreadBase() thread status. */
enum ThreadStatusShift
{
    // Thread extra status
    kBackgroundStatusShift      = 31,
    kJoinStatusShift            = 30,

    // Thread status
    kAliveStatusShift           = 23,
    kRunningStatusShift         = 22
};

enum ThreadStatusMask
{
    // Thread extra status mask
    kBackgroundStatusMask       = (1UL << kBackgroundStatusShift),
    kJoinStatusMask             = (1UL << kJoinStatusShift),

    // Thread status mask
    kAliveStatusMask            = (1UL << kAliveStatusShift),
    kRunningStatusMask          = (1UL << kRunningStatusShift),

    kAliveAndRunningStatusMask  = kAliveStatusMask | kRunningStatusMask,

    kThreadExtraStatusMask      = ((1UL << kBackgroundStatusShift) | (1UL << kJoinStatusShift)),
    kThreadStatusMask           = ~kThreadExtraStatusMask,

    kThreadStatusValueMask      =  0x0000FFFFUL
};

enum ThreadDef
{
    /* The waiting time when Thread::Abort() or Thread::Terminate()
     * terminate the thread, the default value is 5000 ms. */
    kAbortWaitTime  = 5000
};

enum ThreadStatusDef
{
    kThreadStatusFirst = 0,

    kThreadStatus_Unknown = kThreadStatusFirst,

    // All Unrunning Status
    kThreadStatus_UnStarted,
    kThreadStatus_Stopped,
    kThreadStatus_Aborted,

    // All Alive Status
    kThreadStatus_StartPending,
    kThreadStatus_StopPending,
    kThreadStatus_AbortPending,
    kThreadStatus_ClosePending,

    // All Running Status
    kThreadStatus_Running,
    kThreadStatus_Suspended,
    kThreadStatus_SuspendPending,
    kThreadStatus_ResumePending,
    kThreadStatus_ThreadProcOver,

    kThreadStatusLast
};

static const char * s_szThreadStatusString[] = {
    "Unknown",
    "UnStarted",
    "Stopped",
    "Aborted",
    "StartPending",
    "StopPending",
    "AbortPending",
    "ClosePending",
    "Running",
    "Suspended",
    "SuspendPending",
    "ResumePending",
    "ThreadProcOver",
    "ThreadStatusLast"
};

}  /* namespace jimi */

#endif  /* _JIMI_THREAD_THREAD_DEF_H_ */
