#include "StdAfx.h"
#include <windows.h>
#include <windowsx.h>
#include "resource.h"
#include "ShortCutTitleDlg.h"
#include "CreateShortCutDlg.h"
#include "ExplorerFun.h"
#include "FileOperation.h"

extern TCHAR g_szAllPath[MAX_PATH];        //ȫ·��
extern HWND g_hwndMain;

		/*
		HANDLE_MSG(hwnd, message, fn)�Ǻ��ǩ����
		1��hwnd���账����Ϣ�Ĵ��ھ����
		2��message����Ϣ��
		3��fn��ָ������Ϣ����ĺ���ָ�롣
		*/
//�����ڹ���
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

//����WM_INITDIALOG��Ϣ
BOOL ShortCutTitle_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	InitShortCutTitleDlg(hwnd, lParam);
    return TRUE;
}


//����WM_COMMAND��Ϣ
void ShortCutTitle_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(id)
    {
	case ID_BTNLASTSTEP: //��һ��
		{
			LastStepShortCut(hwnd);
		}
		break;
	case ID_BTNCOMPLETE://���
		{
			CompleteShortCut(hwnd);
		}
		break;
	case ID_BTNCANCEL://ȡ��
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
void ShortCutTitle_OnClose(HWND hwnd)
{
    EndDialog(hwnd, 0);
}

//��ʼ����ݷ�ʽ����Ի���

void InitShortCutTitleDlg(HWND hwnd, LPARAM lParam)
{
	TCHAR *pszPath = (TCHAR*)lParam;
	TCHAR *pLastSymbol = strrchr(pszPath, '\\');
	SetDlgItemText(hwnd, IDC_EDITSHORTCUTNAME, pLastSymbol+1);
	SetWindowLong(hwnd, GWL_USERDATA, (LPARAM)pszPath);
	pLastSymbol = NULL;
	pszPath = NULL;
}

//��һ��

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

//��ɴ�����ݷ�ʽ

void CompleteShortCut(HWND hwnd)
{
	TCHAR *pszPath = (TCHAR*)GetWindowLong(hwnd, GWL_USERDATA);
	TCHAR szFileName[MAX_PATH];
	TCHAR szLinkName[MAX_PATH];
	ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
	ZeroMemory(szLinkName, sizeof(szLinkName) / sizeof(TCHAR));

	GetDlgItemText(hwnd, IDC_EDITSHORTCUTNAME, szFileName, sizeof(szFileName) / sizeof(TCHAR));
	wsprintf(szLinkName, "%s\\��ݷ�ʽ ��%s.lnk", g_szAllPath, szFileName);

	CreateShortCut(hwnd, pszPath, szLinkName);
	InsertFileToListView(g_hwndMain, szLinkName, 0, FALSE);
	pszPath = NULL;
	EndDialog(hwnd, 0);
}