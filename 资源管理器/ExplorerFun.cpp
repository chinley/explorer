#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stdio.h>
#include <richedit.h>
#include <io.h>
#include <shellapi.h>
#include <shlobj.h >
#include <objbase.h>
#include "InitOpreation.h"
#include "ExplorerFun.h"
#include "FileOperation.h"
#include "ControlOperation.h"

#define BTOM (1024*1024)
#define BTOG (1024*1024*1024)
#define BTOK (1024)

HIMAGELIST g_ListViewImageListLarge = NULL;
HIMAGELIST g_ListViewImageListSmall = NULL;
HIMAGELIST g_TreeViewImageList = NULL;

TCHAR g_szBackFowardPath[MAX_PATH] = {0}; //后退，前进路径
TCHAR g_szBackPath[MAX_PATH] = {0};       //临时存储后退路径
TCHAR g_szAllPath[MAX_PATH] = {0};        //全路径
TCHAR g_szBeginEditName[MAX_PATH];  //重命名是开始的文件名



//【1】将所有的列删除
void DeleteAllColumn(HWND hwnd)
{
	int i = 0;
	for (i=3; i>=0; i--)
	{
		SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_DELETECOLUMN, i, 0);
	}
}


//【2】设置查看方式
void SetLookMode(HWND hwndListView, DWORD dwLookMode)
{
	DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);  
    dwStyle &= ~LVS_TYPEMASK;
    dwStyle |= dwLookMode;
    SetWindowLong(hwndListView, GWL_STYLE,dwStyle );
}


//【3】得到Image_List图标索引号
int GetImageLIstIndex(HWND hwnd, pFILEINFO pFileInfo, TCHAR *szPath, BOOL IsFile)
{
	int iIndex = 0;
	
	//用 WIN32 标准图标替代
	
	SHFILEINFO shfi;
	ZeroMemory(&shfi, sizeof(SHFILEINFO));
	
	CoInitialize(NULL);
	SHGetFileInfo(szPath, 0, &shfi, sizeof(shfi),
		SHGFI_SMALLICON | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_DISPLAYNAME);
	
	if (TRUE == IsFile)
	{
		if (0 == lstrcmp(shfi.szTypeName, TEXT("文件")))      //文件夹
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("文件夹"));

		}
		else        //文件
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", shfi.szTypeName);
		}

	}
	else
	{
		//小图标
		iIndex = ImageList_AddIcon(g_TreeViewImageList, shfi.hIcon);
		wsprintf(pFileInfo->szFileName, "%s", shfi.szDisplayName);
	}
	
	//小图标
	iIndex = ImageList_AddIcon(g_ListViewImageListSmall, shfi.hIcon);
	DestroyIcon(shfi.hIcon);

	SHGetFileInfo(szPath, 0, &shfi, sizeof(shfi),
		SHGFI_LARGEICON | SHGFI_ICON);

	//大图标
	iIndex = ImageList_AddIcon(g_ListViewImageListLarge, shfi.hIcon);
	DestroyIcon(shfi.hIcon);

	CoUninitialize();
	return iIndex;
}


//【4】响应WM_Notify
void OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmhdr = (LPNMHDR)lParam;
	switch (lpnmhdr->code)
	{
	case NM_CLICK:       //单击
		{
			if (IDC_LISTVIEWFILE == wParam)
			{
				SetSelectedtInfo(hwnd);
			}
		}
		break;
	case NM_DBLCLK:      //双击
		{
			if (IDC_LISTVIEWFILE == wParam)
			{
				DblclkFile(hwnd);
			}
		}
		break;
	case LVN_BEGINLABELEDIT:      //重命名开始
		{
			BeginRename(hwnd, lpnmhdr);
		}
		break;
	case LVN_ENDLABELEDIT:        //重命名结束
		{
			ReFileName(hwnd, lpnmhdr);
		}
		break;
	case EN_MSGFILTER:            //按了Enter键
		{
			MSGFILTER *msgfilter = (MSGFILTER*)lpnmhdr;
			if ((WM_KEYDOWN == msgfilter->msg) && (VK_RETURN == msgfilter->wParam))
			{
				GotoFilePath(hwnd);
			}
		}
		break;
	default:
		break;
	}
}


