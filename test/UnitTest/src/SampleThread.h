
#ifndef _UNITEST_SAMPLE_THREAD_H_
#define _UNITEST_SAMPLE_THREAD_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "UnitTest.h"

#include "jimi/core/jimi_def.h"
#include "jimi/log/log.h"
#include "jimi/thread/Thread.h"

NS_UNITEST_BEGIN

class SampleThread : public jimi::ThreadBase<SampleThread>
{
public:
    SampleThread(void)  { };
    ~SampleThread(void) { };

    static void JIMI_WINAPI ThreadProc(void *lpParam) {
        jmLog.info("invoke SampleThread::ThreadProc() Start.");
        jmLog.info("invoke SampleThread::ThreadProc() Thread::Sleep(5000);");
        //using namespace jimi;
        jimi::Thread::Sleep(5000);
        jmLog.info("invoke SampleThread::ThreadProc() End.");
    }
};

class Worker
{
public:
    Worker() { };
    ~Worker() { };

    static void DoWork(void *lpParam) {
        while (!_shouldStop) {
            jmLog.info("worker thread: working...");
        }
        jmLog.info("worker thread: terminating gracefully.");
    }

    void RequestStop() {
        _shouldStop = true;
    }

    volatile static bool _shouldStop;
};

volatile bool Worker::_shouldStop = false;

class WorkerThread : public jimi::ThreadBase<WorkerThread>
{
public:
    //typedef jimi::ThreadBase<WorkerThread>::thread_proc_t thread_proc_t;

public:
    WorkerThread(void) { };
    WorkerThread(thread_proc_t thread_proc) { pThreadProc = thread_proc; }
    ~WorkerThread(void) { };

    static void JIMI_WINAPI ThreadProc(void *lpParam) {
        Worker *workerObject = new Worker();
        WorkerThread *workerThread = new WorkerThread((thread_proc_t)&Worker::DoWork);

        // Start the worker thread.
        workerThread->Start();
        jmLog.info("main thread: Starting worker thread...");

        // Loop until worker thread activates.
        while (!workerThread->IsAlive());

        // Put the main thread to sleep for 1 millisecond to
        // allow the worker thread to do some work:
        //using namespace jimi;
        jimi::Thread::Sleep(10);

        // Request that the worker thread stop itself:
        workerObject->RequestStop();

        // Use the Join method to block the current thread 
        // until the object's thread terminates.
        workerThread->Join();
        jmLog.info("main thread: Worker thread has terminated.");

        if (workerThread)
            delete workerThread;
        if (workerObject)
            delete workerObject;
    }
};

NS_UNITEST_END

#endif  /* _UNITEST_SAMPLE_THREAD_H_ */
