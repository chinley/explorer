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

TCHAR g_szBackFowardPath[MAX_PATH] = {0}; //���ˣ�ǰ��·��
TCHAR g_szBackPath[MAX_PATH] = {0};       //��ʱ�洢����·��
TCHAR g_szAllPath[MAX_PATH] = {0};        //ȫ·��
TCHAR g_szBeginEditName[MAX_PATH];  //�������ǿ�ʼ���ļ���



//��1�������е���ɾ��
void DeleteAllColumn(HWND hwnd)
{
	int i = 0;
	for (i=3; i>=0; i--)
	{
		SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_DELETECOLUMN, i, 0);
	}
}


//��2�����ò鿴��ʽ
void SetLookMode(HWND hwndListView, DWORD dwLookMode)
{
	DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);  
    dwStyle &= ~LVS_TYPEMASK;
    dwStyle |= dwLookMode;
    SetWindowLong(hwndListView, GWL_STYLE,dwStyle );
}


//��3���õ�Image_Listͼ��������
int GetImageLIstIndex(HWND hwnd, pFILEINFO pFileInfo, TCHAR *szPath, BOOL IsFile)
{
	int iIndex = 0;
	
	//�� WIN32 ��׼ͼ�����
	
	SHFILEINFO shfi;
	ZeroMemory(&shfi, sizeof(SHFILEINFO));
	
	CoInitialize(NULL);
	SHGetFileInfo(szPath, 0, &shfi, sizeof(shfi),
		SHGFI_SMALLICON | SHGFI_TYPENAME | SHGFI_ICON | SHGFI_DISPLAYNAME);
	
	if (TRUE == IsFile)
	{
		if (0 == lstrcmp(shfi.szTypeName, TEXT("�ļ�")))      //�ļ���
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("�ļ���"));

		}
		else        //�ļ�
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", shfi.szTypeName);
		}

	}
	else
	{
		//Сͼ��
		iIndex = ImageList_AddIcon(g_TreeViewImageList, shfi.hIcon);
		wsprintf(pFileInfo->szFileName, "%s", shfi.szDisplayName);
	}
	
	//Сͼ��
	iIndex = ImageList_AddIcon(g_ListViewImageListSmall, shfi.hIcon);
	DestroyIcon(shfi.hIcon);

	SHGetFileInfo(szPath, 0, &shfi, sizeof(shfi),
		SHGFI_LARGEICON | SHGFI_ICON);

	//��ͼ��
	iIndex = ImageList_AddIcon(g_ListViewImageListLarge, shfi.hIcon);
	DestroyIcon(shfi.hIcon);

	CoUninitialize();
	return iIndex;
}


//��4����ӦWM_Notify
void OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnmhdr = (LPNMHDR)lParam;
	switch (lpnmhdr->code)
	{
	case NM_CLICK:       //����
		{
			if (IDC_LISTVIEWFILE == wParam)
			{
				SetSelectedtInfo(hwnd);
			}
		}
		break;
	case NM_DBLCLK:      //˫��
		{
			if (IDC_LISTVIEWFILE == wParam)
			{
				DblclkFile(hwnd);
			}
		}
		break;
	case LVN_BEGINLABELEDIT:      //��������ʼ
		{
			BeginRename(hwnd, lpnmhdr);
		}
		break;
	case LVN_ENDLABELEDIT:        //����������
		{
			ReFileName(hwnd, lpnmhdr);
		}
		break;
	case EN_MSGFILTER:            //����Enter��
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


//��5����������ʼ
void BeginRename(HWND hwnd, LPNMHDR lpnmhdr)
{
	HWND hwndEdit = ListView_GetEditControl(GetDlgItem(hwnd, IDC_LISTVIEWFILE));
	
	ZeroMemory(g_szBeginEditName, sizeof(g_szBeginEditName) / sizeof(TCHAR));
	
	GetWindowText(hwndEdit, g_szBeginEditName, sizeof(g_szBeginEditName) / sizeof(TCHAR));
	
	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //����ҳ��
	{
		TCHAR szTmp[MAX_PATH];
		ZeroMemory(szTmp, sizeof(szTmp) / sizeof(TCHAR));

		lstrcpy(szTmp, g_szBeginEditName);
		szTmp[lstrlen(szTmp)-5] = '\0';
		SetWindowText(hwndEdit, szTmp);
	}
}


//��6���õ��������ͺ����ô�����
void GetDiskTypeAndName(UINT uiDiskType, TCHAR *szFilePath, pFILEINFO pFileInfo)
{
	switch (uiDiskType)
	{
	case DRIVE_UNKNOWN:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("δ֪��������"));
			wsprintf(pFileInfo->szFileName, "δ֪�������� (%s)", szFilePath);
		}
		break;			
	case DRIVE_NO_ROOT_DIR:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("��������Ч"));
			wsprintf(pFileInfo->szFileName, "��������Ч (%s)", szFilePath);
		}
		break;			
	case DRIVE_REMOVABLE:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("���ƶ�����"));
			wsprintf(pFileInfo->szFileName, "���ƶ����� (%s)", szFilePath);
		}
		break;			
	case DRIVE_FIXED:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("���ش���"));
			wsprintf(pFileInfo->szFileName, "���ش��� (%s)", szFilePath);
		}
		break;			
	case DRIVE_REMOTE:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("�������"));
			wsprintf(pFileInfo->szFileName, "������� (%s)", szFilePath);
		}
		break;			
	case DRIVE_CDROM:
		{
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("����"));
			wsprintf(pFileInfo->szFileName, "���� (%s)", szFilePath);
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
			wsprintf(pFileInfo->szFileAttributes, "%s", TEXT("GetDriveType�ķ���ֵ�Ƿ�"));
			wsprintf(pFileInfo->szFileName, "GetDriveType�ķ���ֵ�Ƿ� (%s)", szFilePath);
		}
		break;	
	}
}


