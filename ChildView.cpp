// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
// #include <GL/gl.h>
// #include <GL/glu.h>
// #include <GL/glaux.h>
#include <GL/glew.h>
#if defined(__APPLE__) || defined(__MACOSX)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#if defined (_WIN32)
#include <GL/wglew.h>
#endif
#include "Simplifier.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "InitOpenGL.h"
#include "SimplifyDiag.h"

#include "SmokeSystem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildView
CChildView *outHandle=NULL;
double scale=1;
int winWidth = 1280, winHeight = 1024;
int timer_span=10,timer_span_bak=10;
bool bStereoAvailable=true;
void DrawBuilding()
{	
	if (!outHandle)
		return;
	if (outHandle->m_pMesh3D && outHandle->m_bBildings){
		if (outHandle->m_glError_Building)		
			outHandle->RenderBuilding();
		else
			glCallList (outHandle->m_building_list_id);
	}
}

void DrawBounding()
{

	if (!outHandle)
		return;
	if (outHandle->m_pBound && outHandle->m_bBoudings){
		if (outHandle->m_glError_Bounding)		
			outHandle->RenderBounding();
		else
			glCallList (outHandle->m_bounding_list_id);
	}
}

void DrawAirData()
{
	if (!outHandle)
		return;
	if (obj){
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//glScalef(scale,scale,scale);
		//glTranslatef(offsetX, offsetY, offsetZ);
		outHandle->DrawBoundingBox();
		if (outHandle->m_bAirTet)
		{
			if (outHandle->m_glError_AirTet)		
				outHandle->RenderAirTet();
			else
				glCallList (outHandle->m_airtet_list_id);
		}
		if (outHandle->m_bWindfield)
		{
			if (outHandle->m_glError_Windfield)		
				outHandle->RenderWindfield();
			else
				glCallList (outHandle->m_wind_list_id);
		}
		if (outHandle->m_bPath)
		{
			drawPathes();
		}
		glPopMatrix();

	}
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}
CChildView::CChildView()
{
	m_fViewHeight = 2.0;
	m_fViewNear = -100.0;
	m_fViewFar = 100.0;

	m_nMouseAct = MOUSE_SPIN;
	m_bAnimate = FALSE;
	m_bSimulate= FALSE;
	m_bAxes = TRUE;

	m_spinAngle = 0.0;
	m_spinAxes[0] = m_spinAxes[2] = 0.0; m_spinAxes[1] = 1.0;

	m_sEye.origin.x = m_sEye.origin.y = m_sEye.origin.z = 0.0;
	m_sEye.center.x = m_sEye.center.y = 0.0; m_sEye.center.z = -1.0;
	m_sEye.up.x = m_sEye.up.z = 0.0; m_sEye.up.y = 1.0;
	m_sEye.axisX.y = m_sEye.axisX.z = 0.0; m_sEye.axisX.x = 1.0;
	m_sEye.axisY.x = m_sEye.axisY.z = 0.0; m_sEye.axisY.y = 1.0;
	m_sEye.axisZ.x = m_sEye.axisZ.y = 0.0; m_sEye.axisZ.z = -1.0;
	m_vProjectCenter[0] = 0.0f;
	m_vProjectCenter[1] = 0.0f;
	m_vProjectCenter[2] = (m_fViewNear+m_fViewFar)/2;

	m_nRenderModel = 1;
	m_nRenderMode = 1;
	m_pMesh3D = NULL;
	m_pBound = NULL;
	m_pInlet = NULL;
	m_nFaceNo = 0;

	m_FlatNormal = 1;

	obj = NULL;
	read_air_in = FALSE;
	m_bAirTet   = FALSE;
	m_bKeyDown=FALSE;
	m_bLButtonDown=FALSE;
	m_bPath=FALSE;
	m_bBildings = FALSE;
	m_bWindfield = FALSE;
	m_bGround   = TRUE;
	m_bArrow    = FALSE;
	m_bVelocity = FALSE;
	m_bPressure = FALSE;
	m_bParticleInit = FALSE;
	m_bShowParticleSource= FALSE;
	m_bShowActiveArea=FALSE;
	m_bShowtargetCursor=FALSE;
	m_glError_Windfield=TRUE;
	m_glError_Building=TRUE;
	m_glError_Bounding=TRUE;
	m_glError_AirTet=TRUE;
	m_Particlenum = 1000;
	outHandle=this;
}

CChildView::~CChildView()
{
	if (m_pMesh3D){
		delete m_pMesh3D;
		m_pMesh3D = NULL;
		if (!m_glError_Building)
			glDeleteLists(m_building_list_id,1);		
	}

	if (m_pBound){
		delete m_pBound;
		m_pBound = NULL;
		if (!m_glError_Bounding)
			glDeleteLists(m_bounding_list_id,1);		
	}

	if (obj){
		delete obj;
		obj = NULL;
		if (!m_glError_Windfield)
			glDeleteLists(m_wind_list_id,1);
		if (!m_glError_AirTet)
			glDeleteLists(m_airtet_list_id,1);
	}

	if (m_pInlet)
	{
		delete m_pInlet;
		m_pInlet = NULL;
	}
	exitPaticleSystem(__argc,__argv);
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP( )
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	//	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MOUSE_SPIN, OnMouseSpin)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_SPIN, OnUpdateMouseSpin)
	ON_COMMAND(ID_MOUSE_TRANSLATE, OnMouseTranslate)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_TRANSLATE, OnUpdateMouseTranslate)
	ON_COMMAND(ID_MOUSE_ZOOM, OnMouseZoom)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_ZOOM, OnUpdateMouseZoom)
	ON_COMMAND(ID_ANIMATE, OnAnimate)
	ON_UPDATE_COMMAND_UI(ID_ANIMATE, OnUpdateAnimate)
	ON_COMMAND(ID_SPIN_XCCW, OnSpinXccw)
	ON_COMMAND(ID_SPIN_XCW, OnSpinXcw)
	ON_COMMAND(ID_SPIN_YCCW, OnSpinYccw)
	ON_COMMAND(ID_SPIN_YCW, OnSpinYcw)
	ON_COMMAND(ID_SPIN_ZCCW, OnSpinZccw)
	ON_COMMAND(ID_SPIN_ZCW, OnSpinZcw)
	//ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	//ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_COMMAND(ID_OPEN_FILE, OnOpenFile)
	ON_COMMAND(ID_BUILDINGS, OnShowBuildings)
	ON_COMMAND(ID_AIRDATA, OnShowAirtet)
	ON_COMMAND(ID_SHOWPATH, OnShowPath)
	ON_COMMAND(ID_SHOWGROUND, OnShowGround)
	ON_COMMAND(ID_SIMINIT, OnSimInit)
	ON_COMMAND(ID_3D, On3D)	
	ON_UPDATE_COMMAND_UI(ID_3D, OnUpdate3D)	
	ON_COMMAND(ID_SIMSTART, OnSimStart)
	ON_COMMAND(ID_SHOWPARTICLES, OnShowParticles)
	ON_COMMAND(ID_MOUSEMODE_VIEW, OnMoveView)
	ON_COMMAND(ID_MOUSEMODE_MOVELIGHT, OnMoveLight)
	ON_COMMAND(ID_MOUSEMODE_MOVESOURCE, OnMoveGenerator)
	ON_COMMAND(ID_HELP, OnShowHelp)
	ON_UPDATE_COMMAND_UI(ID_HELP, OnUpdateShowHelp)	
	ON_COMMAND(ID_SKYBOX,OnShowSkybox)
	ON_COMMAND(ID_DISPLAYTEMPERATURE,OnShowTemprature)

	ON_COMMAND(ID_SIMSTOP, OnSimStop)
	ON_COMMAND(IDM_ORIGINAL_MODEL, OnOriginalModel)
	ON_UPDATE_COMMAND_UI(IDM_ORIGINAL_MODEL, OnUpdateOriginalModel)
	ON_COMMAND(IDM_WIREFRAME, OnWireframe)
	ON_UPDATE_COMMAND_UI(IDM_WIREFRAME, OnUpdateWireframe)
	ON_COMMAND(IDM_FLAT, OnFlat)
	ON_UPDATE_COMMAND_UI(IDM_FLAT, OnUpdateFlat)
	ON_COMMAND(ID_AXES, OnAxes)
	ON_UPDATE_COMMAND_UI(ID_AXES, OnUpdateAxes)
	ON_COMMAND(IDM_SAVE_PICTURE, OnSavePicture)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_LOADAIRDATA, &CChildView::OnFileLoadairdata)
	ON_COMMAND(ID_FILE_LOADAIRDATABIN, &CChildView::OnFileLoadairdataBin)
	ON_COMMAND(ID_FILE_READOUTBOUND, &CChildView::OnFileReadoutbound)
	ON_COMMAND(ID_FILE_SAVEAIRDATA, &CChildView::OnFileSaveairdataBin)	
	ON_COMMAND(ID_AIRMODE_WINDFEILD, &CChildView::OnAirmodeWindfeild)
	ON_UPDATE_COMMAND_UI(ID_AIRMODE_WINDFEILD, &CChildView::OnUpdateAirmodeWindfeild)
	ON_COMMAND(ID_AIRMODE_WITHARROW, &CChildView::OnAirmodeWitharrow)
	ON_UPDATE_COMMAND_UI(ID_AIRMODE_WITHARROW, &CChildView::OnUpdateAirmodeWitharrow)
	ON_COMMAND(ID_AIRMODE_WITHPRESSURE, &CChildView::OnAirmodeWithpressure)
	ON_UPDATE_COMMAND_UI(ID_AIRMODE_WITHPRESSURE, &CChildView::OnUpdateAirmodeWithpressure)
	ON_COMMAND(ID_AIRMODE_WITHVELOCITY, &CChildView::OnAirmodeWithvelocity)
	ON_UPDATE_COMMAND_UI(ID_AIRMODE_WITHVELOCITY, &CChildView::OnUpdateAirmodeWithvelocity)
	ON_COMMAND(ID_FILE_LOADINLET, &CChildView::OnFileLoadinlet)
	ON_WM_ERASEBKGND()
	ON_WM_KEYUP()
	ON_COMMAND(ID_PARTICLESOURCE_SHOWSOURCE, &CChildView::OnParticlesourceShowsource)
	ON_UPDATE_COMMAND_UI(ID_PARTICLESOURCE_SHOWSOURCE, &CChildView::OnUpdateParticlesourceShowsource)
	ON_COMMAND(ID_TARGETCURSOR, &CChildView::OnShowTargetCursor)
	ON_UPDATE_COMMAND_UI(ID_TARGETCURSOR, &CChildView::OnUpdateShowTargetCursor)

	ON_COMMAND(ID_PARTICLESOURCE_INITPARTICLES, &CChildView::OnParticlesourceInitparticles)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_PARTICLESOURCE_RANDOM, &CChildView::OnParticlesourceRandom)
	ON_COMMAND(ID_PARTICLESOURCE_RANDOM_V, &CChildView::OnParticlesourceRandomVolumn)
	ON_COMMAND(ID_PARTICLESOURCE_RANDOM_P, &CChildView::OnParticlesourceRandomPlain)
	ON_UPDATE_COMMAND_UI(ID_MOUSEMODE_MOVELIGHT, &CChildView::OnUpdateMousemodeMovelight)
	ON_UPDATE_COMMAND_UI(ID_MOUSEMODE_VIEW, &CChildView::OnUpdateMousemodeView)
	ON_UPDATE_COMMAND_UI(ID_MOUSEMODE_MOVESOURCE, &CChildView::OnUpdateMousemodeMovesource)
	ON_COMMAND(ID_MOUSEMODE_MOVECURSOR, &CChildView::OnMousemodeMovecursor)
	ON_UPDATE_COMMAND_UI(ID_MOUSEMODE_MOVECURSOR, &CChildView::OnUpdateMousemodeMovecursor)
	ON_COMMAND(ID_MOUSEMODE_MOVEACTIVEAREA, &CChildView::OnMousemodeMoveactivearea)
	ON_UPDATE_COMMAND_UI(ID_MOUSEMODE_MOVEACTIVEAREA, &CChildView::OnUpdateMousemodeMoveactivearea)
	ON_COMMAND(ID_VIEW_ACTIVEAREA, &CChildView::OnViewActivearea)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ACTIVEAREA, &CChildView::OnUpdateViewActivearea)
	ON_COMMAND(ID_MOUSEMODE_SCALEACTIVEAREA, &CChildView::OnMousemodeScaleactivearea)
	ON_UPDATE_COMMAND_UI(ID_MOUSEMODE_SCALEACTIVEAREA, &CChildView::OnUpdateMousemodeScaleactivearea)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

