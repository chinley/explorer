#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "AboutDlg.h"


//主窗口过程
BOOL WINAPI About_Proc(
					   HWND hWnd,
					   UINT uMsg,
					   WPARAM wParam,
					   LPARAM lParam)
{
    switch(uMsg)
    {
		/*
		HANDLE_MSG(hwnd, message, fn)是宏的签名。
		1）hwnd是需处理消息的窗口句柄，
		2）message是消息，
		3）fn是指向负责消息处理的函数指针。
		*/
        HANDLE_MSG(hWnd, WM_INITDIALOG, About_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, About_OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, About_OnClose);
    }
    return FALSE;
}


//处理WM_INITDIALOG消息

BOOL About_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    return TRUE;
}

//处理WM_COMMAND消息
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

//处理WM_CLOSE消息
void About_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}