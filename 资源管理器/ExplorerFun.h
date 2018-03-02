#ifndef _MYEXPLORERFUN_H
#define _MYEXPLORERFUN_H

#include <windows.h>

#define SORT_NAME       1
#define SORT_SIZE       2
#define SORT_STYLE      3
#define SORT_WRITETIME  4
#define SORT_GROUP      5

//排序时所传参数的结构体

typedef struct _SORTINFO
{
	int lParamSort;        //排序方式
	HWND hwndListView;     //ListView句柄

} SORTINFO, *pSORTINFO;


typedef struct _FILEINFO
{
	TCHAR szFileName[MAX_PATH];
	TCHAR szFileAttributes[20];
	TCHAR szFileSize[10];
	
	union
	{
		TCHAR szLastWriteTime[20]; //修改时间，用于文件（夹）
		TCHAR szTotal[10];         //总大小，用于磁盘

	} Optional;

} FILEINFO, *pFILEINFO;

void AddFileInfoToListView(HWND hwnd);

void GetDiskTypeAndName(UINT uiDiskType, TCHAR *szFilePath, pFILEINFO pFileInfo);

void OnNotify(HWND hwnd, WPARAM wParam, LPARAM lParam);

BOOL GetFileInfo(PWIN32_FIND_DATA pstFileInfo, pFILEINFO pFileInfo);

void SearchFile(HWND hwnd, TCHAR *szPath);

void BackPath(HWND hwnd);

int GetImageLIstIndex(HWND hwnd, pFILEINFO pFileInfo, TCHAR *szPath, BOOL IsFile);

void DeleteAllColumn(HWND hwnd);

void SetLookMode(HWND hwndListView, DWORD dwLookMode);

void GotoFilePath(HWND hwnd);

void RefreshWindow(HWND hwnd);

void DblclkFile(HWND hwnd);

void SetSelectedtInfo(HWND hwnd);

void SortListFile(HWND hwnd, int lParamSort);

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

void BeginRename(HWND hwnd, LPNMHDR lpnmhdr);

void ShellExecuteError(HWND hwnd, TCHAR *szPath, int iError);

void ShowListViewDispInfo(LPARAM lParam);

void IsRightful(HWND hwnd);

void CreateListFileShortCut(HWND hwnd);

BOOL CreateShortCut(HWND hwnd, TCHAR *szPath, TCHAR *szLinkName);

void FowardPath(HWND hwnd);

void OpenHelpCenter(HWND hwnd);


#endif