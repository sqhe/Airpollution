; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CChildView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "simplifier.h"
LastPage=0

ClassCount=5
Class1=CChildView
Class2=CMainFrame
Class3=CSimplifierApp
Class4=CAboutDlg
Class5=CSimplifyDiag

ResourceCount=12
Resource1=IDR_INTERACT_RIGHT
Resource2=IDR_INTERACT_LEFT
Resource3=IDR_MAINFRAME
Resource4=IDR_CONTEXTMENU
Resource5=IDD_SIMPLIFY
Resource6=IDD_ABOUTBOX
Resource7=IDD_SIMPLIFY (Chinese (P.R.C.))
Resource8=IDR_INTERACT_RIGHT (Chinese (P.R.C.))
Resource9=IDR_INTERACT_LEFT (Chinese (P.R.C.))
Resource10=IDR_CONTEXTMENU (Chinese (P.R.C.))
Resource11=IDR_MAINFRAME (Chinese (P.R.C.))
Resource12=IDD_ABOUTBOX (Chinese (P.R.C.))

[CLS:CChildView]
Type=0
BaseClass=CWnd 
HeaderFile=ChildView.h
ImplementationFile=ChildView.cpp
LastObject=ID_EDGECOLLAPSE_SVE
Filter=W
VirtualFilter=WC

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=CMainFrame

[CLS:CSimplifierApp]
Type=0
BaseClass=CWinApp
HeaderFile=Simplifier.h
ImplementationFile=Simplifier.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Simplifier.cpp
ImplementationFile=Simplifier.cpp
LastObject=CAboutDlg

[CLS:CSimplifyDiag]
Type=0
BaseClass=CDialog
HeaderFile=SimplifyDiag.h
ImplementationFile=SimplifyDiag.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

[DLG:IDD_SIMPLIFY]
Type=1
Class=CSimplifyDiag
ControlCount=8
Control1=IDOK,button,1342373889
Control2=IDCANCEL,button,1342242816
Control3=IDC_FACE_VALUE,edit,1350631552
Control4=IDC_RATE_VALUE,edit,1350631552
Control5=IDC_STATIC,static,1342177280
Control6=IDC_TEXT,static,1342177280
Control7=IDC_FACE,button,1342308361
Control8=IDC_RATE,button,1342177289

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_OPEN_FILE
Command2=ID_CLOSE_FILE
Command3=ID_SAVE_FILE
Command4=IDM_SAVE_PICTURE
Command5=ID_APP_EXIT
Command6=IDM_ORIGINAL_MODEL
Command7=IDM_SIMPLIFIED_MODEL
Command8=IDM_WIREFRAME
Command9=IDM_FLAT
Command10=ID_FLATNORMAL
Command11=ID_POINTNORMAL
Command12=ID_EDGECOLLAPSE
Command13=ID_EDGECOLLAPSE_SVE
Command14=ID_TRICOLLAPSE2
Command15=ID_TRI_COLLAPSE
Command16=ID_TRI_COLLAPSE_CURVATURE
Command17=ID_TRICOLLAPSE_TEST
Command18=ID_ADDNOISE
Command19=SMOOTH_LAPLACIAN
Command20=ID_QSURFACEFITTING
Command21=ID_APP_ABOUT
Command22=ID_import
CommandCount=22

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_OPEN_FILE
Command2=ID_SAVE_FILE
Command3=ID_SIMPLIFY
Command4=ID_AXES
Command5=ID_ANIMATE
Command6=ID_TRICOLLAPSE
CommandCount=6

[TB:IDR_INTERACT_LEFT]
Type=1
Class=?
Command1=ID_MOUSE_SPIN
Command2=ID_MOUSE_ZOOM
Command3=ID_MOUSE_TRANSLATE
CommandCount=3

[TB:IDR_INTERACT_RIGHT]
Type=1
Class=?
Command1=ID_ZOOM_IN
Command2=ID_ZOOM_OUT
Command3=ID_SPIN_XCW
Command4=ID_SPIN_XCCW
Command5=ID_SPIN_YCW
Command6=ID_SPIN_YCCW
Command7=ID_SPIN_ZCW
Command8=ID_SPIN_ZCCW
CommandCount=8

