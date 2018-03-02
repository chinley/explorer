// ResourceManager.cpp :����Ӧ�ó�������
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
HWND g_hwndMain = NULL;//ȫ�ִ��ھ��
extern TCHAR g_szAllPath[MAX_PATH];        //ȫ·��

//���崰�����е���Ϣ������WndProc
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
	
	HWND hwnd;//���ھ��
	WNDCLASSEX wndclass;//���崰����wndclass
	MSG msg;//��Ϣ�ṹmsg����������Ϣ
	
	
			/*MAKEINTRESOURCE
			*��飺
			*��һ����Դ��ת���ĺ꣬������ǰ�һ����������
			*ת����ָ�����͵ĺ꣬���������ͷŵ�����
			
			  *���������á�
			  *��һ��"������ID",ת��Ϊ"�ַ���".����ִ��ǰ��,
			  *��������ݵ����ݺͳ����ǲ����!��ֻ��������C��
			  *������"ǿ������ת��"����.
	*/
	wndclass.cbSize			=	sizeof(WNDCLASSEX);//WNDCLASSEX �Ĵ�С�����ǿ�����sizeof��WNDCLASSEX�������׼ȷ��ֵ��
	wndclass.cbClsExtra		=	0;//�����ڴ�����ṹ��ĸ����ֽ���
	wndclass.cbWndExtra		=	0;//ָ�������ڴ���ʵ���ĸ����ֽ�����
	wndclass.style			=	0;//���ڷ��
	wndclass.hbrBackground	=	(HBRUSH)GetStockObject(COLOR_WINDOW + 1);//������ˢ
	wndclass.hCursor		=	LoadCursor(NULL,IDC_ARROW);//���Ĭ��
	wndclass.hIcon			=	LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//��"IDI_ICON"��Ϊͼ��
	wndclass.hIconSm		=	LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));//��"IDI_ICON"��ΪСͼ��
	wndclass.hInstance		=	hInstance;
	wndclass.lpfnWndProc	=	WndProc;//ָ��������wndclass�������Ĵ��ڵ���Ϣ������
	wndclass.lpszClassName	=	szWindowClass;//ָ�������Ƶ�ָ��
	wndclass.lpszMenuName	=	MAKEINTRESOURCE(IDR_MAINMENU);
	
	//����RegisterClassExע��wndclass������
	if(!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, TEXT("Windows Registration failed!"), TEXT("Error"), 
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	hwnd = CreateWindowEx (
		WS_EX_ACCEPTFILES,//���ڵ���չ��񣺴��ڽ���һ����ҷ���ļ�
		szWindowClass,//ָ��ע��������ָ�� 
		szTitle, //ָ�򴰿����Ƶ�ָ��
		WS_OVERLAPPEDWINDOW , //���ڷ�񣺶��ִ������͵����
		CW_USEDEFAULT,//���ڵ�ˮƽλ��
		0,//���ڵĴ�ֱλ��
		CW_USEDEFAULT,//���ڵĿ�� 
		0, //���ڵĸ߶�
		NULL, //�����ڵľ��
		NULL, //�˵��ľ�������Ӵ��ڵı�ʶ��
		hInstance, //Ӧ�ó���ʵ���ľ��
		NULL);//ָ�򴰿ڵĴ�������
	
	if(hwnd == NULL)
	{
		MessageBox(
			NULL, //�˲���������Ϣ��ӵ�еĴ��ڡ����ΪNULL������Ϣ��û��ӵ�д���
			TEXT("Window Creation Failed!"), //��Ϣ�������
			TEXT("Error"),//��Ϣ��ı���
			MB_OK | MB_ICONEXCLAMATION);//ָ��һ�������Ի�������ݺ���Ϊ�ı�־������ť����ϣ�
		return 0;
	}
	
	g_hwndMain = hwnd;//ȫ�־��
	
	InitCommonControls();//��ע�Ტ��ʼ��ͨ�ÿؼ������ࡣ�Ǹ��պ����������κ�����
	
	ShowWindow(hwnd,nCmdShow);//��ʾ����
	UpdateWindow(hwnd);
	
	
	//���̼���������ݼ���
	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));
	
	//��Ϣѭ��
	while(GetMessage(&msg,NULL,0,0) > 0)
	{
	/*������TranslateAccelerator (hwnd, hAccel, &msg)
	��������
	**hwnd�����ھ��������Ϣ��������
	**hAccel������������ֱ������ٱ����ͨ������LoadAccelerators�������أ�����ͨ������CreateAcceleratorTable������������
	**&msg��ָ��MSG�ṹ��ָ�룬���а���ʹ��GetMessage��PeekMessage�����ӵ����̵߳���Ϣ�����м�������Ϣ��Ϣ��
	
	  �����á�
	  ����˵�����ļ��ټ����ú�����WM_KEYDOWN��WM_SYSKEYDOWN��Ϣת��ΪWM_COMMAND��WM_SYSCOMMAND��Ϣ
	  �������ָ���ļ�����������һ����Ŀ����Ȼ��WM_COMMAND��WM_SYSCOMMAND��Ϣֱ�ӷ��͵�ָ���Ĵ��ڹ��̡�
	  �ڴ��ڹ��̴�����Ϣ֮ǰ��TranslateAccelerator���᷵�ء�
		*/
		if (!TranslateAccelerator (hwnd, hAccel, &msg))//����ֵΪ�㣨������ʧ�ܣ���ִ��if���
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
							   InitExplorerWindow(hwnd);//InitOperation.cpp��1��
						   }
						   break;
					   case WM_SIZE:
						   {
							   InitControlSize(hwnd, lParam);//InitOperation.cpp��2��
						   }
						   break;
						   case WM_INITMENUPOPUP:/*��Ϣ�������˵����Ӳ˵�
												 ��Ҫ�������ʱ�򷢳������û���滻�����˵���
							   �������Ӧ�ó����ڲ˵���ʾ֮ǰ�޸�*/
							   {
								   InitMenuPopUp(hwnd, wParam, lParam);//InitOperation.cpp��5���˵�״̬
							   }
							   break;
							   case WM_CONTEXTMENU:/*��׼�Ĳ˵���Ϣ���Ҽ�����
								   Ȼ���ɿ��ᴥ������Ϣ*/
								   {
									   InitContextMenu(hwnd, wParam, lParam);//InitOperation.cpp��6�����������Ĳ˵�
								   }
								   break;
								   
								   /*WM_NOTIFY
								   *����
								   *���Զ���ؼ��з�����������¼�
								   *��Ҫ֪ͨ������ʱ�������񸸴���
								   *������Ϣ������ʹ��WM_NOTIFY��Ϣ
								   *�񸸴��ڷ�����Ϣ
								   
									 **����˼�壬WM_NOTIFY��ר�����ڿؼ��򸸴��ڷ�����Ϣ
									 **�ġ������Ϣ����������Ϣ��ͬ���û��������ж���ͨ
									 **֪�����ݣ���������Ϣ�ķ�ʽ��ͳһ�ģ�����Ĵ����
									 **���淶����ࡣ
								   */
							   case WM_NOTIFY:
								   {
									   OnNotify(hwnd, wParam, lParam);//ExplorerFun.cpp��4����ӦWM_NOTIFY
								   }
								   break;
								   
								   /*WM_COMMAND
								   *�����Ӵ��ڵ�֪ͨ��Ϣͬ�˵���������Ϣ�����ټ���
								   *Ϣһ������ʹ��WM_COMMAND������
								   
									 **ʲôʱ����һ��WM_COMMAND��Ϣ
									 **1�����û��Ӳ˵�ѡ��һ��������Ŀ��
									 **2����һ���ؼ�����֪ͨ��Ϣ��������
									 **3������һ����ݼ�
									 
								   */
							   case WM_COMMAND:
								   {
									   wmId    = LOWORD(wParam); 
									   wmEvent = HIWORD(wParam); 
									   switch (wmId)
									   {
									   case ID_MENUEXIT://�ļ����ر�
										   {
											   ExitWindow();//InitOperation.cpp��9���رմ���
										   }
										   break;
									   case ID_MENUOPEN://�ļ�����
										   {
											   OpenListFile(hwnd);//FileOperation.cpp��13�����ļ�
										   }
										   break;
									   case ID_MENUOPENFILE://�ļ����ļ��������鿴
										   {
											   OpenMenuFile(hwnd);//FileOperation.cpp��18����ĳ���ļ�
										   }
										   break;
									   case ID_MENUBACK://����
										   {
											   BackPath(hwnd);//ExplorerFun.cpp��13������
										   }
										   break;
									   case ID_MENUFOWARD://ǰ��
										   {
											   FowardPath(hwnd);//ExplorerFun.cpp��14��ǰ��
										   }
										   break;
									   case ID_MENUFOWARDLAST: //����һ��
										   {
											   BackPath(hwnd);//ExplorerFun.cpp��13������
										   }
										   break;
									   case ID_MENULIST://�鿴���б�
										   {
											   SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_LIST);//ExplorerFun.cpp��2�����ò鿴��ʽ
										   }
										   break;
										   
									   case ID_MENURESOURCE: //�鿴����ϸ����
										   {
											   SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_REPORT);//ExplorerFun.cpp��2�����ò鿴��ʽ
										   }
										   break;
									   case ID_MENUICON: //�鿴��ͼ��
										   {
											   SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_ICON);//ExplorerFun.cpp��2�����ò鿴��ʽ
										   }
										   break;
									   case ID_FILE_GOTO: //�鿴��ת��
										   {
											   GotoFilePath(hwnd);//ExplorerFun.cpp��15��ת��·��
										   }
										   break;
									   case ID_MENUDELETE: //�ļ���ɾ��
										   {
											   DeleteListFile(hwnd);//FileOperation.cpp��4��ɾ��
										   }
										   break;
									   case ID_MENUREFRESH: //�鿴��ˢ��
										   {
											   RefreshWindow(hwnd);//ExplorerFun.cpp��16��ˢ�´���
										   }
										   break;
									   case ID_MENURENAME: //�ļ���������
										   {
											   int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);
											   SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_EDITLABEL, iIndex, 0);
										   }
										   break;
									   case ID_MENUNEWDIRECTORY://�ļ����½����ļ���
										   {
											   CreateNewFile(hwnd, CREATE_DIRECTORY);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUNEWSHORTCUT://�ļ����½�����ݷ�ʽ
										   {
											   if (0 == lstrcmp(g_szAllPath, TEXT("")))
											   {
												   MessageBox(hwnd, TEXT("����ҳ�治���½���ݷ�ʽ"), TEXT("VLExplorer"), MB_OK | MB_ICONERROR);
												   break;
											   }
											   HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
											   DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGCREATESHORTCUT), hwnd, CreateShortCut_Proc);
										   }
										   break;
									   case ID_MENUNEWBMP: //�ļ����½���BMPͼ��
										   {
											   CreateNewFile(hwnd, CREATE_BITMAP);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUCREATEWORD: //�ļ����½���Microsoft Word �ĵ�
										   {
											   CreateNewFile(hwnd, CREATE_WORD);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUCREATEPPT: //�ļ����½���Microsoft PowerPoint ��ʾ�ĸ�
										   {
											   CreateNewFile(hwnd, CREATE_PPT);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUNEWTEXT: //�ļ����½����ı��ĵ�
										   {
											   CreateNewFile(hwnd, CREATE_TEXT);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUCREATEWAV://�ļ����½�����������
										   {
											   CreateNewFile(hwnd, CREATE_WAV);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUCREATEEXCEL://�ļ����½���Microsoft Excel ������
										   {
											   CreateNewFile(hwnd, CREATE_EXCEL);//FileOperation.cpp��11���½��ļ�
										   }
										   break;
									   case ID_MENUSORTSIZE: //�鿴������ͼ�꡷��С
										   {
											   SortListFile(hwnd, SORT_SIZE);//ExplorerFun.cpp��17������
										   }
										   break;
									   case ID_MENUSORTWRITETIME://�鿴������ͼ�꡷�޸�ʱ��
										   {
											   SortListFile(hwnd, SORT_WRITETIME);//ExplorerFun.cpp��17������
										   }
										   break;
									   case ID_MENUSORTSTYPE://�鿴������ͼ�꡷����
										   {
											   SortListFile(hwnd, SORT_STYLE);//ExplorerFun.cpp��17������
										   }
										   break;
									   case ID_MENUSORTNAME://�鿴������ͼ�꡷����
										   {
											   SortListFile(hwnd, SORT_NAME);//ExplorerFun.cpp��17������
										   }
										   break;
									   case ID_MENUCUT://�༭������
										   {
											   CopyOrCutListFile(hwnd, FALSE);//FileOperation.cpp��10�������ļ�
										   }
										   break;
									   case ID_MENUCOPY://�༭������
										   {
											   CopyOrCutListFile(hwnd, TRUE);//FileOperation.cpp��10�������ļ�
										   }
										   break;
									   case ID_MENUPASTE://�༭��ճ��
										   {
											   PasteListFile(hwnd);//FileOperation.cpp��9��ճ���ļ�
										   }
										   break;
									   case ID_MENUCOPYTODIRECTORY://�༭�����Ƶ��ļ���
										   {
											   CopyOrMoveToDirectory(hwnd, TRUE);//FileOperation.cpp��8�����Ƶ����ƶ����ļ���
										   }
										   break;
									   case ID_MENUMOVETODIRECTORY://�༭���ƶ����ļ���
										   {
											   CopyOrMoveToDirectory(hwnd, FALSE);//FileOperation.cpp��8�����Ƶ����ƶ����ļ���
										   }
										   break;
									   case ID_MENUSELECTALL://�༭��ȫ��ѡ��
										   {
											   SelectedAllItem(hwnd);//FileOperation.cpp��5��ȫ��ѡ��
										   }
										   break;
									   case ID_MENUINVERSESELECT://�༭������ѡ��
										   {
											   InverSelectedItem(hwnd);//FileOperation.cpp��6������ѡ��
										   }
										   break;
									   case ID_MENUATTRIBUTE://�ļ�������
										   {
											   ShowFileAttribute(hwnd);//FileOperation.cpp��7���ļ����У�����������				}
											   break;
									   case ID_MENUISRIGHTFUL://���������Windows�Ϸ���
										   {
											   IsRightful(hwnd);//ExplorerFun.cpp��19���ж�Windows�Ƿ�Ϸ�
										   }
										   break;
									   case ID_MENUABOUT://����
										   {
											   HINSTANCE  hHelp = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
											   DialogBox(hHelp,MAKEINTRESOURCE(IDD_DIALOGABOUT), hwnd, About_Proc);
										   }
										   break;
									   case ID_MUNEHELPCENTER://������������֧������
										   {
											   OpenHelpCenter(hwnd);//ExplorerFun.cpp��22��������֧������
										   }
										   break;
									   case ID_MENUCREATESHORTCUT://������ݷ�ʽ
										   {
											   CreateListFileShortCut(hwnd);//ExplorerFun.cpp��20��������ݷ�ʽ
										   }
										   break;
									   case ID_MENUHOMEPAGE://��ҳ
										   {
											   GetIEHomePage(hwnd);//FileOperation.cpp��17��IE��ҳ
										   }
										   break;
										   
									   case ID_SHUTDOWN:
										   {
											   Shutdown(false);//�ػ����ػ�
										   }
										   break;
									   case ID_REBOOT:
										   {
											   Reboot(false);//�ػ�������
										   }
										   break;
									   case ID_LOGOFF:
										   {
											   Logoff(false);//�ػ���ע��
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
	case WM_CLOSE://�رմ���
		{
			ExitWindow();//InitOperation.cpp��9���رմ���
		}
		break;
	case WM_DESTROY://�رճ���
		{
			ExitWindow();//InitOperation.cpp��9���رճ���
		}
		break;
		
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	}
	return 0;
	
}
