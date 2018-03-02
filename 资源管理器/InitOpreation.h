#ifndef _INITOPERATION_H
#define _INITOPERATION_H

#include <windows.h>

void InitExplorerWindow(HWND hwnd);
void InitControlSize(HWND hwnd, LPARAM lParam);
void InitImageLIst(HWND hwnd);
void ExitWindow();
void InitListViewColumn(HWND hwnd, BOOL IsFile);
void InitMenuPopUp(HWND hwnd, WPARAM wParam, LPARAM lParam);
void InitContextMenu(HWND hwnd, WPARAM wParam, LPARAM lParam);
void NoIndexSelectedMenu(HWND hwnd, HMENU hPopMenu, LPARAM lParam);
void IndexSelectedMenu(HWND hwnd, HMENU hPopMenu, LPARAM lParam);

#endif