[MNU:IDR_CONTEXTMENU]
Type=1
Class=?
Command1=IDM_ORIGINAL_MODEL
Command2=IDM_SIMPLIFIED_MODEL
Command3=IDM_WIREFRAME
Command4=IDM_FLAT
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=SMOOTH_LAPLACIAN
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_UNDO
CommandCount=11

[TB:IDR_MAINFRAME (Chinese (P.R.C.))]
Type=1
Class=?
Command1=ID_OPEN_FILE
Command2=ID_SAVE_FILE
Command3=ID_SIMPLIFY
Command4=ID_AXES
Command5=ID_ANIMATE
Command6=ID_TRICOLLAPSE
CommandCount=6

[TB:IDR_INTERACT_LEFT (Chinese (P.R.C.))]
Type=1
Class=?
Command1=ID_MOUSE_SPIN
Command2=ID_MOUSE_ZOOM
Command3=ID_MOUSE_TRANSLATE
CommandCount=3

[TB:IDR_INTERACT_RIGHT (Chinese (P.R.C.))]
Type=1
Class=?
Command1=ID_ZOOM_IN
Command2=ID_ZOOM_OUT
Command3=ID_SPIN_XCW
Command4=ID_SPIN_XCCW
Command5=ID_SPIN_YCW
Command6=ID_SPIN_YCCW
Command7=ID_SPIN_ZCW
Command8=ID_SPIN_ZCCW
CommandCount=8

[MNU:IDR_MAINFRAME (Chinese (P.R.C.))]
Type=1
Class=?
Command1=ID_OPEN_FILE
Command2=ID_CLOSE_FILE
Command3=ID_SAVE_FILE
Command4=IDM_SAVE_PICTURE
Command5=ID_APP_EXIT
Command6=IDM_ORIGINAL_MODEL
Command7=IDM_SIMPLIFIED_MODEL
Command8=IDM_WIREFRAME
Command9=IDM_FLAT
Command10=ID_FLATNORMAL
Command11=ID_POINTNORMAL
Command12=ID_EDGECOLLAPSE
Command13=ID_EDGECOLLAPSE_SVE
Command14=ID_TRICOLLAPSE2
Command15=ID_TRI_COLLAPSE
Command16=ID_TRI_COLLAPSE_CURVATURE
Command17=ID_TRICOLLAPSE_TEST
Command18=ID_ADDNOISE
Command19=SMOOTH_LAPLACIAN
Command20=ID_QSURFACEFITTING
Command21=ID_APP_ABOUT
Command22=ID_import
CommandCount=22

[MNU:IDR_CONTEXTMENU (Chinese (P.R.C.))]
Type=1
Class=?
Command1=IDM_ORIGINAL_MODEL
Command2=IDM_SIMPLIFIED_MODEL
Command3=IDM_WIREFRAME
Command4=IDM_FLAT
CommandCount=4

[ACL:IDR_MAINFRAME (Chinese (P.R.C.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_EDIT_PASTE
Command3=ID_EDIT_UNDO
Command4=ID_EDIT_CUT
Command5=SMOOTH_LAPLACIAN
Command6=ID_NEXT_PANE
Command7=ID_PREV_PANE
Command8=ID_EDIT_COPY
Command9=ID_EDIT_PASTE
Command10=ID_EDIT_CUT
Command11=ID_EDIT_UNDO
CommandCount=11

[DLG:IDD_SIMPLIFY (Chinese (P.R.C.))]
Type=1
Class=?
ControlCount=8
Control1=IDOK,button,1342373889
Control2=IDCANCEL,button,1342242816
Control3=IDC_FACE_VALUE,edit,1350631552
Control4=IDC_RATE_VALUE,edit,1350631552
Control5=IDC_STATIC,static,1342177280
Control6=IDC_TEXT,static,1342177280
Control7=IDC_FACE,button,1342308361
Control8=IDC_RATE,button,1342177289

[DLG:IDD_ABOUTBOX (Chinese (P.R.C.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352