//【5】重命名开始
void BeginRename(HWND hwnd, LPNMHDR lpnmhdr)
{
	HWND hwndEdit = ListView_GetEditControl(GetDlgItem(hwnd, IDC_LISTVIEWFILE));
	
	ZeroMemory(g_szBeginEditName, sizeof(g_szBeginEditName) / sizeof(TCHAR));
	
	GetWindowText(hwndEdit, g_szBeginEditName, sizeof(g_szBeginEditName) / sizeof(TCHAR));
	
	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //磁盘页面
	{
		TCHAR szTmp[MAX_PATH];
		ZeroMemory(szTmp, sizeof(szTmp) / sizeof(TCHAR));

		lstrcpy(szTmp, g_szBeginEditName);
		szTmp[lstrlen(szTmp)-5] = '\0';
		SetWindowText(hwndEdit, szTmp);
	}
}


//【6】得到磁盘类型和设置磁盘名
void GetDiskTypeAndName(UINT uiDiskType, TCHAR *szFilePath, pFILEINFO pFileInfo)
{
	switch (uiDiskType)
	{
	case DRIVE_UNKNOWN:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("未知磁盘类型"));
			wsprintf(pFileInfo->szFileName, "未知磁盘类型 (%s)", szFilePath);
		}
		break;			
	case DRIVE_NO_ROOT_DIR:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("磁盘名无效"));
			wsprintf(pFileInfo->szFileName, "磁盘名无效 (%s)", szFilePath);
		}
		break;			
	case DRIVE_REMOVABLE:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("可移动磁盘"));
			wsprintf(pFileInfo->szFileName, "可移动磁盘 (%s)", szFilePath);
		}
		break;			
	case DRIVE_FIXED:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("本地磁盘"));
			wsprintf(pFileInfo->szFileName, "本地磁盘 (%s)", szFilePath);
		}
		break;			
	case DRIVE_REMOTE:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("网络磁盘"));
			wsprintf(pFileInfo->szFileName, "网络磁盘 (%s)", szFilePath);
		}
		break;			
	case DRIVE_CDROM:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("光驱"));
			wsprintf(pFileInfo->szFileName, "光驱 (%s)", szFilePath);
		}
		break;			
	case DRIVE_RAMDISK:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("RAM"));
			wsprintf(pFileInfo->szFileName, "RAM (%s)", szFilePath);
		}
		break;			
	default:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("GetDriveType的返回值非法"));
			wsprintf(pFileInfo->szFileName, "GetDriveType的返回值非法 (%s)", szFilePath);
		}
		break;	
	}
}


