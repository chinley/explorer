#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <Shellapi.h>
#include <io.h>
#include <Shlobj.h>
#include <oleidl.h>//DROPEFFECT_COPY与DROPEFFECT_MOVE
#include <stdio.h>
#include <errno.h>
#include<Shlobj.h>
#include<Commdlg.h>
#include "FileOperation.h"
#include "ControlOperation.h"

#define BIF_NEWDIALOGSTYLE    0x0040       //“浏览文件夹”对话框里新增“新建文件夹”按钮

extern TCHAR g_szAllPath[MAX_PATH];        //全路径
extern TCHAR g_szBeginEditName[MAX_PATH];  //重命名是开始的文件名



//【1】重命名文件（夹），磁盘
void ReFileName(HWND hwnd, LPNMHDR lpnmhdr)
{
	NMLVDISPINFO *p = (NMLVDISPINFO*)lpnmhdr;
	HWND hwndEdit = ListView_GetEditControl(GetDlgItem(hwnd, IDC_LISTVIEWFILE));
	TCHAR szFileName[MAX_PATH];
	ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR)); 
	GetWindowText(hwndEdit, szFileName, sizeof(szFileName) / sizeof(TCHAR));
	
	if (0 == lstrcmp(szFileName, g_szBeginEditName))
	{
		return;
	}
	//扩展名
	
	TCHAR *pOldExtension = strrchr(g_szBeginEditName, '.');

	if (NULL != pOldExtension)      //源路径有扩展名
	{	
		TCHAR *pNewExtension = strrchr(szFileName, '.');
		
		if (0 != lstrcmp(pOldExtension, pNewExtension))
		{
			int iRet = MessageBox(hwnd, TEXT("如果改变文件扩展名，可能会导致文件不可用。\n确实要更改吗?"), TEXT("重命名"), MB_YESNO | MB_ICONWARNING);
			if (IDNO == iRet)
			{
				pOldExtension = NULL;
				pNewExtension = NULL;
				return;
			}
		}
		pNewExtension = NULL;
	}
	pOldExtension = NULL;
	
	TCHAR szOldPath[MAX_PATH];
	TCHAR szNewPath[MAX_PATH];

	ZeroMemory(szOldPath, sizeof(szOldPath) / sizeof(TCHAR));
	ZeroMemory(szNewPath, sizeof(szNewPath) / sizeof(TCHAR));

	wsprintf(szOldPath, "%s\\%s", g_szAllPath, g_szBeginEditName);

	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //磁盘页面
	{
		return;
	}
	else
	{
		wsprintf(szNewPath, "%s\\%s", g_szAllPath, szFileName);
	}
	
	if (FALSE == FileOperation(hwnd, FILE_RENAME, szOldPath, szNewPath))
	{
		return;
	}

	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_DELETEITEM, p->item.iItem, 0);
	InsertFileToListView(hwnd, szNewPath, p->item.iItem, TRUE);
}


//【2】文件操作
BOOL FileOperation(HWND hwnd, int Operation, TCHAR *szOldPath, TCHAR *szNewPath)
{
	SHFILEOPSTRUCT FileOp;
	ZeroMemory(&FileOp, sizeof(SHFILEOPSTRUCT));

	if ((FILE_COPY == Operation) || (FILE_MOVE == Operation))     //移动，复制
	{
		FileOp.fFlags = FOF_MULTIDESTFILES; //需要时直接创建一个文件夹,不需用户确定
		FileOp.pFrom = szOldPath;
		FileOp.pTo = szNewPath;           //一定要是NULL

		if (FILE_COPY == Operation)
		{
			FileOp.wFunc = FO_COPY;
		}
		else
		{
			FileOp.wFunc = FO_MOVE;
		}
	}
	else if (FILE_DELETE == Operation)      //删除
	{
		FileOp.fFlags =  FOF_MULTIDESTFILES;
		FileOp.pFrom = szOldPath;
		FileOp.pTo = NULL;           //一定要是NULL
		FileOp.wFunc = FO_DELETE;    //删除操作
	}
	else   //重命名
	{
		FileOp.fFlags = FOF_NOCONFIRMATION;   //不出现确认对话框
		FileOp.pFrom = szOldPath;
		FileOp.pTo = szNewPath;
		FileOp.wFunc = FO_RENAME;
	}
	if (0 != SHFileOperation(&FileOp))
	{
		return FALSE;
	}
	if (TRUE == FileOp.fAnyOperationsAborted)      //在提示框里单击了“取消”
	{
		return FALSE;
	}
	return TRUE;
}

