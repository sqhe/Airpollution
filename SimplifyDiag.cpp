// SimplifyDiag.cpp : implementation file
//

#include "stdafx.h"
#include "Simplifier.h"
#include "SimplifyDiag.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimplifyDiag dialog


CSimplifyDiag::CSimplifyDiag(int nFace, CWnd* pParent /*=NULL*/)
	: CDialog(CSimplifyDiag::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimplifyDiag)
	char tmps[80];
	sprintf(tmps,"原模型三角形数:  %d", nFace);
	m_nInputMode = 0;
	m_nFaceNo = nFace/5;
	m_fRate = 80.0f;
	m_sText = tmps;
	//}}AFX_DATA_INIT
}


void CSimplifyDiag::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimplifyDiag)
	DDX_Control(pDX, IDC_RATE_VALUE, m_cEditRate);
	DDX_Control(pDX, IDC_FACE_VALUE, m_cEditFace);
	DDX_Radio(pDX, IDC_FACE, m_nInputMode);
	DDX_Text(pDX, IDC_FACE_VALUE, m_nFaceNo);
	DDX_Text(pDX, IDC_RATE_VALUE, m_fRate);
	DDV_MinMaxFloat(pDX, m_fRate, 0.f, 100.f);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimplifyDiag, CDialog)
	//{{AFX_MSG_MAP(CSimplifyDiag)
	ON_BN_CLICKED(IDC_FACE, OnFace)
	ON_BN_CLICKED(IDC_RATE, OnRate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimplifyDiag message handlers

void CSimplifyDiag::OnFace() 
{
	if (m_nInputMode != 0 )
	{
		m_cEditRate.EnableWindow(FALSE);
		m_cEditFace.EnableWindow(TRUE);
		m_nInputMode = 0;
	}
}

void CSimplifyDiag::OnRate() 
{
	if (m_nInputMode != 1 )
	{
		m_cEditRate.EnableWindow(TRUE);
		m_cEditFace.EnableWindow(FALSE);
		m_nInputMode = 1;
	}
}

BOOL CSimplifyDiag::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cEditRate.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
