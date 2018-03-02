#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ControlOperation.h"


//�����ļ���Ϣ
void InsertFileInfo(
					HWND hwnd,
					int iItem,
					int iImage,
					pFILEINFO pFileInfo,
					BOOL IsFile,
					BOOL IsReName)
{
	//�б���ͼ�ؼ�
	HWND hwndListFile = GetDlgItem(hwnd, IDC_LISTVIEWFILE);
	LVITEM lvItem;//ָ��������б���ͼ�������
	ZeroMemory(&lvItem, sizeof(LVITEM));
	
	lvItem.mask    = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;

	if (TRUE == IsReName)   //������
	{
		lvItem.iItem = iItem;
	}
	else       //����������
	{
		lvItem.iItem   = //������Ŀ��
			SendMessage(
			hwndListFile,
			LVM_GETITEMCOUNT,//�����б���ͼ�ؼ��е���Ŀ��
			0, 
			0);
	}	
	lvItem.iImage = iImage;
	lvItem.iSubItem = 0;
	lvItem.pszText = pFileInfo->szFileName;
	lvItem.lParam = lvItem.iItem;        //��������
	SendMessage(
		hwndListFile,
		LVM_INSERTITEM,//���б���ͼ�ؼ��в���һ������Ŀ
		0, //wParam����Ϊ0
		(LPARAM)&lvItem);//lParam��ָ��ָ���б���ͼ������Ե�LVITEM�ṹ��ָ��
	
	//��2��3��4��
	lvItem.mask    = LVIF_TEXT;

	if (FALSE == IsFile)      //����
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
	
	if (FALSE == IsFile)      //����
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
	
	if (FALSE == IsFile)      //�����ļ����У����Ǵ���
	{
		lvItem.pszText  = pFileInfo->Optional.szTotal;
	}
	else                     //�ļ����У�
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

//����ڵ㵽TreeView�ؼ���
//��ExplorerFun.cpp�г��֣����ڸ��ļ�������������
HTREEITEM AddNodeInTree(
						HWND hwndTreeView,
						HTREEITEM hParent,
						int iImage,
						TCHAR *szItemText,
						DWORD dwTextLength)
{
	TVINSERTSTRUCT tviInsert;
	HTREEITEM hTreeitem;
	
	if (TVI_ROOT == hParent)//hParent�������ĸ�
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
