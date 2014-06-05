
#include <jimi/platform/jimi_platform_config.h>

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <tchar.h>
#include <string>
#include <cstring>
/* tchar.h 必须放在strsafe.h前面, stl的string等声明最好也放在strsafe.h前面 */
#include <strsafe.h>
#include <string.h>

#include <jimi/system/CWinService.h>
#include <jimi/log/log.h>

#pragma comment(lib, "Advapi32.lib")

using namespace std;

USING_NS_JIMI;
USING_NS_JIMI_LOG;
//USING_NS_JIMI_IOCP;

#ifndef _ATL_QUOTES_SPACE
#define _ATL_QUOTES_SPACE   2
#endif

extern int _tmain(int argc, TCHAR *argv[]);

extern TCHAR g_ServiceName[];
extern TCHAR g_ServiceDisplayName[];
extern TCHAR g_ServiceDescription[];

extern int g_nServiceStatus;

//SERVICE_STATUS gServiceStatus = {0};

//SERVICE_STATUS_HANDLE gServiceStatusHandle = NULL;

typedef WINADVAPI BOOL (WINAPI *CSD_T)(SC_HANDLE, DWORD, LPCVOID);

NS_JIMI_BEGIN

NS_JIMI_SYSTEM_BEGIN

CWinService *CWinService::s_pServiceInstance = NULL;

CWinService::CWinService( void )
: m_nServiceStatus(SVC_STATUS_UNKNOWN)
, m_nSleepTime(SERVICE_SLEEP_TIME)
, m_ServiceStatusHandle(NULL)
{
    ::ZeroMemory(&m_ServiceStatus, sizeof(m_ServiceStatus));
}

CWinService::~CWinService()
{

}

bool CWinService::InstallService()
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
    size_t dwLen = _tcslen(szPath + 1);
    szPath[0] = _T('\"');
    szPath[dwLen + 1] = _T('\"');
    szPath[dwLen + 2] = _T('\0');
#endif

    ::strcat_s(szPath, _countof(szPath), " -run");

    // Get a handle to the SCM database.
    SC_HANDLE schSCManager = ::OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager) {
        int LastError = GetLastError();
        sLog.error("OpenSCManager failed (%d).", LastError);
        sLog.error("SERVICE: No access to service control manager.");
        if (LastError == 5) {
            sLog.error("please, try run with admin rights.");
        }
        return false;
    }

    // Create the service
    SC_HANDLE schService = ::CreateService(
        schSCManager,                   // SCM database
        g_ServiceName,                  // name of service
        g_ServiceDisplayName,           // service name to display
        SERVICE_ALL_ACCESS,             // desired access
        SERVICE_WIN32_OWN_PROCESS,      // service type
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
    sdBuf.lpDescription = g_ServiceDescription;
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

bool CWinService::UninstallService()
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
        g_ServiceName,                          // name of service
        SERVICE_QUERY_STATUS | DELETE);         // desired access

    if (!schService) {
        ::CloseServiceHandle(schSCManager);
        sLog.error("SERVICE: Service not found: %s", g_ServiceName);
        return false;
    }

    SERVICE_STATUS serviceStatus2;
    if (::QueryServiceStatus(schService, &serviceStatus2)) {
        if (serviceStatus2.dwCurrentState == SERVICE_STOPPED)
            ::DeleteService(schService);
    }

    ::CloseServiceHandle(schService);
    ::CloseServiceHandle(schSCManager);
    return true;
}