//【3】创建文件并插入ListView中
void InsertFileToListView(HWND hwnd, TCHAR *szPath, int iItem, BOOL IsReName)
{
	FILEINFO FileInfo;
	ZeroMemory(&FileInfo, sizeof(FILEINFO));
	
	WIN32_FIND_DATA stFileInfo;
	ZeroMemory(&stFileInfo, sizeof(WIN32_FIND_DATA));
	
	HANDLE hFile = FindFirstFile(szPath, &stFileInfo);
	
	if (FALSE != GetFileInfo(&stFileInfo, &FileInfo))
	{
		int iIcon = GetImageLIstIndex(hwnd, &FileInfo, szPath, TRUE);
		if (TRUE == IsReName)     //重命名
		{
			InsertFileInfo(hwnd, iItem, iIcon, &FileInfo, TRUE, TRUE);
		}
		else               //不是重命名
		{
			InsertFileInfo(hwnd, 0, iIcon, &FileInfo, TRUE, FALSE);
		}
	}
	FindClose(hFile);
}


//【4】删除文件
void DeleteListFile(HWND hwnd)
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szPath[MAX_PATH];

	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);

	int i = 0;
	
	TCHAR *pszBuff = (TCHAR*)malloc(iCount * MAX_PATH);

	if (NULL == pszBuff)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	int *pszItem = (int*)malloc(iCount * sizeof(int)+1);
	if (NULL == pszItem)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	int iNextItem = 0;
	int iFileLen = 0, iTotalLen = 0;

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

		lv.mask = LVIF_TEXT;
		lv.iSubItem = 0;
		lv.iItem = iNextItem;
		lv.pszText = szFileName;
		lv.cchTextMax = sizeof(szFileName) / sizeof(TCHAR);
		SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
		
		ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
		wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
		
		iFileLen = lstrlen(szPath);
		
		memcpy(pszBuff + iTotalLen, szPath, sizeof(szPath) / sizeof(TCHAR));
		memcpy(pszBuff + (i+1)*(iTotalLen+iFileLen) + 1, TEXT("\0"), 1);

		iTotalLen++;

		iTotalLen += iFileLen;
		pszItem[i] = iNextItem;       //保存Item
	}
	if (FALSE == FileOperation(hwnd, FILE_DELETE, pszBuff, NULL))
	{
		free(pszBuff);
		pszBuff = NULL;
		free(pszItem);
		pszItem = NULL;
		return;
	}
	
	i = 0;
	TCHAR *pLastSymbol = NULL;
	TCHAR *pszTmp = pszBuff;
	while(NULL != (pLastSymbol = strchr(pszTmp, '\0')))
	{
		if (0 == lstrcmp(pszTmp, TEXT("")))
		{
			break;
		}
		if (0 == access(pszTmp,0))      //文件存在,删除失败
		{
			pszItem[i] = 0;
		}
		pszTmp = pLastSymbol+1;
		i++;
	}
	for (i=iCount; i>0; i--)
	{
		if (0 != pszItem[i-1])
		{
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_DELETEITEM, pszItem[i-1], 0);
		}
	}
	
	free(pszBuff);
	pszBuff = NULL;
	free(pszItem);
	pszItem = NULL;
	pLastSymbol = NULL;
}


//【5】全部选定
void SelectedAllItem(HWND hwnd)
{	
	SetFocus(GetDlgItem(hwnd, IDC_LISTVIEWFILE));
	LVITEM lv;
	ZeroMemory(&lv, sizeof(LVITEM));
		
	lv.mask = LVIF_STATE;

	lv.state= LVIS_SELECTED;                    
	lv.stateMask= LVIS_SELECTED;

	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_SETITEMSTATE, -1, (LPARAM)(LV_ITEM FAR *)&lv);
}


