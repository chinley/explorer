#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ExplorerFun.h"
#include "InitOpreation.h"
#include <richedit.h>


//全局函数必须以小写前缀"g"开头。
//全局变量必须以"g_"开头
extern HIMAGELIST g_ListViewImageListLarge;
extern HIMAGELIST g_ListViewImageListSmall;
extern HIMAGELIST g_TreeViewImageList;
extern TCHAR g_szAllPath[MAX_PATH];        //全路径

HMODULE g_hModule;


//【1】初始化窗口
void InitExplorerWindow(HWND hwnd)
{
	HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);//用 GetWindowLong 这个函数能够获得指定窗口的信息
	
	//创建treeview
	if (NULL == CreateWindow(
		WC_TREEVIEW,//参数1：lpClassName：指定了窗口的类名。这个类名可以是任何用函数RegisterClass注册的类名，或是任何预定义的控制类名 
		"", //参数2：lpWindowName：窗口标题
		WS_BORDER | WS_VISIBLE | WS_TABSTOP
		| WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS 
		| TVS_LINESATROOT | WS_EX_CLIENTEDGE,//参数3：dwStyle：指定创建窗口的风格
		0,//参数4：X：指定窗口的初始水平位置
		0,//参数5：Y:指定窗口的初始垂直位置
		0,//参数6：nWidth：以设备单元指明窗口的宽度
		0,//参数7：nHeight：以设备单元指明窗口的高度
		hwnd,//参数8：hWndParent：指向被创建窗口的父窗口或所有者窗口的句柄
		(HMENU)IDC_TREEFILE,//参数9：hMenu：菜单句柄，或依据窗口风格指明一个子窗口标识
		hInst,//参数10：hInstance：与窗口相关联的模块实例的句柄
		NULL))//参数11：lpParam：指向一个值的指针，该值传递给窗口WM_CREATE消息
	{
		MessageBox(hwnd, TEXT("创建树状控件失败"), TEXT("提示"), MB_OK);
		return;
	}

	//创建ListView
	if (NULL == CreateWindow(
		WC_LISTVIEW,
		"",
		WS_VSCROLL | WS_HSCROLL | WS_BORDER | WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
		0,
		0,
		0,
		0,
		hwnd,
		(HMENU)IDC_LISTVIEWFILE,
		hInst,
		NULL))
	{
		MessageBox(hwnd, TEXT("创建ListView控件失败"), TEXT("提示"), MB_OK);
		return;
	}

	//设置列表视图控件的扩展样式
	ListView_SetExtendedListViewStyle(
		GetDlgItem(hwnd, IDC_LISTVIEWFILE),//将会收到样式更改的列表视图控件的句柄  
		LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);//指定扩展列表视图控件的风格 

	InitListViewColumn(hwnd, FALSE);
	SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_REPORT);

	//创建工具栏
    HWND hTool =CreateWindowEx(
		WS_EX_TOOLWINDOW, 
		TOOLBARCLASSNAME, //工具栏窗口类
		NULL,
		TBSTYLE_LIST | WS_CHILD | WS_VISIBLE|TBSTYLE_TOOLTIPS,
		0,
		0,
		0,
		0,
		hwnd,
		(HMENU)IDC_TOOLFILE,
		hInst,
		NULL);
	if (NULL == hTool)
	{
		MessageBox(hwnd, TEXT("创建工具栏失败"), TEXT("提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	SendMessage(//该函数将指定的消息发送到一个或多个窗口。是应用程序和应用程序之间进行消息传递的主要手段之一
		hTool,//接受消息的窗口句柄
		TB_BUTTONSTRUCTSIZE,//指定被发送的消息。这里的消息指定TBBUTTON结构的大小
		(WPARAM)sizeof(TBBUTTON),//执行附加的消息特定信息
		0);//指定附加的消息特定信息

	TBBUTTON	tbb[6];//工具栏中的按钮
	

	/*
	TBADDBITMAP结构包含了工具栏添加的位图的信息
	*/
	TBADDBITMAP tbab;//一个包含按钮图像的位图
	


	/*void ZeroMemory(PVOID Destination,SIZE_T Length);
	*参数：
	*Destination：指向一块准备用0来填充的内存区域的开始地址
	*Length：准备用0来填充的内存区域的大小，按字节来计算
	*/
	ZeroMemory(&tbab, sizeof(TBADDBITMAP));
	

	tbab.hInst = HINST_COMMCTRL;
	tbab.nID   = IDB_STD_LARGE_COLOR;
	SendMessage(hTool, TB_ADDBITMAP, 6,(LPARAM)&tbab);

	ZeroMemory(tbb, sizeof(tbb));

	tbb[0].iBitmap = 3;//HIST_BACK;        //后退
	tbb[0].fsState = TBSTATE_ENABLED ;//按钮状态标志。该成员可以是工具栏按钮状态中列出的值的组合
	tbb[0].fsStyle = TBSTYLE_BUTTON;//按钮样式。该成员可以是工具栏按钮样式中列出的值的集合
	tbb[0].idCommand = ID_MENUBACK;//与按钮相关联的命令标识符，当选择按钮时，此标识符用于WM_COMMAND消息。
	tbb[0].iString = (int)TEXT("后退");//基于零的按钮字符串索引

	tbb[1].iBitmap = 4;//HIST_FORWARD;        //前进
	tbb[1].fsState = TBSTATE_ENABLED ;
	tbb[1].fsStyle = TBSTYLE_BUTTON;
	tbb[1].idCommand = ID_MENUFOWARD;
	tbb[1].iString = (int)TEXT("前进");


	tbb[2].iBitmap=STD_CUT;//剪切
	tbb[2].fsState=TBSTATE_ENABLED;//按钮状态
	tbb[2].fsStyle = TBSTYLE_BUTTON;//按钮样式
	tbb[2].idCommand =ID_MENUCUT;//按钮命令ID
	tbb[2].iString = (int)TEXT("剪切");

	tbb[3].iBitmap=STD_COPY;//复制
	tbb[3].fsState=TBSTATE_ENABLED;//按钮状态
	tbb[3].fsStyle = TBSTYLE_BUTTON;//按钮样式
	tbb[3].idCommand =ID_MENUCOPY;//按钮命令ID
	tbb[3].iString = (int)TEXT("复制");



	tbb[4].iBitmap=STD_DELETE;//删除
	tbb[4].fsState=TBSTATE_ENABLED;//按钮状态
	tbb[4].fsStyle = TBSTYLE_BUTTON;//按钮样式
	tbb[4].idCommand =ID_MENUDELETE;//按钮命令ID
	tbb[4].iString = (int)TEXT("删除");

	tbb[5].iBitmap=STD_REPLACE;
	tbb[5].fsState=TBSTATE_ENABLED;
	tbb[5].fsStyle=TBSTYLE_BUTTON;
	tbb[5].idCommand=ID_MENUREFRESH;
	tbb[5].iString=(int)TEXT("刷新");


	SendMessage(hTool,
		TB_ADDBUTTONS,
		sizeof(tbb) / sizeof(TBBUTTON),
		(LPARAM)&tbb);



	  //创建工具栏(地址栏)

   HWND hToolAddress =CreateWindowEx(
	   WS_EX_TOOLWINDOW,
	   TOOLBARCLASSNAME,
	   NULL,
	   TBSTYLE_LIST | WS_CHILD | WS_VISIBLE,
	   0,
	   0,
	   0,
	   0,
	   hwnd,
	   (HMENU)IDC_TOOLADDRESS,
	   hInst,
	   NULL);
	if (NULL == hToolAddress)
	{
		MessageBox(hwnd, TEXT("创建工具栏失败"), TEXT("提示"), MB_OK | MB_ICONWARNING);
		return;
	}
	SendMessage(
		hToolAddress,
		TB_BUTTONSTRUCTSIZE,
		(WPARAM)sizeof(TBBUTTON),
		0);

	TBBUTTON	tbb2[1];

	ZeroMemory(&tbab, sizeof(TBADDBITMAP));

	tbab.hInst = HINST_COMMCTRL;
	tbab.nID   = IDB_HIST_LARGE_COLOR;
	SendMessage(hToolAddress, TB_ADDBITMAP, 2,(LPARAM)&tbab);
	
	ZeroMemory(tbb, sizeof(tbb));

	tbb2[0].fsState = TBSTATE_INDETERMINATE ;
	tbb2[0].fsStyle = TBSTYLE_BUTTON;
	tbb2[0].idCommand = NULL;
	tbb2[0].iString = (int)TEXT("地址(&D)");
		
	SendMessage(hToolAddress,
		TB_ADDBUTTONS,
		sizeof(tbb2) / sizeof(TBBUTTON),
		(LPARAM)&tbb2);

 	//跳转
	g_hModule = LoadLibrary("riched20.dll");// LoadLibrary:将指定的模块加载到调用进程的地址空间中。

	//要创建一个丰富的编辑控件，调用CreateWindowEx函数，指定丰富的编辑窗口类。
	HWND hWndEdit = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		"richedit20a",
		NULL,
		WS_CHILD | WS_BORDER
		| WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN, 
		0,
		0,
		0,
		0,
		hwnd,
		(HMENU)IDM_EDITPATH,
		hInst,
		0);

	long lMask = SendMessage(hWndEdit, EM_GETEVENTMASK, 0, 0);
	lMask |= ENM_KEYEVENTS;
	SendMessage(hWndEdit, EM_SETEVENTMASK, 0, (LPARAM)lMask);

	HWND hwndGoto = CreateWindowEx(
		0,
		TEXT("button"),
		TEXT("跳转"),
		WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
		0,
		0,
		0,
		0,
		hwnd,
		(HMENU)ID_FILE_GOTO,
		hInst,
		NULL);

	SetParent(hWndEdit,hToolAddress);
	SetParent(hwndGoto,hToolAddress);



	//创建状态栏
	HWND hStatus;
	int statwidths[] = {200, 300, -1};

	hStatus = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		STATUSCLASSNAME,//窗口类
		NULL,
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0,
		0,
		0,
		0,
		hwnd,
		(HMENU)IDC_STATUSFILE,
		hInst,
		NULL);

	SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);

	SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)TEXT(""));
	SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)TEXT(""));
	SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM)TEXT("我的电脑"));

	InitImageLIst(hwnd);
	AddFileInfoToListView(hwnd);      //添加磁盘信息到LIstView中

}