//【7】向ListView中添加磁盘信息
void AddFileInfoToListView(HWND hwnd)
{
	// 删除所有的项
	SendMessage(GetDlgItem(hwnd,IDC_LISTVIEWFILE), LVM_DELETEALLITEMS, 0, 0);
	TreeView_DeleteAllItems(GetDlgItem(hwnd,IDC_TREEFILE));

	ULARGE_INTEGER nFreeBytesAvaliable;
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;

	DWORD dwDisk = GetLogicalDrives();

	TCHAR szDiskPath[30];
	ZeroMemory(szDiskPath, sizeof(szDiskPath) / sizeof(TCHAR));

	int iNum = 0;
	char disk='A';
	while(dwDisk>0)
	{
		if((dwDisk&1)>0)
		{
			szDiskPath[iNum]=disk;
			iNum++;
		}
		disk++;
		dwDisk = dwDisk >> 1;
	}
	szDiskPath[lstrlen(szDiskPath) + 1] = '\0';
	
	//文件总数
	TCHAR szFileCount[10];
	ZeroMemory(szFileCount, sizeof(szFileCount) / sizeof(TCHAR));
	
	wsprintf(szFileCount, "%i 个对象", iNum);
	
	SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)szFileCount);
	
	TCHAR szFilePath[5];
	
	FILEINFO FileInfo;

	int j = 0;
	for (j=0; j<iNum; j++)
	{
		ZeroMemory(szFilePath, sizeof(szFilePath) / sizeof(TCHAR));
		wsprintf(szFilePath, "%c:", szDiskPath[j]);

		ZeroMemory(&FileInfo, sizeof(FILEINFO));

		//得到磁盘类型和设置磁盘名
		UINT uiDiskType = GetDriveType(szFilePath);
		GetDiskTypeAndName(uiDiskType, szFilePath, &FileInfo);


		lstrcat(szFilePath, TEXT("\\"));
		int iIcon = GetImageLIstIndex(hwnd, &FileInfo, szFilePath, FALSE);

		//有软盘并且软盘不存在是GetDiskFreeSpaceEx时程序会异常
		if (0 != lstrcmp(FileInfo.szFileAttributes, TEXT("可移动磁盘"))
			|| (NULL == strstr(FileInfo.szFileName, TEXT("软盘"))))			
		{
			if(GetDiskFreeSpaceEx(szFilePath,
				&nFreeBytesAvaliable,
				&nTotalNumberOfBytes,
				&nTotalNumberOfFreeBytes))
			{
				sprintf(FileInfo.szFileSize, "%.3fG" ,
					(float)(nTotalNumberOfBytes.HighPart* 4 + 
					(float)(nTotalNumberOfBytes.LowPart) / BTOG));
				sprintf(FileInfo.Optional.szTotal,"%.3fG",
					(float)(nTotalNumberOfFreeBytes.HighPart *4  + 
					(float)(nTotalNumberOfFreeBytes.LowPart) / BTOG));
			}
		}
		
		InsertFileInfo(hwnd, 0, iIcon, &FileInfo, FALSE, FALSE);
		AddNodeInTree(
			GetDlgItem(hwnd, IDC_TREEFILE), //获取树状视图控件的句柄
			TVI_ROOT, 
			iIcon, 
			FileInfo.szFileName, 
			MAX_PATH);

		ZeroMemory(szFilePath, sizeof(szFilePath) / sizeof(TCHAR));
		wsprintf(szFilePath, "%s", TEXT("我的电脑"));
		SetDlgItemText(GetDlgItem(hwnd, IDC_TOOLADDRESS), IDM_EDITPATH, szFilePath);
	}
}



//【8】得到文件信息
BOOL GetFileInfo(PWIN32_FIND_DATA pstFileInfo, pFILEINFO pFileInfo)
{
	wsprintf(pFileInfo->szFileName, "%s", pstFileInfo->cFileName);

	if (0 == lstrcmp(pstFileInfo->cFileName, TEXT(".")) || 0 == lstrcmp(pstFileInfo->cFileName, TEXT("..")))
	{
		return FALSE;
	}

	if (FILE_ATTRIBUTE_DIRECTORY & pstFileInfo->dwFileAttributes)
	{
		sprintf(pFileInfo->szFileSize, "%s", TEXT(""));
	}
	else
	{
		sprintf(pFileInfo->szFileSize, "%dKB", (DWORD)(pstFileInfo->nFileSizeHigh * (sizeof(DWORD)+1) + pstFileInfo->nFileSizeLow) / BTOK);
	}
	SYSTEMTIME stWriteTime;
	ZeroMemory(&stWriteTime, sizeof(SYSTEMTIME));

	FileTimeToSystemTime(&pstFileInfo->ftLastWriteTime, &stWriteTime);

	wsprintf(pFileInfo->Optional.szLastWriteTime, "%i-%i-%i %i:%i:%i", stWriteTime.wYear,
		stWriteTime.wMonth, stWriteTime.wDay, stWriteTime.wHour+8, stWriteTime.wMinute, stWriteTime.wSecond);

	return TRUE;
}