//【6】反向选定
void InverSelectedItem(HWND hwnd)
{
	SetFocus(GetDlgItem(hwnd, IDC_LISTVIEWFILE));
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEMCOUNT, 0, 0);

	int i = 0;
	
	LVITEM lv;
	ZeroMemory(&lv, sizeof(LVITEM));
	
	lv.mask = LVIF_STATE;
	
	for (i=0; i<iCount; i++)
	{
		if (LVIS_SELECTED == SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEMSTATE, i, (LPARAM) (UINT)LVIS_SELECTED))
		{
			lv.state= LVIS_ACTIVATING;
			lv.stateMask= LVIS_SELECTED;
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_SETITEMSTATE, i, (LPARAM)(LV_ITEM FAR *)&lv);
		}
		else
		{
			lv.state= LVIS_SELECTED;
			lv.stateMask= LVIS_SELECTED;
			
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_SETITEMSTATE, i, (LPARAM)(LV_ITEM FAR *)&lv);
		}
	}
}


//【7】文件(夹), 磁盘属性
void ShowFileAttribute(HWND hwnd)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	if (0 == iCount)   //在空白处右击
	{
		if (0 == lstrcmp(g_szAllPath, TEXT("")))    //磁盘页面
		{
			wsprintf(szPath, "%s", TEXT("explorer"));
		}
		else      //文件页面
		{
			strncpy(szPath, g_szAllPath, 2);
		}
	}
	else
	{
		TCHAR szFileName[MAX_PATH];
		ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));

		int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);

		LVITEM lv;
		ZeroMemory(&lv, sizeof(LVITEM));
		
		lv.mask = LVIF_TEXT;
		lv.iSubItem = 0;
		lv.iItem = iIndex;
		lv.pszText = szFileName;
		lv.cchTextMax = sizeof(szFileName) / sizeof(TCHAR);
		SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
		
		if (0 == lstrcmp(g_szAllPath, TEXT("")))    //磁盘页面
		{
			TCHAR *pLastSymbol = strrchr(szFileName, '(');
			wsprintf(szPath, "%c:", pLastSymbol[1]);     //后退路径
			pLastSymbol = NULL;
		}
		else
		{
			wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
		}
	}
	
	SHELLEXECUTEINFO sei;
	sei.cbSize	= sizeof(sei);
	sei.fMask	= SEE_MASK_NOCLOSEPROCESS | SEE_MASK_INVOKEIDLIST | SEE_MASK_FLAG_NO_UI;
	sei.hwnd	= hwnd;
	sei.lpVerb  = "properties";
	sei.lpFile  = szPath;
	sei.lpParameters = NULL;
	sei.lpDirectory  = NULL;
	sei.nShow		 = 0;
	sei.hInstApp	 = 0;
	sei.lpIDList	 = 0;
	ShellExecuteEx(&sei);
}


//【8】复制到，移动到文件夹
void CopyOrMoveToDirectory(HWND hwnd, BOOL IsCopyToDirectory)
{
	TCHAR szDirectory[MAX_PATH];
	ZeroMemory(szDirectory, sizeof(szDirectory) / sizeof(TCHAR));

	TCHAR szFileName[MAX_PATH];
	TCHAR szPath[MAX_PATH];

	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);

	TCHAR *pszNewBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszNewBuff)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	TCHAR *pszOldBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszOldBuff)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	int iOldFileLen = 0, iNewFileLen = 0, iNewTotalLen = 0, iOldTotalLen = 0;

	int i = 0;
	int iNextItem = 0;

	if (1 != iCount)
	{
		if (FALSE == GetCopyOrMoveDirectory(hwnd, szDirectory, NULL, iCount, IsCopyToDirectory))
		{
			return;
		}
	}
	TCHAR szTmp[MAX_PATH];
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
		wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);
		
		if (1 == iCount)
		{
			if (FALSE == GetCopyOrMoveDirectory(hwnd, szDirectory, szPath, iCount, IsCopyToDirectory))
			{
				free(pszNewBuff);
				pszNewBuff = NULL;
				free(pszOldBuff);
				pszOldBuff = NULL;
				return;
			}
		}
		
		//旧文件名,以文件格式“File1\0File2\0File3\0”
		iOldFileLen = lstrlen(szPath);
		
		memcpy(pszOldBuff + iOldTotalLen, szPath, sizeof(szPath) / sizeof(TCHAR));
		memcpy(pszOldBuff + (i+1)*(iOldTotalLen+iOldFileLen) + 1, TEXT("\0"), 1);

		iOldTotalLen++;

		iOldTotalLen += iOldFileLen;

		//新文件名,以文件格式“File1\0File2\0File3\0”
		ZeroMemory(szTmp, sizeof(szTmp) / sizeof(TCHAR));
		wsprintf(szTmp, "%s\\%s", szDirectory, szFileName);
		iNewFileLen = lstrlen(szTmp);
		
		memcpy(pszNewBuff + iNewTotalLen, szTmp, sizeof(szTmp) / sizeof(TCHAR));
		memcpy(pszNewBuff + (i+1)*(iNewTotalLen+iNewFileLen) + 1, TEXT("\0"), 1);

		iNewTotalLen++;

		iNewTotalLen += iNewFileLen;
	}
	if (TRUE == IsCopyToDirectory)       //复制到文件夹
	{
		FileOperation(hwnd, FILE_COPY, pszOldBuff, pszNewBuff);
	}
	else                      //移动到文件夹
	{
		FileOperation(hwnd, FILE_MOVE, pszOldBuff, pszNewBuff);
	}
	free(pszNewBuff);
	pszNewBuff = NULL;
	free(pszOldBuff);
	pszOldBuff = NULL;
}


