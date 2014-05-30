
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

#include <tchar.h>
#include <string>
#include <cstring>
/* tchar.h 必须放在strsafe.h前面, stl的string等声明最好也放在strsafe.h前面 */
#include <strsafe.h>
#include <string.h>

#include <jimi/log/log_all.h>
//#include <jimi/iocp/IocpServd.h>

#include <jimic/string/jm_strings.h>

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

#define SERVICE_SLEEP_TIME                  5000
#define SERVICE_PAUSE_SLEEP_TIME            100

#define ERROR_SERVICE_INSTANCE_NOT_INITED   "Service Instance is not initialized."

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

typedef WINADVAPI BOOL (WINAPI *CSD_T)(SC_HANDLE, DWORD, LPCVOID);

//
// Service State -- for CurrentState
//
typedef enum eServiceStatus {
    SVC_STATUS_NOTINSERVICE = -1,
    SVC_STATUS_UNKNOWN = 0,

    SVC_STATUS_STOPPED          = SERVICE_STOPPED,
    SVC_STATUS_START_PENDING    = SERVICE_START_PENDING,
    SVC_STATUS_STOP_PENDING     = SERVICE_STOP_PENDING,
    SVC_STATUS_RUNNING          = SERVICE_RUNNING,
    SVC_STATUS_CONTINUE_PENDING = SERVICE_CONTINUE_PENDING,
    SVC_STATUS_PAUSE_PENDING    = SERVICE_PAUSE_PENDING,
    SVC_STATUS_PAUSED           = SERVICE_PAUSED,
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

//
// 参考: MangOS 源码 以及
//
//       Writing Tools for Windows in C++
//       http://sprogram.com.ua/en/articles/how-write-service-for-windows-with-cpp

class JIMI_DLL IWinServiceBase
{
    virtual bool OnInitService() = 0;

    virtual bool OnStartService(int argc, TCHAR *argv[]) = 0;
    virtual bool OnStopService() = 0;

    virtual bool OnPauseService() = 0;
    virtual bool OnContinueService() = 0;

    virtual bool OnShutdownService() = 0;

    virtual bool OnCustomCommand(DWORD dwControlCode) = 0;
    virtual bool OnUnknownCommand(DWORD dwControlCode) = 0;

    virtual bool ServiceWorkerMethod(void *pvData) = 0;
};

template <class T>
class JIMI_DLL WinServiceBase
{
public:
    WinServiceBase(void);
    WinServiceBase(WinServiceBase *pInstance, bool bCreateByNew = true);
    ~WinServiceBase(void);

    void Release();

    bool IsRunning();
    bool IsPausing();

    bool InstallService();
    bool UninstallService();

    bool OnInitService();

    bool OnStartService(int argc, TCHAR *argv[]);
    bool OnStopService();

    bool OnPauseService();
    bool OnContinueService();

    bool OnServiceInterrogate();

    bool OnShutdownService();

    bool OnServiceParamChange();
    bool OnServiceDeviceEvent();
    bool OnServiceSessionChange();
    bool OnServicePowerEvent();
    bool OnServicePreShutdown();

    bool OnCustomCommand(DWORD dwControlCode);
    bool OnUnknownCommand(DWORD dwControlCode);

    bool ServiceWorkerMethod(void *pvData);

    bool SetCreateByNew(bool bCreateByNew);

    TCHAR *GetServiceName()         { return m_ServiceName;        }
    TCHAR *GetServiceDisplayName()  { return m_ServiceDisplayName; }
    TCHAR *GetServiceDescription()  { return m_ServiceDescription; }

    TCHAR *SetServiceName(TCHAR *szServiceName);
    TCHAR *SetServiceDisplayName(TCHAR *szServiceDisplayName);
    TCHAR *SetServiceDescription(TCHAR *szServiceDescription);

    unsigned int GetSleepTime() { return m_nSleepTime; }
    void SetSleepTime(unsigned int nMillsecs) { m_nSleepTime = nMillsecs; }

    unsigned int GetPauseSleepTime() { return m_nPauseSleepTime; }
    void SetPauseSleepTime(unsigned int nMillsecs) { m_nPauseSleepTime = nMillsecs; }

    DWORD GetCurrentState() { return m_ServiceStatus.dwCurrentState; }

    int RunService();
    static int RunService(WinServiceBase *pInstance);

    static WinServiceBase *GetInstance();
    static void SetInstance(WinServiceBase *pInstance, bool bCreateByNew);

    static void ServiceReportEvent(LPTSTR szFunction, int nEventId);
    static void WINAPI ServiceControlHandler(DWORD dwControlCode);
    static void WINAPI ServiceMain(int argc, TCHAR *argv[]);
    static void WINAPI ServiceWorkerLoop(int argc, TCHAR *argv[]);

    void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);

    static WinServiceBase  *s_pServiceInstance;

private:
    bool                    m_bCreateByNew;
    unsigned int            m_nSleepTime;
    unsigned int            m_nPauseSleepTime;
    int                     m_nServiceStatus;

    SERVICE_STATUS          m_ServiceStatus;
    SERVICE_STATUS_HANDLE   m_ServiceStatusHandle;

    TCHAR                   m_ServiceName[32];
    TCHAR                   m_ServiceDisplayName[128];
    TCHAR                   m_ServiceDescription[512];
};

