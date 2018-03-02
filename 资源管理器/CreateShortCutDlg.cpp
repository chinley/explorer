#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>
#include <Shlobj.h>
#include <io.h>
#include "resource.h"
#include "CreateShortCutDlg.h"
#include "ShortCutTitleDlg.h"

#define BIF_NEWDIALOGSTYLE    0x0040       //“浏览文件夹”对话框里新增“新建文件夹”按钮

BOOL WINAPI CreateShortCut_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, CreateShortCut_OnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, CreateShortCut_OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, CreateShortCut_OnClose);
    }
    return FALSE;
}

BOOL CreateShortCut_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	EnableWindow(GetDlgItem(hwnd, ID_BTNLASTSTEP), FALSE);
	InitCreateShortCutTitleDlg(hwnd, lParam);
    return TRUE;
}

void CreateShortCut_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
    {
	case ID_BTNNEXTSTEP:      //下一步
		{
			NextStepShortCut(hwnd);
		}
		break;
	case ID_BTNCANCEL:        //取消
		{
			EndDialog(hwnd, 0);
		}
		break;
	case ID_BTNBROWSE:        //浏览
		{
			BrowseDirectory(hwnd);
		}
		break;
	default:
		break;
	}
	
    return;
}

void CreateShortCut_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}

//下一步

void NextStepShortCut(HWND hwnd)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
	GetDlgItemText(hwnd, IDC_EDITSHORTCUTADDR, szPath, sizeof(szPath) / sizeof(TCHAR));

	if (0 == lstrcmp(szPath, TEXT("")))
	{
		MessageBox(hwnd, TEXT("项目的位置不能为空"), TEXT("创建快捷方式"), MB_OK | MB_ICONERROR);
		return;
	}
	if (0 != access(szPath, 0))
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

		wsprintf(szMsg, "无法找到文件“%s”", szPath);
		MessageBox(hwnd, TEXT(szMsg), TEXT("创建快捷方式"), MB_OK | MB_ICONERROR);
		return;
	}
	EndDialog(hwnd, 0);
	HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOGSHORTCUTTITLE), hwnd, ShortCutTitle_Proc, (LPARAM)szPath);
}

//浏览

void BrowseDirectory(HWND hwnd)
{
	TCHAR szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof(szFilePath) / sizeof(TCHAR));

	BROWSEINFO sBrowseInfo;
	LPITEMIDLIST lpitem;
	ZeroMemory(&sBrowseInfo, sizeof(BROWSEINFO));
	sBrowseInfo.hwndOwner = hwnd;
	sBrowseInfo.lpszTitle = TEXT("从下面选择快捷方式的目标:");
	sBrowseInfo.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE; //不包含网络目录
	
	lpitem = SHBrowseForFolder(&sBrowseInfo);
	
	if (NULL == lpitem)                                    //判断是否已经选择文件夹
	{
		CoTaskMemFree(lpitem);
		return;
	}
	SHGetPathFromIDList(lpitem, szFilePath);	           //获得文件的路径
	SetDlgItemText(hwnd, IDC_EDITSHORTCUTADDR, szFilePath);
	CoTaskMemFree(lpitem);
}

//初始化创建快捷方式对话框

void InitCreateShortCutTitleDlg(HWND hwnd, LPARAM lParam)
{
	TCHAR *pszPath = (TCHAR*)lParam;
	SetDlgItemText(hwnd, IDC_EDITSHORTCUTADDR, pszPath);
	pszPath = NULL;
}