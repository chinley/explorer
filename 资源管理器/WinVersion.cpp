#include "stdafx.h"
#include "WinVersion.h"
#include <stdio.h>
#include <windows.h>
#include <winnt.h>



#ifndef ARRAYSIZE
#define ARRAYSIZE(p) sizeof(p)/sizeof(p[0])
#endif

DWORD PASCAL ReadKernelVersion()
{
	DWORD dwVersion=0;
	HMODULE hinstDLL=LoadLibraryExW(L"kernel32.dll",NULL,LOAD_LIBRARY_AS_DATAFILE);
	if(hinstDLL!=NULL)
	{
		HRSRC hResInfo=FindResource(hinstDLL,MAKEINTRESOURCE(VS_VERSION_INFO),RT_VERSION);
		if(hResInfo!=NULL)
		{
			HGLOBAL hResData=LoadResource(hinstDLL,hResInfo);
			if(hResData!=NULL)
			{
				static const WCHAR wszVerInfo[]=L"VS_VERSION_INFO";
				struct VS_VERSIONINFO{
					WORD wLength;
					WORD wValueLength;
					WORD wType;
					WCHAR szKey[ARRAYSIZE(wszVerInfo)];
					VS_FIXEDFILEINFO Value;
					WORD Children[1024];
				}*lpVI=(struct VS_VERSIONINFO *)LockResource(hResData);

				if((lpVI!=NULL)&&(lstrcmpiW(lpVI->szKey,wszVerInfo)==0)&&(lpVI->wValueLength>0))
				{
					dwVersion=lpVI->Value.dwFileVersionMS;
				}
			}
		}
		FreeLibrary(hinstDLL);
	}
	return dwVersion;
}



void Version(HWND hWnd)
{
    DWORD dwVersion = 0; 
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0; 

	
	dwVersion=ReadKernelVersion();


    // Get the Windows version.
    dwMajorVersion = (DWORD)(HIWORD(dwVersion));
    dwMinorVersion = (DWORD)(LOWORD(dwVersion));

	


	LPTSTR szBuffer=new TCHAR[1024];//定义并申请输入缓冲区空间
	wsprintf(szBuffer, "当前系统版本号为: %d.%d",dwMajorVersion,dwMinorVersion);
	
	MessageBox(
		hWnd,
		szBuffer,
		TEXT("Version of Operating System"),
		MB_OK);
	
	
}





/*
int wmId;
//char vname[1000];
LPSTR Information = TEXT("电脑系统名称：");
LPSTR NameStr=TEXT("无法获得版本号");
HDC hdc;//需要显示器显示文本
PAINTSTRUCT ps;//使用设备的方法

SYSTEM_INFO info;                                   //用SYSTEM_INFO结构判断64位AMD处理器 
GetSystemInfo(&info);                               //调用GetSystemInfo函数填充结构 

OSVERSIONINFOEX os;
os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
if(GetVersionEx((OSVERSIONINFO *)&os))
{
	switch(os.dwMajorVersion)//判断主版本号
	{                         
				case 4: 
					switch(os.dwMinorVersion)//判断次版本号 
					{                
					case 0: 
						if(os.dwPlatformId==VER_PLATFORM_WIN32_NT) 
							//strcpy(NameStr,"Microsoft Windows NT 4.0");
							NameStr = TEXT("Microsoft Windows NT 4.0");
						else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) 
							//strcpy(NameStr,"Microsoft Windows 95");
							NameStr = TEXT("Microsoft Windows 95");
						break; 
					case 10: 
						//strcpy(NameStr,"Microsoft Windows 98");
						NameStr = TEXT("Microsoft Windows 98");
						break; 
					case 90: 
						//strcpy(NameStr,"Microsoft Windows Me");
						NameStr = TEXT("Microsoft Windows Me");
						break; 
					default:
						NameStr = TEXT("Unknown Version 1");
					} 
					break; 
					case 5: 
						switch(os.dwMinorVersion)//再比较dwMinorVersion的值 
						{               
						case 0: 
							//strcpy(NameStr,"Microsoft Windows 2000");
							NameStr = TEXT("Microsoft Windows 2000");
							break; 
						case 1: 
							//strcpy(NameStr,"Microsoft Windows XP");
							NameStr = TEXT("Microsoft Windows XP");
							break;
						default:
							NameStr = TEXT("Unknown Version 2");
						} 
						break; 
						default: 
							//strcpy(NameStr,"未知操作系统");
							NameStr = TEXT("Unknown Version 3");
	}
}
else
{
	NameStr = TEXT("Unknown Version 4");
}
hdc = ::BeginPaint(hDlg,&ps);
TextOut(hdc,120,100,Information,lstrlen(Information));
TextOut(hdc,120,120,NameStr,lstrlen(NameStr));
::EndPaint(hDlg,&ps);//结束使用设备

  */


