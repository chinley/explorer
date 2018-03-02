#ifndef _ABOUT_H
#define _ABOUT_H

#include <windows.h>

BOOL WINAPI About_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL About_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void About_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void About_OnClose(HWND hwnd);

#endif