//【9】设置选中文件信息（在窗口最下面一栏  第一列显示信息）
//SendMessage函数的第4个参数 为0，表示第一列
void SetSelectedtInfo(HWND hwnd)
{
	TCHAR szMsg[MAX_PATH];
	ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);

	if (0 == iCount)
	{
		int iItemCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEMCOUNT, 0, 0);
		wsprintf(szMsg, "%i 个对象", iItemCount);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT(szMsg));
		return;
	}
	else if (1 == iCount)//只选中一个对象
	{
		TCHAR szFileName[MAX_PATH];
		TCHAR szPath[MAX_PATH];

		ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

		int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);

		LVITEM lv;
		ZeroMemory(&lv, sizeof(LVITEM));
		
		lv.mask = LVIF_TEXT;
		lv.iSubItem = 0;
		lv.iItem = iIndex;
		lv.pszText = szFileName;
		lv.cchTextMax = sizeof(szFileName) / sizeof(TCHAR);
		SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
		
		wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
		if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //选中一个对象&&该对象是文件夹
		{
			SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT("选定 1 个对象"));
		}
		else//选中一个对象&&该对象不是文件夹
		{
			FILEINFO FileInfo;
			ZeroMemory(&FileInfo, sizeof(FILEINFO));
			
			LVITEM lv;
			ZeroMemory(&lv, sizeof(LVITEM));
			
			lv.mask = LVIF_TEXT;
			lv.iItem = iIndex;

			lv.iSubItem = 2;
			lv.pszText = FileInfo.szFileAttributes;
			lv.cchTextMax = sizeof(FileInfo.szFileAttributes) / sizeof(TCHAR);
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
			
			lv.iSubItem = 3;
			lv.pszText = FileInfo.Optional.szLastWriteTime;
			lv.cchTextMax = sizeof(FileInfo.Optional.szLastWriteTime) / sizeof(TCHAR);
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);

			lv.iSubItem = 1;
			lv.pszText = FileInfo.szFileSize;
			lv.cchTextMax = sizeof(FileInfo.szFileSize) / sizeof(TCHAR);
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);

			wsprintf(szMsg, "【类型】: %s 【修改日期】: %s  【大小】: %s", FileInfo.szFileAttributes,
				FileInfo.Optional.szLastWriteTime, FileInfo.szFileSize);
			SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT(szMsg));
		}
	}
	else//选中不止一个对象
	{
		wsprintf(szMsg, "选定 %i 个对象", iCount);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT(szMsg));
		return;
	}
}


//【10】双击文件(夹)或磁盘
void DblclkFile(HWND hwnd)
{
	int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);
	
	if (-1 == iIndex)
	{
		return;
	}
	TCHAR szFileName[MAX_PATH];
	ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
	
	LVITEM lv;
	ZeroMemory(&lv, sizeof(LVITEM));
	
	lv.mask = LVIF_TEXT;  //得到文件(夹)名或磁盘名
	lv.iSubItem = 0;
	lv.iItem = iIndex;
	lv.pszText = szFileName;
	lv.cchTextMax = sizeof(szFileName) / sizeof(TCHAR);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
	
	if (0 == lstrcmp(g_szAllPath, TEXT("")))      //磁盘
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
		InitListViewColumn(hwnd, TRUE);
		TCHAR *pLastSymbol = strrchr(szFileName, '(');
				
		wsprintf(g_szAllPath, "%c:", pLastSymbol[1]);     //后退路径
		lstrcpy(g_szBackFowardPath, g_szAllPath);         //设置前进，后退路径
		lstrcpy(g_szBackPath, g_szAllPath);
			
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		lstrcpy(szPath, g_szAllPath);
		
		SearchFile(hwnd, szPath);
	}
	else            //文件(夹)
	{
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		
		wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
		if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //文件夹
		{
			if (NULL == strstr(g_szBackFowardPath, szPath))     //设置前进，后退路径
			{
				lstrcpy(g_szBackFowardPath, szPath);
				lstrcpy(g_szBackPath, szPath);
			}

			ImageList_RemoveAll(g_ListViewImageListSmall);
			ImageList_RemoveAll(g_ListViewImageListLarge);
		}
		SearchFile(hwnd, szPath);
	}
				
}


