
#ifndef _JIMI_SYSTEM_CWIN_SERVICE_H_
#define _JIMI_SYSTEM_CWIN_SERVICE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winsvc.h>
#include <tchar.h>

//#include <jimi/iocp/IocpServd.h>

#if !defined(WINADVAPI)
#if !defined(_ADVAPI32_)
    #define WINADVAPI   DECLSPEC_IMPORT
#else
    #define WINADVAPI
#endif
#endif

#define SERVICE_SLEEP_TIME                  5000

#define ERROR_SERVICE_INSTANCE_NOT_INITED   "Service Instance is not initialized."

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

enum eServiceStatus {
    SVC_STATUS_UNKNOWN = -2,
    SVC_STATUS_NOTINSERVICE = -1,
    SVC_STATUS_STOPPED,
    SVC_STATUS_RUNNING,
    SVC_STATUS_PAUSED,

    SVC_STATUS_START_PENDING,
    SVC_STATUS_PAUSE_PENDING,
    SVC_STATUS_CONTINUE_PENDING,
    SVC_STATUS_STOP_PENDING,
};

//
// 参考: MangOS 源码 以及
//
//       Writing Tools for Windows in C++
//       http://sprogram.com.ua/en/articles/how-write-service-for-windows-with-cpp

class IWinService
{
    virtual bool OnInitService() = 0;

    virtual bool OnStart() = 0;
    virtual bool OnStop() = 0;

    virtual bool OnPause() = 0;
    virtual bool OnContinue() = 0;

    virtual bool OnServiceLoop() = 0;
};

class CWinService : public IWinService
{
public:
    CWinService(void);
    virtual ~CWinService();

    bool InstallService();
    bool UninstallService();

    virtual bool OnInitService();

    virtual bool OnStart();
    virtual bool OnStop();

    virtual bool OnPause();
    virtual bool OnContinue();

    virtual bool OnServiceLoop();

    static CWinService *GetInstance();
    static void SetInstance(CWinService *pInstance);

    static int RunService(int argc, TCHAR *argv[]);

    static void ServiceReportEvent(LPTSTR szFunction, int nEventId);
    static void WINAPI ServiceControlHandler(DWORD dwControlCode);
    static void WINAPI ServiceMain(DWORD argc, TCHAR *argv[]);
    static void WINAPI ServiceLoop(DWORD argc, TCHAR *argv[]);

    void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

private:
    static CWinService     *s_pServiceInstance;

    int                     m_nServiceStatus;
    unsigned int            m_nSleepTime;

    SERVICE_STATUS          m_ServiceStatus;
    SERVICE_STATUS_HANDLE   m_ServiceStatusHandle;
};

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* JIMI_IS_WINDOWS */

#endif  /* _JIMI_SYSTEM_CWIN_SERVICE_H_ */
