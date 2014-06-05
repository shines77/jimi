
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

NS_IOCPSERVD_END

#endif  /* _IOCPSERVD_SAMPLE_THREAD_H_ */