//【9】粘贴文件
void PasteListFile(HWND hwnd)
{
	//注册一个新的剪贴板格式。然后可以将此格式用作有效的剪贴板格式。
	UINT uDropEffect = RegisterClipboardFormat("Preferred DropEffect");
	DWORD *dwEffect = (DWORD*)GetClipboardData(uDropEffect);

	BOOL bIsCopy = FALSE;
	if (DROPEFFECT_MOVE & *dwEffect)
	{
		bIsCopy = FALSE;//剪切后的粘贴
	}
	if (DROPEFFECT_COPY & *dwEffect)
	{
		bIsCopy = TRUE;//复制后的粘贴
	}

	TCHAR szFileName[MAX_PATH];
	
	HDROP hDrop = (HDROP)GetClipboardData(CF_HDROP); 

	// 获得文件数 
	/*DragQueryFile是一个成功拖放操作后获取被拖放文件的名称等信息的函数。

  INT DragQueryFile(
HDROP  hDrop,
UINT  iFile,
LPTSTR  lpszFile,
UINT    cch
);

【参数】
）hDrop：用于区分"包含被拖拽文件名称结构的句柄"。即存放所拖拽文件名称的数据结构的句柄，也就是文件名缓冲区的句柄

2）iFile：文件索引编号
作用：用于指明所要查询文件的序号，如果拖进多个文件，则索引编号从零开始。如果iFile的值为0xFFFFFFFF时，返回的是拖曳到窗体上的文件个数。如果iFile值在0和拖曳文件总数之间时，DragQueryFile拷贝与文件名存储缓冲区大小适应的文件名称到缓冲区中。

3）lpszFile
函数返回时，用于纯粹拖拽文件名称的缓冲区指针。文件名称是一个以空终止”\0”结尾的字符串。如果此参数是NULL，DragQueryFile函数返回拖拽的文件名的长度。函数DragQueryFile得到的文件名，是带完整路径的文件名

4）Cch
存储拖拽文件名称缓冲区的大小，即lpszFile指针所指缓冲区的字符数。

  hDrop=hDrop;iFile=0xFFFFFFFF;lpszFile=NULL;cch=0;
  */
	int iCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);//返回拖曳到窗体上的文件个数

	TCHAR *pszNewBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszNewBuff)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	TCHAR *pszOldBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszOldBuff)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}

	TCHAR *pLastSymbol = NULL;

	TCHAR szNewPath[MAX_PATH];

	int iOldFileLen = 0, iNewFileLen = 0, iNewTotalLen = 0, iOldTotalLen = 0;

	for(int i=0; i<iCount; i++)
	{ 
		ZeroMemory(szFileName, sizeof(szFileName) / sizeof(TCHAR));
		ZeroMemory(szNewPath, sizeof(szNewPath) / sizeof(TCHAR));

		DragQueryFile(hDrop, i, szFileName, sizeof(szFileName) / sizeof(TCHAR));

		pLastSymbol = strrchr(szFileName, '\\');
		wsprintf(szNewPath, "%s%s", g_szAllPath, pLastSymbol); //pLastSymbol第一个字符为"\",所以不需要"\\"
		
		//旧文件名,以文件格式“File1\0File2\0File3\0”
		iOldFileLen = lstrlen(szFileName);
		
		memcpy(pszOldBuff + iOldTotalLen, szFileName, sizeof(szFileName) / sizeof(TCHAR));
		memcpy(pszOldBuff + (i+1)*(iOldTotalLen+iOldFileLen) + 1, TEXT("\0"), 1);

		iOldTotalLen++;

		iOldTotalLen += iOldFileLen;

		//新文件名,以文件格式“File1\0File2\0File3\0”
		iNewFileLen = lstrlen(szNewPath);
		
		memcpy(pszNewBuff + iNewTotalLen, szNewPath, sizeof(szNewPath) / sizeof(TCHAR));
		memcpy(pszNewBuff + (i+1)*(iNewTotalLen+iNewFileLen) + 1, TEXT("\0"), 1);

		iNewTotalLen++;

		iNewTotalLen += iNewFileLen;
	}

	if (TRUE == bIsCopy)       //复制
	{
		if (FALSE == FileOperation(hwnd, FILE_COPY, pszOldBuff, pszNewBuff))
		{
			free(pszNewBuff);
			pszNewBuff = NULL;
			
			free(pszOldBuff);
			pszOldBuff = NULL;
			
			CloseClipboard();
			pLastSymbol = NULL;
			return;
		}
	}
	else                      //剪切
	{
		if (FALSE == FileOperation(hwnd, FILE_MOVE, pszOldBuff, pszNewBuff))
		{
			free(pszNewBuff);
			pszNewBuff = NULL;
			
			free(pszOldBuff);
			pszOldBuff = NULL;
			
			CloseClipboard();
			pLastSymbol = NULL;
			return;
		}
	}
	
	TCHAR *pszTmp = pszNewBuff;
	while(NULL != (pLastSymbol = strchr(pszTmp, '\0')))
	{
		if (0 == lstrcmp(pszTmp, TEXT("")))
		{
			break;
		}
		if (0 == access(pszTmp,0))      //文件存在,删除失败
		{
			InsertFileToListView(hwnd, pszTmp, 0, FALSE);
		}
		pszTmp = pLastSymbol+1;
	}
	
	free(pszNewBuff);
	pszNewBuff = NULL;
	
	free(pszOldBuff);
	pszOldBuff = NULL;
	
	CloseClipboard();
	pLastSymbol = NULL;
}

