// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Simplifier.h"
#include "SmokeSystem.h"
#include "MainFrm.h"

#define	IDC_PROGRESS	105

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
 	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
 		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
 		!m_wndToolBar.LoadToolBar(IDR_INTERACT_LEFT))
 	{
 		TRACE0("Failed to create toolbar\n");
 		return -1;      // fail to create
 	}

	if (!m_wndStatusBar.Create(this)||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

//  	if (!m_wndInteractLeft.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
//  		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
//  		!m_wndInteractLeft.LoadToolBar(IDR_INTERACT_LEFT))
//  	{
//  		TRACE0("Failed to create toolbar\n");
//  		return -1;      // fail to create
//  	}
// 
//  	if (!m_wndInteractRight.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
//  		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
//  		!m_wndInteractRight.LoadToolBar(IDR_INTERACT_RIGHT))
//  	{
//  		TRACE0("Failed to create toolbar\n");
//  		return -1;      // fail to create
//  	}

	m_wndStatusBar.SetPaneText(1,"Buildings info:0");
	m_wndStatusBar.SetPaneText(2,"Air data info:0");
	char tmps[80];
	sprintf(tmps,"Particles: %d", numParticles);
	m_wndStatusBar.SetPaneText(3,tmps);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndInteractLeft.EnableDocking(CBRS_ALIGN_ANY);
	//m_wndInteractRight.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	//DockControlBar(&m_wndInteractLeft, AFX_IDW_DOCKBAR_LEFT);
	//DockControlBar(&m_wndInteractRight, AFX_IDW_DOCKBAR_RIGHT);

	if (!m_Progress.Create( WS_CHILD | WS_VISIBLE , CRect(320,2,450,18), &m_wndStatusBar, IDC_PROGRESS ))
	{
		TRACE0("Failed to create progress control \n");
		return -1;      // fail to create
	}
	m_Progress.SetRange(0,100);
	m_Progress.SetPos(0);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.cx = 472;
	cs.cy = 460;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	UINT	statusPaneID, nStyle;
	int		cxWidth;
	RECT rect;

	GetClientRect( &rect );
	m_wndStatusBar.GetPaneInfo( 0, statusPaneID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo( 0, statusPaneID, SBPS_NOBORDERS, (rect.right-rect.left)/5);
	m_wndStatusBar.SetPaneInfo( 1, statusPaneID, SBPS_NORMAL, (rect.right-rect.left)/5);
	m_wndStatusBar.SetPaneInfo( 2, statusPaneID, SBPS_NORMAL, (rect.right-rect.left)/5);
	m_wndStatusBar.SetPaneInfo( 3, statusPaneID, SBPS_NORMAL, (rect.right-rect.left)/5);
	m_Progress.MoveWindow((rect.right-rect.left)*4/5+25,2,(rect.right-rect.left)/5-25,17);
}
