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


	//TCHAR szCommandLine[] = TEXT("NOTEPAD");//或者WCHAR
    //LPWSTR szCommandLine = TEXT("NOTEPAD");//错误
    //STARTUPINFO si = { sizeof(si) };
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
	
    si.dwFlags = STARTF_USESHOWWINDOW;  // 指定wShowWindow成员有效
    si.wShowWindow = TRUE;          // 此成员设为TRUE的话则显示新建进程的主窗口，
	// 为FALSE的话则不显示
    BOOL bRet = ::CreateProcess (
        NULL,           // 不在此指定可执行文件的文件名
        szCommandLine,      // 命令行参数
        NULL,           // 默认进程安全性
        NULL,           // 默认线程安全性
        FALSE,          // 指定当前进程内的句柄不可以被子进程继承
        CREATE_NEW_CONSOLE, // 为新进程创建一个新的控制台窗口
        NULL,           // 使用本进程的环境变量
        NULL,           // 使用本进程的驱动器和目录
        &si,
        &pi);

	SendMessage(WM_CLOSE);

    if(bRet)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        // 既然我们不使用两个句柄，最好是立刻将它们关闭
        ::CloseHandle (pi.hThread);
        ::CloseHandle (pi.hProcess);
		
    }

}