void CWinService::ServiceReportEvent(LPTSTR szFunction, int nEventId)
{
    int nLastErr = GetLastError();

    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR buffer[128];

    hEventSource = ::RegisterEventSource(NULL, g_ServiceName);

    if (NULL != hEventSource) {
        ::StringCchPrintf(buffer, 128, _T("%s failed with %d"), szFunction, nLastErr);

        lpszStrings[0] = g_ServiceName;
        lpszStrings[1] = buffer;

        ::ReportEvent(hEventSource,        // event log handle
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

bool CWinService::OnInitService()
{
    sLog.info("invoke CWinService::OnInitService()");
    return true;
}

bool CWinService::OnStart()
{
    sLog.info("invoke CWinService::OnStart()");
    return true;
}

bool CWinService::OnStop()
{
    sLog.info("invoke CWinService::OnStop()");
    return true;
}

bool CWinService::OnPause()
{
    sLog.info("invoke CWinService::OnPause()");
    return true;
}

bool CWinService::OnContinue()
{
    sLog.info("invoke CWinService::OnContinue()");
    return true;
}

bool CWinService::OnServiceLoop()
{
    static int s_nOnServiceLoopCnt = 0;
    if (s_nOnServiceLoopCnt < 10) {
        sLog.info("invoke CWinService::OnServiceLoop()");
        s_nOnServiceLoopCnt++;
    }
    return true;
}

CWinService *CWinService::GetInstance()
{
    return s_pServiceInstance;
}

void CWinService::SetInstance(CWinService *pServiceInstance)
{
    s_pServiceInstance = pServiceInstance;
}

void CWinService::ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    sLog.info("invoke CWinService::ReportServiceStatus() enter.");

    CWinService *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.
    pInstance->m_ServiceStatus.dwCurrentState   = dwCurrentState;
    pInstance->m_ServiceStatus.dwWin32ExitCode  = dwWin32ExitCode;
    pInstance->m_ServiceStatus.dwWaitHint       = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING) {
        pInstance->m_ServiceStatus.dwControlsAccepted = 0;
    }
    else {
        pInstance->m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    }

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED)) {
        pInstance->m_ServiceStatus.dwCheckPoint = 0;
    }
    else {
        pInstance->m_ServiceStatus.dwCheckPoint = dwCheckPoint++;
    }

    // Report the status of the service to the SCM.
    ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

    sLog.info("invoke CWinService::ReportServiceStatus() over.");
}

