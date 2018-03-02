// ProcessManagerDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ProcessManager.h"
#include "ProcessManagerDlg.h"
#include "tlhelp32.h"
#include "CreateProcess.h"

#include "ShutDownDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
// No message handlers
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessManagerDlg dialog

CProcessManagerDlg::CProcessManagerDlg(CWnd* pParent /*=NULL*/)
: CDialog(CProcessManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcessManagerDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcessManagerDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProcessManagerDlg, CDialog)
//{{AFX_MSG_MAP(CProcessManagerDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON_FRESH, OnButtonFresh)
ON_COMMAND(ID_LOGOFF, OnLogoff)
ON_WM_SIZE()
ON_COMMAND(ID_CREATEPROCESS, OnCreateprocess)
ON_COMMAND(ID_ENDPROCESS, OnEndprocess)
ON_NOTIFY(NM_CLICK, IDC_LIST1, OnClickList1)
	ON_BN_CLICKED(IDC_ENDPROCESSBTN, OnEndprocessbtn)
	ON_COMMAND(ID_SHUTDOWN, OnShutdown)
	ON_COMMAND(ID_REBOOT, OnReboot)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcessManagerDlg message handlers

BOOL CProcessManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_list.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_list.InsertColumn(100,"进程名称",LVCFMT_LEFT,100);
	m_list.InsertColumn(100,"ID",LVCFMT_LEFT,100);
	m_list.InsertColumn(100,"CPU时间",LVCFMT_LEFT,100);
	m_list.InsertColumn(100,"创建时间",LVCFMT_LEFT,100);
	m_list.InsertColumn(100,"内存占用",LVCFMT_LEFT,100);
	m_list.InsertColumn(100,"优先级",LVCFMT_LEFT,100);
	// Add "About..." menu item to system menu.
	OnFresh();
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	
	
	
	
	////////////////////////////////////
	///实现MFC 对话框最大化时控件也随比例最大化或者还原
	
	
	CRect rect;    
	GetClientRect(&rect);     //取客户区大小  
	old.x=rect.right-rect.left;
	old.y=rect.bottom-rect.top;
	//////////////////////////////////////
	
	
	
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}





/////////////////////////////////////////////////////////////////
////////////////////////////////////
///实现MFC 对话框最大化时控件也随比例最大化或者还原

void CProcessManagerDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	if (nType==SIZE_RESTORED||nType==SIZE_MAXIMIZED)
	{
		ReSize();
	}
}


void CProcessManagerDlg::ReSize()
{
	float fsp[2];
	POINT Newp; //获取现在对话框的大小
	CRect recta;    
	GetClientRect(&recta);     //取客户区大小  
	Newp.x=recta.right-recta.left;
	Newp.y=recta.bottom-recta.top;
	fsp[0]=(float)Newp.x/old.x;
	fsp[1]=(float)Newp.y/old.y;
	CRect Rect;
	int woc;
	CPoint OldTLPoint,TLPoint; //左上角
	CPoint OldBRPoint,BRPoint; //右下角
	HWND  hwndChild=::GetWindow(m_hWnd,GW_CHILD);  //列出所有控件  
	while(hwndChild)    
	{    
		woc=::GetDlgCtrlID(hwndChild);//取得ID
		GetDlgItem(woc)->GetWindowRect(Rect);  
		ScreenToClient(Rect);  
		OldTLPoint = Rect.TopLeft();  
		TLPoint.x = long(OldTLPoint.x*fsp[0]);  
		TLPoint.y = long(OldTLPoint.y*fsp[1]);  
		OldBRPoint = Rect.BottomRight();  
		BRPoint.x = long(OldBRPoint.x *fsp[0]);  
		BRPoint.y = long(OldBRPoint.y *fsp[1]);  
		Rect.SetRect(TLPoint,BRPoint);  
		GetDlgItem(woc)->MoveWindow(Rect,TRUE);
		hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT);    
	}
	old=Newp;
}

//////////////////////////////////////




void CProcessManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 设备上下文
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		CDialog::UpdateWindow(); //UpdateWindow一下  
	}
}


