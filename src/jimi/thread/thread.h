
#ifndef _JIMI_SYSTEM_THREAD_H_
#define _JIMI_SYSTEM_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/thread/ThreadDef.h>
#include <jimi/log/log.h>

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <process.h>

#else

#ifndef CREATE_SUSPENDED
#define CREATE_SUSPENDED        0x00000004
#endif

#endif  /* JIMI_IS_WINDOWS */

#define TERMINATE_WAIT_TIME             5000

#define THREAD_STATUS_MASK              0x00FFFFFFUL

#define THREAD_IS_ALIVE(status)         (((status) & (1 << 30)) != 0)
#define THREAD_IS_RUNNING(status)       (((status) & (1 << 29)) != 0)
#define THREAD_IS_JOIN(status)          (((status) & (1 << 28)) != 0)
#define THREAD_IS_BACKGROUND(status)    (((status) & (1 << 27)) != 0)

#define GET_THREAD_STATUS(status)       ((status) & THREAD_STATUS_MASK)
#define GET_THREAD_STATUS_MASK(status)  ((status) & (~THREAD_STATUS_MASK))

#define THREAD_IS_ALIVE_AND_RUNNING(status) \
    (((status) & ((1 << 30) | (1 << 29))) == ((1 << 30) | (1 << 29)))

#define MAKE_THREAD_STATUS(bAlive, bRunning, nStatus)   \
    (((((bAlive) & 1) << 30) | (((bRunning) & 1) << 29)) + (nStatus))

#define MAKE_THREAD_STATUS_EX(bAlive, bRunning, bJoin, bBackground, nStatus)   \
    (MAKE_THREAD_STATUS(bAlive, bRunning, nStatus) | (((bJoin) & 1) << 28) | (((bBackground) & 1) << 27))

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

enum ThreadDef
{
    kTerminateWaitTime  = 5000
};

enum ThreadStatusDef
{
    kThreadStatusFirst = 0,

    // All Unrunning Status
    kThreadStatus_UnStarted = kThreadStatusFirst,
    kThreadStatus_Stopped,
    kThreadStatus_Aborted,

    // All Alive Status
    kThreadStatus_StartPending,
    kThreadStatus_StopPending,
    kThreadStatus_AbortPending,

    // All Running Status
    kThreadStatus_Running,
    kThreadStatus_Suspended,
    kThreadStatus_SuspendPending,
    kThreadStatus_ResumePending,
    kThreadStatus_ThreadProcOver,

    kThreadStatusLast
};

/*
enum THREAD_STATUS
{
    THREAD_STATUS_FIRST = 0,
    // All Unrunning Status
    THREAD_STATUS_UNSTARTED         = MAKE_THREAD_STATUS(0, 0, kThreadStatus_UnStarted),
    THREAD_STATUS_STOPPED           = MAKE_THREAD_STATUS(0, 0, kThreadStatus_Stopped),
    THREAD_STATUS_ABORTED           = MAKE_THREAD_STATUS(0, 0, kThreadStatus_Aborted),

    // All Alive Status
    THREAD_STATUS_START_PENDING     = MAKE_THREAD_STATUS(1, 0, kThreadStatus_StartPending),
    THREAD_STATUS_STOP_PENDING      = MAKE_THREAD_STATUS(1, 0, kThreadStatus_StopPending),
    THREAD_STATUS_ABORT_PENDING     = MAKE_THREAD_STATUS(1, 0, kThreadStatus_AbortPending),

    // All Running Status
    THREAD_STATUS_RUNNING           = MAKE_THREAD_STATUS(1, 1, kThreadStatus_Running),
    THREAD_STATUS_SUSPENDED         = MAKE_THREAD_STATUS(1, 1, kThreadStatus_Suspended),
    THREAD_STATUS_SUSPEND_PENDING   = MAKE_THREAD_STATUS(1, 1, kThreadStatus_SuspendPending),
    THREAD_STATUS_RESUME_PENDING    = MAKE_THREAD_STATUS(1, 1, kThreadStatus_ResumePending),
    THREAD_STATUS_THREADPROC_OVER   = MAKE_THREAD_STATUS(1, 1, kThreadStatus_ThreadProcOver),

    // wait, for and join
    THREAD_STATUS_WAIT_SLEEP_JOIN   = MAKE_THREAD_STATUS_EX(0, 0, 1, 0, 0),
    // background thread
    THREAD_STATUS_BACKGROUND        = MAKE_THREAD_STATUS_EX(0, 0, 0, 1, 0),

    // Last Status
    THREAD_STATUS_LAST
};
//*/