//【2】设置控件的位置
void InitControlSize(HWND hwnd, LPARAM lParam)
{
	RECT rcTool;
	RECT rcStatus;
	RECT rcToolAddress;
	RECT rcClient;

	HWND hTool = GetDlgItem(hwnd,IDC_TOOLFILE);
	SendMessage(hTool,TB_AUTOSIZE,0,0);
	GetWindowRect(hTool,&rcTool);
	int iToolHeight = rcTool.bottom - rcTool.top;

	HWND ToolAddress = GetDlgItem(hwnd, IDC_TOOLADDRESS);
	SendMessage(ToolAddress, TB_AUTOSIZE, 0, 0);
	GetWindowRect(hTool, &rcToolAddress);
	int iToolAddrHeight = rcToolAddress.bottom - rcToolAddress.top;

	HWND hStatus = GetDlgItem(hwnd,IDC_STATUSFILE);
	SendMessage(hStatus, WM_SIZE, 0, 0);
	GetWindowRect(hStatus,&rcStatus);
	int iStatusHeight = rcStatus.bottom - rcStatus.top;

	GetClientRect(hwnd,&rcClient);

	int iHeight = rcClient.bottom - iToolHeight - iToolAddrHeight - iStatusHeight;

	MoveWindow(hTool, 0, 0, rcTool.right-rcTool.left, iToolHeight,TRUE);
	MoveWindow(ToolAddress, 0, iToolHeight, rcToolAddress.right-rcToolAddress.left, iToolAddrHeight,TRUE);

	int iClientWidth = (int)(rcClient.right - rcClient.left);
	int statwidths[] = {(int)(iClientWidth*0.7), (int)(iClientWidth*0.85), -1};
	SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);

	//列表视图
	MoveWindow(GetDlgItem(hwnd, IDC_LISTVIEWFILE), 190, iToolHeight+iToolAddrHeight, LOWORD(lParam) - 190, iHeight,TRUE);
	//树状控图
	MoveWindow(GetDlgItem(hwnd, IDC_TREEFILE), 0, iToolHeight+iToolAddrHeight, 190, iHeight, TRUE);

	RECT rcAddress;
	SendMessage(GetDlgItem(hwnd, IDC_TOOLADDRESS), TB_GETITEMRECT, (WPARAM)0, (LPARAM)&rcAddress);

	MoveWindow(GetDlgItem(GetDlgItem(hwnd, IDC_TOOLADDRESS), IDM_EDITPATH), rcAddress.right, rcAddress.top,
		rcClient.right-rcAddress.right-rcAddress.right, rcAddress.bottom - rcAddress.top, TRUE);

	//跳转
	MoveWindow(GetDlgItem(GetDlgItem(hwnd, IDC_TOOLADDRESS), ID_FILE_GOTO), rcClient.right - rcAddress.right,
		rcAddress.top, rcAddress.right-rcAddress.left, rcAddress.bottom - rcAddress.top, TRUE);

}


