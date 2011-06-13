// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__BDDF956A_C67B_11D3_91F5_0000E83E7EBA__INCLUDED_)
#define AFX_CHILDVIEW_H__BDDF956A_C67B_11D3_91F5_0000E83E7EBA__INCLUDED_

#include "Transform.h"
#include "Mesh3D.h"
#include "dat.h"
#include "ColorRamp.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	MOUSE_SPIN		1
#define	MOUSE_TRANSLATE	2
#define	MOUSE_ZOOM		3

#define	TIMER_ANIMATE	1
#define	TIMER_TOOLBAR	2
#define	TIMER_SIMULATION	3
#define	ANIMATE_RATE	50
#define	TOOLBAR_RATE	20
#define	ROTATE_RATE		4.0f
#define	ZOOM_RATE		0.05f

/////////////////////////////////////////////////////////////////////////////
// CChildView window



class CChildView : public CWnd
{
	// Construction
public:
	CChildView();

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
public:
	BOOL SaveAsBMP(char *bmpFileName);
	void DrawAxes();
	void Init();
	// 	void Translate(float d[3]);
	// 	void MouseTranslate(UINT nFlags, int x, int y);
	// 	void	MouseZoom(UINT nFlags, int x, int y);
	// 	void	Zoom(float scale);
	void	MouseSpinGlobal(UINT nFlags, int x, int y, int init);
	void	SpinGlobal(float axes[3], float angle);
	void	DrawScene();
	void	Project();

	void    DrawBoundingBox();

	//void    UpdateParticles();
	void    InterpolateTet(Point p, int v0, int v1, int v2, int v3, float &t1, float &t2, float &t3, float &t4);
	void    InterpolateTri(Point p, int v0, int v1, int v2, float &t1, float &t2, float &t3);
	double  det3x3(double a00,  double a01,  double a02,
		double a10,  double a11,  double a12,
		double a20,  double a21,  double a22);

	BOOL    is_in_BoundingBox(Point p);
	virtual	~CChildView();

public:
	HDC		m_hDc;
	HGLRC	m_hRc;
	float	m_fViewWidth, m_fViewHeight;
	float	m_fViewNear, m_fViewFar;

	BOOL	m_bLButtonDown;
	BOOL	m_bKeyDown;
	int		m_nMouseAct;
	BOOL	m_bAnimate;
	BOOL	m_bSimulate;
	BOOL	m_bAxes;
	BOOL    m_bAirTet;
	BOOL    m_bPath;
	BOOL    m_bBildings;
	BOOL	m_bBoudings;
	BOOL    read_air_in;
	BOOL    m_bWindfield;
	BOOL	m_bShowParticleSource;
	BOOL	m_bShowActiveArea;
	BOOL	m_bShowtargetCursor;
	BOOL    m_bPressure;
	BOOL    m_bVelocity;
	BOOL    m_bArrow;
	BOOL    m_bGround;
	BOOL    m_bParticleInit;

	BOOL	m_glError_Windfield;
	BOOL	m_glError_Building;
	BOOL	m_glError_Bounding;
	BOOL	m_glError_AirTet;

	float	m_vProjectCenter[3];
	EYE		m_sEye;
	float	m_spinAxes[3];
	float	m_spinAngle;

	int		m_nRenderMode,m_nRenderModel;
	int		m_nFaceNo;
	int		m_nsFaceNo;

	Mesh3D  * m_pMesh3D;
	Mesh3D	* m_pBound;
	Mesh3D  * m_pInlet;

	int     m_FlatNormal;
	int     m_Particlenum;
	unsigned int m_wind_list_id ;
	unsigned int m_building_list_id ;
	unsigned int m_bounding_list_id ;
	unsigned int m_airtet_list_id ;	


protected:
	void SetupBmpHeader(BITMAPINFOHEADER *pbmih, int sx, int sy, int bpp);
	BOOL	SetThePixelFormat(CDC * pDC);

