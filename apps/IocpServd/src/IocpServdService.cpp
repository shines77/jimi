
#include "IocpServdService.h"

USING_NS_IOCPSERVD;

//system::WinServiceBase<IocpServdService> *system::WinServiceBase<IocpServdService>::s_bCreateByNew = -1;
system::WinServiceBase<IocpServdService> *system::WinServiceBase<IocpServdService>::s_pServiceInstance = NULL;

NS_IOCPSERVD_BEGIN

IocpServdService::IocpServdService(void) : system::WinServiceBase<IocpServdService>()
{

}

IocpServdService::~IocpServdService(void)
{

}

NS_IOCPSERVD_END