//【11】查找目录下文件
void SearchFile(HWND hwnd, TCHAR *szPath)
{
	WIN32_FIND_DATA stFileInfo;
	ZeroMemory(&stFileInfo, sizeof(WIN32_FIND_DATA));

	if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //文件夹
	{
		lstrcpy(g_szAllPath, szPath);     //全路径
		
		lstrcat(szPath, TEXT("\\*.*"));
		HANDLE hFile = FindFirstFile(szPath, &stFileInfo);
		
		DWORD errorcode = 0;
		
		FILEINFO FileInfo;
		
		// 删除所有的项
		SendMessage(GetDlgItem(hwnd,IDC_LISTVIEWFILE), LVM_DELETEALLITEMS, 0, 0);
		
		TCHAR szFileName[MAX_PATH];

		double dwDirectorySize = 0;

		while((INVALID_HANDLE_VALUE != hFile) && (ERROR_NO_MORE_FILES != errorcode))
		{				
			ZeroMemory(&FileInfo, sizeof(FILEINFO));
			
			if (FALSE != GetFileInfo(&stFileInfo, &FileInfo))
			{
				ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
				wsprintf(szFileName, "%s\\%s", g_szAllPath, stFileInfo.cFileName);
				int iIcon = GetImageLIstIndex(hwnd, &FileInfo, szFileName, TRUE);

				InsertFileInfo(hwnd, 0, iIcon, &FileInfo, TRUE, FALSE);
				dwDirectorySize += atoi(FileInfo.szFileSize);
			}
			FindNextFile(hFile, &stFileInfo);
			errorcode = GetLastError();
		}
		FindClose(hFile);
		
		TCHAR szFileCount[10];
		ZeroMemory(szFileCount, sizeof(szFileCount) / sizeof(TCHAR));

		int iFileCount = SendMessage(GetDlgItem(hwnd,IDC_LISTVIEWFILE), LVM_GETITEMCOUNT, 0, 0);
		wsprintf(szFileCount, "%i 个对象", iFileCount);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)szFileCount);
		
		ZeroMemory(szFileCount, sizeof(szFileCount) / sizeof(TCHAR));
		dwDirectorySize = dwDirectorySize / 1024;      //因为dwDirectorySize的单位已经是KB
		sprintf(szFileCount, "%.2lf MB", dwDirectorySize);
		
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 1, (LPARAM)szFileCount);

		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		if (2 == lstrlen(g_szAllPath))     //磁盘
		{
			wsprintf(szPath, "%s\\", g_szAllPath);
		}
		else              //文件夹
		{
			wsprintf(szPath, "%s", g_szAllPath);
		}
		SetDlgItemText(GetDlgItem(hwnd, IDC_TOOLADDRESS), IDM_EDITPATH, szPath);
	}
	
	else            //文件
	{
		int iShell = (int)ShellExecute(hwnd, "open", szPath, NULL, NULL, SW_SHOWNORMAL);
		if (32 > iShell)
		{
			ShellExecuteError(hwnd, szPath, iShell);
		}
	}
}


