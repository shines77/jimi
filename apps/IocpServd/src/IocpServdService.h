
#ifndef _IOCPSERVDSERVICE_H_
#define _IOCPSERVDSERVICE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
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

#if 1
    void InitComponent() {
        sLog.info("IocpServdService::InitComponent().");
    }
#endif

#if 0
    /**
     * 注意: 切记!! 所有回调函数必须返回true, 如果返回false, 则表示不调用WinService的默认行为,
     *       此时, 你必须正确的处理该回调应该完成的事情, 否则程序可能不能正常工作.
     **/
    virtual bool OnInitService() {
        sLog.info("invoke IocpServdService::OnInitService()");
        return true;
    }

    virtual bool OnStartService(int argc, TCHAR *argv[]) {
        sLog.info("invoke IocpServdService::OnStartService()");
        return true;
    }

    virtual bool OnStopService() {
        sLog.info("invoke IocpServdService::OnStopService()");
        return true;
    }

    virtual bool OnPauseService() {
        sLog.info("invoke IocpServdService::OnPauseService()");
        return true;
    }

    virtual bool OnContinueService() {
        sLog.info("invoke IocpServdService::OnContinueService()");
        return true;
    }

    virtual bool OnServiceInterrogate() {
        sLog.info("invoke IocpServdService::OnServiceInterrogate()");
        return true;
    }

    virtual bool OnShutdownService() {
        sLog.info("invoke IocpServdService::OnShutdownService()");
        return true;
    }

    virtual bool OnServiceParamChange() {
        sLog.info("invoke IocpServdService::OnServiceParamChange()");
        return true;
    }

    virtual bool OnServiceDeviceEvent() {
        sLog.info("invoke IocpServdService::OnServiceDeviceEvent()");
        return true;
    }

    virtual bool OnServiceSessionChange() {
        sLog.info("invoke IocpServdService::OnServiceSessionChange()");
        return true;
    }

    virtual bool OnServicePowerEvent() {
        sLog.info("invoke IocpServdService::OnServicePowerEvent()");
        return true;
    }

    virtual bool OnServicePreShutdown() {
        sLog.info("invoke IocpServdService::OnServicePreShutdown()");
        return true;
    }

    virtual bool OnCustomCommand(DWORD dwControlCode) {
        sLog.info("invoke IocpServdService::OnCustomCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }

    virtual bool OnUnknownCommand(DWORD dwControlCode) {
        sLog.info("invoke IocpServdService::OnUnknownCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }
#endif

#if 1
    bool ServiceWorkerMethod(void *pvData) {
        static int s_nOnServiceLoopCnt = 0;
        static int s_nServiceLoopPauseCnt = 0;
        unsigned int nSleepTime = GetSleepTime();
        unsigned int nPauseSleepTime = GetPauseSleepTime();
        while (IsRunning()) {
            if (!IsPausing()) {
                if (s_nOnServiceLoopCnt < 10) {
                    sLog.info("invoke IocpServdService::ServiceWorkerMethod(), cnt = %d", s_nOnServiceLoopCnt);
                    s_nOnServiceLoopCnt++;
                }
                s_nServiceLoopPauseCnt = 0;
                ::Sleep(nSleepTime);
            }
            else {
                if (s_nServiceLoopPauseCnt < 10) {
                    sLog.info("invoke IocpServdService::ServiceWorkerLoop(): Pause, cnt = %d, time = %d",
                        s_nServiceLoopPauseCnt, GetTickCount());
                    s_nServiceLoopPauseCnt++;
                }
                ::Sleep(nPauseSleepTime);
            }
        }
        return true;
    }
#endif
};

NS_IOCPSERVD_END

#endif  /* _IOCPSERVDSERVICE_H_ */