int CChildView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();

	return 0;
}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC|CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	preDisplay();
	setLeftView();
	DrawScene();
	if (bStereo)
	{
		setRightView();
		DrawScene();
	}	
	SwapBuffers(m_hDc);
	if(m_bKeyDown)
		Invalidate();
	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnSize( UINT nType, int cx, int cy )
{

	CWnd::OnSize(nType, cx, cy);
	winWidth=cx;
	winHeight=cy;
	setCgWindow(winWidth,winHeight);
	Project();
}

BOOL CChildView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD nID = wParam & 0x0000ffff;

	switch (nID)
	{
		// 	case ID_ZOOM_IN:
		// 		OnZoomIn();
		// 		break;
		// 	case ID_ZOOM_OUT:
		// 		OnZoomOut();
		// 		break;
	case ID_SPIN_XCW:
		OnSpinXcw();
		break;
	case ID_SPIN_XCCW:
		OnSpinXccw();
		break;
	case ID_SPIN_YCW:
		OnSpinYcw();
		break;
	case ID_SPIN_YCCW:
		OnSpinYccw();
		break;
	case ID_SPIN_ZCW:
		OnSpinZcw();
		break;
	case ID_SPIN_ZCCW:
		OnSpinZccw();
		break;
	}

	return CWnd ::OnCommand(wParam, lParam);
}

void CChildView::OnTimer( UINT nIDEvent )
{

	if (nIDEvent == TIMER_SIMULATION)
	{
		if (timer_span!=timer_span_bak)
		{
			timer_span_bak=timer_span;
			KillTimer(TIMER_SIMULATION);
			SetTimer(TIMER_SIMULATION, timer_span, NULL);
		}
		updatePaticles();
		char tmps[80];
		int initNum=numParticles/initBatchN;
		int currentBatch=initCount/initNum;
		CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
		sprintf(tmps,"Particles: total %d, current%d, batch:%d/%d(%d once),", numParticles,initCount,currentBatch,initBatchN,initNum);
		pFrameWnd->m_wndStatusBar.SetPaneText(3,tmps);
		Invalidate();
	}
}

//void CChildView::OnContextMenu( CWnd* pWnd, CPoint pos )
//{
//	CMenu menu;
//	menu.LoadMenu(IDR_CONTEXTMENU);
//	TRACE("%d  %d  \n",pos.x,pos.y);
//	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
//		pos.x,pos.y,this);
//}

void CChildView::OnMouseMove( UINT nFlags, CPoint point )
{
	//if (m_nMouseAct == MOUSE_SPIN)
	//{
	//	MouseSpinGlobal(nFlags, point.x, point.y, 0);
	//}
	//else if (m_nMouseAct == MOUSE_ZOOM)
	//{
	//	MouseZoom(nFlags, point.x, point.y);
	//}
	//else if (m_nMouseAct == MOUSE_TRANSLATE)
	//{
	//	MouseTranslate(nFlags, point.x, point.y);
	//}
	motion(point.x, point.y);

	CWnd::OnMouseMove(nFlags, point);
	Invalidate();
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{

	CWnd::OnLButtonDown(nFlags, point);
	m_bLButtonDown=TRUE;
	mouse(GLUT_LEFT_BUTTON, 0, point.x, point.y);		
	Invalidate();
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{


	CWnd::OnLButtonUp(nFlags, point);
	m_bLButtonDown=FALSE;
	mouse(GLUT_LEFT_BUTTON, 1, point.x, point.y);
	Invalidate();
}

BOOL CChildView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	HCURSOR hCursor;

	switch (m_nMouseAct)
	{
	case MOUSE_SPIN:
		hCursor = AfxGetApp()->LoadCursor( IDC_SPIN );
		SetCursor( hCursor );
		break;
	case MOUSE_ZOOM:
		hCursor = AfxGetApp()->LoadCursor( IDC_ZOOM );
		SetCursor( hCursor );
		break;
	case MOUSE_TRANSLATE:
		hCursor = AfxGetApp()->LoadCursor( IDC_TRANSLATE );
		SetCursor( hCursor );
		break;
	default:
		hCursor = AfxGetApp()->LoadCursor( IDC_ARROW );
		SetCursor( hCursor );
		break;
	}

	return TRUE;
}

void CChildView::OnOriginalModel() 
{
	if (m_nRenderModel == 1)
	{
		m_nRenderModel = 0;
		BuildDisplayListBuilding();
		Invalidate();
	}
}

void CChildView::OnUpdateOriginalModel(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nRenderModel == 0);
}

void CChildView::OnWireframe() 
{
	if (m_nRenderMode == 1)
	{
		m_nRenderMode = 0;
		BuildDisplayListBuilding();
		Invalidate();
	}
}

void CChildView::OnUpdateWireframe(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nRenderMode == 0);	
}

void CChildView::OnFlat() 
{
	if (m_nRenderMode == 0)
	{
		m_nRenderMode = 1;
		BuildDisplayListBuilding();
		Invalidate();
	}
}

void CChildView::OnUpdateFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nRenderMode == 1);	
}

void CChildView::OnShowBuildings()
{
	m_bBildings = !m_bBildings;
	Invalidate();
}

void CChildView::OnShowBoundings()
{
	m_bBoudings = !m_bBoudings;
	Invalidate();
}

void CChildView::OnShowGround()
{
	m_bGround = !m_bGround;
	Invalidate();
}
void CChildView::On3D()
{
	if(bStereoAvailable)
	{
		bStereo=!bStereo;
	}
	else
	{
		bStereo=false;
	}
}
void CChildView::OnSimInit()
{	
	resetPaticles(CONFIG_RANDOM,0,numParticles);//reset the aphla values
	beingInit=CONFIG_INLET_RANDOM;
	initCount=0;
	// 	if(!resetPaticles(CONFIG_INLET_RANDOM))
	// 		AfxMessageBox("No tetrahedron is found.");
	Invalidate();
}

void CChildView::OnSimStart()
{
	SetTimer(TIMER_SIMULATION, timer_span, NULL);
	m_bSimulate=TRUE;
}
void CChildView::OnShowParticles()
{
	displayEnabled=!displayEnabled;
	Invalidate();
}
void CChildView::OnMoveView()
{
	mode=M_VIEW;
}
void CChildView::OnMoveLight()
{
	mode=M_MOVE_LIGHT;
}
void CChildView::OnMoveGenerator()
{
	m_bShowParticleSource=TRUE;
	mode=M_MOVE_SOURCE;
}

void CChildView::OnShowHelp()
{
	displaySliders=!displaySliders;
	Invalidate();
}

void CChildView::OnShowSkybox()
{
	displaySkybox=!displaySkybox;
	movelight(displaySkybox);;
	Invalidate();
}

void CChildView::OnShowTemprature()
{
	displayTemprature=!displayTemprature;
	setsunpowertransfer();
	Invalidate();
}

void CChildView::OnSimStop()
{
	KillTimer(TIMER_SIMULATION);
	m_bSimulate=FALSE;
}

void CChildView::OnShowAirtet()
{
	m_bAirTet = !m_bAirTet;
	Invalidate();
}

void CChildView::OnOpenFile() 
{
	CFileDialog loadmesh(TRUE,  0);
	loadmesh.m_ofn.lpstrTitle ="Load building";

	//SetCurrentDirectory(".\\");
	if (loadmesh.DoModal() == IDOK )
	{
		CString filename;
		CString fileext;

		fileext  = loadmesh.GetFileExt();
		filename = loadmesh.GetPathName();
		if (!filename.IsEmpty()) {
			FILE *fp;
			fp = fopen(filename, "r");
			if (!fp) {
				AfxMessageBox("Can't open file to load");
				return;
			}
			if (loadmesh.GetFileExt() == "tm")
			{
				if (m_pMesh3D)
					delete m_pMesh3D;
				m_pMesh3D = new Mesh3D;

				HCURSOR hCursor, hOldCursor;
				hCursor = AfxGetApp()->LoadCursor( IDC_MY_WAIT );
				hOldCursor = SetCursor( hCursor );

				CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
				pFrameWnd->m_Progress.SetPos(0);
				m_pMesh3D->m_pProgress = &(pFrameWnd->m_Progress);

				m_nFaceNo = m_pMesh3D->load_data(fp);
				if (m_bAnimate)
					OnAnimate();
				m_spinAngle = 0.0;
				pFrameWnd->m_Progress.SetPos(0);
				m_pMesh3D->m_pProgress = NULL;
				SetCursor(hOldCursor);
			}
			else if (loadmesh.GetFileExt() == "off")
			{
				if (m_pMesh3D)
					delete m_pMesh3D;
				m_pMesh3D = new Mesh3D;

				HCURSOR hCursor, hOldCursor;
				hCursor = AfxGetApp()->LoadCursor( IDC_MY_WAIT );
				hOldCursor = SetCursor( hCursor );

				CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
				pFrameWnd->m_Progress.SetPos(0);
				m_pMesh3D->m_pProgress = &(pFrameWnd->m_Progress);

				m_nFaceNo = m_pMesh3D->load_offdata(fp);
				if (m_bAnimate)
					OnAnimate();
				m_spinAngle = 0.0;
				pFrameWnd->m_Progress.SetPos(0);
				m_pMesh3D->m_pProgress = NULL;
				SetCursor(hOldCursor);

				char tmps[80];
				sprintf(tmps,"Building Data: Vertices %d, Face %d", m_pMesh3D->get_num_vertices(), m_pMesh3D->get_num_faces());
				pFrameWnd->m_wndStatusBar.SetPaneText(1,tmps);
				m_nsFaceNo = m_nFaceNo;

				//DrawScene();
				Invalidate();
				m_bBildings = TRUE;
			}
			BuildDisplayListBuilding();
			fclose(fp);
		}
	}
}

