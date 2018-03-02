#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <Shellapi.h>
#include <io.h>
#include <Shlobj.h>
#include <oleidl.h>//DROPEFFECT_COPY��DROPEFFECT_MOVE
#include <stdio.h>
#include <errno.h>
#include<Shlobj.h>
#include<Commdlg.h>
#include "FileOperation.h"
#include "ControlOperation.h"

#define BIF_NEWDIALOGSTYLE    0x0040       //������ļ��С��Ի������������½��ļ��С���ť

extern TCHAR g_szAllPath[MAX_PATH];        //ȫ·��
extern TCHAR g_szBeginEditName[MAX_PATH];  //�������ǿ�ʼ���ļ���



//��1���������ļ����У�������
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
	//��չ��
	
	TCHAR *pOldExtension = strrchr(g_szBeginEditName, '.');

	if (NULL != pOldExtension)      //Դ·������չ��
	{	
		TCHAR *pNewExtension = strrchr(szFileName, '.');
		
		if (0 != lstrcmp(pOldExtension, pNewExtension))
		{
			int iRet = MessageBox(hwnd, TEXT("����ı��ļ���չ�������ܻᵼ���ļ������á�\nȷʵҪ������?"), TEXT("������"), MB_YESNO | MB_ICONWARNING);
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

	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //����ҳ��
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


//��2���ļ�����
BOOL FileOperation(HWND hwnd, int Operation, TCHAR *szOldPath, TCHAR *szNewPath)
{
	SHFILEOPSTRUCT FileOp;
	ZeroMemory(&FileOp, sizeof(SHFILEOPSTRUCT));

	if ((FILE_COPY == Operation) || (FILE_MOVE == Operation))     //�ƶ�������
	{
		FileOp.fFlags = FOF_MULTIDESTFILES; //��Ҫʱֱ�Ӵ���һ���ļ���,�����û�ȷ��
		FileOp.pFrom = szOldPath;
		FileOp.pTo = szNewPath;           //һ��Ҫ��NULL

		if (FILE_COPY == Operation)
		{
			FileOp.wFunc = FO_COPY;
		}
		else
		{
			FileOp.wFunc = FO_MOVE;
		}
	}
	else if (FILE_DELETE == Operation)      //ɾ��
	{
		FileOp.fFlags =  FOF_MULTIDESTFILES;
		FileOp.pFrom = szOldPath;
		FileOp.pTo = NULL;           //һ��Ҫ��NULL
		FileOp.wFunc = FO_DELETE;    //ɾ������
	}
	else   //������
	{
		FileOp.fFlags = FOF_NOCONFIRMATION;   //������ȷ�϶Ի���
		FileOp.pFrom = szOldPath;
		FileOp.pTo = szNewPath;
		FileOp.wFunc = FO_RENAME;
	}
	if (0 != SHFileOperation(&FileOp))
	{
		return FALSE;
	}
	if (TRUE == FileOp.fAnyOperationsAborted)      //����ʾ���ﵥ���ˡ�ȡ����
	{
		return FALSE;
	}
	return TRUE;
}

//��3�������ļ�������ListView��
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
		if (TRUE == IsReName)     //������
		{
			InsertFileInfo(hwnd, iItem, iIcon, &FileInfo, TRUE, TRUE);
		}
		else               //����������
		{
			InsertFileInfo(hwnd, 0, iIcon, &FileInfo, TRUE, FALSE);
		}
	}
	FindClose(hFile);
}


