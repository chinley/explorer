#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>
#include <Shlobj.h>
#include <io.h>
#include "resource.h"
#include "CreateShortCutDlg.h"
#include "ShortCutTitleDlg.h"

#define BIF_NEWDIALOGSTYLE    0x0040       //������ļ��С��Ի������������½��ļ��С���ť

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
	case ID_BTNNEXTSTEP:      //��һ��
		{
			NextStepShortCut(hwnd);
		}
		break;
	case ID_BTNCANCEL:        //ȡ��
		{
			EndDialog(hwnd, 0);
		}
		break;
	case ID_BTNBROWSE:        //���
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

//��һ��

void NextStepShortCut(HWND hwnd)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
	GetDlgItemText(hwnd, IDC_EDITSHORTCUTADDR, szPath, sizeof(szPath) / sizeof(TCHAR));

	if (0 == lstrcmp(szPath, TEXT("")))
	{
		MessageBox(hwnd, TEXT("��Ŀ��λ�ò���Ϊ��"), TEXT("������ݷ�ʽ"), MB_OK | MB_ICONERROR);
		return;
	}
	if (0 != access(szPath, 0))
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

		wsprintf(szMsg, "�޷��ҵ��ļ���%s��", szPath);
		MessageBox(hwnd, TEXT(szMsg), TEXT("������ݷ�ʽ"), MB_OK | MB_ICONERROR);
		return;
	}
	EndDialog(hwnd, 0);
	HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOGSHORTCUTTITLE), hwnd, ShortCutTitle_Proc, (LPARAM)szPath);
}

//���

void BrowseDirectory(HWND hwnd)
{
	TCHAR szFilePath[MAX_PATH];
	ZeroMemory(szFilePath, sizeof(szFilePath) / sizeof(TCHAR));

	BROWSEINFO sBrowseInfo;
	LPITEMIDLIST lpitem;
	ZeroMemory(&sBrowseInfo, sizeof(BROWSEINFO));
	sBrowseInfo.hwndOwner = hwnd;
	sBrowseInfo.lpszTitle = TEXT("������ѡ���ݷ�ʽ��Ŀ��:");
	sBrowseInfo.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE; //����������Ŀ¼
	
	lpitem = SHBrowseForFolder(&sBrowseInfo);
	
	if (NULL == lpitem)                                    //�ж��Ƿ��Ѿ�ѡ���ļ���
	{
		CoTaskMemFree(lpitem);
		return;
	}
	SHGetPathFromIDList(lpitem, szFilePath);	           //����ļ���·��
	SetDlgItemText(hwnd, IDC_EDITSHORTCUTADDR, szFilePath);
	CoTaskMemFree(lpitem);
}

//��ʼ��������ݷ�ʽ�Ի���

void InitCreateShortCutTitleDlg(HWND hwnd, LPARAM lParam)
{
	TCHAR *pszPath = (TCHAR*)lParam;
	SetDlgItemText(hwnd, IDC_EDITSHORTCUTADDR, pszPath);
	pszPath = NULL;
}