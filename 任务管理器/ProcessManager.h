// ProcessManager.h : main header file for the PROCESSMANAGER application
//

#if !defined(AFX_PROCESSMANAGER_H__F6ECC5A1_8C9C_40EF_BBB4_1A4C8F3E9605__INCLUDED_)
#define AFX_PROCESSMANAGER_H__F6ECC5A1_8C9C_40EF_BBB4_1A4C8F3E9605__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CProcessManagerApp:
// See ProcessManager.cpp for the implementation of this class
//

class CProcessManagerApp : public CWinApp
{
public:
	CProcessManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessManagerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CProcessManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSMANAGER_H__F6ECC5A1_8C9C_40EF_BBB4_1A4C8F3E9605__INCLUDED_)
