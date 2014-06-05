
#ifndef _JIMI_SYSTEM_THREAD_H_
#define _JIMI_SYSTEM_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <process.h>

#define TERMINATE_WAIT_TIME     5000

#define HANDLE_IS_VALID(X)      ((X) != NULL && (X) != INVALID_HANDLE_VALUE)

#define THREAD_STATUS_MASK              0x00FFFFFFUL

#define THREAD_IS_ALIVE(status)         (((status) & (1 << 30)) != 0)
#define THREAD_IS_RUNNING(status)       (((status) & (1 << 29)) != 0)
#define THREAD_IS_JOIN(status)          (((status) & (1 << 28)) != 0)
#define THREAD_IS_BACKGROUND(status)    (((status) & (1 << 27)) != 0)

#define GET_THREAD_STATUS(status)       ((status) & THREAD_STATUS_MASK)
#define GET_THREAD_STATUS_MASK(status)  ((status) & (~THREAD_STATUS_MASK))

#define THREAD_IS_ALIVE_AND_RUNNING(status)   (((status) & (3 << 29)) == (3 << 29))

#define MAKE_THREAD_STATUS(bAlive, bRunning, nStatus)   \
    (((((bAlive) & 1) << 30) | (((bRunning) & 1) << 29)) + (nStatus))

#define MAKE_THREAD_STATUS_EX(bAlive, bRunning, bJoin, bBackground, nStatus)   \
    (MAKE_THREAD_STATUS(bAlive, bRunning, nStatus) | (((bJoin) & 1) << 28) | (((bBackground) & 1) << 27))

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

typedef enum THREAD_STATUS
{
    THREAD_STATUS_FIRST = 0,
    // All Unrunning Status
    THREAD_STATUS_UNSTARTED         = MAKE_THREAD_STATUS(0, 0, 0),
    THREAD_STATUS_STOPPED           = MAKE_THREAD_STATUS(0, 0, 1),
    THREAD_STATUS_ABORTED           = MAKE_THREAD_STATUS(0, 0, 2),

    // All Alive Status
    THREAD_STATUS_STOP_PENDING      = MAKE_THREAD_STATUS(1, 0, 3),
    THREAD_STATUS_ABORT_PENDING     = MAKE_THREAD_STATUS(1, 0, 4),
    THREAD_STATUS_START_PENDING     = MAKE_THREAD_STATUS(1, 0, 5),

    // All Running Status
    THREAD_STATUS_RUNNING           = MAKE_THREAD_STATUS(1, 1, 6),
    THREAD_STATUS_SUSPENDED         = MAKE_THREAD_STATUS(1, 1, 7),
    THREAD_STATUS_SUSPEND_PENDING   = MAKE_THREAD_STATUS(1, 1, 8),
    THREAD_STATUS_RESUME_PENDING    = MAKE_THREAD_STATUS(1, 1, 9),
    THREAD_STATUS_THREADPROC_OVER   = MAKE_THREAD_STATUS(1, 1, 10),

    // wait, for and join
    THREAD_STATUS_WAIT_SLEEP_JOIN   = MAKE_THREAD_STATUS_EX(0, 0, 1, 0, 0),
    // background thread
    THREAD_STATUS_BACKGROUND        = MAKE_THREAD_STATUS_EX(0, 0, 0, 1, 0),

    // Last Status
    THREAD_STATUS_LAST
} THREAD_STATUS;

typedef enum THREAD_INITFLAG
{
    THREAD_INITFLAG_DEFAULT = 0,
    THREAD_INITFLAG_SUSPENDED = CREATE_SUSPENDED,
} THREAD_INITFLAG;

typedef enum THREAD_WAIT_FOR_OBJECT
{
    WAIT_FOR_IGNORE = IGNORE,
    WAIT_FOR_INFINITE = INFINITE,
} THREAD_WAIT_FOR_OBJECT;

typedef HANDLE jm_handle;

class Timeout
{
public:
    static const uint32_t kInfinite = INFINITE;
    static const uint32_t kIgnore   = IGNORE;
};

