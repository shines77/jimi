//
// IocpServd.cpp : 定义控制台应用程序的入口点。
//

#ifdef _DEBUG
//#include <vld.h>
#endif

#include "IocpServd.h"
#include "SampleThread.h"
#include "IocpServdService.h"

#include "jimi/basic/jimi_def.h"

// Windows的Service支持类
#include "jimi/log/Logger.h"
#include "jimi/util/cmd_line.h"
#include "jimi/thread/Thread.h"
#include "jimi/mt/mutex.h"
#include "jimi/mt/scoped_lock.h"

#include "jimi/thread/Event.h"
#include "jimi/lang/Object.h"
#include "jimi/lang/String.h"

#include <string>
//using namespace std;

#if JIMI_IS_WINDOWS

//#include "jimi/system/WinService.h>

TCHAR g_ServiceName[]        = _T("IocpServd");
TCHAR g_ServiceDisplayName[] = _T("Windows Iocp Server Daemon Service");
TCHAR g_ServiceDescription[] = _T("Windows Iocp Server Daemon");
/*
 * -2 - unknown status
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int g_nServiceStatus = jimi::SVC_STATUS_UNKNOWN;

SERVICE_TABLE_ENTRY g_ServiceTable[] = {
#if defined(IOCPSERVD_USE_NAMESPACE) && (IOCPSERVD_USE_NAMESPACE != 0)
    { _T("IocpServd"), (LPSERVICE_MAIN_FUNCTION)&(IocpServd::IocpServdService::ServiceMain) },
#else
    { _T("IocpServd"), (LPSERVICE_MAIN_FUNCTION)&(IocpServdService::ServiceMain) },
#endif
    { NULL, NULL }
};

#else  /* !JIMI_IS_WINDOWS */

// Linux下的Service支持类
#include "PosixDaemon.h"

#endif  /* JIMI_IS_WINDOWS */

using namespace jimi;
using namespace std;

USING_NS_IOCPSERVD

NS_IOCPSERVD_BEGIN

int IocpServd_main(int argc, char *argv[])
{
    using namespace jimi;

    jmLog.log_begin();

    bool isService = true;
    jimi::CommandLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        std::string strCmdLine = cmdLine.getCmdLine();
        jmLog.info(strCmdLine.c_str());
    }

    IocpServdService *service = new IocpServdService();
    if (service) {
        IocpServdService::SetInstance(service, true);

        service->SetServiceName(g_ServiceName);
        service->SetServiceDisplayName(g_ServiceDisplayName);
        service->SetServiceDescription(g_ServiceDescription);

        jmLog.info("cmdLine.parse(argc, argv): argc_cnt = %d", cnt);

        if (cmdLine.hasArgument("-run") || cmdLine.hasArgument("/run")
            || cmdLine.hasArgument("-r") || cmdLine.hasArgument("/r")) {
            service->RunService();
            //service->RunServiceEx(g_ServiceTable);
        }
        else if (cmdLine.hasArgument("-install") || cmdLine.hasArgument("/install")
            || cmdLine.hasArgument("-i") || cmdLine.hasArgument("/i")) {
            service->InstallService();
        }
        else if (cmdLine.hasArgument("-uninstall") || cmdLine.hasArgument("/uninstall")
            || cmdLine.hasArgument("-u") || cmdLine.hasArgument("/u")
            || cmdLine.hasArgument("-uninst") || cmdLine.hasArgument("/uninst")) {
            service->UninstallService();
        }
        else {
            // is not a windows service
            isService = false;
        }

        if (service) {
            //service->Destroy();
            delete service;
        }
    }

    printf("\n");

#if 1
    SampleThread *thread = new SampleThread();
    thread->Start();
    thread->Join();
    thread->Abort(1000);
    if (thread) {
        delete thread;
    }

    WorkerThread *workerThread = new WorkerThread();
    workerThread->Start();
    workerThread->Join();
    if (workerThread) {
        delete workerThread;
    }

    printf("\n");
#endif

     do {
        jimi::Object *object = new jimi::Object();
        jimi::Object newObject = object->Clone();
        printf("newObject.Equals(object) = %d\n\n", newObject.Equals(object));
        object->Close();
        newObject.Close();
        if (object)
            delete object;
    } while (0);

    printf("\n");

    do {
        ManualResetEvent *event = new ManualResetEvent(false);
        //ManualResetEvent event2(false);
        EventWaitHandle *event3 = new EventWaitHandle();
        EventWaitHandle *event_base = (EventWaitHandle *)event;
        event->Set();
        event->Reset();
        if (event_base) {
            delete event_base;
            event_base = NULL;
        }
        if (event) {
            //delete event;
            event = NULL;
        }
        bool bSignal = event3->WaitOne();
        if (event3)
            delete event3;
    } while (0);

    printf("\n");

    ///*
    //mt::mutex read_mutex;
    //mt::scoped_lock<mt::mutex> lock(read_mutex);
    mt::mutex read_mutex;
    mt::scoped_lock<mt::mutex> lock(read_mutex);
    lock.acquire(read_mutex);
    lock.try_acquire(read_mutex, 4000);
    lock.release();
    //*/

    if (!isService)
        ::system("pause");

    jmLog.log_end();
    return 0;
}

NS_IOCPSERVD_END

class JimiProgram
{
public:
    int Run(int argc, char *argv[])    { return 0; };
    int Run(int argc, wchar_t *argv[]) { return 0; };
};

int main(int argc, char *argv[])
{
    JimiProgram program;
    int nResult = program.Run(argc, argv);

#if defined(IOCPSERVD_USE_NAMESPACE) && (IOCPSERVD_USE_NAMESPACE != 0)
    return IocpServd::IocpServd_main(argc, argv);
#else
    return IocpServd_main(argc, argv);
#endif
}
