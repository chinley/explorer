#ifndef _SHORTCUTTITLE_H
#define _SHORTCUTTITLE_H

#include <windows.h>

BOOL WINAPI ShortCutTitle_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL ShortCutTitle_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void ShortCutTitle_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void ShortCutTitle_OnClose(HWND hwnd);
void InitShortCutTitleDlg(HWND hwnd, LPARAM lParam);
void LastStepShortCut(HWND hwnd);
void CompleteShortCut(HWND hwnd);

#endif