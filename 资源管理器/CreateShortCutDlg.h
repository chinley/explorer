#ifndef _CREATESHORTCUT_H
#define _CREATESHORTCUT_H

#include <windows.h>

BOOL WINAPI CreateShortCut_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CreateShortCut_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void CreateShortCut_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void CreateShortCut_OnClose(HWND hwnd);
void NextStepShortCut(HWND hwnd);
void BrowseDirectory(HWND hwnd);
void InitCreateShortCutTitleDlg(HWND hwnd, LPARAM lParam);

#endif
