
#ifndef _IOCPSERVDSERVICE_H_
#define _IOCPSERVDSERVICE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "IocpServd.h"

#include "jimi/core/jimi_def.h"
#include "jimi/system/WinService.h"
#include "jimi/log/log.h"

NS_IOCPSERVD_BEGIN

class IocpServdService : public jimi::WinServiceBase<IocpServdService>
{
public:
    IocpServdService(void);
    ~IocpServdService(void);

#if 1
    void InitComponent() {
        jmLog.info("IocpServdService::InitComponent().");
    }
#endif

#if 1
    /**
     * 注意: 切记!! 所有回调函数必须返回true, 如果返回false, 则表示不调用WinService的默认行为,
     *       此时, 你必须正确的处理该回调应该完成的事情, 否则程序可能不能正常工作.
     **/
    bool OnServiceInit() {
        jmLog.info("invoke IocpServdService::OnServiceInit().");
        return true;
    }

    bool OnServiceCleanup() {
        jmLog.info("invoke IocpServdService::OnServiceCleanup().");
        return true;
    }

    bool OnStart(int argc, TCHAR *argv[]) {
        jmLog.info("invoke IocpServdService::OnStart().");
        return true;
    }

    bool OnStop() {
        jmLog.info("invoke IocpServdService::OnStop().");
        return true;
    }

    bool OnPause() {
        jmLog.info("invoke IocpServdService::OnPause().");
        return true;
    }

    bool OnContinue() {
        jmLog.info("invoke IocpServdService::OnContinue().");
        return true;
    }

    bool OnShutdown() {
        jmLog.info("invoke IocpServdService::OnShutdown().");
        return true;
    }

    bool OnInterrogate() {
        jmLog.info("invoke IocpServdService::OnInterrogate().");
        return true;
    }

    bool OnParamChange(DWORD dwParamChangeReason) {
        jmLog.info("invoke IocpServdService::OnParamChange().");
        return true;
    }

    bool OnDeviceEvent(DWORD dwDeviceEvent) {
        jmLog.info("invoke IocpServdService::OnDeviceEvent().");
        return true;
    }

    bool OnSessionChange(jimi::SessionChangeDescription *changeDescription) {
        jmLog.info("invoke IocpServdService::OnSessionChange().");
        return true;
    }

    bool OnPowerEvent(jimi::PowerBroadcastStatus *powerStatus) {
        jmLog.info("invoke IocpServdService::OnPowerEvent().");
        return true;
    }

    bool OnPreShutdown() {
        jmLog.info("invoke IocpServdService::OnPreShutdown().");
        return true;
    }

    bool OnCustomCommand(DWORD dwControlCode) {
        jmLog.info("invoke IocpServdService::OnCustomCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }

    bool OnUnknownCommand(DWORD dwControlCode) {
        jmLog.info("invoke IocpServdService::OnUnknownCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }

#elif 0
    virtual bool OnServiceInit() {
        jmLog.info("invoke IocpServdService::OnServiceInit().");
        return true;
    }

    virtual bool OnServiceCleanup() {
        jmLog.info("invoke IocpServdService::OnServiceCleanup().");
        return true;
    }}

    virtual bool OnStart(int argc, TCHAR *argv[]) {
        jmLog.info("invoke IocpServdService::OnStart().");
        return true;
    }

    virtual bool OnStop() {
        jmLog.info("invoke IocpServdService::OnStop().");
        return true;
    }

    virtual bool OnPause() {
        jmLog.info("invoke IocpServdService::OnPause().");
        return true;
    }

    virtual bool OnContinue() {
        jmLog.info("invoke IocpServdService::OnContinue().");
        return true;
    }

    virtual bool OnShutdown() {
        jmLog.info("invoke IocpServdService::OnShutdown().");
        return true;
    }

    virtual bool OnInterrogate() {
        jmLog.info("invoke IocpServdService::OnInterrogate().");
        return true;
    }

    virtual bool OnParamChange() {
        jmLog.info("invoke IocpServdService::OnParamChange().");
        return true;
    }

    virtual bool OnDeviceEvent() {
        jmLog.info("invoke IocpServdService::OnDeviceEvent().");
        return true;
    }

    virtual bool OnSessionChange(jimi::system::SessionChangeDescription *changeDescription) {
        jmLog.info("invoke IocpServdService::OnSessionChange().");
        return true;
    }

    virtual bool OnPowerEvent(jimi::system::PowerBroadcastStatus *powerStatus) {
        jmLog.info("invoke IocpServdService::OnPowerEvent().");
        return true;
    }

    virtual bool OnPreShutdown() {
        jmLog.info("invoke IocpServdService::OnPreShutdown().");
        return true;
    }

    virtual bool OnCustomCommand(DWORD dwControlCode) {
        jmLog.info("invoke IocpServdService::OnCustomCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }

    virtual bool OnUnknownCommand(DWORD dwControlCode) {
        jmLog.info("invoke IocpServdService::OnUnknownCommand(), dwControlCode = %d", dwControlCode);
        return true;
    }
#endif

#if 1
    bool ServiceWorkerMethod(void *pvData) {
        static int s_nOnServiceLoopCnt = 0;
        static int s_nServiceLoopPauseCnt = 0;
        DWORD dwSleepTime = GetSleepTime();
        DWORD dwPauseSleepTime = GetPauseSleepTime();
        while (IsRunning()) {
            if (!IsPausing()) {
                if (s_nOnServiceLoopCnt < 10) {
                    jmLog.info("invoke IocpServdService::ServiceWorkerMethod(), cnt = %d", s_nOnServiceLoopCnt);
                    s_nOnServiceLoopCnt++;
                }
                s_nServiceLoopPauseCnt = 0;
                ::Sleep(dwSleepTime);
            }
            else {
                if (s_nServiceLoopPauseCnt < 10) {
                    jmLog.info("invoke IocpServdService::ServiceWorkerMethod(): Pause, cnt = %d, time = %d",
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