//【3】初始化Image_LIst
void InitImageLIst(HWND hwnd)
{
	g_ListViewImageListLarge = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 100, 0);
	g_ListViewImageListSmall = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 100, 0);

	g_TreeViewImageList = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 100, 0);

	TreeView_SetImageList(GetDlgItem(hwnd, IDC_TREEFILE), g_TreeViewImageList, TVSIL_NORMAL);


	ListView_SetImageList(GetDlgItem(hwnd, IDC_LISTVIEWFILE), g_ListViewImageListLarge, LVSIL_NORMAL);
	ListView_SetImageList(GetDlgItem(hwnd, IDC_LISTVIEWFILE), g_ListViewImageListSmall, LVSIL_SMALL);

	DWORD dwStyles = GetWindowLong(GetDlgItem(hwnd, IDC_TREEFILE), GWL_STYLE);//获取树控制原风格
    dwStyles |= TVS_EDITLABELS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetWindowLong(GetDlgItem(hwnd, IDC_TREEFILE), GWL_STYLE, dwStyles);//设置风格
}


//【4】初始化ListView控件列
void InitListViewColumn(HWND hwnd, BOOL IsFile)
{
	HWND hwndListViewFile = GetDlgItem(hwnd, IDC_LISTVIEWFILE);//根据继承关系的函数功能，返回窗口中指定参数ID的子元素的句柄
	DeleteAllColumn(hwnd);

	SendMessage(hwndListViewFile, 
		LVM_SETEXTENDEDLISTVIEWSTYLE, 
		0,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES );//设置列表的风格
	LVCOLUMN columnFile;
	columnFile.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;   //指定LVCOLUMN结构中的pszText，fmt，cx域有效
	
	/*fmt
	列中的列标题和子项文本的对齐。最左列的对齐方式始终为LVCFMT_LEFT; 它不能改变。
	*/
	columnFile.fmt = LVCFMT_CENTER;
	
		
	//列表视图属性列第一项》名称
	columnFile.pszText = TEXT("名称");
	columnFile.cx = 120;//这一列的宽度
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 0, (LPARAM)&columnFile);
	
	//列表视图属性列第二项》类型/大小
	
	if (FALSE == IsFile)      //磁盘
	{
		columnFile.pszText = TEXT("类型");
	}
	else
	{
		columnFile.pszText = TEXT("大小");
	}
	columnFile.cx = 120; 
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 1, (LPARAM)&columnFile);

	//列表视图属性列第三项》总大小/类型

	if (FALSE == IsFile)      //磁盘
	{
		columnFile.pszText = TEXT("总大小");
	}
	else
	{
		columnFile.pszText = TEXT("类型");
	}
	columnFile.cx = 120; 
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 2, (LPARAM)&columnFile);

	//列表视图属性列第四项》可用空间/修改日期
	
	if (FALSE == IsFile)      //磁盘
	{
		columnFile.pszText = TEXT("可用空间");
	}
	else
	{
		columnFile.pszText = TEXT("修改日期");
	}
	columnFile.cx = 120; 
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 3, (LPARAM)&columnFile);
}