//【10】复制文件
void CopyOrCutListFile(HWND hwnd, BOOL bIsCopy)
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	int i = 0;

	TCHAR *pszBuff = (TCHAR*)malloc(iCount * MAX_PATH);

	if (NULL == pszBuff)
	{
		MessageBox(hwnd, TEXT("内存申请失败"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}

	int iNextItem = 0;

	int iFileLen = 0, iTotalLen = 0;
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
		wsprintf(szPath, "%s\\%s", g_szAllPath, szFileName);

		if (FALSE == bIsCopy)  //剪切
		{
			ZeroMemory(&lv, sizeof(LVITEM));
			
			lv.mask = LVIF_STATE;
			lv.state= LVIS_CUT;
			lv.stateMask= -1;
			
			SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_SETITEMSTATE, iNextItem, (LPARAM)(LV_ITEM FAR *)&lv);
		}

		iFileLen = lstrlen(szPath);
		
		memcpy(pszBuff + iTotalLen, szPath, sizeof(szPath) / sizeof(TCHAR));
		memcpy(pszBuff + (i+1)*(iTotalLen+iFileLen) + 1, TEXT("\0"), 1);

		iTotalLen++;

		iTotalLen += iFileLen;
	}
	if (FALSE == bIsCopy)  //剪切
	{
		CutOrCopyFiles(pszBuff, iTotalLen, FALSE);
	}
	else                  //复制
	{
		CutOrCopyFiles(pszBuff, iTotalLen, TRUE);
	}
	free(pszBuff);
	pszBuff = NULL;
}


