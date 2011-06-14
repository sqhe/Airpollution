#if !defined(AFX_SIMPLIFYDIAG_H__7377D9E3_C8D5_11D3_91F5_0000E83E7EBA__INCLUDED_)
#define AFX_SIMPLIFYDIAG_H__7377D9E3_C8D5_11D3_91F5_0000E83E7EBA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SimplifyDiag.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSimplifyDiag dialog

class CSimplifyDiag : public CDialog
{
// Construction
public:
	CSimplifyDiag(int nFace, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSimplifyDiag)
	enum { IDD = IDD_SIMPLIFY };
	CEdit	m_cEditRate;
	CEdit	m_cEditFace;
	int		m_nInputMode;
	int		m_nFaceNo;
	float	m_fRate;
	CString	m_sText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimplifyDiag)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSimplifyDiag)
	afx_msg void OnFace();
	afx_msg void OnRate();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLIFYDIAG_H__7377D9E3_C8D5_11D3_91F5_0000E83E7EBA__INCLUDED_)