	// Generated message map functions
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg int  OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTimer( UINT nIDEvent );
	//	afx_msg void OnContextMenu( CWnd* pWnd, CPoint pos );
	afx_msg void OnMouseSpin();
	afx_msg void OnUpdateMouseSpin(CCmdUI* pCmdUI);
	afx_msg void OnMouseTranslate();
	afx_msg void OnUpdateMouseTranslate(CCmdUI* pCmdUI);
	afx_msg void OnMouseZoom();
	afx_msg void OnUpdateMouseZoom(CCmdUI* pCmdUI);
	afx_msg void OnAnimate();
	afx_msg void OnUpdateAnimate(CCmdUI* pCmdUI);
	afx_msg void OnSpinXccw();
	afx_msg void OnSpinXcw();
	afx_msg void OnSpinYccw();
	afx_msg void OnSpinYcw();
	afx_msg void OnSpinZccw();
	afx_msg void OnSpinZcw();
	//afx_msg void OnZoomIn();
	//afx_msg void OnZoomOut();
	afx_msg void OnOpenFile();
	afx_msg void OnOriginalModel();
	afx_msg void OnUpdateOriginalModel(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSimplifiedModel(CCmdUI* pCmdUI);
	afx_msg void OnWireframe();
	afx_msg void OnUpdateWireframe(CCmdUI* pCmdUI);
	afx_msg void OnFlat();
	afx_msg void OnUpdateFlat(CCmdUI* pCmdUI);
	afx_msg void OnAxes();
	afx_msg void OnUpdateAxes(CCmdUI* pCmdUI);
	afx_msg void OnSavePicture();
	afx_msg void OnSimStart();
	afx_msg void OnSimStop();
	afx_msg void OnShowParticles();
	afx_msg void OnMoveView();
	afx_msg void OnMoveLight();
	afx_msg void OnMoveGenerator();
	afx_msg void OnShowHelp();
	afx_msg void OnShowSkybox();
	afx_msg void OnShowTemprature();

	afx_msg void OnSimInit();
	afx_msg void On3D();
	afx_msg void OnShowBuildings();
	afx_msg void OnShowBoundings();
	afx_msg void OnShowGround();
	afx_msg void OnShowAirtet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileLoadairdata();
	afx_msg void OnFileLoadairdataBin();
	afx_msg void OnFileReadoutbound();
	afx_msg void OnFileSaveairdataBin();
	afx_msg void OnAirmodeWindfeild();
	afx_msg void OnUpdateAirmodeWindfeild(CCmdUI *pCmdUI);
	afx_msg void OnAirmodeWitharrow();
	afx_msg void OnUpdateAirmodeWitharrow(CCmdUI *pCmdUI);
	afx_msg void OnAirmodeWithpressure();
	afx_msg void OnUpdateAirmodeWithpressure(CCmdUI *pCmdUI);
	afx_msg void OnAirmodeWithvelocity();
	afx_msg void OnUpdateAirmodeWithvelocity(CCmdUI *pCmdUI);
	afx_msg void OnFileLoadinlet();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void BuildDisplayListWindfield(void);
	void BuildDisplayListBuilding(void);
	void BuildDisplayListBounding(void);
	void BuildDisplayListAirTet(void);
	void RenderAirTet(void);
	void RenderAirTetTriangle(CColorRamp& colormap,int indices0,int indices1,int indices2);
	void RenderBuilding(void);
	void RenderBounding(void);
	void RenderWindfield(void);
	void OnShowPath(void);
	afx_msg void OnParticlesourceShowsource();
	afx_msg void OnShowTargetCursor();
	afx_msg void OnUpdateParticlesourceShowsource(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowTargetCursor(CCmdUI *pCmdUI);
	afx_msg void OnParticlesourceInitparticles();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnParticlesourceRandom();
	afx_msg void OnParticlesourceRandomVolumn();
	afx_msg void OnParticlesourceRandomPlain();
	afx_msg void OnUpdateMousemodeMovelight(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMousemodeView(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMousemodeMovesource(CCmdUI *pCmdUI);
	afx_msg void OnUpdate3D(CCmdUI *pCmdUI);
	afx_msg void OnUpdateShowHelp(CCmdUI *pCmdUI);

	afx_msg void OnMousemodeMovecursor();
	afx_msg void OnUpdateMousemodeMovecursor(CCmdUI *pCmdUI);
	afx_msg void OnMousemodeMoveactivearea();
	afx_msg void OnUpdateMousemodeMoveactivearea(CCmdUI *pCmdUI);
	afx_msg void OnViewActivearea();
	afx_msg void OnUpdateViewActivearea(CCmdUI *pCmdUI);
	afx_msg void OnMousemodeScaleactivearea();
	afx_msg void OnUpdateMousemodeScaleactivearea(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__BDDF956A_C67B_11D3_91F5_0000E83E7EBA__INCLUDED_)
