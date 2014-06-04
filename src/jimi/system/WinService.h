
#ifndef _JIMI_SYSTEM_WIN_SERVICE_H_
#define _JIMI_SYSTEM_WIN_SERVICE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <jimi/core/jimi_def.h>

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <winsvc.h>
#include <wtsapi32.h>

#include <tchar.h>
#include <string>
#include <cstring>
/* tchar.h 必须放在strsafe.h前面, stl的string等声明最好也放在strsafe.h前面 */
#include <strsafe.h>
#include <string.h>

#include <jimi/log/log_all.h>
//#include <jimi/iocp/IocpServd.h>

#include <jimic/string/jm_strings.h>

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Wtsapi32.lib")

using namespace std;

USING_NS_JIMI;
USING_NS_JIMI_LOG;

#if !defined(WINADVAPI)
#if !defined(_ADVAPI32_)
    #define WINADVAPI   DECLSPEC_IMPORT
#else
    #define WINADVAPI
#endif
#endif

/* ServiceWorkerMethod 默认的休眠时间, 单位为毫秒 */
#define SERVICE_SLEEP_TIME                  5000
/* ServiceWorkerMethod 暂停时的休眠时间, 单位为毫秒 */
#define SERVICE_PAUSE_SLEEP_TIME            200

/* 默认的执行一个命令的等待时间dwWaitHint, 单位为毫秒 */
#define SERVICE_DEFAULT_WAIT_HINT           5000

#define ERROR_SERVICE_INSTANCE_NOT_INITED   "Service Instance is not initialized."

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

typedef WINADVAPI BOOL (WINAPI *CSD_T)(SC_HANDLE, DWORD, LPCVOID);

//
// Service State -- for CurrentState
//
typedef enum eServiceStatus {
    SVC_STATUS_NOTINSERVICE     = -1,
    SVC_STATUS_UNKNOWN          = 0,

    SVC_STATUS_FIRST            = 0,

    SVC_STATUS_STOPPED          = SERVICE_STOPPED,
    SVC_STATUS_START_PENDING    = SERVICE_START_PENDING,
    SVC_STATUS_STOP_PENDING     = SERVICE_STOP_PENDING,
    SVC_STATUS_RUNNING          = SERVICE_RUNNING,
    SVC_STATUS_CONTINUE_PENDING = SERVICE_CONTINUE_PENDING,
    SVC_STATUS_PAUSE_PENDING    = SERVICE_PAUSE_PENDING,
    SVC_STATUS_PAUSED           = SERVICE_PAUSED,

    SVC_STATUS_LAST
} eServiceStatus;

static char * s_szServiceStatusString[] = {
    "SERVICE_UNKNOWN",
    "SERVICE_STOPPED",
    "SERVICE_START_PENDING",
    "SERVICE_STOP_PENDING",
    "SERVICE_RUNNING",
    "SERVICE_CONTINUE_PENDING",
    "SERVICE_PAUSE_PENDING",
    "SERVICE_PAUSED",
    "SERVICE_LAST3",
    "SERVICE_LAST2",
    "SERVICE_LAST1",
    "SERVICE_LAST0"
};

typedef enum eServiceErrorId {
    ERR_SVC_UNKNOWN = 0,
    ERR_SVC_INIT_SERVICE_FAILED = 101,
    ERR_SVC_START_SERVICE_FAILED = 102,
    ERR_SVC_STOP_SERVICE_FAILED = 103,
    ERR_SVC_CLEANUP_SERVICE_FAILED = 109,
    ERR_SVC_MAXID
} eServiceErrorId;

//
// Summary:
//     Specifies the reason for a Terminal Services session change notice.
typedef enum Session_Change_Reason {
    //
    // Summary:
    //     A unknown reason.
    kReasonUnknown = 0,
    //
    // Summary:
    //     A console session has connected.
    kConsoleConnect = 1,
    //
    // Summary:
    //     A console session has disconnected.
    kConsoleDisconnect = 2,
    //
    // Summary:
    //     A remote session has connected.
    kRemoteConnect = 3,
    //
    // Summary:
    //     A remote session has disconnected.
    kRemoteDisconnect = 4,
    //
    // Summary:
    //     A user has logged on to a session.
    kSessionLogon = 5,
    //
    // Summary:
    //     A user has logged off from a session.
    kSessionLogoff = 6,
    //
    // Summary:
    //     A session has been locked.
    kSessionLock = 7,
    //
    // Summary:
    //     A session has been unlocked.
    kSessionUnlock = 8,
    //
    // Summary:
    //     The remote control status of a session has changed.
    kSessionRemoteControl = 9

} Session_Change_Reason;

typedef struct SessionChangeReason
{
    //
    // Summary:
    //     A unknown reason.
    static const DWORD ReasonUnknown = kReasonUnknown;
    //
    // Summary:
    //     A console session has connected.
    static const DWORD ConsoleConnect = kConsoleConnect;
    //
    // Summary:
    //     A console session has disconnected.
    static const DWORD ConsoleDisconnect = kConsoleDisconnect;
    //
    // Summary:
    //     A remote session has connected.
    static const DWORD RemoteConnect = kRemoteConnect;
    //
    // Summary:
    //     A remote session has disconnected.
    static const DWORD RemoteDisconnect = kRemoteDisconnect;
    //
    // Summary:
    //     A user has logged on to a session.
    static const DWORD SessionLogon = kSessionLogon;
    //
    // Summary:
    //     A user has logged off from a session.
    static const DWORD SessionLogoff = kSessionLogoff;
    //
    // Summary:
    //     A session has been locked.
    static const DWORD SessionLock = kSessionLock;
    //
    // Summary:
    //     A session has been unlocked.
    static const DWORD SessionUnlock = kSessionUnlock;
    //
    // Summary:
    //     The remote control status of a session has changed.
    static const DWORD SessionRemoteControl = kSessionRemoteControl;

} SessionChangeReason;

typedef struct SessionChangeDescription
{
    DWORD dwReason;
    DWORD dwSessionId;
    LPVOID lpEventData;
} SessionChangeDescription;

typedef struct PowerBroadcastStatus
{
    DWORD dwStatus;
} PowerBroadcastStatus;

//
// 参考: MangOS 源码 以及
//
//       Writing Tools for Windows in C++
//       http://sprogram.com.ua/en/articles/how-write-service-for-windows-with-cpp

class JIMI_DLL IWinServiceBase
{
    virtual bool OnServiceInit() = 0;
    virtual bool OnServiceCleanup() = 0;

    virtual bool OnStart(int argc, TCHAR *argv[]) = 0;
    virtual bool OnStop() = 0;

    virtual bool OnPause() = 0;
    virtual bool OnContinue() = 0;

    virtual bool OnShutdown() = 0;

