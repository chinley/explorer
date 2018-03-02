#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ControlOperation.h"


//插入文件信息
void InsertFileInfo(
					HWND hwnd,
					int iItem,
					int iImage,
					pFILEINFO pFileInfo,
					BOOL IsFile,
					BOOL IsReName)
{
	//列表视图控件
	HWND hwndListFile = GetDlgItem(hwnd, IDC_LISTVIEWFILE);
	LVITEM lvItem;//指定或接收列表视图项的属性
	ZeroMemory(&lvItem, sizeof(LVITEM));
	
	lvItem.mask    = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

	if (TRUE == IsReName)   //重命名
	{
		lvItem.iItem = iItem;
	}
	else       //不是重命名
	{
		lvItem.iItem   = //返回项目数
			SendMessage(
			hwndListFile,
			LVM_GETITEMCOUNT,//检索列表视图控件中的项目数
			0, 
			0);
	}	
	lvItem.iImage = iImage;
	lvItem.iSubItem = 0;
	lvItem.pszText = pFileInfo->szFileName;
	lvItem.lParam = lvItem.iItem;        //用于排序
	SendMessage(
		hwndListFile,
		LVM_INSERTITEM,//在列表视图控件中插入一个新项目
		0, //wParam必须为0
		(LPARAM)&lvItem);//lParam：指向指定列表视图项的属性的LVITEM结构的指针
	
	//第2，3，4项
	lvItem.mask    = LVIF_TEXT;

	if (FALSE == IsFile)      //磁盘
	{
		lvItem.pszText  = pFileInfo->szFileAttributes;
	}
	else
	{
		lvItem.pszText  = pFileInfo->szFileSize;
	}
	lvItem.iSubItem = 1;
	SendMessage(
		hwndListFile,
		LVM_SETITEM, 
		0,
		(LPARAM)&lvItem);
	
	if (FALSE == IsFile)      //磁盘
	{
		lvItem.pszText  = pFileInfo->szFileSize;
	}
	else
	{
		lvItem.pszText  = pFileInfo->szFileAttributes;
	}
	
	lvItem.iSubItem = 2;
	SendMessage(
		hwndListFile, 
		LVM_SETITEM, 
		0, 
		(LPARAM)&lvItem);
	
	if (FALSE == IsFile)      //不是文件（夹），是磁盘
	{
		lvItem.pszText  = pFileInfo->Optional.szTotal;
	}
	else                     //文件（夹）
	{
		lvItem.pszText  = pFileInfo->Optional.szLastWriteTime;
	}
	lvItem.iSubItem = 3;
	SendMessage(
		hwndListFile, 
		LVM_SETITEM,
		0,
		(LPARAM)&lvItem);
}

//插入节点到TreeView控件中
//在ExplorerFun.cpp中出现（可在该文件搜索函数名）
HTREEITEM AddNodeInTree(
						HWND hwndTreeView,
						HTREEITEM hParent,
						int iImage,
						TCHAR *szItemText,
						DWORD dwTextLength)
{
	TVINSERTSTRUCT tviInsert;
	HTREEITEM hTreeitem;
	
	if (TVI_ROOT == hParent)//hParent等于树的根
	{
		tviInsert.hParent = TVI_ROOT;
		tviInsert.item.mask = TVIF_CHILDREN |  TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT ;
		tviInsert.item.cChildren = 1;
		tviInsert.item.pszText = szItemText;
		tviInsert.item.cchTextMax = dwTextLength;
		tviInsert.item.state = TVIS_OVERLAYMASK;
		tviInsert.item.iImage = iImage;
		tviInsert.item.iSelectedImage = iImage;
		hTreeitem = (HTREEITEM)SendMessage(hwndTreeView, TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT)&tviInsert);
	}
	else
	{
		tviInsert.hParent = hParent;
		tviInsert.hInsertAfter = TVI_LAST;
		tviInsert.item.mask = TVIF_CHILDREN |  TVIF_HANDLE | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_TEXT ;
		tviInsert.item.cChildren = 0;
		tviInsert.item.pszText = szItemText;
		tviInsert.item.iImage = iImage;
		tviInsert.item.iSelectedImage = iImage;
		tviInsert.item.cchTextMax = dwTextLength;
		tviInsert.item.state = TVIS_OVERLAYMASK;
		
		hTreeitem = (HTREEITEM)	SendMessage(hwndTreeView, TVM_INSERTITEM, 0, (LPARAM)&tviInsert);
	}

	return hTreeitem;
}