//��7����ListView����Ӵ�����Ϣ
void AddFileInfoToListView(HWND hwnd)
{
	// ɾ�����е���
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
	
	//�ļ�����
	TCHAR szFileCount[10];
	ZeroMemory(szFileCount, sizeof(szFileCount) / sizeof(TCHAR));
	
	wsprintf(szFileCount, "%i ������", iNum);
	
	SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)szFileCount);
	
	TCHAR szFilePath[5];
	
	FILEINFO FileInfo;

	int j = 0;
	for (j=0; j<iNum; j++)
	{
		ZeroMemory(szFilePath, sizeof(szFilePath) / sizeof(TCHAR));
		wsprintf(szFilePath, "%c:", szDiskPath[j]);

		ZeroMemory(&FileInfo, sizeof(FILEINFO));

		//�õ��������ͺ����ô�����
		UINT uiDiskType = GetDriveType(szFilePath);
		GetDiskTypeAndName(uiDiskType, szFilePath, &FileInfo);


		lstrcat(szFilePath, TEXT("\\"));
		int iIcon = GetImageLIstIndex(hwnd, &FileInfo, szFilePath, FALSE);

		//�����̲������̲�������GetDiskFreeSpaceExʱ������쳣
		if (0 != lstrcmp(FileInfo.szFileAttributes, TEXT("���ƶ�����"))
			|| (NULL == strstr(FileInfo.szFileName, TEXT("����"))))			
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
			GetDlgItem(hwnd, IDC_TREEFILE), //��ȡ��״��ͼ�ؼ��ľ��
			TVI_ROOT, 
			iIcon, 
			FileInfo.szFileName, 
			MAX_PATH);

		ZeroMemory(szFilePath, sizeof(szFilePath) / sizeof(TCHAR));
		wsprintf(szFilePath, "%s", TEXT("�ҵĵ���"));
		SetDlgItemText(GetDlgItem(hwnd, IDC_TOOLADDRESS), IDM_EDITPATH, szFilePath);
	}
}



//��8���õ��ļ���Ϣ
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


//��9������ѡ���ļ���Ϣ���ڴ���������һ��  ��һ����ʾ��Ϣ��
//SendMessage�����ĵ�4������ Ϊ0����ʾ��һ��
void SetSelectedtInfo(HWND hwnd)
{
	TCHAR szMsg[MAX_PATH];
	ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);

	if (0 == iCount)
	{
		int iItemCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEMCOUNT, 0, 0);
		wsprintf(szMsg, "%i ������", iItemCount);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT(szMsg));
		return;
	}
	else if (1 == iCount)//ֻѡ��һ������
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
		if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //ѡ��һ������&&�ö������ļ���
		{
			SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT("ѡ�� 1 ������"));
		}
		else//ѡ��һ������&&�ö������ļ���
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

			wsprintf(szMsg, "�����͡�: %s ���޸����ڡ�: %s  ����С��: %s", FileInfo.szFileAttributes,
				FileInfo.Optional.szLastWriteTime, FileInfo.szFileSize);
			SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT(szMsg));
		}
	}
	else//ѡ�в�ֹһ������
	{
		wsprintf(szMsg, "ѡ�� %i ������", iCount);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)TEXT(szMsg));
		return;
	}
}


