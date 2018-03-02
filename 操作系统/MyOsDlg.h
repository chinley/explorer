// MyOsDlg.h : header file
//

#if !defined(AFX_MYOSDLG_H__B67DF2D2_A642_4FBC_B12A_48E1079152A8__INCLUDED_)
#define AFX_MYOSDLG_H__B67DF2D2_A642_4FBC_B12A_48E1079152A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMyOsDlg dialog

class CMyOsDlg : public CDialog
{
// Construction
public:
	CMyOsDlg(CWnd* pParent = NULL);	// standard constructor



	CBitmapButton m_buttonResource;
	CBitmapButton m_buttonProcess;
	CBitmapButton m_buttonExit;


// Dialog Data
	//{{AFX_DATA(CMyOsDlg)
	enum { IDD = IDD_MYOS_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyOsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMyOsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYOSDLG_H__B67DF2D2_A642_4FBC_B12A_48E1079152A8__INCLUDED_)