HCURSOR CProcessManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CProcessManagerDlg::OnFresh()		//刷新进程
{
	
	PROCESSENTRY32 pe;
	HANDLE hSnapshot;
	hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);		//进程快照
	Process32First(hSnapshot,&pe);
	int i=0,dwPri;
	HANDLE hProcess;
	CString ID;
	CString SyTime,str,strPri;
	do											//设置List控件
	{
		m_list.InsertItem(i,"");
		int index=m_list.SetItemText(i,0,pe.szExeFile);
		m_list.SetItemData(index,pe.th32ProcessID);
		ID.Format("%d",pe.th32ProcessID);
		m_list.SetItemText(i,1,ID);
		hProcess=OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pe.th32ProcessID);
		
		GetProcessTimes(hProcess,&CreateTime,&ExitTime,&KernelTime,&UserTime);
		CPUTime.dwHighDateTime = KernelTime.dwHighDateTime + UserTime.dwHighDateTime;
		CPUTime.dwLowDateTime  = KernelTime.dwLowDateTime  + UserTime.dwLowDateTime;
		FileTimeToSystemTime(&CPUTime,&SystemTime);
		SyTime.Format("%.2d:%.2d:%.2d",SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
		m_list.SetItemText(i,2,SyTime);
		
		FileTimeToSystemTime(&CreateTime,&SystemTime);
		SyTime.Format("%.2d:%.2d:%.2d",SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
		m_list.SetItemText(i,3,SyTime);
		
		GetProcessMemoryInfo(hProcess,&PMCounter,sizeof(PMCounter));
		str.Format("%7d",PMCounter.WorkingSetSize/1024);
		m_list.SetItemText(i,4,str);
		
		dwPri= GetPriorityClass(hProcess);
		switch(dwPri)
		{
		/*	case ABOVE_NORMAL_PRIORITY_CLASS:
		strPri.Format("%13s","高于标准");
		break;
		case BELOW_NORMAL_PRIORITY_CLASS:
		strPri.Format("%13s","低于标准");
			break;*/
		case HIGH_PRIORITY_CLASS:
			strPri.Format("%13s","高");
			break;
		case IDLE_PRIORITY_CLASS:
			strPri.Format("%13s","低");
			break;
		case NORMAL_PRIORITY_CLASS:
			strPri.Format("%13s","正常");
			break;
		case REALTIME_PRIORITY_CLASS:
			strPri.Format("%13s","实时");
			break;
		default:
			strPri.Format("%13s","N/A");
			break;
		}
		m_list.SetItemText(i,5,strPri);
		i++;
	}while(Process32Next(hSnapshot,&pe));
	CloseHandle(hSnapshot);
	CStatic* static1=(CStatic*)GetDlgItem(IDC_STATIC1);
	CString strCount;
	strCount.Format("共有%d个进程",i);
	static1->SetWindowText(strCount);
}

void CProcessManagerDlg::OnButtonFresh()		//刷新按钮按下
{
	// TODO: Add your control notification handler code here
	m_list.DeleteAllItems();
	OnFresh();
}

BOOL CProcessManagerDlg::ShutdownPrivilege(BOOL bEnable)		//关机
{
	BOOL              bResult = TRUE;
	HANDLE            hToken;
	TOKEN_PRIVILEGES  TokenPrivileges;
	
	if(OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken) == 0)
	{
		printf("OpenProcessToken Error: %d\n",GetLastError());
		bResult = FALSE;
	}
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
	LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,&TokenPrivileges.Privileges[0].Luid);
	AdjustTokenPrivileges(hToken,FALSE,&TokenPrivileges,sizeof(TOKEN_PRIVILEGES),NULL,NULL);
    if(GetLastError() != ERROR_SUCCESS)
	{
		bResult = FALSE;
	}
	CloseHandle(hToken);
    
	return bResult;
}

void CProcessManagerDlg::OnShutdown() 
{
	// TODO: Add your command handler code here
	 Shutdown(false);
}


void CProcessManagerDlg::OnLogoff()			//注销
{
	// TODO: Add your command handler code here
//	ShutdownPrivilege(TRUE);
//	ExitWindowsEx(EWX_LOGOFF,0);
	Logoff(false);
}

void CProcessManagerDlg::OnReboot() 
{
	// TODO: Add your command handler code here
	Reboot(false);
}


void CProcessManagerDlg::OnCreateprocess() 
{
	// TODO: Add your command handler code here
	CCreateProcess Dlg;
	Dlg.DoModal();
}