    virtual bool OnInterrogate() = 0;
    virtual bool OnParamChange() = 0;
    virtual bool OnDeviceEvent() = 0;
    virtual bool OnSessionChange(SessionChangeDescription *changeDescription) = 0;
    virtual bool OnPowerEvent(PowerBroadcastStatus *powerStatus) = 0;
    virtual bool OnPreShutdown() = 0;

    virtual bool OnCustomCommand(DWORD dwControlCode) = 0;
    virtual bool OnUnknownCommand(DWORD dwControlCode) = 0;

    //virtual bool ServiceWorkerMethod(void *pvData) = 0;
};

template <class T>
class JIMI_DLL WinServiceBase /* : public IWinServiceBase */
{
public:
    WinServiceBase(void);
    WinServiceBase(WinServiceBase *pInstance, bool bCreateByNew = true);
    ~WinServiceBase(void);

    void Release();
    void OnDestroy();

    void InitComponent();
    void InitializeComponent();

    bool IsRunning();
    bool IsPausing();

    bool InstallService();
    bool UninstallService();    

#if 1
    // callback events
    bool OnServiceInit();
    bool OnServiceCleanup();

    bool OnStart(int argc, TCHAR *argv[]);
    bool OnStop();

    bool OnPause();
    bool OnContinue();

    bool OnShutdown();

    bool OnInterrogate();
    bool OnParamChange(DWORD dwParamChangeReason);
    bool OnDeviceEvent(DWORD dwDeviceEvent);
    bool OnSessionChange(SessionChangeDescription *changeDescription);
    bool OnPowerEvent(PowerBroadcastStatus *powerStatus);
    bool OnPreShutdown();

    bool OnCustomCommand(DWORD dwControlCode);
    bool OnUnknownCommand(DWORD dwControlCode);

#else
    // callback events
    virtual bool OnServiceInit();
    virtual bool OnServiceCleanup();

    virtual bool OnStart(int argc, TCHAR *argv[]);
    virtual bool OnStop();

    virtual bool OnPause();
    virtual bool OnContinue();

    virtual bool OnShutdown();

    virtual bool OnInterrogate();
    virtual bool OnParamChange(DWORD dwParamChangeReason);
    virtual bool OnDeviceEvent(DWORD dwDeviceEvent);
    virtual bool OnSessionChange(SessionChangeDescription *changeDescription);
    virtual bool OnPowerEvent(PowerBroadcastStatus *powerStatus);
    virtual bool OnPreShutdown();

    virtual bool OnCustomCommand(DWORD dwControlCode);
    virtual bool OnUnknownCommand(DWORD dwControlCode);
#endif

    // fire callback events
    bool FireEvent_OnServiceInit();
    bool FireEvent_OnServiceCleanup();

    bool FireEvent_OnStart(int argc, TCHAR *argv[]);
    bool FireEvent_OnStop();

    bool FireEvent_OnPause();
    bool FireEvent_OnContinue();

    bool FireEvent_OnShutdown();

    bool FireEvent_OnInterrogate();

    bool FireEvent_OnParamChange(DWORD dwEventType, LPVOID lpEventData);
    bool FireEvent_OnDeviceEvent(DWORD dwEventType, LPVOID lpEventData);
    bool FireEvent_OnSessionChange(DWORD dwEventType, LPVOID lpEventData);
    bool FireEvent_OnPowerEvent(DWORD dwEventType, LPVOID lpEventData);
    bool FireEvent_OnPreShutdown(DWORD dwEventType, LPVOID lpEventData);

    bool FireEvent_OnCustomCommand(DWORD dwControlCode);
    bool FireEvent_OnUnknownCommand(DWORD dwControlCode);

    // worker thread
    bool ServiceWorkerMethod(void *pvData);

    bool SetCreateByNew(bool bCreateByNew);

    SERVICE_STATUS_HANDLE GetServiceStatusHandle() {
        return m_ServiceStatusHandle;
    }
    void SetServiceStatusHandle(SERVICE_STATUS_HANDLE hSvcStatusHandle) {
        m_ServiceStatusHandle = hSvcStatusHandle;
    }

    TCHAR *GetServiceName()         { return m_ServiceName;        }
    TCHAR *GetServiceDisplayName()  { return m_ServiceDisplayName; }
    TCHAR *GetServiceDescription()  { return m_ServiceDescription; }

    TCHAR *SetServiceName(TCHAR *szServiceName);
    TCHAR *SetServiceDisplayName(TCHAR *szServiceDisplayName);
    TCHAR *SetServiceDescription(TCHAR *szServiceDescription);

    DWORD GetSleepTime() { return m_dwSleepTime; }
    void SetSleepTime(DWORD dwMillsecs) { m_dwSleepTime = dwMillsecs; }

    DWORD GetPauseSleepTime() { return m_dwPauseSleepTime; }
    void SetPauseSleepTime(DWORD dwMillsecs) { m_dwPauseSleepTime = dwMillsecs; }

    DWORD GetCurrentState() { return m_ServiceStatus.dwCurrentState; }

    int RunService();
    int RunServiceEx(SERVICE_TABLE_ENTRY serviceTable[]);

    static int RunService(WinServiceBase *pInstance);
    static int RunServiceEx(WinServiceBase *pInstance, SERVICE_TABLE_ENTRY serviceTable[]);

    static WinServiceBase *GetInstance();
    static void SetInstance(WinServiceBase *pInstance, bool bCreateByNew);

    static void ServiceReportEvent(LPTSTR szFunction, int nEventId);

    static void  WINAPI ServiceControlHandler(DWORD dwControlCode);
    static DWORD WINAPI ServiceControlHandlerEx(DWORD dwControlCode,
        DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);

    static void WINAPI ServiceMain(int argc, TCHAR *argv[]);
    static void WINAPI ServiceWorkerLoop(int argc, TCHAR *argv[]);

    BOOL UpdateServiceStatus(DWORD dwCurrentState);
    BOOL UpdateServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);
    BOOL UpdateServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode,
        DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint, DWORD dwWaitHint);

    static WinServiceBase  *s_pServiceInstance;

private:
    bool                    m_bCreateByNew;
    DWORD                   m_dwSleepTime;
    DWORD                   m_dwPauseSleepTime;
    DWORD                   m_dwWaitHint;
    int                     m_nServiceStatus;

    SERVICE_STATUS          m_ServiceStatus;
    SERVICE_STATUS_HANDLE   m_ServiceStatusHandle;

    TCHAR                   m_ServiceName[64];
    TCHAR                   m_ServiceDisplayName[128];
    TCHAR                   m_ServiceDescription[512];
};