//【5】菜单栏
void InitMenuPopUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//GetDlgItem，是根据继承关系的函数功能，返回窗口中指定参数ID的子元素的句柄，可以通过返回的句柄对窗口内的子元素进行操作。
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	//用iCount值来判断是使菜单栏中的功能有效还是无效（iCount>0 ? MF_ENABLED : MF_GRAYED）

	switch (lParam)
	{
	case 0:     //文件菜单
		{
			/*EnableMenuItem的意思是允许、禁止或变灰指定的菜单条目。
			函数原型：BOOL EnableMenuItem(
　　                  HMENU hMenu, //菜单句柄
　　				  UINT uIDEnableItem, //欲允许或禁止的一个菜单条目的标识符。
　　				  UINT wEnable //
　　				  );
            参数：
			1）菜单句柄
			2）菜单条目的标识符
			3）对于这个函数，只能指定下述常数：MF_BYCOMMAND，MF_BYPOSITION，MF_ENABLED，MF_DISABLED以及MF_GRAYED
			  MF_BYCOMMAND 指定参数给出已存在的菜单项的命令ID号。此为缺省值。
              MF_BYPOSITION 指定参数给出已存在菜单项的位置。第一项所在的位置是0。
              MF_DISABLED 使菜单项无效，以便它不能被选择，但不变灰。
			  MF_ENABLED 使菜单项有效，以便它能够被选择，并可从变灰的状态中恢复出来。
              MF_GRAYED 使菜单项无效，以便它不能被选择并同时变灰。


			*/
			EnableMenuItem((HMENU) wParam, ID_MENUCREATESHORTCUT, iCount>0 ? MF_ENABLED : MF_GRAYED);//创建快捷方式
			EnableMenuItem((HMENU)wParam, ID_MENUOPEN, iCount>0 ? MF_ENABLED : MF_GRAYED);//打开
			EnableMenuItem((HMENU)wParam, ID_MENURENAME, iCount>0 ? MF_ENABLED : MF_GRAYED);//重命名

			/*
			lstrcmp 区分大小写; lstrcmpi 不区分大小写
			  lstrcmp(string1,string2)
			1)string1>string2:返回 1
			2)string1==string2:返回 0
			3)string1<string2:返回 -1
			*/
			if (0 == lstrcmp(g_szAllPath, TEXT("")))      //磁盘
			{
				EnableMenuItem((HMENU)wParam, ID_MENUDELETE, MF_GRAYED);
			}
			else
			{
				EnableMenuItem((HMENU)wParam, ID_MENUDELETE, iCount>0 ? MF_ENABLED : MF_GRAYED);
			}
		}
		break;
	case 1:     //编辑菜单
		{
			EnableMenuItem((HMENU)wParam, ID_MENUCOPYTODIRECTORY, iCount>0 ? MF_ENABLED : MF_GRAYED);//复制到文件夹
			EnableMenuItem((HMENU)wParam, ID_MENUMOVETODIRECTORY, iCount>0 ? MF_ENABLED : MF_GRAYED);//移动到文件夹
			
			if (0 == lstrcmp(g_szAllPath, TEXT("")))      //磁盘
			{
				EnableMenuItem((HMENU)wParam, ID_MENUCUT, MF_GRAYED);//剪切
				EnableMenuItem((HMENU)wParam, ID_MENUCOPY, MF_GRAYED);//复制
				EnableMenuItem((HMENU)wParam, ID_MENUPASTE, MF_GRAYED);//粘贴
			}
			else
			{
				/*
				（1）IsClipboardFormatAvailable(CF_TEXT); 判断剪贴板上的数据,
				如果当前剪切板包含的是以NULL结尾的ASCII字符的文本格式,则该函数返回值为true,否则为false.

                （2）Bool OpenClipboard(HWND hWndNewOwner);
　　            指定关联到打开的剪切板的窗口句柄，传入NULL表示关联到当前任务。每次只允许一个进程打开并访问。
               每打开一次就要关闭，否则其他进程无法访问剪切板。

				*/
				if(IsClipboardFormatAvailable(CF_HDROP) && OpenClipboard(NULL))
				{
					EnableMenuItem((HMENU)wParam, ID_MENUPASTE, MF_ENABLED);//"粘贴"有效
				}
				else
				{
					EnableMenuItem((HMENU)wParam, ID_MENUPASTE, MF_GRAYED);//"粘贴"无效（变灰）
				}
				EnableMenuItem((HMENU)wParam, ID_MENUCUT, iCount>0 ? MF_ENABLED : MF_GRAYED);
				EnableMenuItem((HMENU)wParam, ID_MENUCOPY, iCount>0 ? MF_ENABLED : MF_GRAYED);
			}
		}
		break;
	default:
		break;
	}
}