//��4��ɾ���ļ�
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
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	int *pszItem = (int*)malloc(iCount * sizeof(int)+1);
	if (NULL == pszItem)
	{
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
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
		pszItem[i] = iNextItem;       //����Item
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
		if (0 == access(pszTmp,0))      //�ļ�����,ɾ��ʧ��
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


//��5��ȫ��ѡ��
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


//��6������ѡ��
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


//��7���ļ�(��), ��������
void ShowFileAttribute(HWND hwnd)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	if (0 == iCount)   //�ڿհ״��һ�
	{
		if (0 == lstrcmp(g_szAllPath, TEXT("")))    //����ҳ��
		{
			wsprintf(szPath, "%s", TEXT("explorer"));
		}
		else      //�ļ�ҳ��
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
		
		if (0 == lstrcmp(g_szAllPath, TEXT("")))    //����ҳ��
		{
			TCHAR *pLastSymbol = strrchr(szFileName, '(');
			wsprintf(szPath, "%c:", pLastSymbol[1]);     //����·��
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


//��8�����Ƶ����ƶ����ļ���
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
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	TCHAR *pszOldBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszOldBuff)
	{
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
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
		
		//���ļ���,���ļ���ʽ��File1\0File2\0File3\0��
		iOldFileLen = lstrlen(szPath);
		
		memcpy(pszOldBuff + iOldTotalLen, szPath, sizeof(szPath) / sizeof(TCHAR));
		memcpy(pszOldBuff + (i+1)*(iOldTotalLen+iOldFileLen) + 1, TEXT("\0"), 1);

		iOldTotalLen++;

		iOldTotalLen += iOldFileLen;

		//���ļ���,���ļ���ʽ��File1\0File2\0File3\0��
		ZeroMemory(szTmp, sizeof(szTmp) / sizeof(TCHAR));
		wsprintf(szTmp, "%s\\%s", szDirectory, szFileName);
		iNewFileLen = lstrlen(szTmp);
		
		memcpy(pszNewBuff + iNewTotalLen, szTmp, sizeof(szTmp) / sizeof(TCHAR));
		memcpy(pszNewBuff + (i+1)*(iNewTotalLen+iNewFileLen) + 1, TEXT("\0"), 1);

		iNewTotalLen++;

		iNewTotalLen += iNewFileLen;
	}
	if (TRUE == IsCopyToDirectory)       //���Ƶ��ļ���
	{
		FileOperation(hwnd, FILE_COPY, pszOldBuff, pszNewBuff);
	}
	else                      //�ƶ����ļ���
	{
		FileOperation(hwnd, FILE_MOVE, pszOldBuff, pszNewBuff);
	}
	free(pszNewBuff);
	pszNewBuff = NULL;
	free(pszOldBuff);
	pszOldBuff = NULL;
}


//��9��ճ���ļ�
void PasteListFile(HWND hwnd)
{
	//ע��һ���µļ������ʽ��Ȼ����Խ��˸�ʽ������Ч�ļ������ʽ��
	UINT uDropEffect = RegisterClipboardFormat("Preferred DropEffect");
	DWORD *dwEffect = (DWORD*)GetClipboardData(uDropEffect);

	BOOL bIsCopy = FALSE;
	if (DROPEFFECT_MOVE & *dwEffect)
	{
		bIsCopy = FALSE;//���к��ճ��
	}
	if (DROPEFFECT_COPY & *dwEffect)
	{
		bIsCopy = TRUE;//���ƺ��ճ��
	}

	TCHAR szFileName[MAX_PATH];
	
	HDROP hDrop = (HDROP)GetClipboardData(CF_HDROP); 

	// ����ļ��� 
	/*DragQueryFile��һ���ɹ��ϷŲ������ȡ���Ϸ��ļ������Ƶ���Ϣ�ĺ�����

  INT DragQueryFile(
HDROP  hDrop,
UINT  iFile,
LPTSTR  lpszFile,
UINT    cch
);

��������
��hDrop����������"��������ק�ļ����ƽṹ�ľ��"�����������ק�ļ����Ƶ����ݽṹ�ľ����Ҳ�����ļ����������ľ��

2��iFile���ļ��������
���ã�����ָ����Ҫ��ѯ�ļ�����ţ�����Ͻ�����ļ�����������Ŵ��㿪ʼ�����iFile��ֵΪ0xFFFFFFFFʱ�����ص�����ҷ�������ϵ��ļ����������iFileֵ��0����ҷ�ļ�����֮��ʱ��DragQueryFile�������ļ����洢��������С��Ӧ���ļ����Ƶ��������С�

3��lpszFile
��������ʱ�����ڴ�����ק�ļ����ƵĻ�����ָ�롣�ļ�������һ���Կ���ֹ��\0����β���ַ���������˲�����NULL��DragQueryFile����������ק���ļ����ĳ��ȡ�����DragQueryFile�õ����ļ������Ǵ�����·�����ļ���

4��Cch
�洢��ק�ļ����ƻ������Ĵ�С����lpszFileָ����ָ���������ַ�����

  hDrop=hDrop;iFile=0xFFFFFFFF;lpszFile=NULL;cch=0;
  */
	int iCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);//������ҷ�������ϵ��ļ�����

	TCHAR *pszNewBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszNewBuff)
	{
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	TCHAR *pszOldBuff = (TCHAR*)malloc(iCount * MAX_PATH);
	if (NULL == pszOldBuff)
	{
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
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
		wsprintf(szNewPath, "%s%s", g_szAllPath, pLastSymbol); //pLastSymbol��һ���ַ�Ϊ"\",���Բ���Ҫ"\\"
		
		//���ļ���,���ļ���ʽ��File1\0File2\0File3\0��
		iOldFileLen = lstrlen(szFileName);
		
		memcpy(pszOldBuff + iOldTotalLen, szFileName, sizeof(szFileName) / sizeof(TCHAR));
		memcpy(pszOldBuff + (i+1)*(iOldTotalLen+iOldFileLen) + 1, TEXT("\0"), 1);

		iOldTotalLen++;

		iOldTotalLen += iOldFileLen;

		//���ļ���,���ļ���ʽ��File1\0File2\0File3\0��
		iNewFileLen = lstrlen(szNewPath);
		
		memcpy(pszNewBuff + iNewTotalLen, szNewPath, sizeof(szNewPath) / sizeof(TCHAR));
		memcpy(pszNewBuff + (i+1)*(iNewTotalLen+iNewFileLen) + 1, TEXT("\0"), 1);

		iNewTotalLen++;

		iNewTotalLen += iNewFileLen;
	}

	if (TRUE == bIsCopy)       //����
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
	else                      //����
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
		if (0 == access(pszTmp,0))      //�ļ�����,ɾ��ʧ��
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

//��10�������ļ�
void CopyOrCutListFile(HWND hwnd, BOOL bIsCopy)
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	int i = 0;

	TCHAR *pszBuff = (TCHAR*)malloc(iCount * MAX_PATH);

	if (NULL == pszBuff)
	{
		MessageBox(hwnd, TEXT("�ڴ�����ʧ��"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
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

		if (FALSE == bIsCopy)  //����
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
	if (FALSE == bIsCopy)  //����
	{
		CutOrCopyFiles(pszBuff, iTotalLen, FALSE);
	}
	else                  //����
	{
		CutOrCopyFiles(pszBuff, iTotalLen, TRUE);
	}
	free(pszBuff);
	pszBuff = NULL;
}


//��11���½��ļ�
void CreateNewFile(HWND hwnd, int iCreate)
{
	if (0 == lstrcmp(g_szAllPath, TEXT("")))    //����ҳ��
	{
		MessageBox(hwnd, TEXT("����ҳ�治���½��ļ�"), TEXT("Explorer"), MB_OK | MB_ICONERROR);
		return;
	}
	switch (iCreate)
	{
	case CREATE_DIRECTORY:         //�ļ���
		{
			TCHAR szPath[MAX_PATH];
			ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));
			
			wsprintf(szPath, "%s\\�½��ļ���", g_szAllPath);
			if (0 == CreateDirectory(szPath, NULL))
			{
				ShowErrorMsg(hwnd, TEXT("�½��ļ���ʱ����"));
				return;
			}
			InsertFileToListView(hwnd, szPath, 0, FALSE);
		}
		break;
	case CREATE_BITMAP:           //BMPͼ��
		{
			CreateNormalFile(hwnd, TEXT("�½� BMP ͼ��.bmp"), TEXT("�½�BMPͼ��ʱ����"));
		}
		break;
	case CREATE_WORD:            //Word �ĵ�
		{
			CreateNormalFile(hwnd, TEXT("�½� Microsoft Word �ĵ�.doc"), TEXT("�½�Wordʱ����"));
		}
		break;
	case CREATE_PPT:             //PowerPoint ��ʾ�ĸ�
		{
			CreateNormalFile(hwnd, TEXT("�½� Microsoft PowerPoint ��ʾ�ĸ�.ppt"), TEXT("�½�ѹ���ļ�ʱ����"));
		}
		break;
	case CREATE_TEXT:             //�ı��ĵ�
		{
			CreateNormalFile(hwnd, TEXT("�½� �ı��ĵ�.txt"), TEXT("�½��ı��ĵ�ʱ����"));
		}
		break;
		
	case CREATE_WAV:             //��������
		{
			CreateNormalFile(hwnd, TEXT("�½� ��������.wav"), TEXT("�½����������ļ�ʱ����"));
		}
		break;
	case CREATE_EXCEL:           //Microsoft Excel ������
		{
			CreateNormalFile(hwnd, TEXT("�½� Microsoft Excel ������.xls"), TEXT("�½�Excelʱ����"));
		}
		break;
	default:
		break;
	}
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEMCOUNT, 0, 0);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_EDITLABEL, iCount-1, 0);
}


