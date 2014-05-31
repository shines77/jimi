
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

    /**
     * 注意: 切记!! 所有回调函数必须返回true, 如果返回false, 则表示不调用WinService的默认行为,
     *       此时, 你必须正确的处理该回调应该完成的事情, 否则程序可能不能正常工作.
     **/
    bool OnServiceInit() {
        sLog.info("invoke IocpServdService::OnServiceInit().");
        return true;
    }

    bool OnServiceCleanup() {
        sLog.info("invoke IocpServdService::OnServiceCleanup().");
        return true;
    }

    bool OnStart(int argc, TCHAR *argv[]) {
        sLog.info("invoke IocpServdService::OnStart().");
        return true;
    }

    bool OnStop() {
        sLog.info("invoke IocpServdService::OnStop().");
        return true;
    }

#if 0

    bool OnPause() {
        sLog.info("invoke IocpServdService::OnPause().");
        return true;
    }

    bool OnContinue() {
        sLog.info("invoke IocpServdService::OnContinue().");
        return true;
    }

    bool OnShutdown() {
        sLog.info("invoke IocpServdService::OnShutdown().");
        return true;
    }

    bool OnInterrogate() {
        sLog.info("invoke IocpServdService::OnInterrogate().");
        return true;
    }

    bool OnParamChange() {
        sLog.info("invoke IocpServdService::OnParamChange().");
        return true;
    }

    bool OnDeviceEvent() {
        sLog.info("invoke IocpServdService::OnDeviceEvent().");
        return true;
    }

    bool OnSessionChange() {
        sLog.info("invoke IocpServdService::OnSessionChange().");
        return true;
    }

    bool OnPowerEvent() {
        sLog.info("invoke IocpServdService::OnPowerEvent().");
        return true;
    }

    bool OnPreShutdown() {
        sLog.info("invoke IocpServdService::OnPreShutdown().");
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

#elif 0
    virtual bool OnServiceInit() {
        sLog.info("invoke IocpServdService::OnServiceInit().");
        return true;
    }

    virtual bool OnServiceCleanup() {
        sLog.info("invoke IocpServdService::OnServiceCleanup().");
        return true;
    }}

    virtual bool OnStart(int argc, TCHAR *argv[]) {
        sLog.info("invoke IocpServdService::OnStart().");
        return true;
    }

    virtual bool OnStop() {
        sLog.info("invoke IocpServdService::OnStop().");
        return true;
    }

    virtual bool OnPause() {
        sLog.info("invoke IocpServdService::OnPause().");
        return true;
    }

    virtual bool OnContinue() {
        sLog.info("invoke IocpServdService::OnContinue().");
        return true;
    }

    virtual bool OnShutdown() {
        sLog.info("invoke IocpServdService::OnShutdown().");
        return true;
    }

    virtual bool OnInterrogate() {
        sLog.info("invoke IocpServdService::OnInterrogate().");
        return true;
    }

    virtual bool OnParamChange() {
        sLog.info("invoke IocpServdService::OnParamChange().");
        return true;
    }

    virtual bool OnDeviceEvent() {
        sLog.info("invoke IocpServdService::OnDeviceEvent().");
        return true;
    }

    virtual bool OnSessionChange() {
        sLog.info("invoke IocpServdService::OnSessionChange().");
        return true;
    }

    virtual bool OnPowerEvent() {
        sLog.info("invoke IocpServdService::OnPowerEvent().");
        return true;
    }

    virtual bool OnPreShutdown() {
        sLog.info("invoke IocpServdService::OnPreShutdown().");
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

#if 0
    bool ServiceWorkerMethod(void *pvData) {
        static int s_nOnServiceLoopCnt = 0;
        static int s_nServiceLoopPauseCnt = 0;
        DWORD dwSleepTime = GetSleepTime();
        DWORD dwPauseSleepTime = GetPauseSleepTime();
        while (IsRunning()) {
            if (!IsPausing()) {
                if (s_nOnServiceLoopCnt < 10) {
                    sLog.info("invoke IocpServdService::ServiceWorkerMethod(), cnt = %d", s_nOnServiceLoopCnt);
                    s_nOnServiceLoopCnt++;
                }
                s_nServiceLoopPauseCnt = 0;
                ::Sleep(dwSleepTime);
            }
            else {
                if (s_nServiceLoopPauseCnt < 10) {
                    sLog.info("invoke IocpServdService::ServiceWorkerMethod(): Pause, cnt = %d, time = %d",
                        s_nServiceLoopPauseCnt, GetTickCount());
                    s_nServiceLoopPauseCnt++;
                }
                ::Sleep(dwPauseSleepTime);
            }
        }
        return true;
    }
#endif
};

NS_IOCPSERVD_END

#endif  /* _IOCPSERVDSERVICE_H_ */