void CChildView::OnSavePicture() 
{
	CFileDialog savefile(FALSE, ".bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "BMP文件(*.bmp)|*.bmp||"); 
	savefile.m_ofn.lpstrTitle = "存为BMP图象" ;

	if ( savefile.DoModal() == IDOK )
	{
		CString filename;
		char temp[80];

		filename = savefile.GetPathName();
		sprintf(temp,"%s",filename);
		SaveAsBMP(temp);
	}
}

void CChildView::OnAxes() 
{
	m_bAxes = !m_bAxes;
	DrawScene();
}

void CChildView::OnUpdateAxes(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAxes);	
}

void CChildView::OnAnimate() 
{
	if (m_bAnimate)
		KillTimer(TIMER_ANIMATE);
	else
		SetTimer( TIMER_ANIMATE, ANIMATE_RATE, NULL );
	m_bAnimate = !m_bAnimate;
}

void CChildView::OnUpdateAnimate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAnimate);
}

void CChildView::OnMouseSpin() 
{
	m_nMouseAct = MOUSE_SPIN;
}

void CChildView::OnUpdateMouseSpin(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nMouseAct == MOUSE_SPIN);
}

void CChildView::OnMouseTranslate() 
{
	m_nMouseAct = MOUSE_TRANSLATE;
}

void CChildView::OnUpdateMouseTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nMouseAct == MOUSE_TRANSLATE);
}

void CChildView::OnMouseZoom() 
{
	m_nMouseAct = MOUSE_ZOOM;
}

void CChildView::OnUpdateMouseZoom(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_nMouseAct == MOUSE_ZOOM);
}

BOOL CChildView::SetThePixelFormat(CDC *pDC)
{
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |            // support OpenGL
		PFD_DOUBLEBUFFER	|			// double buffered
		PFD_STEREO,               
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer	
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	int pixelformat;
	if((pixelformat = ChoosePixelFormat(m_hDc, &pfd)) == 0)
	{
		MessageBox( "ChoosePixelFormat failed", "Error", MB_OK);
		return FALSE;
	}

	if(SetPixelFormat(m_hDc, pixelformat, &pfd) == FALSE)
	{
		MessageBox( "SetPixelFormat failed", "Error", MB_OK);
		return FALSE;
	}

	// pCDC, iPixelFormat, pfd are declared in above code

	pixelformat = GetPixelFormat (m_hDc);

	DescribePixelFormat (m_hDc, pixelformat, sizeof
		(PIXELFORMATDESCRIPTOR), &pfd);

	if ((pfd.dwFlags & PFD_STEREO) == 0) // stereo mode not accepted
	{
		bStereo = false;
		bStereoAvailable=false;
	}
	else // yes we're now in stereo
	{
		bStereo = true;
		bStereoAvailable=true;
	}
	CreateRGBPalette(m_hDc);

	return TRUE;
}

void CChildView::MouseSpinGlobal(UINT nFlags, int x, int y, int init)
{
	// use mouse to play 3D trackball transformation
	// see also Intel 3DR

	float centerX, centerY;
	float radius;
	float w, h;  // window

	RECT rect;
	GetClientRect( &rect);
	w =(float)rect.right;
	h =(float)rect.bottom;
	h = (h == 0.0f) ? 1.0f : h;

	centerX = w / 2.0f;
	centerY = h / 2.0f;
	radius  = (w > h) ? centerY : centerX ;

	if (init)
	{
		//	And call Trackball() with current center, radius 
		// and current cursor position (in client coordinates) 
		// to initialize trackball internal parameters.
		//	Note that we must stop animation at that time.

		Trackball( 0, radius, centerX, centerY, (float)x, (float)(h-y), 
			m_spinAxes[0], m_spinAxes[1], m_spinAxes[2], m_spinAngle);
	}
	else if ( nFlags & MK_LBUTTON )
	{
		Trackball(1, radius, centerX, centerY, (float)x, (float)(h-y), 
			m_spinAxes[0], m_spinAxes[1], m_spinAxes[2], m_spinAngle);
		SpinGlobal(m_spinAxes, m_spinAngle);
	}
}

void CChildView::SpinGlobal(float axes[], float angle)
{
	EyeRotate(m_vProjectCenter, axes, angle, m_sEye );
	DrawScene(); 
}

// void CChildView::MouseZoom(UINT nFlags, int x, int y)
// {
// 	float w, h;
// 	float scale;
// 	static float oldY, dy ;
// 
// 	RECT rect;
// 	GetClientRect( &rect);
// 	w =(float)rect.right;
// 	h =(float)rect.bottom;
// 	h = (h == 0.0f) ? 1.0f : h;
// 
// 	if( m_bLButtonDown && (nFlags & MK_LBUTTON))
// 	{
// 		dy=oldY-y;
// 		scale=1.0f+dy/h;
// 		Zoom(scale);
// 		oldY=(float)y;
// 	}
// 	else
// 	{
// 		oldY=(float)y;
// 	}
// }

// void CChildView::Zoom(float scale)
// {
// 	if(scale<0.001) return;
// 	m_fViewHeight/=scale;
// 	Project();
// 	DrawScene();
// }

// void CChildView::MouseTranslate(UINT nFlags, int x, int y)
// {
// 	float w, h;
// 	static float oldX, oldY, dx, dy, d[3] ;
// 
// 	RECT rect;
// 	GetClientRect( &rect);
// 	w =(float)rect.right;
// 	h =(float)rect.bottom;
// 	h = (h == 0.0f) ? 1.0f : h;
// 
// 	if( m_bLButtonDown && (nFlags & MK_LBUTTON))
// 	{
// 		dx=x-oldX;
// 		dy=oldY-y;
// 		d[0]=m_fViewWidth*dx/w*0.8f;
// 		d[1]=m_fViewHeight*dy/h*0.8f;           
// 		d[2]=0.0f;
// 		Translate(d);   
// 		oldX=(float)x;
// 		oldY=(float)y;
// 	}
// 	else
// 	{
// 		oldX=(float)x;
// 		oldY=(float)y;
// 	}
// }

// void CChildView::Translate(float d[])
// {
// 	EyeTranslate(d, m_sEye);
// 	DrawScene();
// }

void CChildView::OnSpinXccw() 
{
	KillTimer(TIMER_ANIMATE);

	glRotatef(ROTATE_RATE, 1.0f, 0.0f, 0.0f);
	DrawScene();
}

void CChildView::OnSpinXcw() 
{
	KillTimer(TIMER_ANIMATE);

	MSG msg;
	while(::PeekMessage( &msg, m_hWnd,
		WM_TIMER, WM_TIMER,
		PM_REMOVE));

	glRotatef(-ROTATE_RATE, 1.0f, 0.0f, 0.0f);
	DrawScene();
}

void CChildView::OnSpinYccw() 
{
	KillTimer(TIMER_ANIMATE);

	MSG msg;
	while(::PeekMessage( &msg, m_hWnd,
		WM_TIMER, WM_TIMER,
		PM_REMOVE));

	glRotatef(ROTATE_RATE, 0.0f, 1.0f, 0.0f);
	DrawScene();
}

void CChildView::OnSpinYcw() 
{
	KillTimer(TIMER_ANIMATE);

	MSG msg;
	while(::PeekMessage( &msg, m_hWnd,
		WM_TIMER, WM_TIMER,
		PM_REMOVE));

	glRotatef(-ROTATE_RATE, 0.0f, 1.0f, 0.0f);
	DrawScene();
}

void CChildView::OnSpinZccw() 
{
	KillTimer(TIMER_ANIMATE);

	MSG msg;
	while(::PeekMessage( &msg, m_hWnd,
		WM_TIMER, WM_TIMER,
		PM_REMOVE));

	glRotatef(ROTATE_RATE, 0.0f, 0.0f, 1.0f);
	DrawScene();
}

void CChildView::OnSpinZcw() 
{
	KillTimer(TIMER_ANIMATE);

	MSG msg;
	while(::PeekMessage( &msg, m_hWnd,
		WM_TIMER, WM_TIMER,
		PM_REMOVE));

	glRotatef(-ROTATE_RATE, 0.0f, 0.0f, 1.0f);
	DrawScene();
}

// void CChildView::OnZoomIn() 
// {
// 	KillTimer(TIMER_ANIMATE);
// 
// 	MSG msg;
// 	while(::PeekMessage( &msg, m_hWnd,
// 			WM_TIMER, WM_TIMER,
// 			PM_REMOVE));
// 
// 	Zoom(1.0f + ZOOM_RATE );
// }
// 
// void CChildView::OnZoomOut() 
// {
// 	KillTimer(TIMER_ANIMATE);
// 
// 	MSG msg;
// 	while(::PeekMessage( &msg, m_hWnd,
// 			WM_TIMER, WM_TIMER,
// 			PM_REMOVE));
// 
// 	Zoom(1.0f - ZOOM_RATE );
// }

void CChildView::Init()
{
	CDC *pDC = GetDC();
	m_hDc = pDC->GetSafeHdc();
	SetThePixelFormat(pDC);
	m_hRc = wglCreateContext(m_hDc);
	wglMakeCurrent(m_hDc,m_hRc);
	ReleaseDC(pDC);
	initPaticleSystem(__argc, __argv);
	setLight();
	SetTimer( TIMER_TOOLBAR, TOOLBAR_RATE, NULL);
	if (m_bAnimate)
		SetTimer( TIMER_ANIMATE, ANIMATE_RATE, NULL );

	CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();

	m_nsFaceNo = m_nFaceNo = 0;
	m_wind_list_id=glGenLists(1);
	m_building_list_id=glGenLists(1);
	m_bounding_list_id=glGenLists(1);
	m_airtet_list_id=glGenLists(1);
}