template <class T>
WinServiceBase<T>::WinServiceBase(void)
: m_bCreateByNew(false)
, m_nSleepTime(SERVICE_SLEEP_TIME)
, m_nPauseSleepTime(SERVICE_PAUSE_SLEEP_TIME)
, m_nServiceStatus(SVC_STATUS_UNKNOWN)
, m_ServiceStatusHandle(NULL)
{
    ::ZeroMemory(&m_ServiceName, sizeof(m_ServiceName));
    ::ZeroMemory(&m_ServiceDisplayName, sizeof(m_ServiceDisplayName));
    ::ZeroMemory(&m_ServiceDescription, sizeof(m_ServiceDescription));
    ::ZeroMemory(&m_ServiceStatus, sizeof(m_ServiceStatus));
}

template <class T>
WinServiceBase<T>::~WinServiceBase(void)
{
    // Do nothing!!
    Release();
}

template <class T>
void WinServiceBase<T>::Release()
{
    sLog.info("WinServiceBase<T> Release(), error = %d.", GetLastError());
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
        SERVICE_QUERY_STATUS | DELETE);         // desired access

    if (!schService) {
        ::CloseServiceHandle(schSCManager);
        sLog.error("SERVICE: Service not found: %s", m_ServiceName);
        return false;
    }

    SERVICE_STATUS serviceStatus2;
    if (::QueryServiceStatus(schService, &serviceStatus2)) {
        if (serviceStatus2.dwCurrentState == SERVICE_STOPPED) {
            if (::DeleteService(schService))
                sLog.info("Service uninstalled successfully.");
        }
    }

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
}

template <class T>
bool WinServiceBase<T>::OnInitService()
{
    sLog.info("invoke WinServiceBase<T>::OnInitService()");
    T *pT = static_cast<T *>(this);
    return pT->OnInitService();
}

template <class T>
bool WinServiceBase<T>::OnStartService(int argc, TCHAR *argv[])
{
    sLog.info("invoke WinServiceBase<T>::OnStartService()");
    T *pT = static_cast<T *>(this);
    return pT->OnStartService(argc, argv);
}

template <class T>
bool WinServiceBase<T>::OnStopService()
{
    sLog.info("invoke WinServiceBase<T>::OnStopService()");
    T *pT = static_cast<T *>(this);
    return pT->OnStopService();
}

template <class T>
bool WinServiceBase<T>::OnPauseService()
{
    sLog.info("invoke WinServiceBase<T>::OnPauseService()");
    T *pT = static_cast<T *>(this);
    return pT->OnPauseService();
}

template <class T>
bool WinServiceBase<T>::OnContinueService()
{
    sLog.info("invoke WinServiceBase<T>::OnContinueService()");
    T *pT = static_cast<T *>(this);
    return pT->OnContinueService();
}

