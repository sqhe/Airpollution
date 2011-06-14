// Simplifier.h : main header file for the SIMPLIFIER application
//

#if !defined(AFX_SIMPLIFIER_H__BDDF9564_C67B_11D3_91F5_0000E83E7EBA__INCLUDED_)
#define AFX_SIMPLIFIER_H__BDDF9564_C67B_11D3_91F5_0000E83E7EBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimplifierApp:
// See Simplifier.cpp for the implementation of this class
//

class CSimplifierApp : public CWinApp
{
public:
	CSimplifierApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimplifierApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CSimplifierApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLIFIER_H__BDDF9564_C67B_11D3_91F5_0000E83E7EBA__INCLUDED_)