template <class T>
WinServiceBase<T>::WinServiceBase(void)
: m_bCreateByNew(false)
, m_dwSleepTime(SERVICE_SLEEP_TIME)
, m_dwPauseSleepTime(SERVICE_PAUSE_SLEEP_TIME)
, m_dwWaitHint(SERVICE_DEFAULT_WAIT_HINT)
, m_nServiceStatus(SVC_STATUS_UNKNOWN)
, m_ServiceStatusHandle(NULL)
{
    ::ZeroMemory(&m_ServiceName, sizeof(m_ServiceName));
    ::ZeroMemory(&m_ServiceDisplayName, sizeof(m_ServiceDisplayName));
    ::ZeroMemory(&m_ServiceDescription, sizeof(m_ServiceDescription));
    ::ZeroMemory(&m_ServiceStatus, sizeof(m_ServiceStatus));

    T *pT = static_cast<T *>(this);
    pT->InitComponent();
}

template <class T>
WinServiceBase<T>::~WinServiceBase(void)
{
    // Do nothing!!
    T *pT = static_cast<T *>(this);
    pT->OnDestroy();
}

template <class T>
void WinServiceBase<T>::Release()
{
    sLog.info("WinServiceBase<T>::Release(), error = %d.", GetLastError());
}

template <class T>
void WinServiceBase<T>::OnDestroy()
{
    sLog.info("WinServiceBase<T>::OnDestroy(), error = %d.", GetLastError());
}

template <class T>
void WinServiceBase<T>::InitComponent()
{
    // Initialize the operating properties for the service.
    sLog.info("WinServiceBase<T>::InitComponent().");
}

template <class T>
bool WinServiceBase<T>::IsRunning()
{
    return (m_nServiceStatus != SVC_STATUS_STOPPED)
        && (m_nServiceStatus != SVC_STATUS_STOP_PENDING);
}

template <class T>
bool WinServiceBase<T>::IsPausing()
{
    return (m_nServiceStatus == SVC_STATUS_PAUSED)
        || (m_nServiceStatus == SVC_STATUS_PAUSE_PENDING);
}

template <class T>
TCHAR * WinServiceBase<T>::SetServiceName(TCHAR *szServiceName)
{
    if (szServiceName)
        ::jm_tcscpy(m_ServiceName, jm_countof(m_ServiceName), szServiceName);
    return m_ServiceName;
}

template <class T>
TCHAR * WinServiceBase<T>::SetServiceDisplayName(TCHAR *szServiceDisplayName)
{
    if (szServiceDisplayName)
        ::jm_tcscpy(m_ServiceDisplayName, jm_countof(m_ServiceDisplayName), szServiceDisplayName);
    return m_ServiceDisplayName;
}

template <class T>
TCHAR * WinServiceBase<T>::SetServiceDescription(TCHAR *szServiceDescription)
{
    if (szServiceDescription)
        ::jm_tcscpy(m_ServiceDescription, jm_countof(m_ServiceDescription), szServiceDescription);
    return m_ServiceDescription;
}

template <class T>
bool WinServiceBase<T>::InstallService()
{
    TCHAR szPath[MAX_PATH + 10];
    size_t nLen = sizeof(szPath) / sizeof(szPath[0]);

    szPath[0] = _T('\0');
#if 1
    if (!::GetModuleFileName(NULL, szPath, nLen)) {
        sLog.error("Cannot install service (%d).", GetLastError());
        sLog.error("SERVICE: Can't get service binary filename.");
        return false;
    }
#else
    if (!GetModuleFileName(NULL, szPath + 1, nLen - _ATL_QUOTES_SPACE)) {
        sLog.error("Cannot install service (%d).", GetLastError());
        sLog.error("SERVICE: Can't get service binary filename.");
        return false;
    }

    // Quote the FilePath before calling CreateService
    size_t dwLen = ::jm_tcslen(szPath + 1);
    szPath[0] = _T('\"');
    szPath[dwLen + 1] = _T('\"');
    szPath[dwLen + 2] = _T('\0');
#endif

    ::jm_tcscat(szPath, jm_countof(szPath), _T(" -run"));

    // Get a handle to the SCM database.
    SC_HANDLE schSCManager = ::OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        int nLastError = GetLastError();
        sLog.error("OpenSCManager failed (%d).", nLastError);
        sLog.error("SERVICE: No access to service control manager.");
        if (nLastError == 5) {
            sLog.error("please, try run with admin rights.");
        }
        return false;
    }

    // Create the service
    SC_HANDLE schService = ::CreateService(
        schSCManager,                   // SCM database
        m_ServiceName,                  // name of service
        m_ServiceDisplayName,           // service name to display
        SERVICE_ALL_ACCESS,             // desired access
        SERVICE_WIN32_OWN_PROCESS,
        //SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
                                        // service type
        SERVICE_DEMAND_START,           // start type
        SERVICE_ERROR_NORMAL,           // error control type
        szPath,                         // path to service's binary
        NULL,                           // no load ordering group
        NULL,                           // no tag identifier
        NULL,                           // no dependencies
        NULL,                           // LocalSystem account
        NULL);                          // no password

    if (schService == NULL) {
        sLog.error("CreateService failed (%d).", GetLastError());
        sLog.error("SERVICE: Can't register service for: %s.", szPath);
        ::CloseServiceHandle(schSCManager);
        return false;
    }
    else {
        sLog.info("Service installed successfully.");
    }

    // Change the service config
    CSD_T ChangeService_Config2;
    HMODULE advapi32 = ::GetModuleHandle("ADVAPI32.DLL");
    if (!advapi32) {
        sLog.error("SERVICE: Can't access ADVAPI32.DLL .");
        ::CloseServiceHandle(schService);
        ::CloseServiceHandle(schSCManager);
        return false;
    }

    ChangeService_Config2 = (CSD_T)::GetProcAddress(advapi32, "ChangeServiceConfig2A");
    if (!ChangeService_Config2) {
        sLog.error("SERVICE: Can't access ChangeServiceConfig2A from ADVAPI32.DLL .");
        ::CloseServiceHandle(schService);
        ::CloseServiceHandle(schSCManager);
        return false;
    }

    SERVICE_DESCRIPTION sdBuf;
    sdBuf.lpDescription = m_ServiceDescription;
    ChangeService_Config2(
        schService,                             // handle to service
        SERVICE_CONFIG_DESCRIPTION,             // change: description
        &sdBuf);                                // new data

    SC_ACTION _action[1];
    _action[0].Type = SC_ACTION_RESTART;
    _action[0].Delay = 10000;

    SERVICE_FAILURE_ACTIONS sfa;
    ::ZeroMemory(&sfa, sizeof(SERVICE_FAILURE_ACTIONS));
    sfa.lpsaActions = _action;
    sfa.cActions = 1;
    sfa.dwResetPeriod = INFINITE;
    ChangeService_Config2(
        schService,                             // handle to service
        SERVICE_CONFIG_FAILURE_ACTIONS,         // information level
        &sfa);                                  // new data

    ::CloseServiceHandle(schService);
    ::CloseServiceHandle(schSCManager);
    return true;
}

