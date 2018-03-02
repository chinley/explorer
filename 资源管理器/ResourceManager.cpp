// ResourceManager.cpp :定义应用程序的入口
//

#include "stdafx.h"
#include "resource.h"

#include "Shutdown.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ExplorerFun.h"
#include "InitOpreation.h"
#include "FileOperation.h"
#include "AboutDlg.h"
#include "CreateShortCutDlg.h"
#include "WinVersion.h"
#include "ShutDownDll.h"



#define MAX_LOADSTRING 100
HWND g_hwndMain = NULL;//全局窗口句柄
extern TCHAR g_szAllPath[MAX_PATH];        //全路径

//定义窗口类中的消息处理函数WndProc
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine,
				   int       nCmdShow)
{
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text
	
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXPLORER, szWindowClass, MAX_LOADSTRING);
	
	HWND hwnd;//窗口句柄
	WNDCLASSEX wndclass;//定义窗口类wndclass
	MSG msg;//消息结构msg用来接收消息
	
	
			/*MAKEINTRESOURCE
			*简介：
			*是一个资源名转换的宏，这个宏是把一个数字类型
			*转换成指针类型的宏，它不存在释放的问题
			
			  *【具体作用】
			  *把一个"数字型ID",转化为"字符串".但是执行前后,
			  *输入的数据的内容和长度是不变的!它只不过就是C语
			  *言里面"强制类型转换"而已.
	*/
	wndclass.cbSize			=	sizeof(WNDCLASSEX);//WNDCLASSEX 的大小。我们可以用sizeof（WNDCLASSEX）来获得准确的值。
	wndclass.cbClsExtra		=	0;//紧跟在窗口类结构后的附加字节数
	wndclass.cbWndExtra		=	0;//指定紧跟在窗口实例的附加字节数。
	wndclass.style			=	0;//窗口风格
	wndclass.hbrBackground	=	(HBRUSH)GetStockObject(COLOR_WINDOW + 1);//背景画刷
	wndclass.hCursor		=	LoadCursor(NULL,IDC_ARROW);//光标默认
	wndclass.hIcon			=	LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//将"IDI_ICON"设为图标
	wndclass.hIconSm		=	LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//将"IDI_ICON"设为小图标
	wndclass.hInstance		=	hInstance;
	wndclass.lpfnWndProc	=	WndProc;//指定窗口类wndclass所创建的窗口的消息处理函数
	wndclass.lpszClassName	=	szWindowClass;//指向类名称的指针
	wndclass.lpszMenuName	=	MAKEINTRESOURCE(IDR_MAINMENU);
	
	//调用RegisterClassEx注册wndclass窗口类
	if(!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("Windows Registration failed!"), TEXT("Error"), 
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	hwnd = CreateWindowEx (
		WS_EX_ACCEPTFILES,//窗口的扩展风格：窗口接受一个拖曳的文件
		szWindowClass,//指向注册类名的指针 
		szTitle, //指向窗口名称的指针
		WS_OVERLAPPEDWINDOW , //窗口风格：多种窗口类型的组合
		CW_USEDEFAULT,//窗口的水平位置
		0,//窗口的垂直位置
		CW_USEDEFAULT,//窗口的宽度 
		0, //窗口的高度
		NULL, //父窗口的句柄
		NULL, //菜单的句柄或是子窗口的标识符
		hInstance, //应用程序实例的句柄
		NULL);//指向窗口的创建数据
	
	if(hwnd == NULL)
	{
		MessageBox(
			NULL, //此参数代表消息框拥有的窗口。如果为NULL，则消息框没有拥有窗口
			TEXT("Window Creation Failed!"), //消息框的内容
			TEXT("Error"),//消息框的标题
			MB_OK | MB_ICONEXCLAMATION);//指定一个决定对话框的内容和行为的标志集（按钮的组合）
		return 0;
	}
	
	g_hwndMain = hwnd;//全局句柄
	
	InitCommonControls();//是注册并初始化通用控件窗口类。是个空函数，不做任何事情
	
	ShowWindow(hwnd,nCmdShow);//显示窗口
	UpdateWindow(hwnd);
	
	
	//键盘加速器（快捷键）
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	
	//消息循环
	while(GetMessage(&msg,NULL,0,0) > 0)
	{
	/*函数：TranslateAccelerator (hwnd, hAccel, &msg)
	【参数】
	**hwnd：窗口句柄，其消息将被翻译
	**hAccel：加速器表的手柄。加速表必须通过调用LoadAccelerators函数加载，或者通过调用CreateAcceleratorTable函数来创建。
	**&msg：指向MSG结构的指针，其中包含使用GetMessage或PeekMessage函数从调用线程的消息队列中检索的消息信息。
	
	  【作用】
	  处理菜单命令的加速键。该函数将WM_KEYDOWN或WM_SYSKEYDOWN消息转换为WM_COMMAND或WM_SYSCOMMAND消息
	  （如果在指定的加速器表中有一个条目），然后将WM_COMMAND或WM_SYSCOMMAND消息直接发送到指定的窗口过程。
	  在窗口过程处理消息之前，TranslateAccelerator不会返回。
		*/
		if (!TranslateAccelerator (hwnd, hAccel, &msg))//返回值为零（即函数失败）就执行if语句
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
				   }
				   

				   LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
				   {
					   int wmId, wmEvent;
					   
					   switch (message) 
					   {
					   case WM_CREATE:
						   {
							   InitExplorerWindow(hwnd);//InitOperation.cpp【1】
						   }
						   break;
					   case WM_SIZE:
						   {
							   InitControlSize(hwnd, lParam);//InitOperation.cpp【2】
						   }
						   break;
						   case WM_INITMENUPOPUP:/*消息在下拉菜单或子菜单
												 将要被激活的时候发出。如果没有替换整个菜单，
							   允许这个应用程序在菜单显示之前修改*/
							   {
								   InitMenuPopUp(hwnd, wParam, lParam);//InitOperation.cpp【5】菜单状态
							   }
							   break;
							   case WM_CONTEXTMENU:/*标准的菜单消息：右键按下
								   然后松开会触发此消息*/
								   {
									   InitContextMenu(hwnd, wParam, lParam);//InitOperation.cpp【6】创建上下文菜单
								   }
								   break;
								   
								   /*WM_NOTIFY
								   *作用
								   *当自定义控件中发生了特殊的事件
								   *需要通知父窗口时，可以像父窗口
								   *发送消息，可以使用WM_NOTIFY消息
								   *像父窗口发送消息
								   
									 **顾名思义，WM_NOTIFY是专门用于控件向父窗口发送消息
									 **的。这个消息与其他的消息不同，用户可以自行定义通
									 **知的内容，但传递消息的方式是统一的，程序的处理非
									 **常规范、简洁。
								   */
							   case WM_NOTIFY:
								   {
									   OnNotify(hwnd, wParam, lParam);//ExplorerFun.cpp【4】响应WM_NOTIFY
								   }
								   break;
								   
								   /*WM_COMMAND
								   *控制子窗口的通知消息同菜单的命令消息及加速键消
								   *息一样，均使用WM_COMMAND来发送
								   
									 **什么时候发送一个WM_COMMAND消息
									 **1）当用户从菜单选中一个命令项目、
									 **2）当一个控件发送通知消息给父窗口
									 **3）按下一个快捷键
									 
								   */
							   case WM_COMMAND:
								   {
									   wmId    = LOWORD(wParam); 
									   wmEvent = HIWORD(wParam); 
									   switch (wmId)
									   {
									   case ID_MENUEXIT://文件》关闭
										   {
											   ExitWindow();//InitOperation.cpp【9】关闭窗口
										   }
										   break;
									   case ID_MENUOPEN://文件》打开
										   {
											   OpenListFile(hwnd);//FileOperation.cpp【13】打开文件
										   }
										   break;
									   case ID_MENUOPENFILE://文件》文件（档）查看
										   {
											   OpenMenuFile(hwnd);//FileOperation.cpp【18】打开某个文件
										   }
										   break;
									   case ID_MENUBACK://后退
										   {
											   BackPath(hwnd);//ExplorerFun.cpp【13】后退
										   }
										   break;
									   case ID_MENUFOWARD://前进
										   {
											   FowardPath(hwnd);//ExplorerFun.cpp【14】前进
										   }
										   break;
									   case ID_MENUFOWARDLAST: //向上一级
										   {
											   BackPath(hwnd);//ExplorerFun.cpp【13】后退
										   }
										   break;
									   case ID_MENULIST://查看》列表
										   {
											   SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_LIST);//ExplorerFun.cpp【2】设置查看方式
										   }
										   break;
										   
									   case ID_MENURESOURCE: //查看》详细资料
										   {
											   SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_REPORT);//ExplorerFun.cpp【2】设置查看方式
										   }
										   break;
									   case ID_MENUICON: //查看》图标
										   {
											   SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_ICON);//ExplorerFun.cpp【2】设置查看方式
										   }
										   break;
									   case ID_FILE_GOTO: //查看》转到
										   {
											   GotoFilePath(hwnd);//ExplorerFun.cpp【15】转到路径
										   }
										   break;
									   case ID_MENUDELETE: //文件》删除
										   {
											   DeleteListFile(hwnd);//FileOperation.cpp【4】删除
										   }
										   break;
									   case ID_MENUREFRESH: //查看》刷新
										   {
											   RefreshWindow(hwnd);//ExplorerFun.cpp【16】刷新窗口
										   }
										   break;
									   case ID_MENURENAME: //文件》重命名
										   {
											   int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);
											   SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_EDITLABEL, iIndex, 0);
										   }
										   break;
									   case ID_MENUNEWDIRECTORY://文件》新建》文件夹
										   {
											   CreateNewFile(hwnd, CREATE_DIRECTORY);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUNEWSHORTCUT://文件》新建》快捷方式
										   {
											   if (0 == lstrcmp(g_szAllPath, TEXT("")))
											   {
												   MessageBox(hwnd, TEXT("磁盘页面不能新建快捷方式"), TEXT("VLExplorer"), MB_OK | MB_ICONERROR);
												   break;
											   }
											   HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
											   DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGCREATESHORTCUT), hwnd, CreateShortCut_Proc);
										   }
										   break;
									   case ID_MENUNEWBMP: //文件》新建》BMP图像
										   {
											   CreateNewFile(hwnd, CREATE_BITMAP);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUCREATEWORD: //文件》新建》Microsoft Word 文档
										   {
											   CreateNewFile(hwnd, CREATE_WORD);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUCREATEPPT: //文件》新建》Microsoft PowerPoint 演示文稿
										   {
											   CreateNewFile(hwnd, CREATE_PPT);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUNEWTEXT: //文件》新建》文本文档
										   {
											   CreateNewFile(hwnd, CREATE_TEXT);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUCREATEWAV://文件》新建》波形声音
										   {
											   CreateNewFile(hwnd, CREATE_WAV);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUCREATEEXCEL://文件》新建》Microsoft Excel 工作表
										   {
											   CreateNewFile(hwnd, CREATE_EXCEL);//FileOperation.cpp【11】新建文件
										   }
										   break;
									   case ID_MENUSORTSIZE: //查看》排列图标》大小
										   {
											   SortListFile(hwnd, SORT_SIZE);//ExplorerFun.cpp【17】排序
										   }
										   break;
									   case ID_MENUSORTWRITETIME://查看》排列图标》修改时间
										   {
											   SortListFile(hwnd, SORT_WRITETIME);//ExplorerFun.cpp【17】排序
										   }
										   break;
									   case ID_MENUSORTSTYPE://查看》排列图标》类型
										   {
											   SortListFile(hwnd, SORT_STYLE);//ExplorerFun.cpp【17】排序
										   }
										   break;
									   case ID_MENUSORTNAME://查看》排列图标》名称
										   {
											   SortListFile(hwnd, SORT_NAME);//ExplorerFun.cpp【17】排序
										   }
										   break;
									   case ID_MENUCUT://编辑》剪切
										   {
											   CopyOrCutListFile(hwnd, FALSE);//FileOperation.cpp【10】复制文件
										   }
										   break;
									   case ID_MENUCOPY://编辑》复制
										   {
											   CopyOrCutListFile(hwnd, TRUE);//FileOperation.cpp【10】复制文件
										   }
										   break;
									   case ID_MENUPASTE://编辑》粘贴
										   {
											   PasteListFile(hwnd);//FileOperation.cpp【9】粘贴文件
										   }
										   break;
									   case ID_MENUCOPYTODIRECTORY://编辑》复制到文件夹
										   {
											   CopyOrMoveToDirectory(hwnd, TRUE);//FileOperation.cpp【8】复制到，移动到文件夹
										   }
										   break;
									   case ID_MENUMOVETODIRECTORY://编辑》移动到文件夹
										   {
											   CopyOrMoveToDirectory(hwnd, FALSE);//FileOperation.cpp【8】复制到，移动到文件夹
										   }
										   break;
									   case ID_MENUSELECTALL://编辑》全部选定
										   {
											   SelectedAllItem(hwnd);//FileOperation.cpp【5】全部选定
										   }
										   break;
									   case ID_MENUINVERSESELECT://编辑》反向选定
										   {
											   InverSelectedItem(hwnd);//FileOperation.cpp【6】反向选定
										   }
										   break;
									   case ID_MENUATTRIBUTE://文件》属性
										   {
											   ShowFileAttribute(hwnd);//FileOperation.cpp【7】文件（夹）、磁盘属性				}
											   break;
									   case ID_MENUISRIGHTFUL://帮助》这份Windows合法吗
										   {
											   IsRightful(hwnd);//ExplorerFun.cpp【19】判断Windows是否合法
										   }
										   break;
									   case ID_MENUABOUT://关于
										   {
											   HINSTANCE  hHelp = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
											   DialogBox(hHelp,MAKEINTRESOURCE(IDD_DIALOGABOUT), hwnd, About_Proc);
										   }
										   break;
									   case ID_MUNEHELPCENTER://帮助》帮助和支持中心
										   {
											   OpenHelpCenter(hwnd);//ExplorerFun.cpp【22】帮助和支持中心
										   }
										   break;
									   case ID_MENUCREATESHORTCUT://创建快捷方式
										   {
											   CreateListFileShortCut(hwnd);//ExplorerFun.cpp【20】创建快捷方式
										   }
										   break;
									   case ID_MENUHOMEPAGE://主页
										   {
											   GetIEHomePage(hwnd);//FileOperation.cpp【17】IE主页
										   }
										   break;
										   
									   case ID_SHUTDOWN:
										   {
											   Shutdown(false);//关机》关机
										   }
										   break;
									   case ID_REBOOT:
										   {
											   Reboot(false);//关机》重启
										   }
										   break;
									   case ID_LOGOFF:
										   {
											   Logoff(false);//关机》注销
										   }
										   break;
									   case ID_OSVERSION:
										   {
											   Version(hwnd);
										   }
										   break;
										   
									   default:
										   return DefWindowProc(hwnd, message, wParam, lParam);
										   }
		}
		break;
	case WM_CLOSE://关闭窗口
		{
			ExitWindow();//InitOperation.cpp【9】关闭窗口
		}
		break;
	case WM_DESTROY://关闭程序
		{
			ExitWindow();//InitOperation.cpp【9】关闭程序
		}
		break;
		
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	}
	return 0;
	
}
