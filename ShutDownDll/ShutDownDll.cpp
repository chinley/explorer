#include "ShutDownDll.h"

BOOL EnableShutdownPrivilege()
{
	HANDLE hProcess = NULL;
	HANDLE hToken = NULL;
	LUID uID = {0};
	TOKEN_PRIVILEGES stToken_Privileges = {0};
	hProcess = ::GetCurrentProcess(); //��ȡ��ǰӦ�ó�����̾��
	if(!::OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES,&hToken)) //�򿪵�ǰ���̵ķ������ƾ��(OpenProcessToken��������ʧ�ܷ���ֵΪ��)
		return FALSE;
	if(!::LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&uID)) //��ȡȨ������Ϊ"SeShutdownPrivilege"��LUID(LookupPrivilegeValue��������ʧ�ܷ���ֵΪ��)
		return FALSE;
	stToken_Privileges.PrivilegeCount = 1; //��������Ȩ�޸���
	stToken_Privileges.Privileges[0].Luid = uID; //Ȩ�޵�LUID
	stToken_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; //Ȩ�޵�����,SE_PRIVILEGE_ENABLEDΪʹ�ܸ�Ȩ��
	if(!::AdjustTokenPrivileges(hToken,FALSE,&stToken_Privileges,sizeof stToken_Privileges,NULL,NULL)) //���������������ָ��Ȩ��(AdjustTokenPrivileges��������ʧ�ܷ���ֵΪ��)
		return FALSE;
	if(::GetLastError() != ERROR_SUCCESS) //�鿴Ȩ���Ƿ�����ɹ�
		return FALSE;
	::CloseHandle(hToken);
	return TRUE;
}
//�ػ�����
BOOL Shutdown(BOOL bForce)
{
	EnableShutdownPrivilege(); //ʹ�ܹػ���Ȩ����
	if(bForce)
		return ::ExitWindowsEx(EWX_SHUTDOWN|EWX_FORCE,0); //ǿ�ƹػ�
	else
		return ::ExitWindowsEx(EWX_SHUTDOWN,0);
}
//ע������
BOOL Logoff(BOOL bForce)
{
	if(bForce)
		return ::ExitWindowsEx(EWX_LOGOFF|EWX_FORCE,0); //ǿ��ע��
	else
		return ::ExitWindowsEx(EWX_LOGOFF,0);
}
//��������
BOOL Reboot(BOOL bForce)
{
	EnableShutdownPrivilege(); //ʹ�ܹػ���Ȩ����
	if(bForce)
		return ::ExitWindowsEx(EWX_REBOOT|EWX_FORCE,0); //ǿ������
	else
		return ::ExitWindowsEx(EWX_REBOOT,0);
}