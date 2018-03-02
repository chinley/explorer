// MyOs.h : main header file for the MYOS application
//

#if !defined(AFX_MYOS_H__FE3411AF_0C05_4221_8833_04918642D88C__INCLUDED_)
#define AFX_MYOS_H__FE3411AF_0C05_4221_8833_04918642D88C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMyOsApp:
// See MyOs.cpp for the implementation of this class
//

class CMyOsApp : public CWinApp
{
public:
	CMyOsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyOsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMyOsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYOS_H__FE3411AF_0C05_4221_8833_04918642D88C__INCLUDED_)