//【6】创建上下文菜单
void InitContextMenu(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (GetDlgItem(hwnd, IDC_LISTVIEWFILE) == (HWND)wParam)
	{
		// LVM_GETSELECTEDCOUNT：确定列表视图控件中所选项目的数量
		int iSelectedCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
		HMENU hPopMenu = CreatePopupMenu();
		if (0 == iSelectedCount)       //没有选中项时右击弹出的菜单
		{
			NoIndexSelectedMenu(hwnd, hPopMenu, lParam);
		}
		else        //选中文件
		{
			IndexSelectedMenu(hwnd, hPopMenu, lParam);
		}
	}
}

//【7】没有选中任何项时右击弹出的菜单
void NoIndexSelectedMenu(HWND hwnd, HMENU hPopMenu, LPARAM lParam)
{
	//查看
	HMENU hPopSubLook = CreatePopupMenu();

	/*AppendMenu：一种可以在指定的菜单条、下拉式菜单、子菜单或快捷菜单的末尾追加一个新菜单项的函数。
	【函数原型】
	BOOL AppendMenu（
			hMenu hMenu，
			UINT uFlags，
			UINT uIDNewltem，
	        LPCTSTR lpNewltem）;
	【实例】
	1）uFlags=MF_STRING：指定菜单项是一个正文字符串；参数lpNewltem（参数4）指向该字符串。
	2）uFlags=MF_POPUP：指定菜单打开一个下拉式菜单或子菜单。参数uIDNewltem（参数3）下拉式菜单
	或子菜单的句柄。此标志用来给菜单条、打开一个下拉式菜单或于菜单的菜单项、子菜单或快捷菜单加一个名字。
	3）uFlags=MF_SEPARATOR：画一条水平区分线。此标志只被下拉式菜单、子菜单或快捷菜单使用。
	此区分线不能被变灰、无效或加亮。参数IpNewltem（参数3）和uIDNewltem（参数4）无用。
	*/
	AppendMenu(hPopSubLook, MF_STRING, ID_MENUICON, TEXT("图标(&N)"));//查看》图标
	AppendMenu(hPopSubLook, MF_STRING, ID_MENULIST, TEXT("列表(&L)"));//查看》列表
	AppendMenu(hPopSubLook, MF_STRING, ID_MENURESOURCE, TEXT("详细信息(&D)"));//查看》详细信息
	AppendMenu(hPopMenu, MF_POPUP | MF_STRING, (UINT)hPopSubLook, TEXT("查看(&V)"));//设置菜单项 "查看"
	
	AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //"查看"和"排列图标"之间 插入分隔符
	
	//排列图标
	HMENU hPopSubSortIcon = CreatePopupMenu();
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTNAME, TEXT("名称(&N)"));//排列图标》名称
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTSIZE, TEXT("大小(&S)"));//排列图标》大小
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTSTYPE, TEXT("类型(&T)"));//排列图标》类型
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTWRITETIME, TEXT("修改时间(&M)"));//排列图标》修改时间
	
	AppendMenu(hPopMenu, MF_POPUP | MF_STRING, (UINT)hPopSubSortIcon, TEXT("排列图标(&I)"));//设置菜单项 "排列图标"
	
	AppendMenu(hPopMenu, MF_STRING, ID_MENUREFRESH, TEXT("刷新(&E)"));//设置菜单项  "刷新"
		
	AppendMenu(hPopMenu, MF_STRING, ID_MENUPASTE, TEXT("粘贴(&P)"));//设置菜单项  "粘贴"	

	if (0 == lstrcmp(g_szAllPath, TEXT("")))      //磁盘
	{
		EnableMenuItem((HMENU)hPopMenu, ID_MENUPASTE, MF_GRAYED);//当页面显示的是磁盘信息，则右键的菜单项"粘贴"无效
	}
	else
	{
		if(IsClipboardFormatAvailable(CF_HDROP) && OpenClipboard(NULL))
		{
			EnableMenuItem((HMENU)hPopMenu, ID_MENUPASTE, MF_ENABLED);
		}
		else
		{
			EnableMenuItem((HMENU)hPopMenu, ID_MENUPASTE, MF_GRAYED);
		}
	}
		
	AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //"粘贴"和"新建（属性）"之间插入分隔符
	
	//新建
	HMENU hPopSubCreateNew = CreatePopupMenu();
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWDIRECTORY, TEXT("文件夹(&F)"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWSHORTCUT, TEXT("快捷方式(&S)"));
	
	AppendMenu(hPopSubCreateNew, MF_SEPARATOR, NULL,NULL);       //分隔符
	
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWBMP, TEXT("BMP图像"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEWORD, TEXT("Microsoft Word 文档"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEPPT, TEXT("Microsoft PowerPoint 演示文稿"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWTEXT, TEXT("文本文档"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEWAV, TEXT("波形声音"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEEXCEL, TEXT("Microsoft Excel 工作表"));
	
	if (0 != lstrcmp(g_szAllPath, TEXT("")))      //不是磁盘
	{
		AppendMenu(hPopMenu, MF_POPUP | MF_STRING, (UINT)hPopSubCreateNew, TEXT("新建(&W)"));
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //分隔符
	}
	
	AppendMenu(hPopMenu, MF_STRING, ID_MENUATTRIBUTE, TEXT("属性(&R)"));
	
	int nTopIndex;
	POINT point= {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	nTopIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_GETTOPINDEX, 0, 0);
	ScreenToClient(GetDlgItem(hwnd, IDC_LISTVIEWFILE), &point);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_SETCURSEL,nTopIndex + point.y / 16, 0); // 用右键模拟左键 
	
	TrackPopupMenuEx(hPopMenu,TPM_LEFTALIGN | TPM_TOPALIGN,
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hwnd, NULL);
	DestroyMenu(hPopSubLook);
	DestroyMenu(hPopSubSortIcon);
	DestroyMenu(hPopSubCreateNew);
	DestroyMenu(hPopMenu);
}

//【8】有选中某项时右击弹出的菜单
void IndexSelectedMenu(HWND hwnd, HMENU hPopMenu, LPARAM lParam)
{
	LVITEM lv;
	ZeroMemory(&lv, sizeof(LVITEM));
	
	int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);
	lv.mask = LVIF_PARAM;
	lv.iSubItem = 3;
	lv.iItem = iIndex;
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
	
	if (0 == lstrcmp(g_szAllPath, TEXT("")))      //磁盘
	{
		AppendMenu(hPopMenu, MF_STRING, ID_MENUOPEN, TEXT("打开(&O)"));//打开
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //分隔符
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCREATESHORTCUT, TEXT("创建快捷方式(&S)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENURENAME, TEXT("重命名(&M)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //分隔符
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUATTRIBUTE, TEXT("属性(&R)"));
	}
	else                  //文件(夹)
	{
		AppendMenu(hPopMenu, MF_STRING, ID_MENUOPEN, TEXT("打开(&O)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //分隔符
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCUT, TEXT("剪切(&T)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCOPY, TEXT("复制(&C)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //分隔符
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCREATESHORTCUT, TEXT("创建快捷方式(&S)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENUDELETE, TEXT("删除(&D)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENURENAME, TEXT("重命名(&M)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //分隔符
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUATTRIBUTE, TEXT("属性(&R)"));
	}
	int nTopIndex;
	POINT point={GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	nTopIndex=SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_GETTOPINDEX,0,0);
	ScreenToClient(GetDlgItem(hwnd, IDC_LISTVIEWFILE),&point);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_SETCURSEL, nTopIndex+point.y / 16, 0); // 用右键模拟左键 
	
	TrackPopupMenuEx(hPopMenu, TPM_LEFTALIGN | TPM_TOPALIGN,
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hwnd, NULL);
	DestroyMenu(hPopMenu);
}

//【9】关闭窗口
void ExitWindow()
{
	ImageList_Destroy(g_ListViewImageListSmall);
	ImageList_Destroy(g_ListViewImageListLarge);
	ImageList_Destroy(g_TreeViewImageList);
	FreeLibrary(g_hModule);
	PostQuitMessage(0);
}

