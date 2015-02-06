
#ifndef _JIMI_THREAD_THREAD_H_
#define _JIMI_THREAD_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "jimi/basic/stddef.h"
#include "jimi/thread/ThreadDef.h"

#include "jimi/log/log.h"
#include "jimic/basic/asm.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS) || defined(__WIN32__) || defined(__NT__)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>
#include <process.h>

#else  /* !_WIN32 */

#include <pthread.h>

#endif  /* _WIN32 */

#ifndef THREAD_STATUS
#define THREAD_STATUS(bAlive, bRunning, nStatus)   \
    (((((bAlive) & 1UL) << kAliveStatusShift) | (((bRunning) & 1UL) << kRunningStatusShift)) + (nStatus))
#endif  /* THREAD_STATUS */

namespace jimi {

typedef struct ThreadStatus
{
    static const uint32_t kUnknown          = THREAD_STATUS(kIsDead,  kIsStopped, kThreadStatus_Unknown);

    // All Unrunning Status
    static const uint32_t kUnStarted        = THREAD_STATUS(kIsDead,  kIsStopped, kThreadStatus_UnStarted);
    static const uint32_t kStopped          = THREAD_STATUS(kIsDead,  kIsStopped, kThreadStatus_Stopped);
    static const uint32_t kAborted          = THREAD_STATUS(kIsDead,  kIsStopped, kThreadStatus_Aborted);

    // All Alive Status
    static const uint32_t kStartPending     = THREAD_STATUS(kIsAlive, kIsStopped, kThreadStatus_StartPending);
    static const uint32_t kStopPending      = THREAD_STATUS(kIsAlive, kIsStopped, kThreadStatus_StopPending);
    static const uint32_t kAbortPending     = THREAD_STATUS(kIsAlive, kIsStopped, kThreadStatus_AbortPending);
    static const uint32_t kClosePending     = THREAD_STATUS(kIsAlive, kIsStopped, kThreadStatus_ClosePending);

    // All Running Status
    static const uint32_t kRunning          = THREAD_STATUS(kIsAlive, kIsRunning, kThreadStatus_Running);
    static const uint32_t kSuspended        = THREAD_STATUS(kIsAlive, kIsRunning, kThreadStatus_Suspended);
    static const uint32_t kSuspendPending   = THREAD_STATUS(kIsAlive, kIsRunning, kThreadStatus_SuspendPending);
    static const uint32_t kResumePending    = THREAD_STATUS(kIsAlive, kIsRunning, kThreadStatus_ResumePending);
    static const uint32_t kThreadProcOver   = THREAD_STATUS(kIsAlive, kIsRunning, kThreadStatus_ThreadProcOver);
} ThreadStatus;

#if defined(WIN32) || defined(_WIN32) || defined(_WINDOWS) || defined(__WIN32__) || defined(__NT__)

/**
 * the Thread base class
 */
template <typename T>
class ThreadBase
{
public:
    typedef jm_handle_t         handle_t;
    typedef jm_handle_t         thread_handle_t;
    typedef uint32_t            thread_status_t;
    typedef unsigned            thread_id_t;
    typedef uint32_t            affinity_t;

    typedef void (*thread_proc_t)(void *lpParam);

