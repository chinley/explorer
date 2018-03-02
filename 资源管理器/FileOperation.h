#ifndef _FILEOPERATION_H
#define _FILEOPERATION_H

#include <windows.h>

#define CREATE_DIRECTORY 0
#define CREATE_BITMAP    1
#define CREATE_WORD      2
#define CREATE_PPT       3
#define CREATE_TEXT      4
#define CREATE_WAV       5
#define CREATE_EXCEL     6

#define FILE_COPY        1
#define FILE_MOVE        2
#define FILE_DELETE      3
#define FILE_RENAME      4

void ReFileName(HWND hwnd, LPNMHDR lpnmhdr);
BOOL FileOperation(HWND hwnd, int Operation, TCHAR *szOldPath, TCHAR *szNewPath);
void InsertFileToListView(HWND hwnd, TCHAR *szPath, int iItem, BOOL IsReName);
void SetSelectedtInfo(HWND hwnd);
void ShowErrorMsg(HWND hwnd, TCHAR *szTitle);
void CreateNewFile(HWND hwnd, int iCreate);
void CreateNormalFile(HWND hwnd, TCHAR *szCreateName, TCHAR *szTitle);
void OpenListFile(HWND hwnd);
void PasteListFile(HWND hwnd);
void CutOrCopyFiles(TCHAR *lpBuffer, UINT uBufLen, BOOL bIsCopy);
void CopyOrCutListFile(HWND hwnd, BOOL bIsCopy);
BOOL GetCopyOrMoveDirectory(HWND hwnd, TCHAR *szDirectory, TCHAR *szFileName, int iFileCount, BOOL IsCopyToDirectory);
void CopyOrMoveToDirectory(HWND hwnd, BOOL IsCopyToDirectory);
void SelectedAllItem(HWND hwnd);
void InverSelectedItem(HWND hwnd);
void ShowFileAttribute(HWND hwnd);
void GetIEHomePage(HWND hwnd);
void DeleteListFile(HWND hwnd);
void OpenMenuFile(HWND hwnd);

#endif