class ThreadInitFlag
{
public:
    static const uint32_t kCreateDefault    = 0;
    static const uint32_t kCreateSuspended  = CREATE_SUSPENDED;
};

class ErrorCode
{
public:
    static const uint32_t kErrorSuccess = ERROR_SUCCESS;
    static const uint32_t kNoError      = NO_ERROR;
};

//template <class T>
//typedef void (__thiscall <T>::*thread_proc_t)(void);

typedef void (*thread_proc_t)(void *lpParam);

template <class T>
class ThreadBase
{
public:
    typedef jm_handle thread_handle_t;
    typedef jm_handle handle_t;
    typedef uint32_t  thread_status_t;
    typedef unsigned  thread_id_t;
    typedef uint32_t  affinity_t;

    //typedef void (__thiscall Ty::*thread_proc_t)(void);

    // 工作者线程的线程参数
    typedef struct Thread_Params
    {
        ThreadBase *pThread;        // 传入的Thread指针
        void       *pObject;        // 传入的对象指针
        int         nThreadIdx;     // 线程Idx编号, 从0计起
    } THREAD_PARAMS, *PTHREAD_PARAMS;

    ThreadBase(void);
    ThreadBase(thread_proc_t thread_proc, int i) : ThreadBase() { pThreadProc = thread_proc; }
    ~ThreadBase(void);

private:
    explicit ThreadBase(const ThreadBase &) { }
    void operator = (const ThreadBase &) { }

public:
    void Destroy();
    void OnDestroy();

    thread_proc_t GetThreadProc() { return pThreadProc; }

    operator thread_handle_t     ()     { return hThread;  }
    thread_handle_t * operator & ()     { return &hThread; }
    thread_handle_t & GetThreadHandle() { return hThread;  }
    thread_id_t GetThreadId()           { return nThreadId; }

    thread_handle_t GetCurrentThread()  { return (thread_handle_t)::GetCurrentThread();  }
    thread_id_t GetCurrentThreadId()    { return ::GetCurrentThreadId(); }

    bool IsValid();
    bool IsAlive();
    bool IsRuning();
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

    thread_status_t Start(unsigned uInitflag = ThreadInitFlag::kCreateDefault, void *pObject = NULL);
    thread_status_t Stop();

    static int32_t SpinWait(int32_t iterations);

    thread_status_t Wait(handle_t hObject, uint32_t uWaitTime = Timeout::kInfinite);
    thread_status_t Join(uint32_t uWaitTime = Timeout::kInfinite);

    thread_status_t Suspend();
    thread_status_t Resume();

    thread_status_t Interrupt(uint32_t uWaitTime = TERMINATE_WAIT_TIME);

    thread_status_t Abort(uint32_t uWaitTime = TERMINATE_WAIT_TIME, uint32_t uExitCode = 0);
    thread_status_t Terminate(uint32_t uWaitTime = TERMINATE_WAIT_TIME, uint32_t uExitCode = 0);

