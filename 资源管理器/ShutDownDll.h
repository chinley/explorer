#ifndef _SHUTDOWNDLL_H
#define _SHUTDOWNDLL_H

#include <windows.h>

#include <Windows.h>

BOOL EnableShutdownPrivilege();

//�ػ�����
BOOL Shutdown(BOOL bForce);
//ע������
BOOL Logoff(BOOL bForce);
//��������
BOOL Reboot(BOOL bForce);


#endif