template <class T>
bool WinServiceBase<T>::UninstallService()
{
    SC_HANDLE schSCManager = ::OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (!schSCManager) {
        sLog.error("SERVICE: No access to service control manager.");
        return false;
    }

    SC_HANDLE schService = ::OpenService(
        schSCManager,
        m_ServiceName,                          // name of service
        SERVICE_ALL_ACCESS | DELETE             // desired access
        /*
        SERVICE_QUERY_STATUS | DELETE
        | SERVICE_STOP
        */
        );

    if (!schService) {
        ::CloseServiceHandle(schSCManager);
        sLog.error("SERVICE: Service not found: %s", m_ServiceName);
        return false;
    }

    SERVICE_STATUS serviceStatus2;
    if (::QueryServiceStatus(schService, &serviceStatus2)) {
        // 卸载之前, 先停止Service
        if (serviceStatus2.dwCurrentState == SERVICE_RUNNING
            || serviceStatus2.dwCurrentState == SERVICE_PAUSED) {
            ::ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus2);
        }

        // 最多等待180秒, 如果服务还未停止, 则强制删除
        const DWORD dwTimeOutLimit = 180;
        DWORD dwTimeOut = GetTickCount() + dwTimeOutLimit * 1000;
        while (serviceStatus2.dwCurrentState != SERVICE_STOPPED) {
            if (!::QueryServiceStatus(schService, &serviceStatus2))
                break;
            if (GetTickCount() > dwTimeOut) {
                ::SetLastError(ERROR_TIMEOUT);
                break;
            }
            ::Sleep(200);
        }
    }

    if (::DeleteService(schService))
        sLog.info("Service uninstalled successfully.");
    else
        sLog.info("Service uninstalled failed.");

    ::CloseServiceHandle(schService);
    ::CloseServiceHandle(schSCManager);
    return true;
}

template <class T>
void WinServiceBase<T>::ServiceReportEvent(LPTSTR szFunction, int nEventId)
{
    int nLastErr = GetLastError();

    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR buffer[128];
    LPTSTR szServiceName = NULL;

    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance != NULL) {
        szServiceName = pInstance->GetServiceName();
    }
    else {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        //throw ERROR_SERVICE_INSTANCE_NOT_INITED;
        szServiceName = _T("Unknown WinServiceBase<T>");
    }

    hEventSource = ::RegisterEventSource(NULL, szServiceName);

    if (NULL != hEventSource) {
        ::StringCchPrintf(buffer, 128, _T("%s failed with %d"), szFunction, nLastErr);

        lpszStrings[0] = szServiceName;
        lpszStrings[1] = buffer;

        ::ReportEvent(hEventSource,      // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    nEventId,            // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data

        ::DeregisterEventSource(hEventSource);
    }

    DWORD dwExitCode = pInstance->m_ServiceStatus.dwWin32ExitCode;
    if (dwExitCode != NO_ERROR) {
        ::ExitProcess(dwExitCode);
    }
}

template <class T>
bool WinServiceBase<T>::SetCreateByNew(bool bCreateByNew)
{
    bool oldCreateByNew = m_bCreateByNew;
    m_bCreateByNew = bCreateByNew;
    return oldCreateByNew;
}

template <class T>
WinServiceBase<T> *WinServiceBase<T>::GetInstance()
{
    if (s_pServiceInstance == NULL)
        s_pServiceInstance = new WinServiceBase<T>();
    return s_pServiceInstance;
}

template <class T>
void WinServiceBase<T>::SetInstance(WinServiceBase *pServiceInstance, bool bCreateByNew)
{
    if (pServiceInstance == NULL) {
        if (s_pServiceInstance != NULL) {
            s_pServiceInstance->Release();
            delete s_pServiceInstance;
        }
    }
    else {
        pServiceInstance->SetCreateByNew(bCreateByNew);
    }
    s_pServiceInstance = pServiceInstance;
}

template <class T>
BOOL WinServiceBase<T>::UpdateServiceStatus(DWORD dwCurrentState)
{
    return UpdateServiceStatus(dwCurrentState, NO_ERROR, NO_ERROR, 0, 0);
}

template <class T>
BOOL WinServiceBase<T>::UpdateServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    return UpdateServiceStatus(dwCurrentState, dwWin32ExitCode, NO_ERROR, 0, dwWaitHint);
}

template <class T>
BOOL WinServiceBase<T>::UpdateServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode,
                                            DWORD dwServiceSpecificExitCode, DWORD dwCheckPoint,
                                            DWORD dwWaitHint)
{
    BOOL bResult = FALSE;
    sLog.info("invoke WinServiceBase<T>::UpdateServiceStatus() enter.");

    this->m_nServiceStatus = dwCurrentState;

    if (dwCurrentState > SVC_STATUS_FIRST && dwCurrentState < SVC_STATUS_LAST) {

        // Fill in the SERVICE_STATUS structure.
        this->m_ServiceStatus.dwCurrentState   = dwCurrentState;
        this->m_ServiceStatus.dwWin32ExitCode  = dwWin32ExitCode;
        this->m_ServiceStatus.dwCheckPoint     = dwCheckPoint;
        this->m_ServiceStatus.dwWaitHint       = dwWaitHint;
        this->m_ServiceStatus.dwServiceSpecificExitCode = dwServiceSpecificExitCode;
        this->m_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;

        sLog.info("UpdateServiceStatus(): dwCurrentState = %s (%d)", s_szServiceStatusString[dwCurrentState], dwCurrentState);

        // Report the status of the service to the SCM.
        bResult = ::SetServiceStatus(this->m_ServiceStatusHandle, &this->m_ServiceStatus);
    }
    else {
        sLog.info("UpdateServiceStatus(): dwCurrentState = Unknown Status (%d)", dwCurrentState);
    }

    sLog.info("invoke WinServiceBase<T>::UpdateServiceStatus() over.");
    return bResult;
}