    int  GetThreadPriority() { return ::GetThreadPriority(hThread); }
    bool SetThreadPriority(int nNewPriority) { return (bool)::SetThreadPriority(hThread, nNewPriority); }

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
, nStatus(THREAD_STATUS_UNSTARTED)
, nThreadId(0)
, pThreadProc(NULL)
{
}

template <class T>
ThreadBase<T>::~ThreadBase(void)
{
    Destroy();
}

template <class T>
void ThreadBase<T>::Destroy()
{
    T *pT = static_cast<T *>(this);
    pT->OnDestroy();

    if (IsValid()) {
        if (IsAlive())
            Terminate();
        if (IsValid())
            ::CloseHandle(hThread);
        hThread = NULL;
        nThreadId = 0;
        nStatus = THREAD_STATUS_UNSTARTED;
    }
}

template <class T>
void ThreadBase<T>::OnDestroy()
{
    sLog.info("Thread<T>::OnDestroy(), error = %d.", ::GetLastError());
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
bool ThreadBase<T>::IsSuspended()
{
    return IsValid() && THREAD_IS_ALIVE_AND_RUNNING(nStatus)
        && (nStatus == THREAD_STATUS_SUSPENDED || nStatus == THREAD_STATUS_SUSPEND_PENDING);
}

template <class T>
bool ThreadBase<T>::IsBackground()
{
    return THREAD_IS_BACKGROUND(nStatus);
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Start(unsigned uInitflag /* = ThreadInitFlag::kCreateDefault */,
                                                             void *pObject /* = NULL */)
{
    sLog.info("Thread<T>::Start() Enter.");

    if (!IsValid()) {
        if (!THREAD_IS_ALIVE_AND_RUNNING(nStatus)) {
            THREAD_PARAMS *pParams = new THREAD_PARAMS;
            pParams->pThread = (ThreadBase<T> *)this;
            pParams->pObject = pObject;
            pParams->nThreadIdx = 0;
            hThread = (thread_handle_t)::_beginthreadex(NULL, 0, &ThreadBase<T>::ThreadProcBase,
                (void *)pParams, uInitflag, (unsigned *)&nThreadId);
            if (HANDLE_IS_VALID(hThread)) {
                if (uInitflag == THREAD_INITFLAG_SUSPENDED)
                    nStatus = THREAD_STATUS_SUSPENDED;
                else
                    nStatus = THREAD_STATUS_START_PENDING;
            }
        }
        else {
            sLog.info("Thread<T>::Start() Thread is running, nThread = 0x%08X.", hThread);
        }
    }
    else {
        sLog.info("Thread<T>::Start() Thread already exists, nThread = 0x%08X.", hThread);
    }

    sLog.info("Thread<T>::Start() Over.");
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Stop()
{
    bool bThreadStartted = false;
    sLog.info("Thread<T>::Stop() Enter.");
    if (IsValid()) {
        if (THREAD_IS_ALIVE(nStatus) && THREAD_IS_RUNNING(nStatus)) {
            bThreadStartted = true;
            Terminate(1000, -1);
            sLog.info("Thread<T>::Stop() successed.");
        }
    }
    if (!bThreadStartted)
        sLog.info("Thread<T>::Stop() Thread is not startted, nThread = 0x%08X.", hThread);
    sLog.info("Thread<T>::Stop() Over.");
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Suspend()
{
    if (!IsSuspended()) {
        thread_status_t oldStatus = GetThreadState();
        SetThreadState(THREAD_STATUS_SUSPEND_PENDING);
        if (::SuspendThread(hThread) != (DWORD)(-1))
            SetThreadState(THREAD_STATUS_SUSPENDED);
        else
            SetThreadState(oldStatus);
    }
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Resume()
{
    if (IsSuspended()) {
        thread_status_t oldStatus = GetThreadState();
        SetThreadState(THREAD_STATUS_RESUME_PENDING);
        if (::ResumeThread (hThread) != (DWORD)(-1))
            SetThreadState(THREAD_STATUS_RUNNING);
        else
            SetThreadState(oldStatus);
    }
    return GetThreadState();
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
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Wait(handle_t hObject, uint32_t uWaitTime /* = WAIT_FOR_INFINITE */)
{
    sLog.info("Thread<T>::Wait() Enter.");
    static int counter = 0;
    if (HANDLE_IS_VALID(hObject)) {
        if (::WaitForSingleObject(hObject, 10) != WAIT_OBJECT_0)
            AddThreadState(THREAD_STATUS_WAIT_SLEEP_JOIN);
        while (::WaitForSingleObject(hObject, uWaitTime) != WAIT_OBJECT_0) {
            ThreadBase<T>::Sleep(1);
            counter++;
            if (counter > 5)
                break;
        }
        RemoveThreadState(THREAD_STATUS_WAIT_SLEEP_JOIN);
        if (::WaitForSingleObject(hObject, 10) == WAIT_OBJECT_0) {
            // wait for success
        }
        else {
            // wait for failed
        }
    }

    sLog.info("Thread<T>::Wait() Over.");
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Join(uint32_t uWaitTime /* = WAIT_FOR_INFINITE */)
{
    sLog.info("Thread<T>::Join() Enter.");
    static int counter = 0;
    if (IsValid()) {
        if (::WaitForSingleObject(hThread, 10) != WAIT_OBJECT_0)
            AddThreadState(THREAD_STATUS_WAIT_SLEEP_JOIN);
        while (::WaitForSingleObject(hThread, uWaitTime) != WAIT_OBJECT_0) {
            ThreadBase<T>::Sleep(1);
            counter++;
            if (counter > 5)
                break;
        }
        RemoveThreadState(THREAD_STATUS_WAIT_SLEEP_JOIN);
        if (IsValid()) {
            if (::WaitForSingleObject(hThread, 10) == WAIT_OBJECT_0) {
                SetThreadState(THREAD_STATUS_STOP_PENDING);
                if (IsValid()) {
                    if (::CloseHandle(hThread))
                        SetThreadState(THREAD_STATUS_STOPPED);
                }
                hThread = NULL;
                nThreadId = 0;
                nStatus = THREAD_STATUS_UNSTARTED;
            }
        }
    }

    sLog.info("Thread<T>::Join() Over.");
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Interrupt(uint32_t uWaitTime /* = TERMINATE_WAIT_TIME */)
{
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Abort(uint32_t uWaitTime /* = Timeout.kInfinite */,
                                                             uint32_t uExitCode /* = 0 */)
{
    sLog.info("Thread<T>::Abort() Enter.");
    if (IsValid()) {
        if (IsAlive()) {
            SetThreadState(THREAD_STATUS_ABORT_PENDING);
            if (::WaitForSingleObject(hThread, uWaitTime) != WAIT_OBJECT_0) {
                if (::TerminateThread(hThread, uExitCode)) {
                    SetThreadState(THREAD_STATUS_ABORTED);
                }
            }
        }
        SetThreadState(THREAD_STATUS_STOP_PENDING);
        if (IsValid()) {
            if (::CloseHandle(hThread))
                SetThreadState(THREAD_STATUS_STOPPED);
        }
        hThread = NULL;
        nThreadId = 0;
        nStatus = THREAD_STATUS_UNSTARTED;
    }
    sLog.info("Thread<T>::Abort() Over.");
    return GetThreadState();
}

template <class T>
typename ThreadBase<T>::thread_status_t ThreadBase<T>::Terminate(uint32_t uWaitTime /* = Timeout.kInfinite */,
                                                                 uint32_t uExitCode /* = 0 */)
{
    return Abort(uWaitTime, uExitCode);
}

template <class T>
void JIMI_WINAPI ThreadBase<T>::ThreadProc(void *lpParam)
{
    // Do nothing !!
    sLog.info("Thread<T>::ThreadProc().");
}

template <class T>
unsigned JIMI_WINAPI ThreadBase<T>::ThreadProcBase(void *lpParam)
{
    sLog.info("Thread<T>::ThreadProcBase() Enter.");
    sLog.info("Thread<T>::ThreadProcBase(), lpParam = 0x%08X.", lpParam);

    THREAD_PARAMS *pParam = (THREAD_PARAMS *)lpParam;
    ThreadBase<T> *pThread = NULL;
    void *pObject= NULL;
    if (pParam) {
        pThread = pParam->pThread;
        pObject = pParam->pObject;

        if (pThread && pThread->IsValid())
            pThread->SetThreadState(THREAD_STATUS_RUNNING);

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
    sLog.info("Thread<T>::ThreadProcBase() Over.");

    if (pThread && pThread->IsValid()) {
        //pThread->SetThreadState(THREAD_STATUS_THREADPROC_OVER);
        pThread->SetThreadState(THREAD_STATUS_STOPPED);
    }
    return 0;
}

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
    Thread(thread_proc_t thread_proc) : ThreadBase<Thread>() { pThreadProc = thread_proc; }
    ~Thread(void) { };

private:
    explicit Thread(const Thread &) { }
    void operator = (const Thread &) { }
};

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* _JIMI_SYSTEM_THREAD_H_ */