template <class T>
bool WinServiceBase<T>::OnServiceInterrogate()
{
    sLog.info("invoke WinServiceBase<T>::OnServiceInterrogate().");
    T *pT = static_cast<T *>(this);
    return pT->OnServiceInterrogate();
}

template <class T>
bool WinServiceBase<T>::OnShutdownService()
{
    sLog.info("invoke WinServiceBase<T>::OnShutdownService()");
    T *pT = static_cast<T *>(this);
    return pT->OnShutdownService();
}

template <class T>
bool WinServiceBase<T>::OnServiceParamChange()
{
    sLog.info("invoke WinServiceBase<T>::OnServiceParamChange().");
    T *pT = static_cast<T *>(this);
    return pT->OnServiceParamChange();
}

template <class T>
bool WinServiceBase<T>::OnServiceDeviceEvent()
{
    sLog.info("invoke WinServiceBase<T>::OnServiceDeviceEvent().");
    T *pT = static_cast<T *>(this);
    return pT->OnServiceDeviceEvent();
}

template <class T>
bool WinServiceBase<T>::OnServiceSessionChange()
{
    sLog.info("invoke WinServiceBase<T>::OnServiceSessionChange().");
    T *pT = static_cast<T *>(this);
    return pT->OnServiceSessionChange();
}

template <class T>
bool WinServiceBase<T>::OnServicePowerEvent()
{
    sLog.info("invoke WinServiceBase<T>::OnServicePowerEvent().");
    T *pT = static_cast<T *>(this);
    return pT->OnServicePowerEvent();
}

template <class T>
bool WinServiceBase<T>::OnServicePreShutdown()
{
    sLog.info("invoke WinServiceBase<T>::OnServicePreShutdown().");
    T *pT = static_cast<T *>(this);
    return pT->OnServicePreShutdown();
}

template <class T>
bool WinServiceBase<T>::OnCustomCommand(DWORD dwControlCode)
{
    sLog.info("invoke WinServiceBase<T>::OnCustomCommand(), dwControlCode = %d.", dwControlCode);
    T *pT = static_cast<T *>(this);
    return pT->OnCustomCommand(dwControlCode);
}

template <class T>
bool WinServiceBase<T>::OnUnknownCommand(DWORD dwControlCode)
{
    sLog.info("invoke WinServiceBase<T>::OnUnknownCommand(), dwControlCode = %d.", dwControlCode);
    T *pT = static_cast<T *>(this);
    return pT->OnUnknownCommand(dwControlCode);
}

template <class T>
bool WinServiceBase<T>::ServiceWorkerMethod(void *pvData)
{
    static int s_nOnServiceLoopCnt = 0;
    if (s_nOnServiceLoopCnt < 10) {
        sLog.info("invoke WinServiceBase<T>::ServiceWorkerMethod(), cnt = %d", s_nOnServiceLoopCnt);
        s_nOnServiceLoopCnt++;
    }
    T *pT = static_cast<T *>(this);
    return pT->ServiceWorkerMethod(pvData);
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
void WinServiceBase<T>::ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    sLog.info("invoke WinServiceBase<T>::ReportServiceStatus() enter.");

    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.
    pInstance->m_ServiceStatus.dwCurrentState   = dwCurrentState;
    pInstance->m_ServiceStatus.dwWin32ExitCode  = dwWin32ExitCode;
    pInstance->m_ServiceStatus.dwWaitHint       = dwWaitHint;

    pInstance->m_ServiceStatus.dwControlsAccepted |= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
    /*
    if (dwCurrentState == SERVICE_START_PENDING) {
        pInstance->m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }
    else {
        pInstance->m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
    }
    //*/

    pInstance->m_ServiceStatus.dwCheckPoint = dwCheckPoint++;
    /*
    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) {
        pInstance->m_ServiceStatus.dwCheckPoint = 0;
    }
    else {
        pInstance->m_ServiceStatus.dwCheckPoint = dwCheckPoint++;
    }
    //*/

    sLog.info("ReportServiceStatus(): dwCurrentState = %s (%d)", s_szServiceStatusString[dwCurrentState], dwCurrentState);

    // Report the status of the service to the SCM.
    ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

    sLog.info("invoke WinServiceBase<T>::ReportServiceStatus() over.");
}

