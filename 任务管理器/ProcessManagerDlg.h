// ProcessManagerDlg.h : header file
//
#include "Psapi.h"
#include <windows.h>

#if !defined(AFX_PROCESSMANAGERDLG_H__1E7365B9_83B1_47B0_835B_5D4B9C843FD5__INCLUDED_)
#define AFX_PROCESSMANAGERDLG_H__1E7365B9_83B1_47B0_835B_5D4B9C843FD5__INCLUDED_

#if _MSC_VER > 1000
#pragma  comment (lib,"psapi")
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CProcessManagerDlg dialog

class CProcessManagerDlg : public CDialog
{
	// Construction
public:
	BOOL ShutdownPrivilege(BOOL bEnable);
	void OnFresh();


////////////////////////////////////
	///实现MFC 对话框最大化时控件也随比例最大化或者还原
		void ReSize();
		POINT old;
		//afx_msg void OnSize(UINT nType, int cx, int cy);  

//////////////////////////////////////
	
	CProcessManagerDlg(CWnd* pParent = NULL);	// standard constructor
	FILETIME        CreateTime;
	FILETIME        ExitTime;
	FILETIME        KernelTime;
	FILETIME        UserTime;
	FILETIME        CPUTime;
	SYSTEMTIME      SystemTime;
	DWORD           dwPriority;
	PROCESS_MEMORY_COUNTERS   PMCounter;
	
	// Dialog Data
	//{{AFX_DATA(CProcessManagerDlg)
	enum { IDD = IDD_PROCESSMANAGER_DIALOG };
	CListCtrl	m_list;
	//}}AFX_DATA
	
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessManagerDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	HICON m_hIcon;
	CString m_strID;

	DWORD TerminateProcessFromId(DWORD dwId);
	BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable);
	// Generated message map functions
	//{{AFX_MSG(CProcessManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonFresh();
	afx_msg void OnLogoff();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCreateprocess();
	afx_msg void OnEndprocess();
	afx_msg void OnClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndprocessbtn();
	afx_msg void OnShutdown();
	afx_msg void OnReboot();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMANAGERDLG_H__1E7365B9_83B1_47B0_835B_5D4B9C843FD5__INCLUDED_)
