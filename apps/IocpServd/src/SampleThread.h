
#ifndef _IOCPSERVD_SAMPLE_THREAD_H_
#define _IOCPSERVD_SAMPLE_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log.h>
#include <jimi/thread/thread.h>

#include "IocpServd.h"

USING_NS_JIMI;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

NS_IOCPSERVD_BEGIN

class SampleThread : public ThreadBase<SampleThread>
{
public:
    SampleThread(void)  { };
    ~SampleThread(void) { };

    static void JIMI_WINAPI ThreadProc(void *lpParam) {
        sLog.info("invoke SampleThread::ThreadProc() Start.");
        sLog.info("invoke SampleThread::ThreadProc() Thread::Sleep(5000);");
        Thread::Sleep(5000);
        sLog.info("invoke SampleThread::ThreadProc() End.");
    }
};

class Worker
{
public:
    Worker() { };
    ~Worker() { };

    static void DoWork(void *lpParam) {
        while (!_shouldStop) {
            sLog.info("worker thread: working...");
        }
        sLog.info("worker thread: terminating gracefully.");
    }

    void RequestStop() {
        _shouldStop = true;
    }

    volatile static bool _shouldStop;
};

volatile bool Worker::_shouldStop = false;

class WorkerThread : public ThreadBase<WorkerThread>
{
public:
    WorkerThread(void) { };
    WorkerThread(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~WorkerThread(void) { };

    static void JIMI_WINAPI ThreadProc(void *lpParam) {
        Worker *workerObject = new Worker();
        WorkerThread *workerThread = new WorkerThread((thread_proc_t)&Worker::DoWork);

        // Start the worker thread.
        workerThread->Start();
        sLog.info("main thread: Starting worker thread...");

        // Loop until worker thread activates.
        while (!workerThread->IsAlive());

        // Put the main thread to sleep for 1 millisecond to
        // allow the worker thread to do some work:
        Thread::Sleep(10);

        // Request that the worker thread stop itself:
        workerObject->RequestStop();

        // Use the Join method to block the current thread 
        // until the object's thread terminates.
        workerThread->Join();
        sLog.info("main thread: Worker thread has terminated.");

        if (workerThread)
            delete workerThread;
        if (workerObject)
            delete workerObject;
    }
};

NS_IOCPSERVD_END

#endif  /* _IOCPSERVD_SAMPLE_THREAD_H_ */