typedef struct ThreadStatus
{
public:
    // All Unrunning Status
    static const uint32_t kUnStarted        = MAKE_THREAD_STATUS(0, 0, kThreadStatus_UnStarted);
    static const uint32_t kStopped          = MAKE_THREAD_STATUS(0, 0, kThreadStatus_Stopped);
    static const uint32_t kAborted          = MAKE_THREAD_STATUS(0, 0, kThreadStatus_Aborted);

    // All Alive Status
    static const uint32_t kStartPending     = MAKE_THREAD_STATUS(1, 0, kThreadStatus_StartPending);
    static const uint32_t kStopPending      = MAKE_THREAD_STATUS(1, 0, kThreadStatus_StopPending);
    static const uint32_t kAbortPending     = MAKE_THREAD_STATUS(1, 0, kThreadStatus_AbortPending);

    // All Running Status
    static const uint32_t kRunning          = MAKE_THREAD_STATUS(1, 1, kThreadStatus_Running);
    static const uint32_t kSuspended        = MAKE_THREAD_STATUS(1, 1, kThreadStatus_Suspended);
    static const uint32_t kSuspendPending   = MAKE_THREAD_STATUS(1, 1, kThreadStatus_SuspendPending);
    static const uint32_t kResumePending    = MAKE_THREAD_STATUS(1, 1, kThreadStatus_ResumePending);
    static const uint32_t kThreadProcOver   = MAKE_THREAD_STATUS(1, 1, kThreadStatus_ThreadProcOver);

    // wait, for and join
    static const uint32_t kWaitSleepJoin    = MAKE_THREAD_STATUS_EX(0, 0, 1, 0, 0);
    // background thread
    static const uint32_t kBackground       = MAKE_THREAD_STATUS_EX(0, 0, 0, 1, 0);
} ThreadStatus;

typedef struct ThreadInitFlag
{
public:
    static const uint32_t kCreateDefault    = 0;
    static const uint32_t kCreateSuspended  = CREATE_SUSPENDED;
} ThreadInitFlag;

typedef void (*thread_proc_t)(void *lpParam);

#if JIMI_IS_WINDOWS

/**
 * the Thread base class
 */
template <class T>
class ThreadBase
{
public:
    typedef jm_handle_t thread_handle_t;
    typedef jm_handle_t handle_t;
    typedef uint32_t    thread_status_t;
    typedef unsigned    thread_id_t;
    typedef uint32_t    affinity_t;

    // 工作者线程的线程参数
    typedef struct Thread_Params
    {
        ThreadBase *pThread;        // 传入的Thread指针
        void       *pObject;        // 传入的对象指针
        int         nThreadIdx;     // 线程Idx编号, 从0计起
    } THREAD_PARAMS, *PTHREAD_PARAMS;