template <class T>
bool WinServiceBase<T>::OnServiceInit()
{
    sLog.info("invoke WinServiceBase<T>::OnServiceInit().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnServiceCleanup()
{
    sLog.info("invoke WinServiceBase<T>::OnServiceCleanup().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnStart(int argc, TCHAR *argv[])
{
    sLog.info("invoke WinServiceBase<T>::OnStart().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnStop()
{
    sLog.info("invoke WinServiceBase<T>::OnStop().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnPause()
{
    sLog.info("invoke WinServiceBase<T>::OnPause().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnContinue()
{
    sLog.info("invoke WinServiceBase<T>::OnContinue().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnInterrogate()
{
    sLog.info("invoke WinServiceBase<T>::OnInterrogate().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnShutdown()
{
    sLog.info("invoke WinServiceBase<T>::OnShutdown().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnParamChange(DWORD dwParamChangeReason)
{
    sLog.info("invoke WinServiceBase<T>::OnParamChange().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnDeviceEvent(DWORD dwDeviceEvent)
{
    sLog.info("invoke WinServiceBase<T>::OnDeviceEvent().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnSessionChange(SessionChangeDescription *changeDescription)
{
    sLog.info("invoke WinServiceBase<T>::OnSessionChange() Enter.");

    switch (changeDescription->dwReason) {
        case Session_Change_Reason.kConsoleConnect:
            break;

        case Session_Change_Reason.kConsoleDisconnect:
            break;

        case Session_Change_Reason.kRemoteConnect:
            break;

        case Session_Change_Reason.kRemoteDisconnect:
            break;

        case Session_Change_Reason.kSessionLock:
            break;

        case Session_Change_Reason.kSessionLogon:
            break;

        case Session_Change_Reason.kSessionLogoff:
            break;

        case Session_Change_Reason.kSessionLock:
            break;

        case Session_Change_Reason.kSessionUnlock:
            break;

        case Session_Change_Reason.kSessionRemoteControl:
            break;

        default:
            break;
    }

    sLog.info("invoke WinServiceBase<T>::OnSessionChange() Over.");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnPowerEvent(PowerBroadcastStatus *powerStatus)
{
    sLog.info("invoke WinServiceBase<T>::OnPowerEvent().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnPreShutdown()
{
    sLog.info("invoke WinServiceBase<T>::OnPreShutdown().");
    return true;
}

template <class T>
bool WinServiceBase<T>::OnCustomCommand(DWORD dwControlCode)
{
    sLog.info("invoke WinServiceBase<T>::OnCustomCommand(), dwControlCode = %d.", dwControlCode);
    return true;
}

template <class T>
bool WinServiceBase<T>::OnUnknownCommand(DWORD dwControlCode)
{
    sLog.info("invoke WinServiceBase<T>::OnUnknownCommand(), dwControlCode = %d.", dwControlCode);
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnServiceInit()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnServiceInit().");
    T *pT = static_cast<T *>(this);
    return pT->OnServiceInit();
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnServiceCleanup()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnServiceCleanup().");
    T *pT = static_cast<T *>(this);
    return pT->OnServiceCleanup();
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnStart(int argc, TCHAR *argv[])
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnStart().");
    T *pT = static_cast<T *>(this);
    return pT->OnStart(argc, argv);
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnStop()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnStop() Enter.");

    // Signal that service try stop
    UpdateServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, SERVICE_DEFAULT_WAIT_HINT);

    // Signal the service to stop
    T *pT = static_cast<T *>(this);
    if (pT->OnStop()) {
        UpdateServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
    }

    sLog.info("invoke WinServiceBase<T>::FireEvent_OnStop() Over.");
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnPause()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnPause() Enter.");

    // Signal that service try pause
    UpdateServiceStatus(SERVICE_PAUSE_PENDING, NO_ERROR, SERVICE_DEFAULT_WAIT_HINT);

    // Signal the service to pause
    T *pT = static_cast<T *>(this);
    if (pT->OnPause()) {
        UpdateServiceStatus(SERVICE_PAUSED, NO_ERROR, 0);
    }

    sLog.info("invoke WinServiceBase<T>::FireEvent_OnPause() Over.");
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnContinue()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnContinue() Enter.");

    // Signal that service try continue
    UpdateServiceStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, SERVICE_DEFAULT_WAIT_HINT);

    // Signal the service to continue
    T *pT = static_cast<T *>(this);
    if (pT->OnContinue()) {
        UpdateServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);
    }

    sLog.info("invoke WinServiceBase<T>::FireEvent_OnContinue() Over.");
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnInterrogate()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnInterrogate() Enter.");

    T *pT = static_cast<T *>(this);
    if (pT->OnInterrogate()) {
        // ServiceInterrogate
    }

    sLog.info("invoke WinServiceBase<T>::FireEvent_OnInterrogate() Over.");
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnShutdown()
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnShutdown() Enter.");

    // Signal that service try stop (shutdown)
    UpdateServiceStatus(SVC_STATUS_STOP_PENDING, NO_ERROR, SERVICE_DEFAULT_WAIT_HINT * 2);

    // Signal the service to stop (shutdown)
    T *pT = static_cast<T *>(this);
    if (pT->OnShutdown()) {
        UpdateServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
    }

    sLog.info("invoke WinServiceBase<T>::FireEvent_OnShutdown() Over.");
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnParamChange(DWORD dwEventType, LPVOID lpEventData)
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnParamChange().");
    DWORD dwParamChangeReason = 0;

    T *pT = static_cast<T *>(this);
    return pT->OnParamChange(dwParamChangeReason);
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnDeviceEvent(DWORD dwEventType, LPVOID lpEventData)
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnDeviceEvent().");
    DWORD dwDeviceEvent = 0;

    T *pT = static_cast<T *>(this);
    return pT->OnDeviceEvent(dwDeviceEvent);
}

/// <comment>
///
/// 参考: http://msdn.microsoft.com/en-us/library/ms683241.aspx
///      http://stackoverflow.com/questions/4242776/how-to-monitor-windows-server-2003-2008-user-logins
/// </comment>

template <class T>
bool WinServiceBase<T>::FireEvent_OnSessionChange(DWORD dwEventType, LPVOID lpEventData)
{
    bool bSuccess = false;
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnSessionChange() Enter.");

    // Service Session Change
    SessionChangeDescription changeDescription;
    ::ZeroMemory((void *)&changeDescription, sizeof(changeDescription));

    sLog.info("WinServiceBase<T>::FireEvent_OnSessionChange() dwEventType = %d", dwEventType);

    DWORD dwSessionId;
    int nConnectState = 0;
    DWORD dwReason = kReasonUnknown;
    DWORD bytesReturned = 0;
    LPTSTR pBuffer = NULL;
    dwSessionId = ((WTSSESSION_NOTIFICATION *)lpEventData)->dwSessionId;
    if (dwSessionId != (DWORD)(-2)) {
        if (::WTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE,
            dwSessionId, WTSConnectState, &pBuffer, &bytesReturned)) {
            // WTSActive
            nConnectState = WTSActive;
            nConnectState = *(reinterpret_cast<int*>(pBuffer));
            sLog.info("WinServiceBase<T>::FireEvent_OnSessionChange() nConnectState = %d", nConnectState);
            changeDescription.dwReason    = dwEventType;
            changeDescription.dwSessionId = dwSessionId;
            changeDescription.lpEventData = lpEventData;
            bSuccess = true;
        }
        else {
            // this is not exceptional situation, the session may be closed at the moment, 
            // or close operation is pending on the session
            bSuccess = false;
            sLog.info("WinServiceBase<T>::FireEvent_OnSessionChange() - WTSQuerySessionInformation() error = %d", ::GetLastError());
        }

        if (pBuffer != NULL) {
            ::WTSFreeMemory(pBuffer);
        }

        DWORD dwEventsFlag = WTS_EVENT_NONE;
        // WTS_SESSION_LOGON, WTS_SESSION_LOGOFF
        // WTS_EVENT_CONNECT | WTS_EVENT_DISCONNECT | WTS_EVENT_LOGON | WTS_EVENT_LOGOFF
        if (::WTSWaitSystemEvent(WTS_CURRENT_SERVER_HANDLE, WTS_EVENT_ALL, &dwEventsFlag)) {
            if ((dwEventsFlag & WTS_EVENT_CONNECT) == WTS_EVENT_CONNECT)
                dwReason = kConsoleConnect;
            else if ((dwEventsFlag & WTS_EVENT_DISCONNECT) == WTS_EVENT_DISCONNECT)
                dwReason = kConsoleDisconnect;
            else if ((dwEventsFlag & WTS_EVENT_LOGON) == WTS_EVENT_LOGON)
                dwReason = kSessionLogon;
            else if ((dwEventsFlag & WTS_EVENT_LOGOFF) == WTS_EVENT_LOGOFF)
                dwReason = kSessionLogoff;
            else if ((dwEventsFlag & WTS_EVENT_STATECHANGE) == WTS_EVENT_STATECHANGE) {
                dwReason = kSessionRemoteControl;
            }
            sLog.info("WinServiceBase<T>::FireEvent_OnSessionChange() dwReason = %d", dwReason);
            changeDescription.dwReason = dwReason;
            bSuccess = true;
        }
        else {
            if (::GetLastError() == ERROR_OPERATION_ABORTED)
                bSuccess = false;
            sLog.info("WinServiceBase<T>::FireEvent_OnSessionChange() - WTSWaitSystemEvent() error = %d", ::GetLastError());
        }
    }
    changeDescription.dwReason = dwEventType;

    if (bSuccess) {
        T *pT = static_cast<T *>(this);
        if (pT->OnSessionChange(&changeDescription)) {
            //
        }
    }

    sLog.info("invoke WinServiceBase<T>::FireEvent_OnSessionChange() Over.");
    sLog.flush();
    return true;
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnPowerEvent(DWORD dwEventType, LPVOID lpEventData)
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnPowerEvent().");
    PowerBroadcastStatus powerStatus = { 0 };

    T *pT = static_cast<T *>(this);
    return pT->OnPowerEvent(&powerStatus);
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnPreShutdown(DWORD dwEventType, LPVOID lpEventData)
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnPreShutdown().");
    T *pT = static_cast<T *>(this);
    return pT->OnPreShutdown();
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnCustomCommand(DWORD dwControlCode)
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnCustomCommand(), dwControlCode = %d.", dwControlCode);
    T *pT = static_cast<T *>(this);
    return pT->OnCustomCommand(dwControlCode);
}

template <class T>
bool WinServiceBase<T>::FireEvent_OnUnknownCommand(DWORD dwControlCode)
{
    sLog.info("invoke WinServiceBase<T>::FireEvent_OnUnknownCommand(), dwControlCode = %d.", dwControlCode);
    T *pT = static_cast<T *>(this);
    return pT->OnUnknownCommand(dwControlCode);
}

#if 0
//
// Controls
//
#define SERVICE_CONTROL_STOP                   0x00000001
#define SERVICE_CONTROL_PAUSE                  0x00000002
#define SERVICE_CONTROL_CONTINUE               0x00000003
#define SERVICE_CONTROL_INTERROGATE            0x00000004
#define SERVICE_CONTROL_SHUTDOWN               0x00000005
#define SERVICE_CONTROL_PARAMCHANGE            0x00000006
#define SERVICE_CONTROL_NETBINDADD             0x00000007
#define SERVICE_CONTROL_NETBINDREMOVE          0x00000008
#define SERVICE_CONTROL_NETBINDENABLE          0x00000009
#define SERVICE_CONTROL_NETBINDDISABLE         0x0000000A
#define SERVICE_CONTROL_DEVICEEVENT            0x0000000B
#define SERVICE_CONTROL_HARDWAREPROFILECHANGE  0x0000000C
#define SERVICE_CONTROL_POWEREVENT             0x0000000D
#define SERVICE_CONTROL_SESSIONCHANGE          0x0000000E
#define SERVICE_CONTROL_PRESHUTDOWN            0x0000000F
#endif

template <class T>
void WinServiceBase<T>::ServiceControlHandler(DWORD dwControlCode)
{
#if 1
    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    switch (dwControlCode) {
        case SERVICE_CONTROL_STOP:
            pInstance->FireEvent_OnStop();
            break;

        case SERVICE_CONTROL_PAUSE:
            pInstance->FireEvent_OnPause();
            break;

        case SERVICE_CONTROL_CONTINUE:
            pInstance->FireEvent_OnContinue();
            break;

        case SERVICE_CONTROL_INTERROGATE:
            pInstance->FireEvent_OnInterrogate();
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            pInstance->FireEvent_OnShutdown();
            break;

        case SERVICE_CONTROL_PARAMCHANGE:
            pInstance->FireEvent_OnParamChange();
            break;

        case SERVICE_CONTROL_DEVICEEVENT:
            pInstance->FireEvent_OnDeviceEvent();
            break;

        case SERVICE_CONTROL_SESSIONCHANGE:
            sessionid = ((PWTSSESSION_NOTIFICATION)lpEventData)->dwSessionId
            pInstance->FireEvent_OnSessionChange();
            break;

        case SERVICE_CONTROL_POWEREVENT:
            pInstance->FireEvent_OnPowerEvent();
            break;

        case SERVICE_CONTROL_PRESHUTDOWN:
            pInstance->FireEvent_OnPreShutdown();
            break;

        default:
            if (dwControlCode >= 128 && dwControlCode <= 255) {
                // user defined control code
                if (pInstance->FireEvent_OnCustomCommand(dwControlCode)) {
                    //
                }
            }
            else {
                // unrecognized control code
                if (pInstance->FireEvent_OnUnknownCommand(dwControlCode)) {
                    //
                }
            }
            break;
    }

#else
    T *pInstanceImpl = static_cast<T *>(s_pServiceInstance);
    if (pInstanceImpl == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    switch (dwControlCode) {
        case SERVICE_CONTROL_STOP:
            pInstanceImpl->OnStop();
            break;

        case SERVICE_CONTROL_PAUSE:
            pInstanceImpl->OnPause();
            break;

        case SERVICE_CONTROL_CONTINUE:
            pInstanceImpl->OnContinue();
            break;

        case SERVICE_CONTROL_INTERROGATE:
            pInstanceImpl->OnInterrogate();
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            pInstanceImpl->OnShutdown();
            break;

        case SERVICE_CONTROL_PARAMCHANGE:
            pInstanceImpl->OnParamChange();
            break;

        case SERVICE_CONTROL_DEVICEEVENT:
            pInstanceImpl->OnDeviceEvent();
            break;

        case SERVICE_CONTROL_SESSIONCHANGE:
            pInstanceImpl->OnSessionChange();
            break;

        case SERVICE_CONTROL_POWEREVENT:
            pInstanceImpl->OnPowerEvent();
            break;

        case SERVICE_CONTROL_PRESHUTDOWN:
            pInstanceImpl->OnPreShutdown();
            break;

        default:
            if (dwControlCode >= 128 && dwControlCode <= 255) {
                // user defined control code
                if (pInstanceImpl->OnCustomCommand(dwControlCode)) {
                    //
                }
            }
            else {
                // unrecognized control code
                if (pInstanceImpl->OnUnknownCommand(dwControlCode)) {
                    //
                }
            }
            break;
    }
#endif
}

template <class T>
DWORD WinServiceBase<T>::ServiceControlHandlerEx(DWORD dwControlCode, DWORD dwEventType,
                                                LPVOID lpEventData, LPVOID lpContext)
{
    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    switch (dwControlCode) {
        case SERVICE_CONTROL_STOP:
            pInstance->FireEvent_OnStop();
            break;

        case SERVICE_CONTROL_PAUSE:
            pInstance->FireEvent_OnPause();
            break;

        case SERVICE_CONTROL_CONTINUE:
            pInstance->FireEvent_OnContinue();
            break;

        case SERVICE_CONTROL_INTERROGATE:
            pInstance->FireEvent_OnInterrogate();
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            pInstance->FireEvent_OnShutdown();
            break;

        case SERVICE_CONTROL_PARAMCHANGE:
            pInstance->FireEvent_OnParamChange(dwEventType, lpEventData);
            break;

        case SERVICE_CONTROL_DEVICEEVENT:
            pInstance->FireEvent_OnDeviceEvent(dwEventType, lpEventData);
            break;

        case SERVICE_CONTROL_SESSIONCHANGE:
            pInstance->FireEvent_OnSessionChange(dwEventType, lpEventData);
            break;

        case SERVICE_CONTROL_POWEREVENT:
            pInstance->FireEvent_OnPowerEvent(dwEventType, lpEventData);
            break;

        case SERVICE_CONTROL_PRESHUTDOWN:
            pInstance->FireEvent_OnPreShutdown(dwEventType, lpEventData);
            break;

        default:
            if (dwControlCode >= 128 && dwControlCode <= 255) {
                // user defined control code
                if (pInstance->FireEvent_OnCustomCommand(dwControlCode)) {
                    //
                }
            }
            else {
                // unrecognized control code
                if (pInstance->FireEvent_OnUnknownCommand(dwControlCode)) {
                    //
                }
            }
            break;
    }
    return 0;
}

template <class T>
bool WinServiceBase<T>::ServiceWorkerMethod(void *pvData)
{
    static int s_nOnServiceLoopCnt = 0;
    static int s_nServiceLoopPauseCnt = 0;
    unsigned int nSleepTime = GetSleepTime();
    unsigned int nPauseSleepTime = GetPauseSleepTime();

    sLog.info("invoke WinServiceBase<T>::ServiceWorkerMethod() Enter.");
    while (IsRunning()) {
        if (!IsPausing()) {
            if (s_nOnServiceLoopCnt < 10) {
                sLog.info("invoke WinServiceBase<T>::ServiceWorkerMethod(), cnt = %d", s_nOnServiceLoopCnt);
                s_nOnServiceLoopCnt++;
            }
            s_nServiceLoopPauseCnt = 0;
            ::Sleep(nSleepTime);
        }
        else {
            if (s_nServiceLoopPauseCnt < 10) {
                sLog.info("invoke WinServiceBase<T>::ServiceWorkerMethod(): Pause, cnt = %d, time = %d",
                    s_nServiceLoopPauseCnt, GetTickCount());
                s_nServiceLoopPauseCnt++;
            }
            ::Sleep(nPauseSleepTime);
        }
    }
    sLog.info("invoke WinServiceBase<T>::ServiceWorkerMethod() Over.");
    return true;
}

template <class T>
void WINAPI WinServiceBase<T>::ServiceWorkerLoop(int argc, TCHAR *argv[])
{
    T *pInstanceImpl = static_cast<T *>(s_pServiceInstance);
    if (pInstanceImpl == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    static int s_nServiceLoopCnt = 0;
    static int s_nServiceLoopPauseCnt = 0;

    sLog.info("WinServiceBase<T>::ServiceWorkerLoop() Enter.");

    // The worker loop of a service
    while (pInstance->IsRunning()) {
        if (!pInstance->IsPausing()) {
            if (!pInstanceImpl->ServiceWorkerMethod(NULL)) {
                pInstance->m_ServiceStatus.dwCurrentState    = SERVICE_STOPPED;
                pInstance->m_ServiceStatus.dwWin32ExitCode   = NO_ERROR;
                ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
                break;
            }
            if (s_nServiceLoopCnt < 10) {
                sLog.info("WinServiceBase<T>::ServiceWorkerLoop(): time = %d", GetTickCount());
                s_nServiceLoopCnt++;
            }
            s_nServiceLoopPauseCnt = 0;
            ::Sleep(pInstance->m_dwSleepTime);
        }
        else {
            if (s_nServiceLoopPauseCnt < 10) {
                sLog.info("WinServiceBase<T>::ServiceWorkerLoop(): Pause, cnt = %d, time = %d",
                    s_nServiceLoopPauseCnt, GetTickCount());
                s_nServiceLoopPauseCnt++;
            }
            ::Sleep(pInstance->m_dwPauseSleepTime);
        }
    }

    sLog.info("WinServiceBase<T>::ServiceWorkerLoop() Over.");
}

template <class T>
void WINAPI WinServiceBase<T>::ServiceMain(int argc, TCHAR *argv[])
{
    T *pInstanceImpl = static_cast<T *>(s_pServiceInstance);
    if (pInstanceImpl == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    DWORD dwControlAcceptedBase;
    DWORD dwControlAcceptedRemove;
    dwControlAcceptedBase  = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    dwControlAcceptedBase |= SERVICE_ACCEPT_PARAMCHANGE | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_POWEREVENT;
    dwControlAcceptedRemove = 0;

    // initialise service status
    pInstance->m_ServiceStatus.dwServiceType             = SERVICE_WIN32;
    pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
    pInstance->m_ServiceStatus.dwControlsAccepted        = dwControlAcceptedBase | dwControlAcceptedRemove;
    pInstance->m_ServiceStatus.dwWin32ExitCode           = NO_ERROR;
    pInstance->m_ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
    pInstance->m_ServiceStatus.dwCheckPoint              = 0;
    pInstance->m_ServiceStatus.dwWaitHint                = 0;

    //pInstance->m_ServiceStatusHandle = ::RegisterServiceCtrlHandler(pInstance->m_ServiceName, WinServiceBase<T>::ServiceControlHandler);
    pInstance->m_ServiceStatusHandle = ::RegisterServiceCtrlHandlerEx(pInstance->m_ServiceName, WinServiceBase<T>::ServiceControlHandlerEx, NULL);

    if (pInstance->m_ServiceStatusHandle) {
        TCHAR szPath[_MAX_PATH + 1];
        size_t nBufLen = sizeof(szPath) / sizeof(szPath[0]);
        unsigned int i, last_slash = 0;

        if (::GetModuleFileName(NULL, szPath, nBufLen) <= 0) {
            // can't get the app's exe name
            sLog.error("WinServiceBase<T>::ServiceMain: GetModuleFileName() call failed.");
            return;
        }

        size_t nLen = ::jm_tcslen(szPath);
        for (i = 0; i < nLen; ++i) {
            if (szPath[i] == _T('\\'))
                last_slash = i;
        }
        szPath[last_slash] = _T('\0');

        // set the work dir
        ::SetCurrentDirectory(szPath);

        // service is starting
        pInstance->m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        // do initialisation here

        // Initialize Service
        if (!pInstanceImpl->OnServiceInit()) {
            sLog.error("WinServiceBase<T>::ServiceMain: Initialization failed.");
            pInstance->m_ServiceStatus.dwControlsAccepted  &= ~dwControlAcceptedRemove;
            pInstance->m_ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode      = ERROR_SERVICE_SPECIFIC_ERROR;
            pInstance->m_ServiceStatus.dwServiceSpecificExitCode = ERR_SVC_INIT_SERVICE_FAILED;
            ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        // It's running, we report the running status to SCM.
        pInstance->m_ServiceStatus.dwControlsAccepted |= dwControlAcceptedRemove;
        pInstance->m_ServiceStatus.dwCurrentState      = SERVICE_RUNNING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
        argc = 1;

        // Start Service
        if (!pInstanceImpl->OnStart(argc, argv)) {
            sLog.error("WinServiceBase<T>::ServiceMain: OnStart() failed.");
            pInstance->m_ServiceStatus.dwControlsAccepted       &= ~dwControlAcceptedRemove;
            pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode           = ERROR_SERVICE_SPECIFIC_ERROR;
            pInstance->m_ServiceStatus.dwServiceSpecificExitCode = ERR_SVC_START_SERVICE_FAILED;
            ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        ////////////////////////
        // service main loop  //
        ////////////////////////

        // The worker loop of a service, or invoke WinServiceBase<T>::OnServiceLoop()
        //_tmain(argc, argv);
        WinServiceBase<T>::ServiceWorkerLoop(argc, argv);

        // service was stopped
        pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_STOP_PENDING;
        pInstance->m_ServiceStatus.dwWin32ExitCode           = NO_ERROR;
        pInstance->m_ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        ////////////////////////
        // do cleanup here    //
        ////////////////////////

        // Do Cleanup Callback
        if (!pInstanceImpl->OnServiceCleanup()) {
            sLog.error("WinServiceBase<T>::ServiceMain: OnServiceCleanup() failed.");
            pInstance->m_ServiceStatus.dwControlsAccepted       &= ~dwControlAcceptedRemove;
            pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode           = ERROR_SERVICE_SPECIFIC_ERROR;
            pInstance->m_ServiceStatus.dwServiceSpecificExitCode = ERR_SVC_CLEANUP_SERVICE_FAILED;
            ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        // service is now stopped
        pInstance->m_ServiceStatus.dwControlsAccepted       &= ~dwControlAcceptedRemove;
        pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_STOPPED;
        pInstance->m_ServiceStatus.dwWin32ExitCode           = NO_ERROR;
        pInstance->m_ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);
    }
    else {
        // Registering service failed
        sLog.error("Registering Control Handler failed (ErrCode = %d).", GetLastError());
    }
}

template <class T>
int WinServiceBase<T>::RunService()
{
    return WinServiceBase<T>::RunService(this);
}

template <class T>
int WinServiceBase<T>::RunService(WinServiceBase<T> *pServiceInstance)
{
    sLog.info("WinServiceBase<T>::RunService() Enter.");

    T *pInstance = static_cast<T *>(pServiceInstance);

    // If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.
    if (pInstance == NULL)
        pInstance = static_cast<T *>(s_pServiceInstance);
    if (pInstance == NULL) {
        sLog.error("%s%s", "WinServiceBase<T>::RunService(): ", ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    // TO_DO: Add any additional services for the process to this table.
    // 一个Service进程可以有多个线程，这是每个线程的入口表
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { pInstance->GetServiceName(), (LPSERVICE_MAIN_FUNCTION)&(WinServiceBase<T>::ServiceMain) },
        { NULL, NULL }
    };

    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.
    if (!::StartServiceCtrlDispatcher(serviceTable)) {
        sLog.error("StartService Failed. Error [%u].", ::GetLastError());
        WinServiceBase<T>::ServiceReportEvent(_T("WinServiceBase<T>::RunService(): StartServiceCtrlDispatcher() error."), ::GetLastError());
        return 0;
    }

    sLog.info("WinServiceBase<T>::RunService() Over.");
    return 1;
}

template <class T>
int WinServiceBase<T>::RunServiceEx(SERVICE_TABLE_ENTRY serviceTable[])
{
    return WinServiceBase<T>::RunServiceEx(this, serviceTable);
}

template <class T>
int WinServiceBase<T>::RunServiceEx(WinServiceBase<T> *pServiceInstance, SERVICE_TABLE_ENTRY serviceTable[])
{
    sLog.info("WinServiceBase<T>::RunServiceEx() Enter.");
    T *pInstance = static_cast<T *>(pServiceInstance);

    // If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.
    if (pInstance == NULL)
        pInstance = static_cast<T *>(s_pServiceInstance);
    if (pInstance == NULL) {
        sLog.error("%s%s", "WinServiceBase<T>::RunServiceEx(): ", ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    // TO_DO: Add any additional services for the process to this table.
    // 一个Service进程可以有多个线程，这是每个线程的入口表
    /*
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { pServiceInstance->GetServiceName(), (LPSERVICE_MAIN_FUNCTION)&(WinServiceBase<T>::ServiceMain) },
        { NULL, NULL }
    };
    //*/

    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.
    if (!::StartServiceCtrlDispatcher(serviceTable)) {
        sLog.error("StartService Failed. Error [%u].", ::GetLastError());
        WinServiceBase<T>::ServiceReportEvent(_T("WinServiceBase<T>::RunServiceEx(): StartServiceCtrlDispatcher() error."), ::GetLastError());
        return 0;
    }

    sLog.info("WinServiceBase<T>::RunServiceEx() Over.");
    return 1;
}

template <class T>
WinServiceBase<T> *WinServiceBase<T>::s_pServiceInstance = NULL;

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* JIMI_IS_WINDOWS */

#endif  /* _JIMI_SYSTEM_WIN_SERVICE_H_ */
