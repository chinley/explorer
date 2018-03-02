#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "ShortCutTitleDlg.h"
#include "CreateShortCutDlg.h"
#include "ExplorerFun.h"
#include "FileOperation.h"

extern TCHAR g_szAllPath[MAX_PATH];        //全路径
extern HWND g_hwndMain;

		/*
		HANDLE_MSG(hwnd, message, fn)是宏的签名。
		1）hwnd是需处理消息的窗口句柄，
		2）message是消息，
		3）fn是指向负责消息处理的函数指针。
		*/
//主窗口过程
BOOL WINAPI ShortCutTitle_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, ShortCutTitle_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, ShortCutTitle_OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, ShortCutTitle_OnClose);		
    }	
    return FALSE;
}

//处理WM_INITDIALOG消息
BOOL ShortCutTitle_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	InitShortCutTitleDlg(hwnd, lParam);
    return TRUE;
}


//处理WM_COMMAND消息
void ShortCutTitle_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
    {
	case ID_BTNLASTSTEP: //上一步
		{
			LastStepShortCut(hwnd);
		}
		break;
	case ID_BTNCOMPLETE://完成
		{
			CompleteShortCut(hwnd);
		}
		break;
	case ID_BTNCANCEL://取消
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
void ShortCutTitle_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}

//初始化快捷方式标题对话框

void InitShortCutTitleDlg(HWND hwnd, LPARAM lParam)
{
	TCHAR *pszPath = (TCHAR*)lParam;
	TCHAR *pLastSymbol = strrchr(pszPath, '\\');
	SetDlgItemText(hwnd, IDC_EDITSHORTCUTNAME, pLastSymbol+1);
	SetWindowLong(hwnd, GWL_USERDATA, (LPARAM)pszPath);
	pLastSymbol = NULL;
	pszPath = NULL;
}

//上一步

void LastStepShortCut(HWND hwnd)
{
	TCHAR *pszPath = (TCHAR*)GetWindowLong(hwnd, GWL_USERDATA);
	EndDialog(hwnd, 0);
	HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	DialogBoxParam(
		hInst,
		MAKEINTRESOURCE(IDD_DIALOGCREATESHORTCUT),
		hwnd,
		CreateShortCut_Proc,
		(LPARAM)pszPath);
}

//完成创建快捷方式

void CompleteShortCut(HWND hwnd)
{
	TCHAR *pszPath = (TCHAR*)GetWindowLong(hwnd, GWL_USERDATA);
	TCHAR szFileName[MAX_PATH];
	TCHAR szLinkName[MAX_PATH];
	ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
	ZeroMemory(szLinkName, sizeof(szLinkName) / sizeof(TCHAR));

	GetDlgItemText(hwnd, IDC_EDITSHORTCUTNAME, szFileName, sizeof(szFileName) / sizeof(TCHAR));
	wsprintf(szLinkName, "%s\\快捷方式 到%s.lnk", g_szAllPath, szFileName);

	CreateShortCut(hwnd, pszPath, szLinkName);
	InsertFileToListView(g_hwndMain, szLinkName, 0, FALSE);
	pszPath = NULL;
	EndDialog(hwnd, 0);
}