    typedef struct Thread_Params
    {
        ThreadBase *    pThread;        // ThreadBase object.
        void *          pParam;         // The address of the thread parameters.
        unsigned int    uVerifySign;    // The verify sign, value is string "JIMI".
        int             nThreadIdx;     // The index of thread, base on 0.
    } thread_param_t;

public:
    ThreadBase(void);
    ThreadBase(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~ThreadBase(void);

private:
#if defined(JIMI_HAS_DELETED_FUNCTIONS) && (JIMI_HAS_DELETED_FUNCTIONS != 0)
    //! Deny copy construction
    explicit ThreadBase(const ThreadBase &) = delete;
    //! Deny assignment operator
    void operator = (const ThreadBase &) = delete;
#else
    //! Deny copy construction
    explicit ThreadBase(const ThreadBase &) {}
    //! Deny assignment operator
    void operator = (const ThreadBase &) {}
#endif  /* JIMI_HAS_DELETED_FUNCTIONS */

public:
    void Destroy();
    void OnDestroy();

    void Detach();
    void OnDetach();

    void ThreadProcDone();

    thread_proc_t GetThreadProc() const { return pThreadProc; }
    bool SetThreadProc(thread_proc_t thread_proc);

    operator thread_handle_t     () const     { return hThread;   }
    thread_handle_t * operator & () const     { return &hThread;  }
    thread_handle_t & GetThreadHandle() const { return hThread;   }
    thread_id_t GetThreadId() const           { return nThreadId; }

    thread_id_t GetCurrentThreadId() const   { return ::GetCurrentThreadId(); }
    thread_handle_t GetCurrentThread() const { return (thread_handle_t)::GetCurrentThread(); }

    bool IsValid() const;

    bool IsAlive() const;
    bool IsRunning() const;
    bool IsAliveOrRunning() const;
    bool IsAliveAndRunning() const;
    bool IsSuspended() const;

    thread_status_t GetThreadState() const { return nStatus; }

    void SetThreadState(thread_status_t nNewStatus) {
        jmLog.debug("ThreadBase<T>::SetThreadState():");
        jmLog.debug("newStatus = 0x%08X(%s), oldStatus = 0x%08X(%s).",
                   nNewStatus, GetThreadStateString(nNewStatus),
                   nStatus, GetThreadStateString(nStatus));
        nStatus = nNewStatus;
    }

    const char * GetThreadStateString(thread_status_t nSpecStatus) const {
        int index = nSpecStatus & kThreadStatusValueMask;
        index = JIMI_MIN(index, kThreadStatusLast - 1);
        return s_szThreadStatusString[index];
    }

    const char * GetThreadStateString() const {
        return GetThreadStateString(nStatus);
    }

    thread_status_t Start(void *pParam = NULL,
                          unsigned uInitFlag = ThreadInitFlag::kCreateDefault);
    thread_status_t Stop(uint32_t uWaitTime = Timeout::kAbortWaitTime,
                         uint32_t uExitCode = uint32_t(-1));

    thread_status_t Wait(handle_t hObject, uint32_t uWaitTime = Timeout::kInfinite);
    thread_status_t Join(uint32_t uWaitTime = Timeout::kInfinite);

    thread_status_t Suspend();
    thread_status_t Resume();

    thread_status_t Abort(uint32_t uWaitTime = Timeout::kAbortWaitTime,
                          uint32_t uExitCode = 0);
    thread_status_t Interrupt(uint32_t uWaitTime = Timeout::kAbortWaitTime,
                              uint32_t uExitCode = 0);
    thread_status_t Terminate(uint32_t uWaitTime = Timeout::kAbortWaitTime,
                              uint32_t uExitCode = 0);

    int  GetThreadPriority() const { return ::GetThreadPriority(hThread); }
    bool SetThreadPriority(int nNewPriority) {
        return (bool)(::SetThreadPriority(hThread, nNewPriority) == TRUE);
    }

    affinity_t GetProcessorAffinity() const;
    affinity_t GetThreadAffinity() const;

    affinity_t SetProcessorAffinity();
    affinity_t SetThreadAffinity();

    static void SpinWait(int32_t iterations);
    static void SpinOne();

    static void Sleep(uint32_t uMilliSecs);
    static int  YieldTo();

    void ThreadProc(void *lpParam);

    static unsigned JIMI_WINAPI ThreadProcBase(void *lpParam);

protected:
    thread_proc_t   pThreadProc;
    
private:
    thread_handle_t hThread;
    thread_id_t     nThreadId;
    thread_status_t nStatus;
};

template <typename T>
ThreadBase<T>::ThreadBase(void)
: pThreadProc(NULL)
, hThread(NULL)
, nThreadId(0)
, nStatus(ThreadStatus::kUnknown)
{
}

template <typename T>
ThreadBase<T>::~ThreadBase(void)
{
    this->Destroy();
}

template <typename T>
inline
void ThreadBase<T>::Destroy()
{
    T *pT = static_cast<T *>(this);
    pT->OnDestroy();

    if (IsValid()) {
        if (IsAlive())
            this->Terminate();
        if (IsValid())
            ::CloseHandle(hThread);
        hThread = NULL;
        nThreadId = 0;
        nStatus = ThreadStatus::kUnknown;
        pThreadProc = NULL;
    }
}

template <typename T>
inline
void ThreadBase<T>::OnDestroy()
{
    jmLog.debug("ThreadBase<T>::OnDestroy(), error = %d.", ::GetLastError());
}

template <typename T>
inline
bool ThreadBase<T>::IsValid() const
{
    return (hThread != NULL && hThread != INVALID_HANDLE_VALUE);
}

template <typename T>
inline
bool ThreadBase<T>::IsAlive() const
{
    return ((nStatus & kAliveStatusMask) != 0);
}

template <typename T>
inline
bool ThreadBase<T>::IsRunning() const
{
    return ((nStatus & kRunningStatusMask) != 0);
}

template <typename T>
inline
bool ThreadBase<T>::IsAliveOrRunning() const
{
    return ((nStatus & kAliveAndRunningStatusMask) != 0);
}

template <typename T>
inline
bool ThreadBase<T>::IsAliveAndRunning() const
{
    return ((nStatus & kAliveAndRunningStatusMask) == kAliveAndRunningStatusMask);
}

template <typename T>
inline
bool ThreadBase<T>::IsSuspended() const
{
    return (IsAliveAndRunning()
        && (this->GetThreadState() == ThreadStatus::kSuspended
            || this->GetThreadState() == ThreadStatus::kSuspendPending));
}

template <typename T>
inline
bool ThreadBase<T>::SetThreadProc(thread_proc_t thread_proc)
{
    if ((!IsAlive()) && (!IsRunning())) {
        pThreadProc = thread_proc;
        return true;
    }
    return false;
}

template <typename T>
void ThreadBase<T>::ThreadProcDone()
{
    jmLog.debug("ThreadBase<T>::ThreadProcDone(), error = %d.", ::GetLastError());
    if (IsValid()) {
        hThread = NULL;
        nThreadId = 0;
        nStatus = ThreadStatus::kUnknown;
        nExtraStatus = ThreadExtraStatus::kUnknown;
    }
}

/* ThreadBase() local default thread_proc function. */
template <typename T>
void ThreadBase<T>::ThreadProc(void *lpParam)
{
    // Do nothing !!
    jmLog.debug("ThreadBase<T>::ThreadProc().");
}

template <typename T>
/* static */
unsigned JIMI_WINAPI ThreadBase<T>::ThreadProcBase(void *lpParam)
{
    jmLog.debug("ThreadBase<T>::ThreadProcBase() Enter.");
    jmLog.debug("ThreadBase<T>::ThreadProcBase(), lpParam = 0x%08X.", lpParam);

    thread_param_t *pThreadParam = static_cast<thread_param_t *>(lpParam);
    ThreadBase<T> *pThread = NULL;
    void *pObject = NULL;
    if (pThreadParam != NULL) {
        pThread = pThreadParam->pThread;
        pObject = pThreadParam->pParam;

        if (pThread && pThread->IsValid())
            pThread->SetThreadState(ThreadStatus::kRunning);

        T *pThis = static_cast<T *>(pThread);
        if (pThis != NULL) {
            thread_proc_t pThreadProc = pThis->GetThreadProc();
            if (pThreadProc != NULL)
                pThreadProc(lpParam);
            else
                pThis->ThreadProc(pObject);

            if (pThreadParam) {
                pThreadParam->pThread = NULL;
                pThreadParam->pParam = NULL;
                pThreadParam->uVerifySign = 0;
                pThreadParam->nThreadIdx = -1;
                delete pThreadParam;
            }
        }

        //ThreadBase<T>::Sleep(3000);
    }
    jmLog.debug("ThreadBase<T>::ThreadProcBase() Over.");

    if (pThread && pThread->IsValid()) {
        //pThread->SetThreadState(ThreadStatus::kThreadProcOver);
        pThread->SetThreadState(ThreadStatus::kStopped);
        //pThread->ThreadProcDone();
    }
    return 0;
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Start(void *pParam /* = NULL */,
                                                             unsigned uInitFlag /* = ThreadInitFlag::kCreateDefault */)
{
    jmLog.debug("ThreadBase<T>::Start() Enter.");

    if (!IsValid()) {
        if (!IsAliveOrRunning()) {
            thread_param_t *pParams = new thread_param_t;
            pParams->pThread = (ThreadBase<T> *)this;
            pParams->pParam = pParam;
            pParams->uVerifySign = ThreadVerifySign::kVerifySign;
            pParams->nThreadIdx = 0;
            hThread = (thread_handle_t)::_beginthreadex(NULL, 0, &ThreadBase<T>::ThreadProcBase,
                (void *)pParams, uInitFlag, (unsigned *)&nThreadId);
            // 统一把无效的句柄设置为NULL, 方便检测是否有效?(IsValid())
            if (hThread == INVALID_HANDLE_VALUE)
                hThread = NULL;
            if (IsValid()) {
                if (uInitFlag == ThreadInitFlag::kCreateSuspended)
                    this->SetThreadState(ThreadStatus::kSuspended);
                else
                    this->SetThreadState(ThreadStatus::kStartPending);
            }
        }
        else {
            jmLog.debug("ThreadBase<T>::Start() Thread is running or alive, nThread = 0x%08X.", hThread);
        }
    }
    else {
        jmLog.debug("ThreadBase<T>::Start() Thread already exists, nThread = 0x%08X.", hThread);
    }

    jmLog.debug("ThreadBase<T>::Start() Over.");
    return this->GetThreadState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Stop(uint32_t uWaitTime /* = Timeout::kAbortWaitTime */,
                                                            uint32_t nExitCode /* = -1 */)
{
    bool bThreadStartted = false;
    jmLog.debug("ThreadBase<T>::Stop() Enter.");
    if (IsValid()) {
        if (IsAliveOrRunning()) {
            bThreadStartted = true;
            this->Terminate(uWaitTime, nExitCode);
            jmLog.debug("ThreadBase<T>::Stop() successed.");
        }
    }
    if (!bThreadStartted)
        jmLog.debug("ThreadBase<T>::Stop() Thread is not startted, nThread = 0x%08X.", hThread);
    jmLog.debug("ThreadBase<T>::Stop() Over.");
    return this->GetThreadState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Suspend()
{
    if (!IsSuspended()) {
        thread_status_t oldStatus = this->GetThreadState();
        this->SetThreadState(ThreadStatus::kSuspendPending);
        if (::SuspendThread(hThread) != (DWORD)(-1))
            this->SetThreadState(ThreadStatus::kSuspended);
        else
            this->SetThreadState(oldStatus);
    }
    return this->GetThreadState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Resume()
{
    if (IsSuspended()) {
        thread_status_t oldStatus = this->GetThreadState();
        this->SetThreadState(ThreadStatus::kResumePending);
        if (::ResumeThread (hThread) != (DWORD)(-1))
            this->SetThreadState(ThreadStatus::kRunning);
        else
            this->SetThreadState(oldStatus);
    }
    return this->GetThreadState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Wait(handle_t hObject,
                                                            uint32_t uWaitTime /* = Timeout::kInfinite */)
{
    DWORD dwResult;
    jmLog.debug("ThreadBase<T>::Wait() Enter.");

    if (HANDLE_IS_VALID(hObject)) {
        int failed_counter = 0;
        this->AddThreadExtraState(ThreadStatus::kWaitSleepJoin);
        while ((dwResult = ::WaitForSingleObject(hObject, uWaitTime)) != WAIT_OBJECT_0) {
#ifdef _DEBUG
            ThreadBase<T>::Sleep(1);
            failed_counter++;
            if (failed_counter > 5)
                break;
#endif
        }
        this->RemoveThreadExtraState(ThreadStatus::kWaitSleepJoin);
        if ((dwResult = ::WaitForSingleObject(hObject, 10)) == WAIT_OBJECT_0) {
            // wait for success
        }
        else {
            // wait for failed
        }
    }

    jmLog.debug("ThreadBase<T>::Wait() Over.");
    return this->GetThreadFullState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Join(uint32_t uWaitTime /* = Timeout::kInfinite */)
{
    DWORD dwResult;
    BOOL bResult;
    jmLog.debug("ThreadBase<T>::Join() Enter.");

    if (IsValid()) {
        int failed_counter = 0;
        while (IsValid() &&
               ((dwResult = ::WaitForSingleObject(hThread, uWaitTime)) != WAIT_OBJECT_0)) {
#ifdef _DEBUG
            ThreadBase<T>::Sleep(1);
            failed_counter++;
            if (failed_counter > 5)
                break;
#endif
        }
        if (IsValid()) {
            thread_status_t oldStatus = this->GetThreadState();
            bool bIsStopped = false;
            if (IsAliveOrRunning())
                this->SetThreadState(ThreadStatus::kStopPending);
            if (IsValid()) {
                if ((dwResult = ::WaitForSingleObject(hThread, 10)) == WAIT_OBJECT_0) {
                    if (IsValid()) {
                        this->SetThreadState(ThreadStatus::kClosePending);
                        if (bResult = ::CloseHandle(hThread)) {
                            this->SetThreadState(ThreadStatus::kStopped);

                            hThread = NULL;
                            nThreadId = 0;
                            nStatus = ThreadStatus::kUnStarted;

                            bIsStopped = true;
                        }
                    }
                }
            }
            if (!bIsStopped)
                this->SetThreadState(oldStatus);
        }
    }

    jmLog.debug("ThreadBase<T>::Join() Over.");
    return this->GetThreadState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Abort(uint32_t uWaitTime /* = Timeout::kAbortWaitTime */,
                                                             uint32_t uExitCode /* = 0 */)
{
    DWORD dwResult;
    BOOL bResult;
    jmLog.debug("ThreadBase<T>::Abort() Enter.");

    if (IsValid()) {
        thread_status_t oldStatus = this->GetThreadState();
        if (IsAliveOrRunning()) {
            this->SetThreadState(ThreadStatus::kAbortPending);
            if ((dwResult = ::WaitForSingleObject(hThread, uWaitTime)) != WAIT_OBJECT_0) {
                if (bResult = ::TerminateThread(hThread, uExitCode)) {
                    this->SetThreadState(ThreadStatus::kAborted);
                }
                else {
                    jmLog.debug("ThreadBase<T>::Abort(): TerminateThread() failed. oldStatus = 0x%08X, nowStatus = 0x%08X.",
                           oldStatus, this->GetThreadState());
                    this->SetThreadState(oldStatus);
                }
            }
        }
        bool bIsStopped = false;
        oldStatus = this->GetThreadState();
        this->SetThreadState(ThreadStatus::kStopPending);
        if (IsValid()) {
            this->SetThreadState(ThreadStatus::kClosePending);
            if (bResult = ::CloseHandle(hThread)) {
                bIsStopped = true;
                this->SetThreadState(ThreadStatus::kStopped);
            }
            else {
                jmLog.debug("ThreadBase<T>::Abort(): CloseHandle() failed. oldStatus = 0x%08X, nowStatus = 0x%08X.",
                           oldStatus, this->GetThreadState());
            }
        }
        if (!bIsStopped)
            this->SetThreadState(oldStatus);
        hThread = NULL;
        nThreadId = 0;
        nStatus = ThreadStatus::kUnStarted;
    }
    jmLog.debug("ThreadBase<T>::Abort() Over.");
    return this->GetThreadState();
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Interrupt(uint32_t uWaitTime /* = Timeout::kAbortWaitTime */,
                                                                 uint32_t uExitCode /* = 0 */)
{
    return this->Abort(uWaitTime, uExitCode);
}

template <typename T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Terminate(uint32_t uWaitTime /* = Timeout::kAbortWaitTime */,
                                                                 uint32_t uExitCode /* = 0 */)
{
    return this->Abort(uWaitTime, uExitCode);
}

/* static */
template <typename T>
void ThreadBase<T>::SpinWait(int32_t iterations)
{
    int32_t i;
    for (i = iterations; i > 0; --i) {
        jimi_mm_pause();
    }
}

/* static */
template <typename T>
void ThreadBase<T>::SpinOne()
{
    jimi_mm_pause();
}

/* static */
template <typename T>
inline
void ThreadBase<T>::Sleep(uint32_t uMilliSecs)
{
    ::Sleep(uMilliSecs);
}

/* static */
template <typename T>
inline
int ThreadBase<T>::YieldTo()
{
    return (int)::SwitchToThread();
}

/**
 * the Thread class
 */
class Thread : public ThreadBase<Thread>
{
public:
    Thread(void) : ThreadBase<Thread>() {}
    Thread(thread_proc_t thread_proc) : ThreadBase<Thread>(thread_proc) {}
    ~Thread(void) {}

private:
#if defined(JIMI_HAS_DELETED_FUNCTIONS) && (JIMI_HAS_DELETED_FUNCTIONS != 0)
    //! Deny copy construction
    explicit Thread(const Thread &) = delete;
    //! Deny assignment operator
    void operator = (const Thread &) = delete;
#else
    //! Deny copy construction
    explicit Thread(const Thread &) {}
    //! Deny assignment operator
    void operator = (const Thread &) {}
#endif  /* JIMI_HAS_DELETED_FUNCTIONS */
};

#else  /* !_WIN32 */

/**
 * the Thread base class
 */
template <typename T>
class ThreadBase
{
public:
    typedef jm_handle_t thread_handle_t;
    typedef jm_handle_t handle_t;
    typedef uint32_t    thread_status_t;
    typedef unsigned    thread_id_t;
    typedef uint32_t    affinity_t;

    typedef void (*thread_proc_t)(void *lpParam);

public:
    ThreadBase(void);
    ThreadBase(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~ThreadBase(void);

private:
#if defined(JIMI_HAS_DELETED_FUNCTIONS) && (JIMI_HAS_DELETED_FUNCTIONS != 0)
    //! Deny copy construction
    explicit ThreadBase(const ThreadBase &) = delete;
    //! Deny assignment operator
    void operator = (const ThreadBase &) = delete;
#else
    //! Deny copy construction
    explicit ThreadBase(const ThreadBase &) {}
    //! Deny assignment operator
    void operator = (const ThreadBase &) {}
#endif  /* JIMI_HAS_DELETED_FUNCTIONS */

protected:
    thread_proc_t   pThreadProc;
};

template <typename T>
ThreadBase<T>::ThreadBase(void)
: pThreadProc(NULL)
{
}

template <typename T>
ThreadBase<T>::~ThreadBase(void)
{
}

/**
 * the Thread class
 */
class Thread : public ThreadBase<Thread>
{
public:
    Thread(void) : ThreadBase<Thread>() { };
    Thread(thread_proc_t thread_proc) : ThreadBase<Thread>(thread_proc) {}
    ~Thread(void) { };

private:
#if defined(JIMI_HAS_DELETED_FUNCTIONS) && (JIMI_HAS_DELETED_FUNCTIONS != 0)
    //! Deny copy construction
    explicit Thread(const Thread &) = delete;
    //! Deny assignment operator
    void operator = (const Thread &) = delete;
#else
    //! Deny copy construction
    explicit Thread(const Thread &) {}
    //! Deny assignment operator
    void operator = (const Thread &) {}
#endif  /* JIMI_HAS_DELETED_FUNCTIONS */
};

#endif  /* _WIN32 */

}  /* namespace jimi */

#endif  /* _JIMI_THREAD_THREAD_H_ */
