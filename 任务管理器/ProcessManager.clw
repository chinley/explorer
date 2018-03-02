; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CProcessManagerDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ProcessManager.h"

ClassCount=4
Class1=CProcessManagerApp
Class2=CProcessManagerDlg
Class3=CAboutDlg

ResourceCount=6
Resource1=IDD_CREATEPROCESS
Resource2=IDR_MAINFRAME
Resource3=IDD_OLE_PROPPAGE_LARGE (English (U.S.))
Resource4=IDD_ABOUTBOX
Resource5=IDD_PROCESSMANAGER_DIALOG
Class4=CCreateProcess
Resource6=IDR_MENU1

[CLS:CProcessManagerApp]
Type=0
HeaderFile=ProcessManager.h
ImplementationFile=ProcessManager.cpp
Filter=N
LastObject=CProcessManagerApp

[CLS:CProcessManagerDlg]
Type=0
HeaderFile=ProcessManagerDlg.h
ImplementationFile=ProcessManagerDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=ID_REBOOT

[CLS:CAboutDlg]
Type=0
HeaderFile=ProcessManagerDlg.h
ImplementationFile=ProcessManagerDlg.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PROCESSMANAGER_DIALOG]
Type=1
Class=CProcessManagerDlg
ControlCount=6
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_LIST1,SysListView32,1350631429
Control4=IDC_BUTTON_FRESH,button,1342242816
Control5=IDC_STATIC1,static,1342308352
Control6=IDC_ENDPROCESSBTN,button,1342242816

[MNU:IDR_MENU1]
Type=1
Class=CProcessManagerDlg
Command1=ID_RUN
Command2=ID_EXIT
Command3=ID_CREATEPROCESS
Command4=ID_ENDPROCESS
Command5=ID_SHUTDOWN
Command6=ID_LOGOFF
Command7=ID_REBOOT
Command8=IDS_ABOUTBOX
CommandCount=8

[DLG:IDD_OLE_PROPPAGE_LARGE (English (U.S.))]
Type=1
Class=?
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[CLS:CCreateProcess]
Type=0
HeaderFile=CreateProcess.h
ImplementationFile=CreateProcess.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_EXIT
VirtualFilter=dWC

[DLG:IDD_CREATEPROCESS]
Type=1
Class=CCreateProcess
ControlCount=4
Control1=IDOKcp,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_EDIT_APPID,edit,1350631552
Control4=IDC_STATIC,static,1342308352