//��10��˫���ļ�(��)�����
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
	
	lv.mask = LVIF_TEXT;  //�õ��ļ�(��)���������
	lv.iSubItem = 0;
	lv.iItem = iIndex;
	lv.pszText = szFileName;
	lv.cchTextMax = sizeof(szFileName) / sizeof(TCHAR);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
	
	if (0 == lstrcmp(g_szAllPath, TEXT("")))      //����
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
		InitListViewColumn(hwnd, TRUE);
		TCHAR *pLastSymbol = strrchr(szFileName, '(');
				
		wsprintf(g_szAllPath, "%c:", pLastSymbol[1]);     //����·��
		lstrcpy(g_szBackFowardPath, g_szAllPath);         //����ǰ��������·��
		lstrcpy(g_szBackPath, g_szAllPath);
			
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		lstrcpy(szPath, g_szAllPath);
		
		SearchFile(hwnd, szPath);
	}
	else            //�ļ�(��)
	{
		TCHAR szPath[MAX_PATH];
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		
		wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
		if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //�ļ���
		{
			if (NULL == strstr(g_szBackFowardPath, szPath))     //����ǰ��������·��
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


//��11������Ŀ¼���ļ�
void SearchFile(HWND hwnd, TCHAR *szPath)
{
	WIN32_FIND_DATA stFileInfo;
	ZeroMemory(&stFileInfo, sizeof(WIN32_FIND_DATA));

	if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //�ļ���
	{
		lstrcpy(g_szAllPath, szPath);     //ȫ·��
		
		lstrcat(szPath, TEXT("\\*.*"));
		HANDLE hFile = FindFirstFile(szPath, &stFileInfo);
		
		DWORD errorcode = 0;
		
		FILEINFO FileInfo;
		
		// ɾ�����е���
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
		wsprintf(szFileCount, "%i ������", iFileCount);
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 0, (LPARAM)szFileCount);
		
		ZeroMemory(szFileCount, sizeof(szFileCount) / sizeof(TCHAR));
		dwDirectorySize = dwDirectorySize / 1024;      //��ΪdwDirectorySize�ĵ�λ�Ѿ���KB
		sprintf(szFileCount, "%.2lf MB", dwDirectorySize);
		
		SendMessage(GetDlgItem(hwnd, IDC_STATUSFILE), SB_SETTEXT, 1, (LPARAM)szFileCount);

		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		if (2 == lstrlen(g_szAllPath))     //����
		{
			wsprintf(szPath, "%s\\", g_szAllPath);
		}
		else              //�ļ���
		{
			wsprintf(szPath, "%s", g_szAllPath);
		}
		SetDlgItemText(GetDlgItem(hwnd, IDC_TOOLADDRESS), IDM_EDITPATH, szPath);
	}
	
	else            //�ļ�
	{
		int iShell = (int)ShellExecute(hwnd, "open", szPath, NULL, NULL, SW_SHOWNORMAL);
		if (32 > iShell)
		{
			ShellExecuteError(hwnd, szPath, iShell);
		}
	}
}


//��12��ShellExecuteִ��ʧ�ܵĴ�����Ϣ
void ShellExecuteError(HWND hwnd, TCHAR *szPath, int iError)
{
	switch (iError)
	{
	case SE_ERR_ACCESSDENIED:          //��Ч��exe�ļ�
		{
			TCHAR szMsg[MAX_PATH];
			ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));

			wsprintf(szMsg, "%s ������Ч�� Win32 Ӧ�ó���", szPath);
			MessageBox(hwnd, TEXT(szMsg), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	case SE_ERR_NOASSOC:    //�޳������
		{
			MessageBox(hwnd, TEXT("û����������ļ���չ��������Ӧ�ó���"), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	case SE_ERR_SHARE:      //���������ͻ
		{
			MessageBox(hwnd, TEXT("���������ͻ"), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	case SE_ERR_DLLNOTFOUND: //ָ���Ķ�̬���ӿ�û�б��ҵ�
		{
			MessageBox(hwnd, TEXT("ָ���Ķ�̬���ӿ�û�б��ҵ�"), TEXT(szPath), MB_OK | MB_ICONERROR);
		}
		break;
	default:
		break;
	}
}

//��13������
void BackPath(HWND hwnd)
{
	if (0 == lstrcmp(g_szBackPath, TEXT("")))       //����ҳ��
	{
		return;
	}
	TCHAR *pLastSymbol = strrchr(g_szBackPath, '\\');        //���̵���һ��Ŀ¼

	if (NULL == pLastSymbol)      //C: ʱ
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


//��14��ǰ��
void FowardPath(HWND hwnd)
{
	if (0 == lstrcmp(g_szBackFowardPath, TEXT("")))      //���������ʱ
	{
		return;
	}
	if (0 == lstrcmp(g_szBackPath, g_szBackFowardPath))  //��δ���˹�
	{
		return;
	}

	ImageList_RemoveAll(g_ListViewImageListSmall);
	ImageList_RemoveAll(g_ListViewImageListLarge);

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	//���̵���һ��Ŀ¼
	TCHAR *pLastSymbol = strchr(g_szBackFowardPath+lstrlen(g_szBackPath)+1, '\\');

	if (NULL == pLastSymbol)       //�ٴ�ǰ��ʱ�����һ��
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

//��15��ת��·��
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
	
	if (0 == lstrcmp(szPath, TEXT("�ҵĵ���")))
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

		wsprintf(szMsg, "�Ҳ����ļ�(��)��%s������ȷ��·����ȷ��",szPath);
		MessageBox(hwnd,TEXT(szMsg),TEXT("VLExplorer"),MB_OK | MB_ICONERROR);
		return;
	}
	if (GetFileAttributes(szPath) & FILE_ATTRIBUTE_DIRECTORY)      //�ļ���
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
	}

	//��·���е�'\'ȥ��

	if ('\\' == szPath[lstrlen(szPath)-1])
	{
		if ('\\' == szPath[lstrlen(szPath)-2])
		{
			TCHAR szMsg[MAX_PATH];
			ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));
			
			wsprintf(szMsg, "Windows �Ҳ�����%s��������ƴд�Ƿ���ȷ��Ȼ�����ԣ��򵥻�����ʼ����ť���ٵ�����������������\n��Ŀ��",szPath);
			MessageBox(hwnd,TEXT(szMsg),TEXT("��ַ��"),MB_OK | MB_ICONERROR);
			return;
		}
		szPath[lstrlen(szPath)-1] = '\0';
	}
	SearchFile(hwnd, szPath);
}


//��16��ˢ�´���
void RefreshWindow(HWND hwnd)
{
	if (0 == lstrcmp(g_szAllPath, TEXT("")))           //����Ŀ¼
	{
		ImageList_RemoveAll(g_ListViewImageListSmall);
		ImageList_RemoveAll(g_ListViewImageListLarge);
		ImageList_RemoveAll(g_TreeViewImageList);
		
		InitListViewColumn(hwnd, FALSE);
		AddFileInfoToListView(hwnd);
		return;
	}
	
	//�ļ���Ŀ¼

	ImageList_RemoveAll(g_ListViewImageListSmall);
	ImageList_RemoveAll(g_ListViewImageListLarge);

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
	lstrcpy(szPath, g_szAllPath);
	SearchFile(hwnd, szPath);
}


//��17������, ����ʹ��ȫ�ֱ�����������һ���ṹ��
void SortListFile(HWND hwnd, int lParamSort)
{
	SORTINFO SortInfo;
	ZeroMemory(&SortInfo, sizeof(SORTINFO));
	SortInfo.hwndListView = GetDlgItem(hwnd, IDC_LISTVIEWFILE);
	SortInfo.lParamSort = lParamSort;
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_SORTITEMS, (WPARAM)(LPARAM)&SortInfo, (LPARAM)(PFNLVCOMPARE)CompareFunc);
}


//��18������Ļص����� lParam1, lParam2��LVM_SETITEMʱ��lparam
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
	case SORT_NAME:          //����
		{
			lv.iSubItem = 0;
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);
			
			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);
			
			iRet = lstrcmp(szlParam1, szlParam2);      //��С����
		}
		break;
	case SORT_SIZE:          //��С
		{
			if (0 == lstrcmp(g_szAllPath, TEXT("")))           //����Ŀ¼
			{
				lv.iSubItem = 2;
			} 
			else        //�ļ�(��)
			{
				lv.iSubItem = 1;
			}
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);

			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);

			iRet = atoi(szlParam1) - atoi(szlParam2);      //��С����
		}
		break;
	case SORT_STYLE:         //����
		{
			if (0 == lstrcmp(g_szAllPath, TEXT("")))           //����Ŀ¼
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
			
			iRet = lstrcmp(szlParam1, szlParam2);      //��С����
		}
		break;
	case SORT_WRITETIME:     //�޸�ʱ��
		{
			if (0 == lstrcmp(g_szAllPath, TEXT("")))           //����Ŀ¼
			{
				return 0;         //return 0������
			} 
			lv.iSubItem = 3;
			lv.pszText = szlParam1;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam1, (LPARAM)(LV_ITEM FAR*)&lv);
			
			lv.pszText = szlParam2;
			SendMessage(pSort->hwndListView, LVM_GETITEMTEXT, lParam2, (LPARAM)(LV_ITEM FAR*)&lv);
			
			iRet = lstrcmp(szlParam1, szlParam2);      //��С����
		}
		break;
	case SORT_GROUP:         //��������
		{
		}
		break;
	default:
		break;
	}

	return iRet;
}