/*
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
//*/

template <class T>
void WinServiceBase<T>::ServiceControlHandler(DWORD dwControlCode)
{
    bool bHandleCommand = false;
    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    switch (dwControlCode) {
        case SERVICE_CONTROL_STOP:
            // Signal that service try stop
            pInstance->ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_STOP_PENDING;

            // Signal the service to stop.
            if (pInstance->OnStopService()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
                pInstance->m_nServiceStatus = SVC_STATUS_STOPPED;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_PAUSE:
            // Signal that service try pause
            pInstance->ReportServiceStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_PAUSE_PENDING;

            // Signal the service to pause
            if (pInstance->OnPauseService()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_PAUSED;
                pInstance->m_nServiceStatus = SVC_STATUS_PAUSED;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_CONTINUE:
            // Signal that service try continue(resume)
            pInstance->ReportServiceStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_CONTINUE_PENDING;

            // Signal the service to continue(resume)
            if (pInstance->OnContinueService()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_PAUSED;
                pInstance->m_nServiceStatus = SVC_STATUS_PAUSED;
            }
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_INTERROGATE:
            pInstance->OnServiceInterrogate();
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            // Signal that service try stop
            pInstance->ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_STOP_PENDING;

            // Signal the service to shutdown.
            if (pInstance->OnShutdownService()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
                pInstance->m_nServiceStatus = SVC_STATUS_STOPPED;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_PARAMCHANGE:
            pInstance->OnServiceParamChange();
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_DEVICEEVENT:
            pInstance->OnServiceDeviceEvent();
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_SESSIONCHANGE:
            pInstance->OnServiceSessionChange();
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_POWEREVENT:
            pInstance->OnServicePowerEvent();
            bHandleCommand = true;
            break;

        case SERVICE_CONTROL_PRESHUTDOWN:
            pInstance->OnServicePreShutdown();
            bHandleCommand = true;
            break;

        default:
            if (dwControlCode >= 128 && dwControlCode <= 255) {
                // user defined control code
                if (pInstance->OnCustomCommand(dwControlCode)) {
                    //
                    bHandleCommand = true;
                }
            }
            else {
                // unrecognized control code
                if (pInstance->OnUnknownCommand(dwControlCode)) {
                    //
                    bHandleCommand = true;
                }
            }
            break;
    }

    if (bHandleCommand)
        pInstance->ReportServiceStatus(pInstance->m_ServiceStatus.dwCurrentState, NO_ERROR, 0);
}

template <class T>
void WINAPI WinServiceBase<T>::ServiceWorkerLoop(int argc, TCHAR *argv[])
{
    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    static int s_nServiceLoopCnt = 0;
    static int s_nServiceLoopPauseCnt = 0;

    // The worker loop of a service
    while (pInstance->IsRunning()) {
        if (!pInstance->IsPausing()) {
            if (!pInstance->ServiceWorkerMethod(NULL)) {
                pInstance->m_ServiceStatus.dwCurrentState    = SERVICE_STOPPED;
                pInstance->m_ServiceStatus.dwWin32ExitCode   = 0;
                ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
                break;
            }
            if (s_nServiceLoopCnt < 10) {
                sLog.info("WinServiceBase<T>::ServiceWorkerLoop(): time = %d", GetTickCount());
                s_nServiceLoopCnt++;
            }
            s_nServiceLoopPauseCnt = 0;
            ::Sleep(pInstance->m_nSleepTime);
        }
        else {
            if (s_nServiceLoopPauseCnt < 10) {
                sLog.info("WinServiceBase<T>::ServiceWorkerLoop(): Pause, cnt = %d, time = %d",
                    s_nServiceLoopPauseCnt, GetTickCount());
                s_nServiceLoopPauseCnt++;
            }
            ::Sleep(pInstance->m_nPauseSleepTime);
        }
    }
}

template <class T>
void WINAPI WinServiceBase<T>::ServiceMain(int argc, TCHAR *argv[])
{
    WinServiceBase<T> *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    DWORD dwControlAcceptedBase;
    DWORD dwControlAcceptedRemove;
    dwControlAcceptedBase  = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
    dwControlAcceptedBase |= SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_POWEREVENT;
    dwControlAcceptedRemove = 0;

    // initialise service status
    pInstance->m_ServiceStatus.dwServiceType             = SERVICE_WIN32;
    pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
    pInstance->m_ServiceStatus.dwControlsAccepted        = dwControlAcceptedBase | dwControlAcceptedRemove;
    pInstance->m_ServiceStatus.dwWin32ExitCode           = NO_ERROR;
    pInstance->m_ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
    pInstance->m_ServiceStatus.dwCheckPoint              = 0;
    pInstance->m_ServiceStatus.dwWaitHint                = 0;

    pInstance->m_ServiceStatusHandle = ::RegisterServiceCtrlHandler(pInstance->m_ServiceName, WinServiceBase<T>::ServiceControlHandler);

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

        // service is starting
        pInstance->m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        // do initialisation here
        ::SetCurrentDirectory(szPath);

        // Initialize Service
        if (!pInstance->OnInitService()) {
            sLog.error("WinServiceBase<T>::ServiceMain: Initialization failed.");
            pInstance->m_ServiceStatus.dwControlsAccepted  &= ~dwControlAcceptedRemove;
            pInstance->m_ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode      = -1;
            SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        // It's running, we report the running status to SCM.
        pInstance->m_ServiceStatus.dwControlsAccepted |= dwControlAcceptedRemove;
        pInstance->m_ServiceStatus.dwCurrentState      = SERVICE_RUNNING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        ////////////////////////
        // service main cycle //
        ////////////////////////

        pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
        argc = 1;

        if (!pInstance->OnStartService(argc, argv)) {
            sLog.error("WinServiceBase<T>::ServiceMain: Initialization failed.");
            pInstance->m_ServiceStatus.dwControlsAccepted  &= ~dwControlAcceptedRemove;
            pInstance->m_ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode      = -1;
            SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        // The worker loop of a service, or invoke WinServiceBase<T>::OnServiceLoop()
        //_tmain(argc, argv);
        WinServiceBase<T>::ServiceWorkerLoop(argc, argv);

        // service was stopped
        pInstance->m_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        ////////////////////////
        // do cleanup here
        ////////////////////////

        // do cleanup

        // service is now stopped
        pInstance->m_ServiceStatus.dwControlsAccepted  &= ~dwControlAcceptedRemove;
        pInstance->m_ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
        pInstance->m_ServiceStatus.dwWin32ExitCode      = 0;
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
    sLog.info("RunService() Enter.");

    // If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.
    if (pServiceInstance == NULL)
        pServiceInstance = s_pServiceInstance;
    if (pServiceInstance == NULL) {
        sLog.error("%s%s", "WinServiceBase<T>::RunService(): ", ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { pServiceInstance->GetServiceName(), (LPSERVICE_MAIN_FUNCTION)&WinServiceBase<T>::ServiceMain },
        { 0, 0 }
    };

    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.
    if (!::StartServiceCtrlDispatcher(serviceTable)) {
        sLog.error("StartService Failed. Error [%u].", ::GetLastError());
        WinServiceBase<T>::ServiceReportEvent(_T("WinServiceBase<T>::RunService(): StartServiceCtrlDispatcher() error."), ::GetLastError());
        return 0;
    }

    sLog.info("RunService() Over.");
    return 1;
}

template <class T>
WinServiceBase<T> *WinServiceBase<T>::s_pServiceInstance = NULL;

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* JIMI_IS_WINDOWS */

#endif  /* _JIMI_SYSTEM_WIN_SERVICE_H_ */
