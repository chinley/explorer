; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMyOsDlg
LastTemplate=CButton
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MyOs.h"

ClassCount=3
Class1=CMyOsApp
Class2=CMyOsDlg

ResourceCount=2
Resource1=IDR_MAINFRAME
Class3=CMenuButton
Resource2=IDD_MYOS_DIALOG

[CLS:CMyOsApp]
Type=0
HeaderFile=MyOs.h
ImplementationFile=MyOs.cpp
Filter=N

[CLS:CMyOsDlg]
Type=0
HeaderFile=MyOsDlg.h
ImplementationFile=MyOsDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CMyOsDlg



[DLG:IDD_MYOS_DIALOG]
Type=1
Class=CMyOsDlg
ControlCount=8
Control1=IDC_BUTTON1,button,1342242816
Control2=IDC_BUTTON2,button,1342242816
Control3=IDC_BUTTON3,button,1342242816
Control4=IDC_STATIC,button,1342177287
Control5=IDC_CHECK1,button,1342242819
Control6=IDC_CHECK2,button,1342242819
Control7=IDC_CHECK3,button,1342242819
Control8=IDC_CHECK4,button,1342242819

[CLS:CMenuButton]
Type=0
HeaderFile=MenuButton.h
ImplementationFile=MenuButton.cpp
BaseClass=CButton
Filter=W
LastObject=CMenuButton
VirtualFilter=BWC

