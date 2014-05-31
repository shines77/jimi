//
// IocpServd.cpp : 定义控制台应用程序的入口点。
//

#include <jimi/core/jimi_def.h>

// Windows的Service支持类
//#include <jimi/log/log_all.h>
#include <jimi/log/Logger.h>
#include <jimi/util/cmd_line.h>
#include <jimi/system/mutex.h>
#include <jimi/system/scoped_lock.h>

#if JIMI_IS_WINDOWS

#include <jimi/system/WinService.h>
#include "IocpServdService.h"

TCHAR g_ServiceName[]        = _T("IocpServd");
TCHAR g_ServiceDisplayName[] = _T("Iocp Server Daemon Service");
TCHAR g_ServiceDescription[] = _T("Windows IOCP Server Daemon");
/*
 * -2 - unknown status
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int g_nServiceStatus = jimi::system::SVC_STATUS_UNKNOWN;

SERVICE_TABLE_ENTRY g_ServiceTable[] = {
    { _T("IocpServd"), (LPSERVICE_MAIN_FUNCTION)&(IocpServd::IocpServdService::ServiceMain) },
    { NULL, NULL }
};

#else  /* !JIMI_IS_WINDOWS */

// Linux下的Service支持类
#include "PosixDaemon.h"

#endif  /* JIMI_IS_WINDOWS */

#include "IocpServd.h"

USING_NS_JIMI;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

USING_NS_IOCPSERVD;

NS_IOCPSERVD_BEGIN

//IocpServdService iocpServdService;

int IocpServd_main(int argc, char *argv[])
{
    sLog.log_begin();

    bool isService = true;
    util::CCmdLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        string &strCmdLine = cmdLine.getCmdLine();
        sLog.info(strCmdLine.c_str());
    }

    IocpServdService *service = new IocpServdService();
    //IocpServdService *service = &iocpServdService;
    if (service) {
        IocpServdService::SetInstance(service, true);

        service->SetServiceName(g_ServiceName);
        service->SetServiceDisplayName(g_ServiceDisplayName);
        service->SetServiceDescription(g_ServiceDescription);

        sLog.info("cmdLine.parse(argc, argv): argc_cnt = %d", cnt);

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
            //service->Release();
            delete service;
        }
    }

    ///*
    //system::mutex read_mutex;
    //system::scoped_lock<system::mutex> lock(read_mutex);
    system::mutex read_mutex;
    system::scoped_lock<mutex> lock(read_mutex);
    lock.acquire(read_mutex);
    lock.try_acquire(read_mutex, 4000);
    lock.release();
    //*/

    if (!isService)
        ::system("pause");

    sLog.log_end();
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

    return IocpServd_main(argc, argv);
}
