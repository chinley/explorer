#ifndef _CONTROLOPERATION_H
#define _CONTROLOPERATION_H

#include <windows.h>
#include "ExplorerFun.h"

#define STYLE_DISK  0
#define STYLE_FILE  1

void InsertFileInfo(HWND hwnd, int iItem, int iImage, pFILEINFO pFileInfo, BOOL IsFile, BOOL IsReName);
HTREEITEM AddNodeInTree(HWND hwndTreeView, HTREEITEM hParent, int iImage, TCHAR *szItemText, DWORD dwTextLength);

#endif