//【12】ShellExecute执行失败的错误信息
void ShellExecuteError(HWND hwnd, TCHAR *szPath, int iError)
{
	switch (iError)
	{
	case SE_ERR_ACCESSDENIED:          //无效的exe文件
		{
			TCHAR szMsg[MAX_PATH];
			ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

			wsprintf(szMsg, "%s 不是有效的 Win32 应用程序", szPath);
			MessageBox(hwnd, TEXT(szMsg), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	case SE_ERR_NOASSOC:    //无程序关联
		{
			MessageBox(hwnd, TEXT("没有与给定的文件扩展名关联的应用程序"), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	case SE_ERR_SHARE:      //发生共享冲突
		{
			MessageBox(hwnd, TEXT("发生共享冲突"), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	case SE_ERR_DLLNOTFOUND: //指定的动态链接库没有被找到
		{
			MessageBox(hwnd, TEXT("指定的动态链接库没有被找到"), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	default:
		break;
	}
}

//【13】后退
void BackPath(HWND hwnd)
{
	if (0 == lstrcmp(g_szBackPath, TEXT("")))       //磁盘页面
	{
		return;
	}
	TCHAR *pLastSymbol = strrchr(g_szBackPath, '\\');        //磁盘的下一级目录

	if (NULL == pLastSymbol)      //C: 时
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
		ImageList_RemoveAll(g_TreeViewImageList);

		InitListViewColumn(hwnd, FALSE);
		AddFileInfoToListView(hwnd);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 1, (LPARAM)TEXT(""));
		ZeroMemory(g_szBackPath, sizeof(TCHAR) / sizeof(TCHAR));
		ZeroMemory(g_szAllPath, sizeof(TCHAR) / sizeof(TCHAR));
		pLastSymbol = NULL;
		return;
	}
	
	ImageList_RemoveAll(g_ListViewImageListSmall);
	ImageList_RemoveAll(g_ListViewImageListLarge);

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	strncpy(szPath, g_szBackPath, pLastSymbol - g_szBackPath);
	lstrcpy(g_szBackPath, szPath);
	pLastSymbol = NULL;
	SearchFile(hwnd, szPath);
}


//【14】前进
void FowardPath(HWND hwnd)
{
	if (0 == lstrcmp(g_szBackFowardPath, TEXT("")))      //程序刚启动时
	{
		return;
	}
	if (0 == lstrcmp(g_szBackPath, g_szBackFowardPath))  //还未后退过
	{
		return;
	}

	ImageList_RemoveAll(g_ListViewImageListSmall);
	ImageList_RemoveAll(g_ListViewImageListLarge);

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	//磁盘的下一级目录
	TCHAR *pLastSymbol = strchr(g_szBackFowardPath+lstrlen(g_szBackPath)+1, '\\');

	if (NULL == pLastSymbol)       //再次前进时是最后一层
	{
		lstrcpy(szPath, g_szBackFowardPath);
	}
	else
	{
		strncpy(szPath, g_szBackFowardPath, pLastSymbol - g_szBackFowardPath);
	}

	lstrcpy(g_szBackPath, szPath);
	pLastSymbol = NULL;

	SearchFile(hwnd, szPath);

}

//【15】转到路径
void GotoFilePath(HWND hwnd)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	GetDlgItemText(
		GetDlgItem(hwnd, IDC_TOOLADDRESS), 
		IDM_EDITPATH,
		szPath, 
		sizeof(szPath) / sizeof(TCHAR)
		);
	
	if (0 == lstrcmp(szPath, TEXT("我的电脑")))
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
		ImageList_RemoveAll(g_TreeViewImageList);
		
		InitListViewColumn(hwnd, FALSE);
		AddFileInfoToListView(hwnd);

		ZeroMemory(g_szAllPath, sizeof(TCHAR) / sizeof(TCHAR));
		return;
	}
	if (0 != access(szPath,0))
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

		wsprintf(szMsg, "找不到文件(夹)“%s”，请确认路径正确。",szPath);
		MessageBox(hwnd,TEXT(szMsg),TEXT("VLExplorer"),MB_OK | MB_ICONERROR);
		return;
	}
	if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //文件夹
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
	}

	//将路径中的'\'去掉

	if ('\\' == szPath[lstrlen(szPath)-1])
	{
		if ('\\' == szPath[lstrlen(szPath)-2])
		{
			TCHAR szMsg[MAX_PATH];
			ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));
			
			wsprintf(szMsg, "Windows 找不到“%s”。请检查拼写是否正确，然后重试，或单击“开始”按钮，再单击“搜索”，搜索\n项目。",szPath);
			MessageBox(hwnd,TEXT(szMsg),TEXT("地址栏"),MB_OK | MB_ICONERROR);
			return;
		}
		szPath[lstrlen(szPath)-1] = '\0';
	}
	SearchFile(hwnd, szPath);
}


//【16】刷新窗口
void RefreshWindow(HWND hwnd)
{
	if (0 == lstrcmp(g_szAllPath, TEXT("")))           //磁盘目录
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
		ImageList_RemoveAll(g_TreeViewImageList);
		
		InitListViewColumn(hwnd, FALSE);
		AddFileInfoToListView(hwnd);
		return;
	}
	
	//文件夹目录

	ImageList_RemoveAll(g_ListViewImageListSmall);
	ImageList_RemoveAll(g_ListViewImageListLarge);

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
	lstrcpy(szPath, g_szAllPath);
	SearchFile(hwnd, szPath);
}


//【17】排序, 避免使用全局变量，申明了一个结构体
void SortListFile(HWND hwnd, int lParamSort)
{
	SORTINFO SortInfo;
	ZeroMemory(&SortInfo, sizeof(SORTINFO));
	SortInfo.hwndListView = GetDlgItem(hwnd, IDC_LISTVIEWFILE);
	SortInfo.lParamSort = lParamSort;
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_SORTITEMS, (WPARAM)(LPARAM)&SortInfo, (LPARAM)(PFNLVCOMPARE)CompareFunc);
}