//【11】新建文件
void CreateNewFile(HWND hwnd, int iCreate)
{
	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //磁盘页面
	{
		MessageBox(hwnd, TEXT("磁盘页面不能新建文件"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	switch (iCreate)
	{
	case CREATE_DIRECTORY:         //文件夹
		{
			TCHAR szPath[MAX_PATH];
			ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
			
			wsprintf(szPath, "%s\\新建文件夹", g_szAllPath);
			if (0 == CreateDirectory(szPath, NULL))
			{
				ShowErrorMsg(hwnd, TEXT("新建文件夹时出错"));
				return;
			}
			InsertFileToListView(hwnd, szPath, 0, FALSE);
		}
		break;
	case CREATE_BITMAP:           //BMP图像
		{
			CreateNormalFile(hwnd, TEXT("新建 BMP 图像.bmp"), TEXT("新建BMP图像时出错"));
		}
		break;
	case CREATE_WORD:            //Word 文档
		{
			CreateNormalFile(hwnd, TEXT("新建 Microsoft Word 文档.doc"), TEXT("新建Word时出错"));
		}
		break;
	case CREATE_PPT:             //PowerPoint 演示文稿
		{
			CreateNormalFile(hwnd, TEXT("新建 Microsoft PowerPoint 演示文稿.ppt"), TEXT("新建压缩文件时出错"));
		}
		break;
	case CREATE_TEXT:             //文本文档
		{
			CreateNormalFile(hwnd, TEXT("新建 文本文档.txt"), TEXT("新建文本文档时出错"));
		}
		break;
		
	case CREATE_WAV:             //波形声音
		{
			CreateNormalFile(hwnd, TEXT("新建 波形声音.wav"), TEXT("新建波形声音文件时出错"));
		}
		break;
	case CREATE_EXCEL:           //Microsoft Excel 工作表
		{
			CreateNormalFile(hwnd, TEXT("新建 Microsoft Excel 工作表.xls"), TEXT("新建Excel时出错"));
		}
		break;
	default:
		break;
	}
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEMCOUNT, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_EDITLABEL, iCount-1, 0);
}


//【12】新建普通文件
void CreateNormalFile(HWND hwnd, TCHAR *szCreateName, TCHAR *szTitle)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	wsprintf(szPath, "%s\\%s", g_szAllPath, szCreateName);
	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,0);

	if (ERROR_ALREADY_EXISTS == GetLastError())    //ERROR_ALREADY_EXISTS时文件不会INVALID_HANDLE_VALUE
	{
		MessageBox(hwnd, TEXT("当文件已存在时，无法创建该文件"), TEXT(szTitle), MB_OK | MB_ICONERROR);
		CloseHandle(hFile);
		return;
	}

	if(hFile == INVALID_HANDLE_VALUE)
	{
		ShowErrorMsg(hwnd, szTitle);
		CloseHandle(hFile);
		return;
	}
	InsertFileToListView(hwnd, szPath, 0, FALSE);
	CloseHandle(hFile);
}


//【13】打开文件
void OpenListFile(HWND hwnd)
{
	DblclkFile(hwnd);
}


//【14】剪切或复制文件
void CutOrCopyFiles(TCHAR *lpBuffer, UINT uBufLen, BOOL bIsCopy)
{
	UINT uDropEffect;
	DROPFILES dropFiles;
	UINT uGblLen, uDropFilesLen;
	HGLOBAL hGblFiles, hGblEffect;
	TCHAR *szData = NULL, *szFileList = NULL;

	DWORD *dwDropEffect;

	uDropEffect = RegisterClipboardFormat("Preferred DropEffect");
	hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
	dwDropEffect = (DWORD*)GlobalLock(hGblEffect);
	if(TRUE == bIsCopy)     //复制
	{
		*dwDropEffect = DROPEFFECT_COPY;
	}
	else    //剪切
	{
		*dwDropEffect = DROPEFFECT_MOVE;
	}
	GlobalUnlock(hGblEffect);

	uDropFilesLen = sizeof(DROPFILES);
	dropFiles.pFiles = uDropFilesLen;
	dropFiles.pt.x = 0;
	dropFiles.pt.y = 0;
	dropFiles.fNC = FALSE;
	dropFiles.fWide = TRUE;

	uGblLen = uDropFilesLen + uBufLen * 2 + 8;
	hGblFiles= GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, uGblLen);
	szData=(char*)GlobalLock(hGblFiles);
	memcpy(szData, (LPVOID)(&dropFiles), uDropFilesLen);
	szFileList = szData + uDropFilesLen;

	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,
			lpBuffer, uBufLen, (WCHAR *)szFileList, uBufLen);
	
	GlobalUnlock(hGblFiles);

	if(0 != OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_HDROP, hGblFiles );
		SetClipboardData(uDropEffect, hGblEffect);
	}
	CloseClipboard();
	szData = NULL;
	szFileList = NULL;
	dwDropEffect = NULL;
}