void CWinService::ServiceControlHandler(DWORD dwControlCode)
{
    CWinService *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    switch (dwControlCode) {
        case SERVICE_CONTROL_INTERROGATE:
            break;

        case SERVICE_CONTROL_SHUTDOWN:
        case SERVICE_CONTROL_STOP:
            // Signal that service try stop
            pInstance->ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_STOP_PENDING;

            // Signal the service to stop.
            if (pInstance->OnStop()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
                pInstance->m_nServiceStatus = SVC_STATUS_STOPPED;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            break;

        case SERVICE_CONTROL_PAUSE:
            // Signal that service try pause
            pInstance->ReportServiceStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_PAUSE_PENDING;

            // Signal the service to pause
            if (pInstance->OnPause()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_PAUSED;
                pInstance->m_nServiceStatus = SVC_STATUS_PAUSED;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            break;

        case SERVICE_CONTROL_CONTINUE:
            // Signal that service try continue(resume)
            pInstance->ReportServiceStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0);
            pInstance->m_nServiceStatus = SVC_STATUS_CONTINUE_PENDING;

            // Signal the service to continue(resume)
            if (pInstance->OnContinue()) {
                pInstance->m_ServiceStatus.dwWin32ExitCode = 0;
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_RUNNING;
                pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
            }
            else {
                pInstance->m_ServiceStatus.dwCurrentState  = SERVICE_PAUSED;
                pInstance->m_nServiceStatus = SVC_STATUS_PAUSED;
            }
            break;

        default:
            if (dwControlCode >= 128 && dwControlCode <= 255) {
                // user defined control code
                break;
            }
            else {
                // unrecognized control code
                break;
            }
    }

    pInstance->ReportServiceStatus(pInstance->m_ServiceStatus.dwCurrentState, NO_ERROR, 0);
    //SetServiceStatus(pInstance->gServiceStatusHandle, &pInstance->gServiceStatus);
}

void WINAPI CWinService::ServiceLoop(DWORD argc, TCHAR *argv[])
{
    CWinService *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    static int s_nServiceLoopCnt = 0;
    // The worker loop of a service
    while (pInstance->m_ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        if (!pInstance->OnServiceLoop()) {
            pInstance->m_ServiceStatus.dwCurrentState    = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode   = -1;
            ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        if (s_nServiceLoopCnt < 10) {
            sLog.info("CWinService::ServiceLoop(): time = %d", GetTickCount());;
            s_nServiceLoopCnt++;
        }
        ::Sleep(pInstance->m_nSleepTime);
    }
}

void WINAPI CWinService::ServiceMain(DWORD argc, TCHAR *argv[])
{
    CWinService *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error(ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    // initialise service status
    pInstance->m_ServiceStatus.dwServiceType             = SERVICE_WIN32;
    pInstance->m_ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
    pInstance->m_ServiceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;      // SERVICE_ACCEPT_SHUTDOWN
    pInstance->m_ServiceStatus.dwWin32ExitCode           = NO_ERROR;
    pInstance->m_ServiceStatus.dwServiceSpecificExitCode = NO_ERROR;
    pInstance->m_ServiceStatus.dwCheckPoint              = 0;
    pInstance->m_ServiceStatus.dwWaitHint                = 0;

    pInstance->m_ServiceStatusHandle = ::RegisterServiceCtrlHandler(g_ServiceName, CWinService::ServiceControlHandler);

    if (pInstance->m_ServiceStatusHandle) {
        TCHAR szPath[_MAX_PATH + 1];
        size_t nBufLen = sizeof(szPath) / sizeof(szPath[0]);
        unsigned int i, last_slash = 0;

        if (::GetModuleFileName(NULL, szPath, nBufLen) <= 0) {
            // can't get the app's exe name
            sLog.error("CWinService::ServiceMain: GetModuleFileName() call failed.");
            return;
        }

        size_t nLen = std::_tcslen(szPath);
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
            sLog.error("CWinService::ServiceMain: Initialization failed.");
            pInstance->m_ServiceStatus.dwCurrentState    = SERVICE_STOPPED;
            pInstance->m_ServiceStatus.dwWin32ExitCode   = -1;
            SetServiceStatus(pInstance->m_ServiceStatusHandle, &(pInstance->m_ServiceStatus));
            return;
        }

        // It's running, we report the running status to SCM.
        pInstance->m_ServiceStatus.dwControlsAccepted |= (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        pInstance->m_ServiceStatus.dwCurrentState      = SERVICE_RUNNING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        ////////////////////////
        // service main cycle //
        ////////////////////////

        pInstance->m_nServiceStatus = SVC_STATUS_RUNNING;
        argc = 1;

        // The worker loop of a service, or invoke CWinService::OnServiceLoop()
        //_tmain(argc, argv);
        CWinService::ServiceLoop(argc, argv);

        // service was stopped
        pInstance->m_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);

        ////////////////////////
        // do cleanup here
        ////////////////////////

        // do cleanup

        // service is now stopped
        pInstance->m_ServiceStatus.dwControlsAccepted  &= ~(SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN);
        pInstance->m_ServiceStatus.dwCurrentState       = SERVICE_STOPPED;
        ::SetServiceStatus(pInstance->m_ServiceStatusHandle, &pInstance->m_ServiceStatus);
    }
    else {
        // Registering service failed
        sLog.error("Registering Control Handler failed (ErrCode = %d).", GetLastError());
    }
}

int CWinService::RunService(int argc, TCHAR *argv[])
{
    // If command-line parameter is "install", install the service.
    // Otherwise, the service is probably being started by the SCM.

    CWinService *pInstance = s_pServiceInstance;
    if (pInstance == NULL) {
        sLog.error("%s%s", "CWinService::RunService(): ", ERROR_SERVICE_INSTANCE_NOT_INITED);
        throw ERROR_SERVICE_INSTANCE_NOT_INITED;
    }

    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY serviceTable[] = {
        { g_ServiceName, (LPSERVICE_MAIN_FUNCTION)CWinService::ServiceMain },
        { 0, 0 }
    };

    // This call returns when the service has stopped.
    // The process should simply terminate when the call returns.
    if (!::StartServiceCtrlDispatcher(serviceTable)) {
        sLog.error("StartService Failed. Error [%u].", ::GetLastError());
        CWinService::ServiceReportEvent(_T("CWinService::RunService(): StartServiceCtrlDispatcher() error."), ::GetLastError());
        return 0;
    }

    sLog.info("StartService Succeed.");
    return 1;
}

NS_JIMI_SYSTEM_END

NS_JIMI_END

#endif  /* JIMI_IS_WINDOWS */