//【18】排序的回调函数 lParam1, lParam2是LVM_SETITEM时的lparam
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	pSORTINFO pSort = (pSORTINFO)lParamSort;

	TCHAR szlParam1[MAX_PATH];
	TCHAR szlParam2[MAX_PATH];
	LVITEM lv;

	ZeroMemory(szlParam1, sizeof(szlParam1) / sizeof(TCHAR));
	ZeroMemory(szlParam2, sizeof(szlParam2) / sizeof(TCHAR));
	ZeroMemory(&lv, sizeof(LVITEM));

	lv.mask = LVIF_TEXT | LVIF_PARAM;
	lv.cchTextMax = sizeof(szlParam1) / sizeof(TCHAR);

	int iRet = 0;

	switch (pSort->lParamSort)
	{
	case SORT_NAME:          //名称
		{
			lv.iSubItem = 0;
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);
			
			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);
			
			iRet = lstrcmp(szlParam1, szlParam2);      //从小到大
		}
		break;
	case SORT_SIZE:          //大小
		{
			if (0 == lstrcmp(g_szAllPath, TEXT("")))           //磁盘目录
			{
				lv.iSubItem = 2;
			} 
			else        //文件(夹)
			{
				lv.iSubItem = 1;
			}
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);

			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);

			iRet = atoi(szlParam1) - atoi(szlParam2);      //从小到大
		}
		break;
	case SORT_STYLE:         //类型
		{
			if (0 == lstrcmp(g_szAllPath, TEXT("")))           //磁盘目录
			{
				lv.iSubItem = 1;
			}
			else
			{
				lv.iSubItem = 2;
			}
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);
			
			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);
			
			iRet = lstrcmp(szlParam1, szlParam2);      //从小到大
		}
		break;
	case SORT_WRITETIME:     //修改时间
		{
			if (0 == lstrcmp(g_szAllPath, TEXT("")))           //磁盘目录
			{
				return 0;         //return 0不排序
			} 
			lv.iSubItem = 3;
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);
			
			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);
			
			iRet = lstrcmp(szlParam1, szlParam2);      //从小到大
		}
		break;
	case SORT_GROUP:         //按组排列
		{
		}
		break;
	default:
		break;
	}

	return iRet;
}


//【19】判断Windows是否合法
void IsRightful(HWND hwnd)
{
	TCHAR szURL[MAX_PATH];
	ZeroMemory(szURL, sizeof(szURL) / sizeof(TCHAR));
	
	wsprintf(szURL, "%s", TEXT("http://www.microsoft.com/zh-cn/howtotell/default.aspx"));
	if (32 > (int)ShellExecute(hwnd, "open", szURL, NULL, NULL, SW_SHOWNORMAL))
	{
		MessageBox(hwnd, TEXT("打开Windows合法网页失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
	}
}

//【20】创建快捷方式
void CreateListFileShortCut(HWND hwnd)
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szLinkName[MAX_PATH];
	
	BOOL IsDisk = FALSE;
	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //磁盘页面
	{
		int iRet = MessageBox(hwnd, TEXT("Windows 无法再当前位置创建快捷方式。\n要把快捷方式放在桌面上吗?"), TEXT("快捷方式"), MB_YESNO | MB_ICONQUESTION);
		if (IDNO == iRet)
		{
			return;
		}
		IsDisk = TRUE;
	}
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	int i = 0;
	int iNextItem = 0;
	for (i=0; i<iCount; i++)
	{
		if (0 == i)
		{
			iNextItem = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETNEXTITEM, (WPARAM)(int)-1, MAKELPARAM((UINT)LVNI_SELECTED, 0));
		}
		else
		{
			iNextItem = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETNEXTITEM, (WPARAM)(int)iNextItem, MAKELPARAM((UINT)LVNI_SELECTED, 0));
		}

		LVITEM lv;
		ZeroMemory(&lv, sizeof(LVITEM));
		
		ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

		lv.mask = LVIF_TEXT;
		lv.iSubItem = 0;
		lv.iItem = iNextItem;
		lv.pszText = szFileName;
		lv.cchTextMax = sizeof(szFileName) / sizeof(TCHAR);
		SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
		
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		ZeroMemory(szLinkName, sizeof(szLinkName) / sizeof(TCHAR));
		if (TRUE == IsDisk)
		{
			TCHAR *pLastSymbol = strrchr(szFileName, '(');
			wsprintf(szPath, "%c:\\", pLastSymbol[1]);

			TCHAR szDeskTopPath[MAX_PATH];
			ZeroMemory(szDeskTopPath, sizeof(szDeskTopPath) / sizeof(TCHAR));
			SHGetSpecialFolderPath(0, szDeskTopPath, CSIDL_DESKTOPDIRECTORY, 0);
			
			//文件名种不能包含/\:等字符
			szFileName[lstrlen(szFileName)-2] = ')';
			szFileName[lstrlen(szFileName)-1] = '\0';
			wsprintf(szLinkName, "%s\\快捷方式 到%s.lnk", szDeskTopPath, szFileName);
			pLastSymbol = NULL;
		}
		else
		{
			wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
			wsprintf(szLinkName, "%s\\快捷方式 到%s.lnk", g_szAllPath, szFileName);
		}

		if (FALSE == CreateShortCut(hwnd, szPath, szLinkName))
		{
			return;
		}
		if (FALSE == IsDisk)
		{
			InsertFileToListView(hwnd, szLinkName, 0, FALSE);
		}
	}
}