    ThreadBase(void);
    ThreadBase(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~ThreadBase(void);

private:
    explicit ThreadBase(const ThreadBase &) {}
    void operator = (const ThreadBase &) {}

public:
    void Destroy();
    void OnDestroy();

    thread_proc_t GetThreadProc() { return pThreadProc; }
    bool SetThreadProc(thread_proc_t thread_proc);

    operator thread_handle_t     ()     { return hThread;  }
    thread_handle_t * operator & ()     { return &hThread; }
    thread_handle_t & GetThreadHandle() { return hThread;  }
    thread_id_t GetThreadId()           { return nThreadId; }

    thread_handle_t GetCurrentThread()  { return (thread_handle_t)::GetCurrentThread();  }
    thread_id_t GetCurrentThreadId()    { return ::GetCurrentThreadId(); }

    bool IsValid();
    bool IsAlive();
    bool IsRuning();
    bool IsAliveAndRuning();
    bool IsSuspended();

    bool IsBackground();

    thread_status_t GetThreadState() { return nStatus; }

    void SetThreadState(thread_status_t nNewStatus) {
        nStatus |= GET_THREAD_STATUS(nNewStatus);
    }
    void SetThreadStateFull(thread_status_t nNewStatus) {
        nStatus = nNewStatus;
    }

    void AddThreadState(thread_status_t nAddStatus) {
        nStatus |= GET_THREAD_STATUS_MASK(nAddStatus);
    }
    void RemoveThreadState(thread_status_t nRemoveStatus) {
        nStatus &= GET_THREAD_STATUS_MASK(~nRemoveStatus);
    }

    thread_status_t Start(void *pObject = NULL, unsigned uInitflag = ThreadInitFlag::kCreateDefault);
    thread_status_t Stop();

    static int32_t SpinWait(int32_t iterations);

    thread_status_t Wait(handle_t hObject, uint32_t uWaitTime = Timeout::kInfinite);
    thread_status_t Join(uint32_t uWaitTime = Timeout::kInfinite);

    thread_status_t Suspend();
    thread_status_t Resume();

    thread_status_t Interrupt(uint32_t uWaitTime = ThreadDef::kTerminateWaitTime);

    thread_status_t Abort(uint32_t uWaitTime = ThreadDef::kTerminateWaitTime, uint32_t uExitCode = 0);
    thread_status_t Terminate(uint32_t uWaitTime = ThreadDef::kTerminateWaitTime, uint32_t uExitCode = 0);

    int  GetThreadPriority() { return ::GetThreadPriority(hThread); }
    bool SetThreadPriority(int nNewPriority) {
        return (bool)::SetThreadPriority(hThread, nNewPriority);
    }

    affinity_t GetProcessorAffinity();
    affinity_t GetThreadAffinity();

    affinity_t SetProcessorAffinity();
    affinity_t SetThreadAffinity();

    static void Sleep(uint32_t uMilliSecs);

    void JIMI_WINAPI ThreadProc(void *lpParam);

    static unsigned JIMI_WINAPI ThreadProcBase(void *lpParam);

private:
    thread_handle_t hThread;
    thread_status_t nStatus;
    thread_id_t     nThreadId;
protected:
    thread_proc_t   pThreadProc;
};

template <class T>
ThreadBase<T>::ThreadBase(void)
: hThread(NULL)
, nStatus(ThreadStatus::kUnStarted)
, nThreadId(0)
, pThreadProc(NULL)
{
}

template <class T>
ThreadBase<T>::~ThreadBase(void)
{
    this->Destroy();
}

template <class T>
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
        nStatus = ThreadStatus::kUnStarted;
    }
}

template <class T>
void ThreadBase<T>::OnDestroy()
{
    jmLog.info("Thread<T>::OnDestroy(), error = %d.", ::GetLastError());
}

template <class T>
bool ThreadBase<T>::IsValid()
{
    return HANDLE_IS_VALID(hThread);
}

template <class T>
bool ThreadBase<T>::IsAlive()
{
    return IsValid() && THREAD_IS_ALIVE(nStatus);
}

template <class T>
bool ThreadBase<T>::IsRuning()
{
    return IsValid() && THREAD_IS_RUNNING(nStatus);
}

template <class T>
bool ThreadBase<T>::IsAliveAndRuning()
{
    return IsValid() && THREAD_IS_ALIVE_AND_RUNNING(nStatus);
}

template <class T>
bool ThreadBase<T>::IsSuspended()
{
    return IsAliveAndRuning(nStatus)
        && (nStatus == ThreadStatus::kSuspended || nStatus == ThreadStatus::kSuspendPending);
}

template <class T>
bool ThreadBase<T>::IsBackground()
{
    return THREAD_IS_BACKGROUND(nStatus);
}

template <class T>
bool ThreadBase<T>::SetThreadProc(thread_proc_t thread_proc)
{
    if ((!IsAlive()) && (!IsRuning())) {
        pThreadProc = thread_proc;
        return true;
    }
    return false;
}

template <class T>
void JIMI_WINAPI ThreadBase<T>::ThreadProc(void *lpParam)
{
    // Do nothing !!
    jmLog.info("Thread<T>::ThreadProc().");
}

