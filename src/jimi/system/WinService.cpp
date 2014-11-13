
#include "jimi/system/WinService.h"

#if JIMI_IS_WINDOWS

#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <tchar.h>
#include <string>
#include <cstring>
/* tchar.h 必须放在strsafe.h前面, stl的string等声明最好也放在strsafe.h前面 */
#pragma warning(disable : 4995)         // 禁止不带_s函数的废弃warning信息
#pragma warning(disable : 4996)
#include <strsafe.h>
#pragma warning(default : 4995)         // 恢复默认的warning
#pragma warning(default : 4996)
#include <string.h>

#include "jimi/log/log.h"

//#pragma comment(lib, "Advapi32.lib")
//#pragma comment(lib, "Wtsapi32.lib")

#ifndef _ATL_QUOTES_SPACE
#define _ATL_QUOTES_SPACE   2
#endif

#ifdef _UNICODE
extern int _tmain(int argc, TCHAR *argv[]);
#else
extern int   main(int argc, char *argv[]);
#endif

extern TCHAR g_ServiceName[];
extern TCHAR g_ServiceDisplayName[];
extern TCHAR g_ServiceDescription[];

extern int g_nServiceStatus;

//SERVICE_STATUS gServiceStatus = {0};

//SERVICE_STATUS_HANDLE gServiceStatusHandle = NULL;

/*
template <class T>
WinServiceBase<T> *WinServiceBase<T>::s_pServiceInstance = NULL;
//*/

NS_JIMI_BEGIN

//

NS_JIMI_END

#endif  /* JIMI_IS_WINDOWS */