//��12���½���ͨ�ļ�
void CreateNormalFile(HWND hwnd, TCHAR *szCreateName, TCHAR *szTitle)
{
	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath) / sizeof(TCHAR));

	wsprintf(szPath, "%s\\%s", g_szAllPath, szCreateName);
	HANDLE hFile = CreateFile(szPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,0);

	if (ERROR_ALREADY_EXISTS == GetLastError())    //ERROR_ALREADY_EXISTSʱ�ļ�����INVALID_HANDLE_VALUE
	{
		MessageBox(hwnd, TEXT("���ļ��Ѵ���ʱ���޷��������ļ�"), TEXT(szTitle), MB_OK | MB_ICONERROR);
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


//��13�����ļ�
void OpenListFile(HWND hwnd)
{
	DblclkFile(hwnd);
}


//��14�����л����ļ�
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
	if(TRUE == bIsCopy)     //����
	{
		*dwDropEffect = DROPEFFECT_COPY;
	}
	else    //����
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


//��15���õ�Ҫ���Ƶ����ƶ����ļ��е�Ŀ¼·��
BOOL GetCopyOrMoveDirectory(HWND hwnd, TCHAR *szDirectory, TCHAR *szFileName, int iFileCount, BOOL IsCopyToDirectory)
{
	BROWSEINFO sBrowseInfo;
	LPITEMIDLIST lpitem;
	ZeroMemory(&sBrowseInfo, sizeof(BROWSEINFO));
	sBrowseInfo.hwndOwner = hwnd;

	if (1 != iFileCount)  //���ƶ���ļ�
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));
		
		if (TRUE == IsCopyToDirectory)      //���Ƶ��ļ���
		{
			wsprintf(szMsg, "ѡ�����뽲�� %i ����Ŀ���Ƶ��ĵط���Ȼ�󵥻���ȷ������ť", iFileCount);
		}
		else
		{
			wsprintf(szMsg, "ѡ�����뽲�� %i ����Ŀ�ƶ����ĵط���Ȼ�󵥻���ȷ������ť", iFileCount);
		}
		sBrowseInfo.lpszTitle = TEXT(szMsg);
	}
	else
	{
		TCHAR szMsg[MAX_PATH];
		ZeroMemory(szMsg, sizeof(szMsg) / sizeof(TCHAR));
		
		if (TRUE == IsCopyToDirectory)      //���Ƶ��ļ���
		{
			wsprintf(szMsg, "ѡ�����뽲�� %s ����Ŀ���Ƶ��ĵط���Ȼ�󵥻���ȷ������ť", szFileName);
		}
		else
		{
			wsprintf(szMsg, "ѡ�����뽲�� %s ����Ŀ�ƶ����ĵط���Ȼ�󵥻���ȷ������ť", szFileName);
		}
		sBrowseInfo.lpszTitle = TEXT(szMsg);
	}
	
	sBrowseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NEWDIALOGSTYLE; //����������Ŀ¼
	
	lpitem = SHBrowseForFolder(&sBrowseInfo);
	
	if (NULL == lpitem)                                    //�ж��Ƿ��Ѿ�ѡ���ļ���
	{
		CoTaskMemFree(lpitem);
		return FALSE;
	}
	
	SHGetPathFromIDList(lpitem, szDirectory);	     //����ļ��е�·��
	CoTaskMemFree(lpitem);

	return TRUE;
}


//��16����ʾ������Ϣ
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


//��17��IE��ҳ
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

//FileOperation.cpp��18����ĳ���ļ�
void OpenMenuFile(HWND hwnd)
{
	//DblclkFile(hwnd);

	OPENFILENAME ofn = { 0 };
	TCHAR strFilename[MAX_PATH] = { 0 };//���ڽ����ļ���
	ofn.lStructSize = sizeof(OPENFILENAME);//�ṹ���С
	ofn.hwndOwner = NULL;//ӵ���Ŵ��ھ����ΪNULL��ʾ�Ի����Ƿ�ģ̬�ģ�ʵ��Ӧ����һ�㶼Ҫ��������
	ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0C/C++ Flie\0*.cpp;*.c;*.h\0\0");//���ù���
	ofn.nFilterIndex = 1;//����������
	ofn.lpstrFile = strFilename;//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
	ofn.nMaxFile = sizeof(strFilename);//����������
	ofn.lpstrInitialDir = NULL;//��ʼĿ¼ΪĬ��
	ofn.lpstrTitle = TEXT("��ѡ��һ���ļ�");//ʹ��ϵͳĬ�ϱ������ռ���
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
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