//��19���ж�Windows�Ƿ�Ϸ�
void IsRightful(HWND hwnd)
{
	TCHAR szURL[MAX_PATH];
	ZeroMemory(szURL, sizeof(szURL) / sizeof(TCHAR));
	
	wsprintf(szURL, "%s", TEXT("http://www.microsoft.com/zh-cn/howtotell/default.aspx"));
	if (32 > (int)ShellExecute(hwnd, "open", szURL, NULL, NULL, SW_SHOWNORMAL))
	{
		MessageBox(hwnd, TEXT("��Windows�Ϸ���ҳʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
	}
}

//��20��������ݷ�ʽ
void CreateListFileShortCut(HWND hwnd)
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	TCHAR szLinkName[MAX_PATH];
	
	BOOL IsDisk = FALSE;
	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //����ҳ��
	{
		int iRet = MessageBox(hwnd, TEXT("Windows �޷��ٵ�ǰλ�ô�����ݷ�ʽ��\nҪ�ѿ�ݷ�ʽ������������?"), TEXT("��ݷ�ʽ"), MB_YESNO | MB_ICONQUESTION);
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
			
			//�ļ����ֲ��ܰ���/\:���ַ�
			szFileName[lstrlen(szFileName)-2] = ')';
			szFileName[lstrlen(szFileName)-1] = '\0';
			wsprintf(szLinkName, "%s\\��ݷ�ʽ ��%s.lnk", szDeskTopPath, szFileName);
			pLastSymbol = NULL;
		}
		else
		{
			wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
			wsprintf(szLinkName, "%s\\��ݷ�ʽ ��%s.lnk", g_szAllPath, szFileName);
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

//��21��������ݷ�ʽ
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
			
			pisl->SetPath(szPath);      //����Ҫ������ݷ�ʽ���ļ�
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
				MessageBox(hwnd, TEXT("������ݷ�ʽʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
				pisl->Release();
				CoUninitialize();
				return FALSE;
			}
		}
		else
		{
			MessageBox(hwnd, TEXT("������ݷ�ʽʧ��"), TEXT("VLExplorer"), MB_OK | MB_ICONERROR);
			pisl->Release();
			CoUninitialize();
			return FALSE;
		}
		CoUninitialize();
	}
	return TRUE;
}

//��22��������֧������
void OpenHelpCenter(HWND hwnd)
{/*
	TCHAR szWindowsPath[MAX_PATH];
	ZeroMemory(szWindowsPath, sizeof(szWindowsPath) / sizeof(TCHAR));

	GetWindowsDirectory(szWindowsPath, sizeof(szWindowsPath) / sizeof(TCHAR));
	lstrcat(szWindowsPath,TEXT("ReadMe.txt"));// TEXT("\\pchealth\\helpctr\\binaries\\HelpCtr.exe"));//�ú�����һ���ַ�����������һ���ַ������档
	if (0 != access(szWindowsPath, 0))
	{
		MessageBox(hwnd,TEXT("�Ҳ���������֧�������ļ�"),TEXT("MyResourceExplorer"),MB_OK | MB_ICONERROR);
		return;
	}
	ShellExecute(hwnd, "open", szWindowsPath, NULL, NULL, SW_SHOWNORMAL);
	*/

	TCHAR szWindowsPath[MAX_PATH]=TEXT(".\\ʹ��˵����.CHM");
	if (0 != access(szWindowsPath, 0))
	{
		MessageBox(hwnd,TEXT("�Ҳ���������֧�������ļ�"),TEXT("MyResourceExplorer"),MB_OK | MB_ICONERROR);
		return;
	}
	ShellExecute(hwnd, "open", szWindowsPath, NULL, NULL, SW_SHOWNORMAL);

}