//【15】得到要复制到，移动到文件夹的目录路径
BOOL GetCopyOrMoveDirectory(HWND hwnd, TCHAR *szDirectory, TCHAR *szFileName, int iFileCount, BOOL IsCopyToDirectory)
{
	BROWSEINFO sBrowseInfo;
	LPITEMIDLIST lpitem;
	ZeroMemory(&sBrowseInfo, sizeof(BROWSEINFO));
	sBrowseInfo.hwndOwner = hwnd;

	if (1 != iFileCount)  //复制多个文件
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));
		
		if (TRUE == IsCopyToDirectory)      //复制到文件夹
		{
			wsprintf(szMsg, "选择你想讲这 %i 个项目复制到的地方，然后单击“确定”按钮", iFileCount);
		}
		else
		{
			wsprintf(szMsg, "选择你想讲这 %i 个项目移动到的地方，然后单击“确定”按钮", iFileCount);
		}
		sBrowseInfo.lpszTitle = TEXT(szMsg);
	}
	else
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));
		
		if (TRUE == IsCopyToDirectory)      //复制到文件夹
		{
			wsprintf(szMsg, "选择你想讲这 %s 个项目复制到的地方，然后单击“确定”按钮", szFileName);
		}
		else
		{
			wsprintf(szMsg, "选择你想讲这 %s 个项目移动到的地方，然后单击“确定”按钮", szFileName);
		}
		sBrowseInfo.lpszTitle = TEXT(szMsg);
	}
	
	sBrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE; //不包含网络目录
	
	lpitem = SHBrowseForFolder(&sBrowseInfo);
	
	if (NULL == lpitem)                                    //判断是否已经选择文件夹
	{
		CoTaskMemFree(lpitem);
		return FALSE;
	}
	
	SHGetPathFromIDList(lpitem, szDirectory);	     //获得文件夹的路径
	CoTaskMemFree(lpitem);

	return TRUE;
}


//【16】显示出错信息
void ShowErrorMsg(HWND hwnd, TCHAR *szTitle)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf, 0, NULL );
	MessageBox(hwnd, (LPCTSTR)lpMsgBuf, TEXT(szTitle), MB_OK | MB_ICONERROR);
	LocalFree(lpMsgBuf);
}


//【17】IE主页
void GetIEHomePage(HWND hwnd)
{
	HKEY hKey;
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Microsoft\\Internet Explorer\\Main"), 0, KEY_READ,&hKey);
	if(lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return;
	}
	DWORD dwKeyType = REG_SZ;
	TCHAR szValueData[MAX_PATH];
	ZeroMemory(szValueData, sizeof(szValueData) / sizeof(TCHAR));
	DWORD dwSize = sizeof(szValueData) / sizeof(TCHAR);
	lRet = RegQueryValueEx(hKey, TEXT("Default_Page_URL"), NULL, &dwKeyType, (LPBYTE)szValueData, &dwSize);
	if(lRet != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return;
	}
	RegCloseKey(hKey);
	ShellExecute(hwnd, "open", szValueData, NULL, NULL, SW_SHOWNORMAL);
}

//FileOperation.cpp【18】打开某个文件
void OpenMenuFile(HWND hwnd)
{
	//DblclkFile(hwnd);

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名
	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
	ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	ofn.lpstrFilter = TEXT("所有文件\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//设置过滤
	ofn.nFilterIndex = 1;//过滤器索引
	ofn.lpstrFile = strFilename;//接收返回的文件名，注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
	ofn.lpstrInitialDir = NULL;//初始目录为默认
	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
	if (GetOpenFileName(&ofn))
	{
		SHELLEXECUTEINFO ShExecInfo;
		memset( &ShExecInfo, 0, sizeof( SHELLEXECUTEINFO ) );
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = hwnd;
		ShExecInfo.lpVerb = "Open";
		ShExecInfo.lpFile= ofn.lpstrFile;      
		ShExecInfo.lpParameters = NULL; 
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = SW_SHOWNORMAL;
		ShExecInfo.hInstApp = NULL; 
		ShellExecuteEx(&ShExecInfo);

		
	}

}
