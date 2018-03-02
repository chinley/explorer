#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "AboutDlg.h"


//�����ڹ���
BOOL WINAPI About_Proc(
					   HWND hWnd,
					   UINT uMsg,
					   WPARAM wParam,
					   LPARAM lParam)
{
    switch(uMsg)
    {
		/*
		HANDLE_MSG(hwnd, message, fn)�Ǻ��ǩ����
		1��hwnd���账����Ϣ�Ĵ��ھ����
		2��message����Ϣ��
		3��fn��ָ������Ϣ����ĺ���ָ�롣
		*/
        HANDLE_MSG(hWnd, WM_INITDIALOG, About_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, About_OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, About_OnClose);
    }
    return FALSE;
}


//����WM_INITDIALOG��Ϣ

BOOL About_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}

//����WM_COMMAND��Ϣ
void About_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
    {
	case IDC_DIALOG_OK:
		{
			EndDialog(hwnd, 0);
		}
		break;
	default:
		break;
	}
	
    return;
}

//����WM_CLOSE��Ϣ
void About_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}