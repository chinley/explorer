#ifndef _SHUTDOWNDLL_H
#define _SHUTDOWNDLL_H

#include <windows.h>

#include <Windows.h>

BOOL EnableShutdownPrivilege();

//关机函数
BOOL Shutdown(BOOL bForce);
//注销函数
BOOL Logoff(BOOL bForce);
//重启函数
BOOL Reboot(BOOL bForce);


#endif