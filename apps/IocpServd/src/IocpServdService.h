
#ifndef _IOCPSERVDSERVICE_H_
#define _IOCPSERVDSERVICE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <jimi/core/jimi_def.h>
#include <jimi/log/log_all.h>
#include <jimi/system/WinService.h>

#include "IocpServd.h"

USING_NS_JIMI;
USING_NS_JIMI_LOG;
USING_NS_JIMI_SYSTEM;

NS_IOCPSERVD_BEGIN

class IocpServdService : public system::WinServiceBase<IocpServdService>
{
public:
    IocpServdService(void);
    ~IocpServdService(void);

    /**
     * ע��: �м�!! ���лص��������뷵��true, ������Ĵ�������������Ĵ���(�쳣), 
     *       ����Է���false, ����᳢�Դ���÷������¼�, �����жϴ����˳�, ֹͣ���е�.
     **/
    bool OnInitService() {
        sLog.info("invoke IocpServdService::OnPauseService()");
        return true;
    }

    bool OnStartService(int argc, TCHAR *argv[]) {
        sLog.info("invoke IocpServdService::OnStartService()");
        return true;
    }

    bool OnStopService() {
        sLog.info("invoke IocpServdService::OnStopService()");
        return true;
    }

    bool OnPauseService() {
        sLog.info("invoke IocpServdService::OnPauseService()");
        return true;
    }

    bool OnResumeService() {
        sLog.info("invoke IocpServdService::OnResumeService()");
        return true;
    }

    bool OnShutdownService() {
        sLog.info("invoke IocpServdService::OnShutdownService()");
        return true;
    }

    bool OnCustomCommand(DWORD dwControlCode) {
        sLog.info("invoke IocpServdService::OnCustomCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }

    bool OnUnknownCommand(DWORD dwControlCode) {
        sLog.info("invoke IocpServdService::OnUnknownCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }

    bool ServiceWorkerMethod(void *pvData) {
        static int s_nOnServiceLoopCnt = 0;
        if (s_nOnServiceLoopCnt < 10) {
            sLog.info("invoke IocpServdService::ServiceWorkerMethod(), cnt = %d", s_nOnServiceLoopCnt);
            s_nOnServiceLoopCnt++;
        }
        return true;
    }
};

NS_IOCPSERVD_END

#endif  /* _IOCPSERVDSERVICE_H_ */