void CChildView::DrawBoundingBox()
{
	if (obj == NULL)
		return;
	//glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glColor3f(0.f, 0.f, 1.f);
	glLineWidth(5.f);

	glBegin(GL_LINE_LOOP);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[4]);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[4]);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[5]);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[5]);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[4]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[5]);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[5]);
	glVertex3f(obj->boud_box[1], obj->boud_box[3], obj->boud_box[5]);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[5]);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[5]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[5]);
	glVertex3f(obj->boud_box[1], obj->boud_box[3], obj->boud_box[5]);
	glVertex3f(obj->boud_box[1], obj->boud_box[3], obj->boud_box[4]);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[4]);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[5]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[4]);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[4]);
	glVertex3f(obj->boud_box[1], obj->boud_box[3], obj->boud_box[4]);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[4]);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[4]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[4]);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[4]);
	glVertex3f(obj->boud_box[0], obj->boud_box[3], obj->boud_box[5]);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[5]);
	glVertex3f(obj->boud_box[0], obj->boud_box[2], obj->boud_box[4]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[5]);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[4]);
	glVertex3f(obj->boud_box[1], obj->boud_box[3], obj->boud_box[4]);
	glVertex3f(obj->boud_box[1], obj->boud_box[3], obj->boud_box[5]);
	glVertex3f(obj->boud_box[1], obj->boud_box[2], obj->boud_box[5]);
	glEnd();

	glLineWidth(1.f);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glColor3f ( 1.0f, 1.0f, 0.0f);
}