void CProcessManagerDlg::OnClickList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		CString strtemp;					//得到所选项appid
		//		CString path; 
		//得到被选中的列表项的全路径   
		//		path = m_wndList.GetItemText(pNMListView->iItem,pNMListView->iSubItem); 
		//		m_wndList. strSelItemPath = path; //保存此路径
		//	strtemp.Format("单击的是第%d行第%d列",
		//		pNMListView->iItem, pNMListView->iSubItem);
		
		//		CString GetItemText(int nItem, int nSubItem) const;
		m_strID = m_list.GetItemText(pNMListView->iItem,0);
		//		strtemp.Format("单击的是第%d行\nAPPID:%d",
		//			pNMListView->iItem+1, m_strID);
		//		MessageBox(strtemp);
		//		MessageBox("APPID:"+m_strID);
		
	}
	
	*pResult = 0;
}

void CProcessManagerDlg::OnEndprocessbtn() 
{
	// TODO: Add your control notification handler code here
		if (!m_strID.IsEmpty())
	{
		PROCESSENTRY32 pe;
		HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(handle, &pe))
		{
			while (TRUE)
			{
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (!Process32Next(handle, &pe))
					break;
				if (CString(pe.szExeFile).CompareNoCase(m_strID) == 0)
				{
					TerminateProcessFromId(pe.th32ProcessID);
				}
			}
		}
		CloseHandle(handle);
}
}

void CProcessManagerDlg::OnEndprocess() 
{
	// TODO: Add your command handler code here
	if (!m_strID.IsEmpty())
	{
		PROCESSENTRY32 pe;
		HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(handle, &pe))
		{
			while (TRUE)
			{
				pe.dwSize = sizeof(PROCESSENTRY32);
				if (!Process32Next(handle, &pe))
					break;
				if (CString(pe.szExeFile).CompareNoCase(m_strID) == 0)
				{
					TerminateProcessFromId(pe.th32ProcessID);
				}
			}
		}
		CloseHandle(handle);
	}
	/*
	DWORD id;
	char* ch;
	
	ch = (LPSTR)(LPCSTR)m_strID;
	id = atol(ch); 
	
	TerminateProcessFromId(id);*/
}


DWORD CProcessManagerDlg::TerminateProcessFromId(DWORD dwId)
{
/*
BOOL bRet = FALSE;
//打开目标进程，取得进程句柄
HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId);
if(hProcess != NULL)
{ // 终止进程
bRet = ::TerminateProcess(hProcess, 0);
}
CloseHandle(hProcess);
return bRet;
	*/
	HANDLE hProcess = NULL, hProcessToken = NULL;
	BOOL IsKilled = FALSE, bRet = FALSE;
	if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId)) != NULL)
	{
		if (TerminateProcess(hProcess, 1))
		{
			return TRUE;
		}
	}
	__try
	{
		SetPrivilege(SE_DEBUG_NAME, TRUE);
		if ((hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwId)) == NULL)
		{
			printf("\nOpen Process %d failed:%d", dwId, GetLastError());
			__leave;
		}
		//printf("\nOpen Process %d ok!",id);
		if (!TerminateProcess(hProcess, 1))
		{
			printf("\nTerminateProcess failed:%d", GetLastError());
			__leave;
		}
		IsKilled = TRUE;
	}
	__finally
	{
		if (hProcessToken != NULL) CloseHandle(hProcessToken);
		if (hProcess != NULL) CloseHandle(hProcess);
	}
	return(IsKilled);
}


//请求获取特权
BOOL CProcessManagerDlg::SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable)
{
	OutputDebugString(lpszPrivilege);
	BOOL bRet = FALSE;
	HANDLE hToken = NULL;
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());
	if (!::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		goto __EXIT;
	}
	LUID Luid;
	if (!::LookupPrivilegeValue(NULL, lpszPrivilege, &Luid))
	{
		goto __EXIT;
	}
	TOKEN_PRIVILEGES newPrivilege;
	newPrivilege.PrivilegeCount = 1;
	newPrivilege.Privileges[0].Luid = Luid;
	newPrivilege.Privileges[0].Attributes = //设置特权属性
		bEnable ?
SE_PRIVILEGE_ENABLED :
	SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	if (!::AdjustTokenPrivileges(hToken, FALSE, &newPrivilege,
		sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		CString s;
		s.Format(_T("AdjustTokenPrivileges error: %u\n"), GetLastError());
		OutputDebugString(s);
		MessageBox(s);
		goto __EXIT;
	}
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)//查看是否真的设置成功了
	{
		MessageBox(_T("The token does not have the specified privilege. \n"));
		goto __EXIT;
	}
	bRet = TRUE;
	MessageBox(_T("Set OK"));
__EXIT:
	if (hProcess)
	{
		::CloseHandle(hProcess);
	}
	if (hToken)
	{
		::CloseHandle(hToken);
	}
	return bRet;
}





