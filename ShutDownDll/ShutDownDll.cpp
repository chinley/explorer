#include "ShutDownDll.h"

BOOL EnableShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};
	hProcess = ::GetCurrentProcess(); //获取当前应用程序进程句柄
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken)) //打开当前进程的访问令牌句柄(OpenProcessToken函数调用失败返回值为零)
		return FALSE;
	if(!::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&uID)) //获取权限名称为"SeShutdownPrivilege"的LUID(LookupPrivilegeValue函数调用失败返回值为零)
		return FALSE;
	stToken_Privileges.PrivilegeCount = 1; //欲调整的权限个数
	stToken_Privileges.Privileges[0].Luid = uID; //权限的LUID
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; //权限的属性,SE_PRIVILEGE_ENABLED为使能该权限
	if(!::AdjustTokenPrivileges(hToken,FALSE,&stToken_Privileges,sizeof stToken_Privileges,NULL,NULL)) //调整访问令牌里的指定权限(AdjustTokenPrivileges函数调用失败返回值为零)
		return FALSE;
	if(::GetLastError() != ERROR_SUCCESS) //查看权限是否调整成功
		return FALSE;
	::CloseHandle(hToken);
	return TRUE;
}
//关机函数
BOOL Shutdown(BOOL bForce)
{
	EnableShutdownPrivilege(); //使能关机特权函数
	if(bForce)
		return ::ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0); //强制关机
	else
		return ::ExitWindowsEx(EWX_SHUTDOWN,0);
}
//注销函数
BOOL Logoff(BOOL bForce)
{
	if(bForce)
		return ::ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0); //强制注销
	else
		return ::ExitWindowsEx(EWX_LOGOFF,0);
}
//重启函数
BOOL Reboot(BOOL bForce)
{
	EnableShutdownPrivilege(); //使能关机特权函数
	if(bForce)
		return ::ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0); //强制重启
	else
		return ::ExitWindowsEx(EWX_REBOOT,0);
}