/* static */
template <class T>
unsigned JIMI_WINAPI ThreadBase<T>::ThreadProcBase(void *lpParam)
{
    jmLog.info("Thread<T>::ThreadProcBase() Enter.");
    jmLog.info("Thread<T>::ThreadProcBase(), lpParam = 0x%08X.", lpParam);

    THREAD_PARAMS *pParam = (THREAD_PARAMS *)lpParam;
    ThreadBase<T> *pThread = NULL;
    void *pObject= NULL;
    if (pParam) {
        pThread = pParam->pThread;
        pObject = pParam->pObject;

        if (pThread && pThread->IsValid())
            pThread->SetThreadState(ThreadStatus::kRunning);

        T *pThis = static_cast<T *>(pThread);
        thread_proc_t pThreadProc = pThis->GetThreadProc();
        if (pThreadProc != NULL)
            pThreadProc(lpParam);
        else
            pThis->ThreadProc(lpParam);

        if (pParam)
            delete pParam;

        //ThreadBase<T>::Sleep(3000);
    }
    jmLog.info("Thread<T>::ThreadProcBase() Over.");

    if (pThread && pThread->IsValid()) {
        //pThread->SetThreadState(ThreadStatus::kThreadProcOver);
        pThread->SetThreadState(ThreadStatus::kStopped);
    }
    return 0;
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Start(void *pObject /* = NULL */,
                                                             unsigned uInitflag /* = ThreadInitFlag::kCreateDefault */)
{
    jmLog.info("Thread<T>::Start() Enter.");

    if (!IsValid()) {
        if (!THREAD_IS_ALIVE_AND_RUNNING(nStatus)) {
            THREAD_PARAMS *pParams = new THREAD_PARAMS;
            pParams->pThread = (ThreadBase<T> *)this;
            pParams->pObject = pObject;
            pParams->nThreadIdx = 0;
            hThread = (thread_handle_t)::_beginthreadex(NULL, 0, &ThreadBase<T>::ThreadProcBase,
                (void *)pParams, uInitflag, (unsigned *)&nThreadId);
            if (HANDLE_IS_VALID(hThread)) {
                if (uInitflag == ThreadInitFlag::kCreateSuspended)
                    nStatus = ThreadStatus::kSuspended;
                else
                    nStatus = ThreadStatus::kStartPending;
            }
        }
        else {
            jmLog.info("Thread<T>::Start() Thread is running, nThread = 0x%08X.", hThread);
        }
    }
    else {
        jmLog.info("Thread<T>::Start() Thread already exists, nThread = 0x%08X.", hThread);
    }

    jmLog.info("Thread<T>::Start() Over.");
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Stop()
{
    bool bThreadStartted = false;
    jmLog.info("Thread<T>::Stop() Enter.");
    if (IsValid()) {
        if (THREAD_IS_ALIVE(nStatus) && THREAD_IS_RUNNING(nStatus)) {
            bThreadStartted = true;
            this->Terminate(1000, -1);
            jmLog.info("Thread<T>::Stop() successed.");
        }
    }
    if (!bThreadStartted)
        jmLog.info("Thread<T>::Stop() Thread is not startted, nThread = 0x%08X.", hThread);
    jmLog.info("Thread<T>::Stop() Over.");
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Suspend()
{
    if (!IsSuspended()) {
        thread_status_t oldStatus = this->GetThreadState();
        this->SetThreadState(THREAD_STATUS_SUSPEND_PENDING);
        if (::SuspendThread(hThread) != (DWORD)(-1))
            this->SetThreadState(THREAD_STATUS_SUSPENDED);
        else
            this->SetThreadState(oldStatus);
    }
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Resume()
{
    if (IsSuspended()) {
        thread_status_t oldStatus = this->GetThreadState();
        this->SetThreadState(THREAD_STATUS_RESUME_PENDING);
        if (::ResumeThread (hThread) != (DWORD)(-1))
            this->SetThreadState(THREAD_STATUS_RUNNING);
        else
            this->SetThreadState(oldStatus);
    }
    return this->GetThreadState();
}


template <class T>
int32_t ThreadBase<T>::SpinWait(int32_t iterations)
{
    volatile int32_t sum = 0;
    int32_t i;
    for (i = 0; i < iterations; ++i)
        sum += i;

    return sum;
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Wait(handle_t hObject,
                                                            uint32_t uWaitTime /* = Timeout::kInfinite */)
{
    jmLog.info("Thread<T>::Wait() Enter.");
    static int counter = 0;
    if (HANDLE_IS_VALID(hObject)) {
        if (::WaitForSingleObject(hObject, 10) != WAIT_OBJECT_0)
            this->AddThreadState(THREAD_STATUS_WAIT_SLEEP_JOIN);
        while (::WaitForSingleObject(hObject, uWaitTime) != WAIT_OBJECT_0) {
            ThreadBase<T>::Sleep(1);
            counter++;
            if (counter > 5)
                break;
        }
        this->RemoveThreadState(THREAD_STATUS_WAIT_SLEEP_JOIN);
        if (::WaitForSingleObject(hObject, 10) == WAIT_OBJECT_0) {
            // wait for success
        }
        else {
            // wait for failed
        }
    }

    jmLog.info("Thread<T>::Wait() Over.");
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Join(uint32_t uWaitTime /* = Timeout::kInfinite */)
{
    jmLog.info("Thread<T>::Join() Enter.");
    static int counter = 0;
    if (IsValid()) {
        if (::WaitForSingleObject(hThread, 10) != WAIT_OBJECT_0)
            this->AddThreadState(ThreadStatus::kWaitSleepJoin);
        while (::WaitForSingleObject(hThread, uWaitTime) != WAIT_OBJECT_0) {
            ThreadBase<T>::Sleep(1);
            counter++;
            if (counter > 5)
                break;
        }
        this->RemoveThreadState(ThreadStatus::kWaitSleepJoin);
        if (IsValid()) {
            if (::WaitForSingleObject(hThread, 10) == WAIT_OBJECT_0) {
                this->SetThreadState(ThreadStatus::kStopPending);
                if (IsValid()) {
                    if (::CloseHandle(hThread))
                        this->SetThreadState(ThreadStatus::kStopped);
                }
                hThread = NULL;
                nThreadId = 0;
                nStatus = ThreadStatus::kUnStarted;
            }
        }
    }

    jmLog.info("Thread<T>::Join() Over.");
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Interrupt(uint32_t uWaitTime /* = ThreadDef::kTerminateWaitTime */)
{
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Abort(uint32_t uWaitTime /* = Timeout::kInfinite */,
                                                             uint32_t uExitCode /* = 0 */)
{
    jmLog.info("Thread<T>::Abort() Enter.");
    if (IsValid()) {
        if (IsAlive()) {
            this->SetThreadState(ThreadStatus::kAbortPending);
            if (::WaitForSingleObject(hThread, uWaitTime) != WAIT_OBJECT_0) {
                if (::TerminateThread(hThread, uExitCode)) {
                    this->SetThreadState(ThreadStatus::kAborted);
                }
            }
        }
        this->SetThreadState(ThreadStatus::kStopPending);
        if (IsValid()) {
            if (::CloseHandle(hThread))
                this->SetThreadState(ThreadStatus::kStopped);
        }
        hThread = NULL;
        nThreadId = 0;
        nStatus = ThreadStatus::kUnStarted;
    }
    jmLog.info("Thread<T>::Abort() Over.");
    return this->GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Terminate(uint32_t uWaitTime /* = Timeout::kInfinite */,
                                                                 uint32_t uExitCode /* = 0 */)
{
    return this->Abort(uWaitTime, uExitCode);
}

/* static */
template <class T>
void ThreadBase<T>::Sleep(uint32_t uMilliSecs)
{
    ::Sleep(uMilliSecs);
}

/**
 * the Thread class
 */
class Thread : public ThreadBase<Thread>
{
public:
    Thread(void) : ThreadBase<Thread>() { };
    Thread(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~Thread(void) { };

private:
    explicit Thread(const Thread &) { }
    void operator = (const Thread &) { }
};

#else

/**
 * the Thread base class
 */
template <class T>
class ThreadBase {
public:
    ThreadBase(void);
    ThreadBase(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~ThreadBase(void);

private:
    explicit ThreadBase(const ThreadBase &) { }
    void operator = (const ThreadBase &) { }

protected:
    thread_proc_t   pThreadProc;
};

template <class T>
ThreadBase<T>::ThreadBase(void)
: pThreadProc(NULL)
{
}

template <class T>
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
    Thread(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~Thread(void) { };

private:
    explicit Thread(const Thread &) { }
    void operator = (const Thread &) { }
};

#endif  /* JIMI_IS_WINDOWS */

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* _JIMI_SYSTEM_THREAD_H_ */
