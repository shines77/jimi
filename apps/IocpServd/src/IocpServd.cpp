//
// IocpServd.cpp : 定义控制台应用程序的入口点。
//

#include <jimi/core/jimi_def.h>

#if JIMI_IS_WINDOWS

// Windows的Service支持类
#include <jimi/system/WinService.h>
//#include <jimi/log/log_all.h>
#include <jimi/log/Logger.h>
#include <jimi/util/cmd_line.h>

#include <jimi/system/mutex.h>
#include <jimi/system/scoped_lock.h>

USING_NS_JIMI;
//USING_NS_JIMI_IOCP;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

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

#else  /* !JIMI_IS_WINDOWS */

// Linux下的Service支持类
#include "PosixDaemon.h"

#endif  /* JIMI_IS_WINDOWS */

system::CWinService iocpServd_Service;

int main(int argc, char *argv[])
{
    sLog.log_begin();

    bool isService = true;
    util::CCmdLine cmdLine;
    int cnt;
    if ((cnt = cmdLine.parse(argc, argv)) >= 0) {
        string &strCmdLine = cmdLine.getCmdLine();
        sLog.info(strCmdLine.c_str());
    }

    //system::CWinService *service = new CWinService();
    system::CWinService *service = &iocpServd_Service;
    if (service) {
        system::CWinService::SetInstance(service);
    }

    sLog.info("cmdLine.parse(argc, argv): argc_cnt = %d", cnt);

    if (cmdLine.hasArgument("-run") || cmdLine.hasArgument("/run")
        || cmdLine.hasArgument("-r") || cmdLine.hasArgument("/r")) {
        service->RunService(argc, argv);
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
        service->OnStopService();
        //delete service;
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
