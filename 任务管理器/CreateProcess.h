#if !defined(AFX_CREATEPROCESS_H__AE9E7869_4911_4A9E_A3BC_B84A358537A4__INCLUDED_)
#define AFX_CREATEPROCESS_H__AE9E7869_4911_4A9E_A3BC_B84A358537A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateProcess.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateProcess dialog

class CCreateProcess : public CDialog
{
// Construction
public:
	CCreateProcess(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateProcess)
	enum { IDD = IDD_CREATEPROCESS };
	CString	m_ExeName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateProcess)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateProcess)
	afx_msg void OnCreateprocess();
	afx_msg void OnOKcp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEPROCESS_H__AE9E7869_4911_4A9E_A3BC_B84A358537A4__INCLUDED_)
