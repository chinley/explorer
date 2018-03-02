// MyOsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyOs.h"
#include "MyOsDlg.h"
#include "ShutDown.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyOsDlg dialog

CMyOsDlg::CMyOsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyOsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON);
}

void CMyOsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_BUTTON1, m_buttonResource);  
    DDX_Control(pDX, IDC_BUTTON2, m_buttonProcess);  
    DDX_Control(pDX, IDC_BUTTON3, m_buttonExit); 

}

BEGIN_MESSAGE_MAP(CMyOsDlg, CDialog)
	//{{AFX_MSG_MAP(CMyOsDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_CHECK2, OnCheck2)
	ON_BN_CLICKED(IDC_CHECK3, OnCheck3)
	ON_BN_CLICKED(IDC_CHECK4, OnCheck4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyOsDlg message handlers

BOOL CMyOsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

  

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyOsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
	 //ע�͸���䣺��ֹ�ظ������ػ�����  
    //CDialogEx::OnPaint();  
  
    //��Ӵ���Ի��򱳾���ͼ  
    CPaintDC   dc(this);     
    CRect   rect;     
    GetClientRect(&rect);                                 //��ȡ�Ի��򳤿�         
    CDC   dcBmp;                                           //���岢����һ���ڴ��豸����  
    dcBmp.CreateCompatibleDC(&dc);                         //����������DC  
    CBitmap   bmpBackground;     
    bmpBackground.LoadBitmap(IDB_BITMAP1);                 //������Դ�е�IDB_BITMAP1ͼƬ  
    BITMAP   m_bitmap;                                     //ͼƬ����                  
    bmpBackground.GetBitmap(&m_bitmap);                    //��ͼƬ����λͼ��  
    CBitmap   *pbmpOld=dcBmp.SelectObject(&bmpBackground); //��λͼѡ����ʱ�ڴ��豸����    
    //���ú�����ʾͼƬ StretchBlt��ʾ��״�ɱ�  
    dc.StretchBlt(0,0,rect.Width(),rect.Height(),&dcBmp,0,0,  
        m_bitmap.bmWidth,m_bitmap.bmHeight,SRCCOPY);      
  

}  
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyOsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMyOsDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	STARTUPINFO   si; 
	PROCESS_INFORMATION   pi; 
	ZeroMemory(&si,sizeof(si)); 
	si.cb = sizeof(si); 
	ZeroMemory(&pi,sizeof(pi)); 
	if(!CreateProcess(NULL, "ResourceManager.exe",NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi))   
	{ 
		if(!CreateProcess(NULL, "..\\ResourceManager.exe",NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi)){
			MessageBox( "���� ", "��ʾ ",MB_OK|MB_ICONINFORMATION); 
		}
	} 
	WaitForSingleObject(pi.hProcess,INFINITE); 
	CloseHandle(pi.hProcess); 
	CloseHandle(pi.hThread); 
}

void CMyOsDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
		STARTUPINFO   si; 
	PROCESS_INFORMATION   pi; 
	ZeroMemory(&si,sizeof(si)); 
	si.cb = sizeof(si); 
	ZeroMemory(&pi,sizeof(pi)); 
	if(!CreateProcess(NULL, "ProcessManager.exe",NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi))   
	{ 
		if(!CreateProcess(NULL, "..\\ProcessManager.exe ",NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi)){
			MessageBox( "���� ", "��ʾ ",MB_OK|MB_ICONINFORMATION); 
		}
	} 
	WaitForSingleObject(pi.hProcess,INFINITE); 
	CloseHandle(pi.hProcess); 
	CloseHandle(pi.hThread); 
}

void CMyOsDlg::OnButton3() 
{
	// TODO: Add your control notification handler code here
	ExitProcess(0);
	
}


//�ػ�
void CMyOsDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck())
		Shutdown(false);
	
}


//����
void CMyOsDlg::OnCheck2() 
{
	// TODO: Add your control notification handler code here
	if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck())//�ж��Ƿ�ѡ�У�
		StandBy();
	
}


//����
void CMyOsDlg::OnCheck3() 
{
	// TODO: Add your control notification handler code here
	if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK3))->GetCheck())//�ж��Ƿ�ѡ�У�
		Reboot(false);
}


//ע��
void CMyOsDlg::OnCheck4() 
{
	// TODO: Add your control notification handler code here
	if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck())//�ж��Ƿ�ѡ�У�
		Logoff(false);
	
}


