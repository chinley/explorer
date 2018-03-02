// CreateProcess.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessManager.h"
#include "CreateProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CCreateProcess dialog


CCreateProcess::CCreateProcess(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateProcess::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCreateProcess)
	m_ExeName = _T("");
	//}}AFX_DATA_INIT
}


void CCreateProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateProcess)
	DDX_Text(pDX, IDC_EDIT_APPID, m_ExeName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCreateProcess, CDialog)
	//{{AFX_MSG_MAP(CCreateProcess)
	ON_COMMAND(ID_CREATEPROCESS, OnCreateprocess)
	ON_BN_CLICKED(IDOKcp, OnOKcp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCreateProcess message handlers

void CCreateProcess::OnCreateprocess() 
{
	// TODO: Add your command handler code here
	
}

void CCreateProcess::OnOKcp() 
{
	CString name;
	GetDlgItemText(IDC_EDIT_APPID, name);
	// TODO: Add your control notification handler code here
		TCHAR* szCommandLine = const_cast<char*>((LPCTSTR) name);


	//TCHAR szCommandLine[] = TEXT("NOTEPAD");//����WCHAR
    //LPWSTR szCommandLine = TEXT("NOTEPAD");//����
    //STARTUPINFO si = { sizeof(si) };
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
	
    si.dwFlags = STARTF_USESHOWWINDOW;  // ָ��wShowWindow��Ա��Ч
    si.wShowWindow = TRUE;          // �˳�Ա��ΪTRUE�Ļ�����ʾ�½����̵������ڣ�
	// ΪFALSE�Ļ�����ʾ
    BOOL bRet = ::CreateProcess (
        NULL,           // ���ڴ�ָ����ִ���ļ����ļ���
        szCommandLine,      // �����в���
        NULL,           // Ĭ�Ͻ��̰�ȫ��
        NULL,           // Ĭ���̰߳�ȫ��
        FALSE,          // ָ����ǰ�����ڵľ�������Ա��ӽ��̼̳�
        CREATE_NEW_CONSOLE, // Ϊ�½��̴���һ���µĿ���̨����
        NULL,           // ʹ�ñ����̵Ļ�������
        NULL,           // ʹ�ñ����̵���������Ŀ¼
        &si,
        &pi);

	SendMessage(WM_CLOSE);

    if(bRet)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        // ��Ȼ���ǲ�ʹ�������������������̽����ǹر�
        ::CloseHandle (pi.hThread);
        ::CloseHandle (pi.hProcess);
		
    }

}