//【21】创建快捷方式
BOOL CreateShortCut(HWND hwnd, TCHAR *szPath, TCHAR *szLinkName)
{
	HRESULT hr = CoInitialize(NULL);
	
	if (SUCCEEDED(hr))
	{
		IShellLink *pisl;
		
		hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (void**)&pisl);
		
		if (SUCCEEDED(hr))
		{
			IPersistFile* pIPF;
			
			pisl->SetPath(szPath);      //设置要创建快捷方式的文件
			hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
			
			if (SUCCEEDED(hr))
			{
				WCHAR wPath[MAX_PATH];
				ZeroMemory(wPath, sizeof(wPath) / sizeof(WCHAR));

				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szLinkName, -1, wPath, MAX_PATH);
				
				pIPF->Save(wPath, FALSE);
				pIPF->Release();
				pisl->Release();
			}
			else
			{
				MessageBox(hwnd, TEXT("创建快捷方式失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
				pisl->Release();
				CoUninitialize();
				return FALSE;
			}
		}
		else
		{
			MessageBox(hwnd, TEXT("创建快捷方式失败"), TEXT("VLExplorer"), MB_OK | MB_ICONERROR);
			pisl->Release();
			CoUninitialize();
			return FALSE;
		}
		CoUninitialize();
	}
	return TRUE;
}

//【22】帮助和支持中心
void OpenHelpCenter(HWND hwnd)
{/*
	TCHAR szWindowsPath[MAX_PATH];
	ZeroMemory(szWindowsPath, sizeof(szWindowsPath) / sizeof(TCHAR));

	GetWindowsDirectory(szWindowsPath, sizeof(szWindowsPath) / sizeof(TCHAR));
	lstrcat(szWindowsPath,TEXT("ReadMe.txt"));// TEXT("\\pchealth\\helpctr\\binaries\\HelpCtr.exe"));//该函数将一个字符串附加在另一个字符串后面。
	if (0 != access(szWindowsPath, 0))
	{
		MessageBox(hwnd,TEXT("找不到帮助和支持中心文件"),TEXT("MyResourceExplorer"),MB_OK | MB_ICONERROR);
		return;
	}
	ShellExecute(hwnd, "open", szWindowsPath, NULL, NULL, SW_SHOWNORMAL);
	*/

	TCHAR szWindowsPath[MAX_PATH]=TEXT(".\\使用说明书.CHM");
	if (0 != access(szWindowsPath, 0))
	{
		MessageBox(hwnd,TEXT("找不到帮助和支持中心文件"),TEXT("MyResourceExplorer"),MB_OK | MB_ICONERROR);
		return;
	}
	ShellExecute(hwnd, "open", szWindowsPath, NULL, NULL, SW_SHOWNORMAL);

}