void CChildView::DrawAxes()
{
	//glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glScalef(1.4f, 1.4f, 1.4f);

	glBegin(GL_LINES);	
	// x axis
	glColor3f ( 1.0f, 0.0f, 0.0f);
	glVertex3f( 0.0f, 0.0f, 0.0f);
	glVertex3f( 1.0f, 0.0f, 0.0f);

	// y axis
	glColor3f ( 0.0f, 1.0f, 0.0f);
	glVertex3f( 0.0f, 0.0f, 0.0f);
	glVertex3f( 0.0f, 1.0f, 0.0f);

	// z axis
	glColor3f ( 0.0f, 0.0f, 1.0f);
	glVertex3f( 0.0f, 0.0f, 0.0f);
	glVertex3f( 0.0f, 0.0f, 1.0f);
	glEnd();

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBegin(GL_TRIANGLES);	

	// x axis arrow
	glColor3f ( 1.0f, 0.0f,  0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    0.0f,    0.04f);
	glVertex3f(0.9f,    0.028f,  0.028f);
	glVertex3f(0.9f,    0.0f,    0.04f);
	glVertex3f(0.9f,    0.028f,  0.028f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    0.028f,  0.028f);
	glVertex3f(0.9f,    0.04f,   0.0f);
	glVertex3f(0.9f,    0.028f,  0.028f);
	glVertex3f(0.9f,    0.04f,   0.0f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    0.04f,   0.0f);
	glVertex3f(0.9f,    0.028f,  -0.028f);
	glVertex3f(0.9f,    0.04f,   0.0f);
	glVertex3f(0.9f,    0.028f,  -0.028f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    0.028f,  -0.028f);
	glVertex3f(0.9f,    0.0f,    -0.04f);
	glVertex3f(0.9f,    0.028f,  -0.028f);
	glVertex3f(0.9f,    0.0f,    -0.04f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    0.0f,    -0.04f);
	glVertex3f(0.9f,    -0.028f, -0.028f);
	glVertex3f(0.9f,    0.0f,    -0.04f);
	glVertex3f(0.9f,    -0.028f, -0.028f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    -0.028f, -0.028f);
	glVertex3f(0.9f,    -0.04f,  0.0f);
	glVertex3f(0.9f,    -0.028f, -0.028f);
	glVertex3f(0.9f,    -0.04f,  0.0f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    -0.04f,  0.0f);
	glVertex3f(0.9f,    -0.028f, 0.028f);
	glVertex3f(0.9f,    -0.04f,  0.0f);
	glVertex3f(0.9f,    -0.028f, 0.028f);
	glVertex3f(0.9f,    0.0f,    0.0f);
	glVertex3f(1.0f,    0.0f,    0.0f);
	glVertex3f(0.9f,    -0.028f, 0.028f);
	glVertex3f(0.9f,    0.0f,    0.04f);
	glVertex3f(0.9f,    -0.028f, 0.028f);
	glVertex3f(0.9f,    0.0f,    0.04f);
	glVertex3f(0.9f,    0.0f,    0.0f);

	// y axis arrow
	glColor3f ( 0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(0.0f,    0.9f,    0.04f);
	glVertex3f(0.028f,  0.9f,    0.028f);
	glVertex3f(0.0f,    0.9f,    0.04f);
	glVertex3f(0.028f,  0.9f,    0.028f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(0.028f,  0.9f,    0.028f);
	glVertex3f(0.04f,   0.9f,    0.0f);
	glVertex3f(0.028f,  0.9f,    0.028f);
	glVertex3f(0.04f,   0.9f,    0.0f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(0.04f,   0.9f,    0.0f);
	glVertex3f(0.028f,  0.9f,    -0.028f);
	glVertex3f(0.04f,   0.9f,    0.0f);
	glVertex3f(0.028f,  0.9f,    -0.028f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(0.028f,  0.9f,    -0.028f);
	glVertex3f(0.0f,    0.9f,    -0.04f);
	glVertex3f(0.028f,  0.9f,    -0.028f);
	glVertex3f(0.0f,    0.9f,    -0.04f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(0.0f,    0.9f,    -0.04f);
	glVertex3f(-0.028f, 0.9f,    -0.028f);
	glVertex3f(0.0f,    0.9f,    -0.04f);
	glVertex3f(-0.028f, 0.9f,    -0.028f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(-0.028f, 0.9f,    -0.028f);
	glVertex3f(-0.04f,  0.9f,    0.0f);
	glVertex3f(-0.028f, 0.9f,    -0.028f);
	glVertex3f(-0.04f,  0.9f,    0.0f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(-0.04f,  0.9f,    0.0f);
	glVertex3f(-0.028f, 0.9f,    0.028f);
	glVertex3f(-0.04f,  0.9f,    0.0f);
	glVertex3f(-0.028f, 0.9f,    0.028f);
	glVertex3f(0.0f,    0.9f,    0.0f);
	glVertex3f(0.0f,    1.0f,    0.0f);
	glVertex3f(-0.028f, 0.9f,    0.028f);
	glVertex3f(0.0f,    0.9f,    0.04f);
	glVertex3f(-0.028f, 0.9f,    0.028f);
	glVertex3f(0.0f,    0.9f,    0.04f);
	glVertex3f(0.0f,    0.9f,    0.0f);

	// z axis arrow
	glColor3f ( 0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(0.0f,    0.04f,   0.9f);
	glVertex3f(0.028f,  0.028f,  0.9f);
	glVertex3f(0.0f,    0.04f,   0.9f);
	glVertex3f(0.028f,  0.028f,  0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(0.028f,  0.028f,  0.9f);
	glVertex3f(0.04f,   0.0f,    0.9f);
	glVertex3f(0.028f,  0.028f,  0.9f);
	glVertex3f(0.04f,   0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(0.04f,   0.0f,    0.9f);
	glVertex3f(0.028f,  -0.028f, 0.9f);
	glVertex3f(0.04f,   0.0f,    0.9f);
	glVertex3f(0.028f,  -0.028f, 0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(0.028f,  -0.028f, 0.9f);
	glVertex3f(0.0f,    -0.04f,  0.9f);
	glVertex3f(0.028f,  -0.028f, 0.9f);
	glVertex3f(0.0f,    -0.04f,  0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(0.0f,    -0.04f,  0.9f);
	glVertex3f(-0.028f, -0.028f, 0.9f);
	glVertex3f(0.0f,    -0.04f,  0.9f);
	glVertex3f(-0.028f, -0.028f, 0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(-0.028f, -0.028f, 0.9f);
	glVertex3f(-0.04f,  0.0f,    0.9f);
	glVertex3f(-0.028f, -0.028f, 0.9f);
	glVertex3f(-0.04f,  0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(-0.04f,  0.0f,    0.9f);
	glVertex3f(-0.028f, 0.028f,  0.9f);
	glVertex3f(-0.04f,  0.0f,    0.9f);
	glVertex3f(-0.028f, 0.028f,  0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);
	glVertex3f(0.0f,    0.0f,    1.0f);
	glVertex3f(-0.028f, 0.028f,  0.9f);
	glVertex3f(0.0f,    0.04f,   0.9f);
	glVertex3f(-0.028f, 0.028f,  0.9f);
	glVertex3f(0.0f,    0.04f,   0.9f);
	glVertex3f(0.0f,    0.0f,    0.9f);


	// 	glPolygonMode(GL_FRONT,GL_LINE);
	// 	glPolygonMode(GL_BACK,GL_LINE);
	glEnd();
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glColor3f ( 1.0f, 1.0f, 0.0f);
}

void CChildView::SetupBmpHeader(BITMAPINFOHEADER *pbmih, int sx, int sy, int bpp)
{
	pbmih->biSize = sizeof(BITMAPINFOHEADER);
	pbmih->biWidth = sx;
	pbmih->biHeight = sy;
	pbmih->biPlanes = 1;
	pbmih->biBitCount = bpp;
	pbmih->biCompression = BI_RGB;
	pbmih->biSizeImage = sx * sy * (bpp/8);
	pbmih->biXPelsPerMeter = 2925;
	pbmih->biYPelsPerMeter = 2925;
	pbmih->biClrUsed = 0;
	pbmih->biClrImportant = 0;
}

BOOL CChildView::SaveAsBMP(char *bmpFileName)
{
	GLubyte  *lpBits;
	RECT  rect;

	GetClientRect(&rect);

	CDC *pDC = GetDC();
	ASSERT(pDC->m_hDC);

	// Creating Compatible Memory Device Context
	CDC *pMemDC = new CDC;
	if (!pMemDC->CreateCompatibleDC(pDC))
	{
		MessageBox("CompatibleDC Error");
		return 0;
	}

	// Preparing bitmap header for DIB section
	BITMAPINFO bi;
	int bpp, sx, sy;
	ZeroMemory(&bi, sizeof(BITMAPINFO));
	// Making a DIB image which is half size of GL window and full color(24 bpp)
	sx = (rect.right-rect.left) ;
	sy = (rect.bottom-rect.top) ;
	sx=sx-sx % 4;  // BMP 要求扫描线为 4 的倍数
	bpp = 24;

	long dwSizeImage = sx * sy * bpp / 8; // 8 means 8 bit/byte
	SetupBmpHeader(&(bi.bmiHeader), sx, sy, bpp);

	// Creating a DIB surface
	HBITMAP hBm, hBmOld;
	hBm = CreateDIBSection(pDC->GetSafeHdc(), &bi, DIB_RGB_COLORS, 
		(void **)&lpBits, NULL, (DWORD)0);
	if (!hBm)
	{
		MessageBox("CreateDIBSection Error");
		return 0;
	}

	// Selecting the DIB Surface
	hBmOld = (HBITMAP)::SelectObject(pMemDC->GetSafeHdc(), hBm);
	if (!hBmOld)
	{
		MessageBox("Select Object Error");
		return 0;
	}


	glReadBuffer(GL_BACK);
	glReadPixels(rect.left,rect.top,sx,//rect.right-rect.left,
		rect.bottom-rect.top,GL_BGR_EXT,GL_UNSIGNED_BYTE,lpBits);

	// Preparing BMP file header information

	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 0x4d42;  // 'BM'
	long nSize =  sizeof(BITMAPINFOHEADER) +  dwSizeImage;
	bmfh.bfSize = nSize + sizeof(BITMAPFILEHEADER);
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);	

	CFile *pbmpFile;
	CFileException fileException;

	pbmpFile = new CFile;
	if (!pbmpFile->Open(bmpFileName , 
		CFile::modeCreate | CFile::modeWrite ) )
	{
		TRACE( "Unable to open file: %s\n, error = %u\n",
			"" ,fileException.m_cause);
	}

	// Wrinting the DIB image
	TRY 
	{
		pbmpFile->Write(&bmfh, sizeof(BITMAPFILEHEADER));
		pbmpFile->Write(&(bi.bmiHeader), sizeof(BITMAPINFOHEADER));
		pbmpFile->Write(lpBits,dwSizeImage);
		pbmpFile->Flush();
	}
	CATCH (CException, e) 
	{
		AfxMessageBox("write error");
		return 0;
	}
	END_CATCH

		// Cleaning
		pbmpFile->Close();
	delete pbmpFile;

	hBm = (HBITMAP)::SelectObject(pMemDC->GetSafeHdc(), hBmOld);
	DeleteObject(hBm);

	delete pMemDC;
	ReleaseDC(pDC);

	return 1;
}

void CChildView::Project()
{
	RECT	rect;

	GetClientRect( &rect);
	GLsizei nWidth = rect.right;
	GLsizei nHeight = rect.bottom;
	reshape(nWidth, nHeight);
}

void CChildView::DrawScene()
{

	display();
	if (m_bAxes)
		DrawAxes();
	if (m_bShowParticleSource)
		drawSphereSource();
	if (m_bShowActiveArea)
	{
		drawAciveArea();
	}
	if (m_bShowtargetCursor)
		drawTargetCursor();
	/*
	int nTriangles = 0;

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if (m_pMesh3D && m_bBildings)
	m_pMesh3D->DrawModel(m_nRenderMode, m_nRenderModel, m_FlatNormal);

	if (m_bAxes)
	DrawAxes();

	if (obj && m_bAirTet)
	DrawAirTet();

	if (m_bGround)
	{
	if (m_pMesh3D)
	DrawGround(m_pMesh3D->zmin);
	else
	DrawGround();
	}

	if (obj)
	DrawBoundingBox();

	if (obj && m_bWindfield)
	DrawWindfield();

	if (m_bParticleInit)
	DrawParticles();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glFlush();*/


}
void CChildView::OnFileSaveairdataBin()
{
	CFileDialog loadmesh(FALSE,_T("bin"));
	loadmesh.m_ofn.lpstrTitle ="Open air data(bin)";
	if (loadmesh.DoModal() == IDOK )
	{
		CString filename;
		CString fileext;

		fileext  = loadmesh.GetFileExt();
		filename = loadmesh.GetPathName();

		char * pBuffer = (char *)(LPCTSTR)filename;
		obj->write_to_binary(pBuffer);
		CString str;
		str.Format(_T("Save File Finished:%s"),pBuffer);
		AfxMessageBox(str);
	}

}
void CChildView::OnFileLoadairdataBin()
{
	// TODO: Add your command handler code here

	CFileDialog loadmesh(TRUE,  0);
	loadmesh.m_ofn.lpstrTitle ="Open air data(bin)";

	//SetCurrentDirectory(".\\");
	if (loadmesh.DoModal() == IDOK )
	{
		CString filename;
		CString fileext;

		fileext  = loadmesh.GetFileExt();
		filename = loadmesh.GetPathName();
		if (fileext!="bin")
		{
			CString str;
			str.Format(_T("Please choose a binary file!"));
			AfxMessageBox(str);
			return;
		}
		if (!filename.IsEmpty()) {
			char * pBuffer = (char *)(LPCTSTR)filename;
			if (obj)
				delete obj;
			obj = new re_dat_tet();
			obj->read_dat_binary(pBuffer);
			read_air_in=true;
		}


		CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
		char tmps[80];
		sprintf(tmps,"Air Data: Vertices %d, Face %d", obj->size_vert_co(), obj->size_tet_in_ord());
		pFrameWnd->m_wndStatusBar.SetPaneText(2,tmps);

		m_bAirTet = FALSE;

		//Buildtetmesh();
		setAirData();
		//BuildDisplayListWindfield();
		//BuildDisplayListAirTet();
		set3dtexture();
		Invalidate();
	}
}

void CChildView::OnFileReadoutbound()
{
	CFileDialog loadmesh(TRUE,  0);
	loadmesh.m_ofn.lpstrTitle ="Load building";

	//SetCurrentDirectory(".\\");
	if (loadmesh.DoModal() == IDOK )
	{
		CString filename;
		CString fileext;

		fileext  = loadmesh.GetFileExt();
		filename = loadmesh.GetPathName();
		if (!filename.IsEmpty()) {
			FILE *fp;
			fp = fopen(filename, "r");
			if (!fp) {
				AfxMessageBox("Can't open file to load");
				return;
			}
			if (loadmesh.GetFileExt() == "tm")
			{
				if (m_pBound)
					delete m_pBound;
				m_pBound = new Mesh3D;

				HCURSOR hCursor, hOldCursor;
				hCursor = AfxGetApp()->LoadCursor( IDC_MY_WAIT );
				hOldCursor = SetCursor( hCursor );

				CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
				pFrameWnd->m_Progress.SetPos(0);
				m_pBound->m_pProgress = &(pFrameWnd->m_Progress);

				m_nFaceNo = m_pMesh3D->load_data(fp);
				if (m_bAnimate)
					OnAnimate();
				m_spinAngle = 0.0;
				pFrameWnd->m_Progress.SetPos(0);
				m_pBound->m_pProgress = NULL;
				SetCursor(hOldCursor);
			}
			else if (loadmesh.GetFileExt() == "off")
			{
				if (m_pBound)
					delete m_pBound;
				m_pBound = new Mesh3D;

				HCURSOR hCursor, hOldCursor;
				hCursor = AfxGetApp()->LoadCursor( IDC_MY_WAIT );
				hOldCursor = SetCursor( hCursor );

				CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
				pFrameWnd->m_Progress.SetPos(0);
				m_pBound->m_pProgress = &(pFrameWnd->m_Progress);

				m_nFaceNo = m_pBound->load_offdata(fp);
				if (m_bAnimate)
					OnAnimate();
				m_spinAngle = 0.0;
				pFrameWnd->m_Progress.SetPos(0);
				m_pBound->m_pProgress = NULL;
				SetCursor(hOldCursor);

				char tmps[80];
				sprintf(tmps,"Building Data: Vertices %d, Face %d", m_pBound->get_num_vertices(), m_pBound->get_num_faces());
				pFrameWnd->m_wndStatusBar.SetPaneText(1,tmps);
				m_nsFaceNo = m_nFaceNo;

				//DrawScene();
				Invalidate();
				m_bBoudings = TRUE;
			}
			//BuildDisplayListBuilding();
			BuildDisplayListBounding();
			fclose(fp);
		}
	}
}

void CChildView::OnFileLoadairdata()
{
	// TODO: Add your command handler code here

	CFileDialog loadmesh(TRUE,  0);
	loadmesh.m_ofn.lpstrTitle ="Open air data(txt)";

	//SetCurrentDirectory(".\\");
	if (loadmesh.DoModal() == IDOK )
	{
		CString filename;
		CString fileext;

		fileext  = loadmesh.GetFileExt();
		filename = loadmesh.GetPathName();
		if (!filename.IsEmpty()) {
			char * pBuffer = (char *)(LPCTSTR)filename;
			if (obj)
				delete obj;
			obj = new re_dat_tet();
			obj->read_dat_tet(pBuffer);
			read_air_in=true;
		}


		CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
		char tmps[80];
		sprintf(tmps,"Air Data: Vertices %d, Face %d", obj->size_vert_co(), obj->size_tet_in_ord());
		pFrameWnd->m_wndStatusBar.SetPaneText(2,tmps);

		m_bAirTet = FALSE;

		//Buildtetmesh();
		setAirData();
		//BuildDisplayListWindfield();
		//BuildDisplayListAirTet();
		set3dtexture();
		Invalidate();
	}
}

void CChildView::OnAirmodeWindfeild()
{
	// TODO: Add your command handler code here

	m_bWindfield = !m_bWindfield;
	DrawScene();
}

void CChildView::OnUpdateAirmodeWindfeild(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bWindfield);
}

void CChildView::OnAirmodeWitharrow()
{
	// TODO: Add your command handler code here

	m_bArrow = !m_bArrow;
	BuildDisplayListWindfield();
	Invalidate();
}

void CChildView::OnUpdateAirmodeWitharrow(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(m_bArrow);
}

void CChildView::OnAirmodeWithpressure()
{
	// TODO: Add your command handler code here

	m_bPressure = !m_bPressure;
	DrawScene();
}

void CChildView::OnUpdateAirmodeWithpressure(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(m_bPressure);
}

void CChildView::OnAirmodeWithvelocity()
{
	// TODO: Add your command handler code here

	m_bVelocity = !m_bVelocity;
	DrawScene();
}

void CChildView::OnUpdateAirmodeWithvelocity(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	pCmdUI->SetCheck(m_bVelocity);
}

void CChildView::OnFileLoadinlet()
{
	// TODO: Add your command handler code here

	CFileDialog loadmesh(TRUE,  0);
	loadmesh.m_ofn.lpstrTitle ="Open File";

	//SetCurrentDirectory(".\\");
	if (loadmesh.DoModal() == IDOK )
	{
		CString filename;
		CString fileext;

		fileext  = loadmesh.GetFileExt();
		filename = loadmesh.GetPathName();
		if (!filename.IsEmpty()) {
			FILE *fp;
			fp = fopen(filename, "r");
			if (!fp) {
				AfxMessageBox("Can't open file to load");
				return;
			}
			if (loadmesh.GetFileExt() == "tm")
			{
				if (m_pMesh3D)
					delete m_pMesh3D;
				m_pMesh3D = new Mesh3D;

				HCURSOR hCursor, hOldCursor;
				hCursor = AfxGetApp()->LoadCursor( IDC_MY_WAIT );
				hOldCursor = SetCursor( hCursor );

				CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
				pFrameWnd->m_Progress.SetPos(0);
				m_pMesh3D->m_pProgress = &(pFrameWnd->m_Progress);

				m_nFaceNo = m_pMesh3D->load_data(fp);
				if (m_bAnimate)
					OnAnimate();
				m_spinAngle = 0.0;
				pFrameWnd->m_Progress.SetPos(0);
				m_pMesh3D->m_pProgress = NULL;
				SetCursor(hOldCursor);
			}
			else if (loadmesh.GetFileExt() == "off")
			{
				if (m_pInlet)
					delete m_pInlet;
				m_pInlet = new Mesh3D;

				HCURSOR hCursor, hOldCursor;
				hCursor = AfxGetApp()->LoadCursor( IDC_MY_WAIT );
				hOldCursor = SetCursor( hCursor );

				CMainFrame *pFrameWnd = (CMainFrame*)AfxGetMainWnd();
				pFrameWnd->m_Progress.SetPos(0);
				m_pInlet->m_pProgress = &(pFrameWnd->m_Progress);

				m_nFaceNo = m_pInlet->load_offdata(fp);

				/*
				double temp1 = m_pInlet->ymin;
				double temp2 = m_pInlet->ymax;
				m_pInlet->ymin = m_pInlet->zmin;
				m_pInlet->ymax = m_pInlet->zmax;
				m_pInlet->zmin = temp1;
				m_pInlet->zmax = temp2;*/
				//float xmin,ymin,zmin,xmax,ymax,zmax;
				//float xl=(m_pInlet->xmax-m_pInlet->xmin)/2;
				//float yl=(m_pInlet->ymax-m_pInlet->ymin)/2;
				//float zl=(m_pInlet->zmax-m_pInlet->zmin)/2;

				//xmin=xmax=m_pInlet->xmin+xl;
				//ymin=ymax=m_pInlet->ymin/*+yl*/;
				//zmin=zmax=m_pInlet->zmin+zl;
				//xmin-=xl*3;
				//xmax+=xl*3;
				//zmin-=zl/10;
				//zmax+=zl/10;
				////zmin-=zl/10;
				//ymax+=yl/10;


				obj->find_tet_in_box(m_pInlet->xmin,m_pInlet->xmax,m_pInlet->ymin,m_pInlet->ymax,m_pInlet->zmin,m_pInlet->zmax,0,0,0);
				float xoffset,yoffset,zoffset;
				/*
				xoffset=obj->boud_box[1]-(obj->boud_box[1]-obj->boud_box[0])/4;
				yoffset=obj->boud_box[2]+(obj->boud_box[3]-obj->boud_box[2])/5;
				zoffset=obj->boud_box[4]+(obj->boud_box[5]-obj->boud_box[4])/2;
				obj->find_in_let_index(xoffset,xoffset+1,
				yoffset,yoffset+1,
				zoffset,zoffset+1,0.1,0.1,0.1);*/

				if (m_bAnimate)
					OnAnimate();
				m_spinAngle = 0.0;
				pFrameWnd->m_Progress.SetPos(0);
				m_pInlet->m_pProgress = NULL;
				resetPaticles(CONFIG_RANDOM,0,numParticles);//reset the aphla values
				beingInit=CONFIG_INLET_RANDOM;
				initCount=0;
				// 				if(!resetPaticles(CONFIG_INLET_RANDOM))
				// 					AfxMessageBox("No tetrahedron is found.");
				Invalidate();
				SetCursor(hOldCursor);
			}
			fclose(fp);
		}
	}
}


//void  CChildView::UpdateParticles()
//{
//	for (int i = 0; i < particles.size(); i ++)
//	{
//		particles[i]->p[0] = particles[i]->p[0] + 0.01*particles[i]->velocity*particles[i]->velvector[0];
//		particles[i]->p[1] = particles[i]->p[1] + 0.01*particles[i]->velocity*particles[i]->velvector[1];
//		particles[i]->p[2] = particles[i]->p[2] + 0.01*particles[i]->velocity*particles[i]->velvector[2];
//
//		if (!is_in_BoundingBox(particles[i]->p))
//		{
//			InitParticle(particles[i]);
//		}
//		else
//		{
//			Point_3 fp = Point_3(particles[i]->p[0], particles[i]->p[1], particles[i]->p[2]);
//			MyDelaunay::Locate_type lt;
//			int li, lj;
//			MyDelaunay::Vertex_handle inserted;
//			MyDelaunay::Cell_handle ch = delaunay_->all_cells_[0];
//			MyDelaunay::Cell_handle loc = delaunay_->locate(fp, lt, li, lj, ch);
//
//			if (lt == MyDelaunay::CELL)
//			{
//				int vindex0 = loc->vertex(0)->idx();
//				int vindex1 = loc->vertex(1)->idx();
//				int vindex2 = loc->vertex(2)->idx();
//				int vindex3 = loc->vertex(3)->idx();
//
//				float t1, t2, t3, t4;
//				InterpolateTet(particles[i]->p, vindex0, vindex1, vindex2, vindex3, t1, t2, t3, t4);
//				particles[i]->velocity = t1*obj->v_wind[vindex0][3] + t2*obj->v_wind[vindex1][3] + t3*obj->v_wind[vindex2][3] + t4*obj->v_wind[vindex3][3];
//				particles[i]->velvector[0] = t1*obj->v_wind[vindex0][0] + t2*obj->v_wind[vindex1][0] + t3*obj->v_wind[vindex2][0] + t4*obj->v_wind[vindex3][0];
//				particles[i]->velvector[1] = t1*obj->v_wind[vindex0][1] + t2*obj->v_wind[vindex1][1] + t3*obj->v_wind[vindex2][1] + t4*obj->v_wind[vindex3][1];
//				particles[i]->velvector[2] = t1*obj->v_wind[vindex0][2] + t2*obj->v_wind[vindex1][2] + t3*obj->v_wind[vindex2][2] + t4*obj->v_wind[vindex3][2];
//				particles[i]->cur_cell = loc;
//			}
//			else if (lt == MyDelaunay::VERTEX)
//			{
//				int vindex0 = loc->vertex(li)->idx();
//				particles[i]->velocity = obj->v_wind[vindex0][3];
//				particles[i]->velvector[0] = obj->v_wind[vindex0][0];
//				particles[i]->velvector[1] = obj->v_wind[vindex0][1];
//				particles[i]->velvector[2] = obj->v_wind[vindex0][2];
//				particles[i]->cur_cell = loc;
//			}
//			else if (lt == MyDelaunay::EDGE)
//			{
//				int vindex0 = loc->vertex(li)->idx();
//				int vindex1 = loc->vertex(lj)->idx();
//
//				float x1, y1, z1, x2, y2, z2;
//				x1 = obj->vert_co[vindex0][0];
//				y1 = obj->vert_co[vindex0][1];
//				z1 = obj->vert_co[vindex0][2];
//
//				x2 = obj->vert_co[vindex1][0];
//				y2 = obj->vert_co[vindex1][1];
//				z2 = obj->vert_co[vindex1][2];
//
//				float T = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
//				float t1 = sqrt((particles[i]->p[0] - x1)*(particles[i]->p[0] - x1) + (particles[i]->p[1] - y1)*(particles[i]->p[1] - y1) + (particles[i]->p[2] - z1)*(particles[i]->p[2] - z1));
//				float t2 = 1 - t1;
//				particles[i]->velocity = t1*obj->v_wind[vindex0][3] + t2*obj->v_wind[vindex1][3];
//				particles[i]->velvector[0] = t1*obj->v_wind[vindex0][0] + t2*obj->v_wind[vindex1][0];
//				particles[i]->velvector[1] = t1*obj->v_wind[vindex0][1] + t2*obj->v_wind[vindex1][1];
//				particles[i]->velvector[2] = t1*obj->v_wind[vindex0][2] + t2*obj->v_wind[vindex1][2];
//				particles[i]->cur_cell = loc;
//			}
//			else if (lt == MyDelaunay::FACET)
//			{
//				int vindex0 = loc->vertex((i+1)&3)->idx();
//				int vindex1 = loc->vertex((i+2)&3)->idx();
//				int vindex2 = loc->vertex((i+3)&3)->idx();
//
//				float t1, t2, t3;
//				InterpolateTri(particles[i]->p, vindex0, vindex1, vindex2, t1, t2, t3);
//				particles[i]->velocity = t1*obj->v_wind[vindex0][3] + t2*obj->v_wind[vindex1][3] + t3*obj->v_wind[vindex2][3];
//				particles[i]->velvector[0] = t1*obj->v_wind[vindex0][0] + t2*obj->v_wind[vindex1][0] + t3*obj->v_wind[vindex2][0];
//				particles[i]->velvector[1] = t1*obj->v_wind[vindex0][1] + t2*obj->v_wind[vindex1][1] + t3*obj->v_wind[vindex2][1];
//				particles[i]->velvector[2] = t1*obj->v_wind[vindex0][2] + t2*obj->v_wind[vindex1][2] + t3*obj->v_wind[vindex2][2];
//				particles[i]->cur_cell = loc;
//			}
//			else
//			{
//				particles[i]->velocity = 1;
//				particles[i]->velvector[0] = 0;
//				particles[i]->velvector[1] = 0;
//				particles[i]->velvector[2] = 1;
//			}
//		}
//	}
//}


BOOL  CChildView::is_in_BoundingBox(Point p)
{
	BOOL res;

	if ((p[0] > obj->boud_box[0]) && (p[0] < obj->boud_box[1]) 
		&& (p[1] > obj->boud_box[2]) && (p[1] < obj->boud_box[3]) 
		&& (p[2] > obj->boud_box[4]) && (p[2] < obj->boud_box[5]))
	{
		res = TRUE;
	}
	else
		res = FALSE;

	return res;
}

void  CChildView::InterpolateTet(Point p, int v0, int v1, int v2, int v3, float &t1, float &t2, float &t3, float &t4)
{
	double x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
	x1 = obj->vert_co[3*v0];
	y1 = obj->vert_co[3*v0+1];
	z1 = obj->vert_co[3*v0+2];

	x2 = obj->vert_co[3*v1];
	y2 = obj->vert_co[3*v1+1];
	z2 = obj->vert_co[3*v1+2];

	x3 = obj->vert_co[3*v2];
	y3 = obj->vert_co[3*v2+1];
	z3 = obj->vert_co[3*v2+2];

	x4 = obj->vert_co[3*v3];
	y4 = obj->vert_co[3*v3+1];
	z4 = obj->vert_co[3*v3+2];

	double T = fabs(det3x3(x2 - x1, y2 - y1, z2 - z1, x3 - x1, y3 - y1, z3 - z1, x4 - x1, y4 - y1, z4 - z1));
	t4 = fabs(det3x3(x1 - p[0], y1 - p[1], z1 - p[2], x2 - p[0], y2 - p[1], z2 - p[2], x3 - p[0], y3 - p[1], z3 - p[2])) / T;
	t3 = fabs(det3x3(x1 - p[0], y1 - p[1], z1 - p[2], x2 - p[0], y2 - p[1], z2 - p[2], x4 - p[0], y4 - p[1], z4 - p[2])) / T;
	t2 = fabs(det3x3(x1 - p[0], y1 - p[1], z1 - p[2], x3 - p[0], y3 - p[1], z3 - p[2], x4 - p[0], y4 - p[1], z4 - p[2])) / T;
	t1 = fabs(det3x3(x2 - p[0], y2 - p[1], z2 - p[2], x3 - p[0], y3 - p[1], z3 - p[2], x4 - p[0], y4 - p[1], z4 - p[2])) / T;
}

void  CChildView::InterpolateTri(Point p, int v0, int v1, int v2, float &t1, float &t2, float &t3)
{
	double x1, x2, x3, y1, y2, y3, y4, z1, z2, z3;
	x1 = obj->vert_co[3*v0];
	y1 = obj->vert_co[3*v0+1];
	z1 = obj->vert_co[3*v0+2];

	x2 = obj->vert_co[3*v1];
	y2 = obj->vert_co[3*v1+1];
	z2 = obj->vert_co[3*v1+2];

	x3 = obj->vert_co[3*v2];
	y3 = obj->vert_co[3*v2+1];
	z3 = obj->vert_co[3*v2+2];

	double pp, a, b, c;
	a = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
	b = sqrt((x3 - x1)*(x3 - x1) + (y3 - y1)*(y3 - y1) + (z3 - z1)*(z3 - z1));
	c = sqrt((x2 - x3)*(x2 - x3) + (y2 - y3)*(y2 - y3) + (z2 - z3)*(z2 - z3));
	pp=(a+b+c)/2;
	double T = (pp*(pp-a)*(pp-b)*(pp-c));

	a = sqrt((x2 - p[0])*(x2 - p[0]) + (y2 - p[1])*(y2 - p[1]) + (z2 - p[2])*(z2 - p[2]));
	b = sqrt((x3 - p[0])*(x3 - p[0]) + (y3 - p[1])*(y3 - p[1]) + (z3 - p[2])*(z3 - p[2]));
	c = sqrt((x2 - x3)*(x2 - x3) + (y2 - y3)*(y2 - y3) + (z2 - z3)*(z2 - z3));
	pp=(a+b+c)/2;
	t1 = (pp*(pp-a)*(pp-b)*(pp-c)) / T;

	a = sqrt((x1 - p[0])*(x1 - p[0]) + (y1 - p[1])*(y1 - p[1]) + (z1 - p[2])*(z1 - p[2]));
	b = sqrt((x3 - p[0])*(x3 - p[0]) + (y3 - p[1])*(y3 - p[1]) + (z3 - p[2])*(z3 - p[2]));
	c = sqrt((x1 - x3)*(x1 - x3) + (y1 - y3)*(y1 - y3) + (z1 - z3)*(z1 - z3));
	pp=(a+b+c)/2;
	t2 = (pp*(pp-a)*(pp-b)*(pp-c)) / T;

	a = sqrt((x2 - p[0])*(x2 - p[0]) + (y2 - p[1])*(y2 - p[1]) + (z2 - p[2])*(z2 - p[2]));
	b = sqrt((x1 - p[0])*(x1 - p[0]) + (y1 - p[1])*(y1 - p[1]) + (z1 - p[2])*(z1 - p[2]));
	c = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));
	pp=(a+b+c)/2;
	t3 = (pp*(pp-a)*(pp-b)*(pp-c)) / T;
}

double CChildView::det3x3(double a00,  double a01,  double a02,
						  double a10,  double a11,  double a12,
						  double a20,  double a21,  double a22)
{
	double m01 = a00*a11 - a10*a01;
	double m02 = a00*a21 - a20*a01;
	double m12 = a10*a21 - a20*a11;
	return m01*a22 - m02*a12 + m12*a02;
}
BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return true;
	//	return CWnd::OnEraseBkgnd(pDC);
}

void CChildView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
	m_bKeyDown=FALSE;
	char nChar2=nChar;
	if (!GetAsyncKeyState(VK_LSHIFT)&&nChar2<='Z'&&nChar2>='A')
	{
		nChar2+='a'-'A';
	}
	keyUp(nChar2,0,0);
	Invalidate();
}

void CChildView::BuildDisplayListWindfield(void)
{
	glNewList (m_wind_list_id, GL_COMPILE);
	RenderWindfield();
	glEndList ();
	GLenum gl_error = glGetError();
	if (gl_error != GL_NO_ERROR) 
	{
		//CString str;
		//str.Format(_T("Build wind field error:%s"),gluErrorString(gl_error));
		//AfxMessageBox(str);
		m_glError_Windfield=TRUE;
		glDeleteLists(m_glError_Windfield,1);
	}else
	{
		m_glError_Windfield=FALSE;
	}
}

void CChildView::BuildDisplayListBuilding(void)
{
	glNewList (m_building_list_id, GL_COMPILE);
	RenderBuilding();
	glEndList ();
	GLenum gl_error = glGetError();
	if (gl_error != GL_NO_ERROR) 
	{
		//CString str;
		//str.Format(_T("Build building error:%s"),gluErrorString(gl_error));
		//AfxMessageBox(str);
		m_glError_Building=TRUE;
		glDeleteLists(m_building_list_id,1);
	}else
	{
		m_glError_Building=FALSE;
	}
}

void CChildView::BuildDisplayListBounding(void)
{
	glNewList (m_bounding_list_id, GL_COMPILE);
	RenderBounding();
	glEndList ();
	GLenum gl_error = glGetError();
	if (gl_error != GL_NO_ERROR) 
	{
		//CString str;
		//str.Format(_T("Build building error:%s"),gluErrorString(gl_error));
		//AfxMessageBox(str);
		m_glError_Bounding=TRUE;
		glDeleteLists(m_bounding_list_id,1);
	}else
	{
		m_glError_Bounding=FALSE;
	}
}

void CChildView::BuildDisplayListAirTet(void)
{
	if (obj == NULL)
		return;
	//glDisable(GL_LIGHTING);
	glNewList (m_airtet_list_id, GL_COMPILE);
	RenderAirTet();
	glEndList ();
	GLenum gl_error = glGetError();
	if (gl_error != GL_NO_ERROR) 
	{
		//CString str;
		//str.Format(_T("Build air tet error:%s"),gluErrorString(gl_error));
		//AfxMessageBox(str);
		m_glError_AirTet=TRUE;
		glDeleteLists(m_airtet_list_id,1);
	}else
	{
		m_glError_AirTet=FALSE;
	}
}
void CChildView::RenderAirTetTriangle(CColorRamp& colormap,int indices0,int indices1,int indices2)
{
	glBegin(GL_TRIANGLES);
	if (m_bPressure)
	{
		float red, green, blue;
		float texcoord = (obj->p[indices0] - obj->pres_min_max[0]) / (obj->pres_min_max[1] - obj->pres_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
	}
	if (m_bVelocity)
	{
		float red, green, blue;
		float texcoord = (obj->v_wind[4*indices0+3] - obj->v_win_min_max[0]) / (obj->v_win_min_max[1] - obj->v_win_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
	}
	glVertex3f(obj->vert_co[3*indices0],obj->vert_co[3*indices0+1],obj->vert_co[3*indices0+2]);

	if (m_bPressure)
	{
		float red, green, blue;
		float texcoord = (obj->p[indices1] - obj->pres_min_max[0]) / (obj->pres_min_max[1] - obj->pres_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
	}
	if (m_bVelocity)
	{
		float red, green, blue;
		float texcoord = (obj->v_wind[4*indices1+3] - obj->v_win_min_max[0]) / (obj->v_win_min_max[1] - obj->v_win_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
	}
	glVertex3f(obj->vert_co[3*indices1],obj->vert_co[3*indices1+1],obj->vert_co[3*indices1+2]);

	if (m_bPressure)
	{
		float red, green, blue;
		float texcoord = (obj->p[indices2] - obj->pres_min_max[0]) / (obj->pres_min_max[1] - obj->pres_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
	}
	if (m_bVelocity)
	{
		float red, green, blue;
		float texcoord = (obj->v_wind[4*indices2+3] - obj->v_win_min_max[0]) / (obj->v_win_min_max[1] - obj->v_win_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );
		glColor3f( red, green, blue );
	}
	glVertex3f(obj->vert_co[3*indices2],obj->vert_co[3*indices2+1],obj->vert_co[3*indices2+2]);
	glEnd();
}
void CChildView::RenderAirTet(void)
{
	if (obj == NULL)
		return;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	CColorRamp colormap;
	if (!m_bPressure && !m_bVelocity)
		glColor3f(1.f, 0.f, 1.f);
	else
	{
		colormap.BuildRainbow();
	}
	for(int i=0; i < obj->size_tet_in_ord(); i++)
	{
		//////////////////////////////////////////////////////////////////////////
		// 		bool showThis=false;
		// 		for (int nid=0;nid<4;nid++)
		// 		{
		// 			if (obj->tet_neigb[4*i+nid]==-1)
		// 			{
		// 				showThis=true;
		// 				break;
		// 			}
		// 		}
		// 		if (!showThis)
		// 		{
		// 			continue;
		// 		}
		//////////////////////////////////////////////////////////////////////////
		//glPolygonMode(GL_FRONT,GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		int indices0, indices1, indices2;
		indices0=obj->tet_in_ord[4*i];
		indices1=obj->tet_in_ord[4*i+1];
		indices2=obj->tet_in_ord[4*i+2];
		RenderAirTetTriangle(colormap,indices0, indices1, indices2);

		indices0=obj->tet_in_ord[4*i+1];
		indices1=obj->tet_in_ord[4*i+3];
		indices2=obj->tet_in_ord[4*i+2];
		RenderAirTetTriangle(colormap,indices0, indices1, indices2);

		indices0=obj->tet_in_ord[4*i];
		indices1=obj->tet_in_ord[4*i+2];
		indices2=obj->tet_in_ord[4*i+3];
		RenderAirTetTriangle(colormap,indices0, indices1, indices2);

		indices0=obj->tet_in_ord[4*i];
		indices1=obj->tet_in_ord[4*i+3];
		indices2=obj->tet_in_ord[4*i+1];
		RenderAirTetTriangle(colormap,indices0, indices1, indices2);
	}


	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glColor3f ( 1.0f, 1.0f, 0.0f);
}

void CChildView::RenderBuilding(void)
{
	if (m_pMesh3D)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0, 0.01, 0);
		//scale=2.0/(m_pMesh3D->xmax-m_pMesh3D->xmin);

		//offsetX=-m_pMesh3D->xmin;
		//offsetY=-m_pMesh3D->ymin+0.1;
		//offsetZ=-m_pMesh3D->zmin;
		//glScalef(scale,scale,scale);
		//glTranslatef(offsetX, offsetY, offsetZ);

		/*glColor3f(0.6,0.0,0.0);
		//glBegin(GL_TRIANGLES);
		//	glVertex3f(m_pMesh3D->xmin,m_pMesh3D->ymin,m_pMesh3D->zmin);
		//	glVertex3f(m_pMesh3D->xmin,m_pMesh3D->ymax,m_pMesh3D->zmin);
		//	glVertex3f(m_pMesh3D->xmax,m_pMesh3D->ymax,m_pMesh3D->zmin);
		//	glEnd();
		glLineWidth(5.f);
		// x_min,x_max,y_min,y_max,z_min,z_max
		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glEnd();

		glLineWidth(1.f);*/
		glColor3f(0.6,0.6,0.6);
		glEnable(GL_LIGHTING);
		m_pMesh3D->DrawModel(m_nRenderMode, m_nRenderModel, m_FlatNormal);
		glDisable(GL_LIGHTING);

		glPopMatrix();
	}

}

void CChildView::RenderBounding(void)
{
	if (m_pBound)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0, 0.01, 0);
		//scale=2.0/(m_pMesh3D->xmax-m_pMesh3D->xmin);

		//offsetX=-m_pMesh3D->xmin;
		//offsetY=-m_pMesh3D->ymin+0.1;
		//offsetZ=-m_pMesh3D->zmin;
		//glScalef(scale,scale,scale);
		//glTranslatef(offsetX, offsetY, offsetZ);

		/*glColor3f(0.6,0.0,0.0);
		//glBegin(GL_TRIANGLES);
		//	glVertex3f(m_pMesh3D->xmin,m_pMesh3D->ymin,m_pMesh3D->zmin);
		//	glVertex3f(m_pMesh3D->xmin,m_pMesh3D->ymax,m_pMesh3D->zmin);
		//	glVertex3f(m_pMesh3D->xmax,m_pMesh3D->ymax,m_pMesh3D->zmin);
		//	glEnd();
		glLineWidth(5.f);
		// x_min,x_max,y_min,y_max,z_min,z_max
		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmin, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmin);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymax, m_pMesh3D->zmax);
		glVertex3f(m_pMesh3D->xmax, m_pMesh3D->ymin, m_pMesh3D->zmax);
		glEnd();

		glLineWidth(1.f);*/
		glColor3f(0.6,0.6,0.6);
		glEnable(GL_LIGHTING);
		m_pBound->DrawModel(m_nRenderMode, m_nRenderModel, m_FlatNormal);
		glDisable(GL_LIGHTING);

		glPopMatrix();
	}

}

void CChildView::RenderWindfield(void)
{
	CColorRamp colormap;
	colormap.BuildRainbow();

	float red, green, blue,old_red=-1,old_green=-1,old_blue=-1;
	if (obj == NULL)
		return;
	GLUquadricObj *quadratic = gluNewQuadric();
	//glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glPolygonMode(GL_FRONT,GL_FILL);
	float wind[3];
	for (int i = 0; i < obj->size_vert_co(); i ++)
	{
		float texcoord = (obj->v_wind[4*i+3] - obj->v_win_min_max[0]) / (obj->v_win_min_max[1] - obj->v_win_min_max[0]) * 255;
		red = min( max((float)colormap.Red(texcoord)/255.f, 0.f), 1.f );
		green = min( max((float)colormap.Green(texcoord)/255.f, 0.f), 1.f );
		blue = min( max((float)colormap.Blue(texcoord)/255.f, 0.f), 1.f );

		if (red!=old_red||green!=old_green||blue!=old_blue)
		{
			old_red=red;
			old_green=green;
			old_blue=blue;
			glColor3f( red, green, blue );
		}

		wind[0]=obj->v_wind[4*i];
		wind[1]=obj->v_wind[4*i+1];
		wind[2]=obj->v_wind[4*i+2];
		if (obj->v_wind[4*i+3]>1e-6)
		{
			wind[0]/=obj->v_wind[4*i+3];
			wind[1]/=obj->v_wind[4*i+3];
			wind[2]/=obj->v_wind[4*i+3];
		}
		glBegin(GL_LINES);
		glVertex3f(obj->vert_co[3*i], obj->vert_co[3*i+1], obj->vert_co[3*i+2]);
		glVertex3f(obj->vert_co[3*i] + 0.15*wind[0], obj->vert_co[3*i+1] + 0.15*wind[1], obj->vert_co[3*i+2] + 0.15*wind[2]);
		glEnd();

		if (m_bArrow)
		{


			glPushMatrix();
			double angle = acos(obj->v_wind[4*i+2]/obj->v_wind[4*i+3]) / (2*M_PI) * 360;
			double rotx = -obj->v_wind[4*i+1]/obj->v_wind[4*i+3];
			double roty = obj->v_wind[4*i]/obj->v_wind[4*i+3];
			double rotz = 0;
			rotx = rotx / sqrt(rotx*rotx + roty*roty);
			roty = roty / sqrt(rotx*rotx + roty*roty);
			glTranslatef(obj->vert_co[3*i] + 0.15*wind[0], obj->vert_co[3*i+1] + 0.15*wind[1], obj->vert_co[3*i+2] + 0.15*wind[2]);
			glRotatef(angle, rotx, roty, rotz);
			gluCylinder(quadratic,0.01f,0.0f,0.05f,4,1);

			glPopMatrix();
		}
	}
	gluDeleteQuadric(quadratic);
	glPopMatrix();
	//glEnable(GL_LIGHTING);
	glColor3f ( 1.0f, 1.0f, 0.0f );
}

void CChildView::OnShowPath(void)
{
	m_bPath = !m_bPath;
	Invalidate();
}

void CChildView::OnParticlesourceShowsource()
{
	// TODO: Add your command handler code here
	m_bShowParticleSource=!m_bShowParticleSource;
	// 	if (m_bShowParticleSource)
	// 		mode=M_MOVE_SOURCE;
	// 	else
	// 		mode=M_VIEW;	
	Invalidate();
}

void CChildView::OnUpdateParticlesourceShowsource(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bShowParticleSource);
}
void CChildView::OnShowTargetCursor()
{
	// TODO: Add your command handler code here
	m_bShowtargetCursor=!m_bShowtargetCursor;	
	Invalidate();
}
void CChildView::OnUpdateShowTargetCursor(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bShowtargetCursor);
}
void CChildView::OnParticlesourceInitparticles()
{
	// TODO: Add your command handler code here
	//initparticlesInShpere();
	resetPaticles(CONFIG_RANDOM,0,numParticles);//reset the aphla values
	beingInit=CONFIG_INLET_RANDOM;
	initCount=0;
	initparticlesInShpere(0,numParticles/initBatchN);
}

void CChildView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMButtonDown(nFlags, point);
	mouse(GLUT_MIDDLE_BUTTON, 0, point.x, point.y);
	Invalidate();
}

void CChildView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnMButtonUp(nFlags, point);
	mouse(GLUT_MIDDLE_BUTTON, 1, point.x, point.y);
	Invalidate();
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonDown(nFlags, point);
	mouse(GLUT_RIGHT_BUTTON, 0, point.x, point.y);
	Invalidate();
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp(nFlags, point);
	mouse(GLUT_RIGHT_BUTTON, 1, point.x, point.y);
	Invalidate();
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	m_bKeyDown=TRUE;
	char nChar2=nChar;
	if (!GetAsyncKeyState(VK_LSHIFT)&&nChar2<='Z'&&nChar2>='A')
	{
		nChar2+='a'-'A';
	}
	key(nChar2,0,0);
	Invalidate();
}

void CChildView::OnParticlesourceRandom()
{
	// TODO: Add your command handler code here
	//resetPaticles(CONFIG_AIR_RANDOM);
	resetPaticles(CONFIG_RANDOM,0,numParticles);//reset the aphla values
	beingInit=CONFIG_AIR_RANDOM;
	initCount=0;
	Invalidate();
}

void CChildView::OnParticlesourceRandomVolumn()
{
	// TODO: Add your command handler code here
	//initparticlesInShpere();
	resetPaticles(CONFIG_RANDOM,0,numParticles);//reset the aphla values
	beingInit=CONFIG_INLET_RANDOM;
	initCount=0;
	initparticlesInRandomVolumn(0,numParticles/initBatchN);
}

void CChildView::OnParticlesourceRandomPlain()
{
	// TODO: Add your command handler code here
	//initparticlesInShpere();
	resetPaticles(CONFIG_RANDOM,0,numParticles);//reset the aphla values
	beingInit=CONFIG_INLET_RANDOM;
	initCount=0;
	initparticlesInRandomPlain(0,numParticles/initBatchN);
}

void CChildView::OnUpdateMousemodeMovelight(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(mode==M_MOVE_LIGHT);
}

void CChildView::OnUpdateMousemodeView(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(mode==M_VIEW);
}

void CChildView::OnUpdateMousemodeMovesource(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(mode==M_MOVE_SOURCE);
}
void CChildView::OnUpdate3D(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(bStereo);
}
void CChildView::OnUpdateShowHelp(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(displaySliders);
}
void CChildView::OnMousemodeMovecursor()
{
	// TODO: Add your command handler code here
	m_bShowtargetCursor=TRUE;
	mode=M_MOVE_CURSOR;
}

void CChildView::OnUpdateMousemodeMovecursor(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(mode==M_MOVE_CURSOR);
}

void CChildView::OnMousemodeMoveactivearea()
{
	// TODO: Add your command handler code here
	m_bShowActiveArea=TRUE;
	mode=M_MOVE_ACTIVEAREA;
}

void CChildView::OnUpdateMousemodeMoveactivearea(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(mode==M_MOVE_ACTIVEAREA);
}

void CChildView::OnViewActivearea()
{
	// TODO: Add your command handler code here
	m_bShowActiveArea = !m_bShowActiveArea;
	Invalidate();
}

void CChildView::OnUpdateViewActivearea(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_bShowActiveArea);
}

void CChildView::OnMousemodeScaleactivearea()
{
	// TODO: Add your command handler code here
	m_bShowActiveArea=TRUE;
	mode=M_SCALE_ACTIVEAREA;
}

void CChildView::OnUpdateMousemodeScaleactivearea(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(mode==M_SCALE_ACTIVEAREA);
}
