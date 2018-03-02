#include "stdafx.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "ExplorerFun.h"
#include "InitOpreation.h"
#include <richedit.h>


//ȫ�ֺ���������Сдǰ׺"g"��ͷ��
//ȫ�ֱ���������"g_"��ͷ
extern HIMAGELIST g_ListViewImageListLarge;
extern HIMAGELIST g_ListViewImageListSmall;
extern HIMAGELIST g_TreeViewImageList;
extern TCHAR g_szAllPath[MAX_PATH];        //ȫ·��

HMODULE g_hModule;


//��1����ʼ������
void InitExplorerWindow(HWND hwnd)
{
	HINSTANCE  hInst = (HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE);//�� GetWindowLong ��������ܹ����ָ�����ڵ���Ϣ
	
	//����treeview
	if (NULL == CreateWindow(
		WC_TREEVIEW,//����1��lpClassName��ָ���˴��ڵ���������������������κ��ú���RegisterClassע��������������κ�Ԥ����Ŀ������� 
		"", //����2��lpWindowName�����ڱ���
		WS_BORDER | WS_VISIBLE | WS_TABSTOP
		| WS_CHILD | TVS_HASLINES | TVS_HASBUTTONS 
		| TVS_LINESATROOT | WS_EX_CLIENTEDGE,//����3��dwStyle��ָ���������ڵķ��
		0,//����4��X��ָ�����ڵĳ�ʼˮƽλ��
		0,//����5��Y:ָ�����ڵĳ�ʼ��ֱλ��
		0,//����6��nWidth�����豸��Ԫָ�����ڵĿ��
		0,//����7��nHeight�����豸��Ԫָ�����ڵĸ߶�
		hwnd,//����8��hWndParent��ָ�򱻴������ڵĸ����ڻ������ߴ��ڵľ��
		(HMENU)IDC_TREEFILE,//����9��hMenu���˵�����������ݴ��ڷ��ָ��һ���Ӵ��ڱ�ʶ
		hInst,//����10��hInstance���봰���������ģ��ʵ���ľ��
		NULL))//����11��lpParam��ָ��һ��ֵ��ָ�룬��ֵ���ݸ�����WM_CREATE��Ϣ
	{
		MessageBox(hwnd, TEXT("������״�ؼ�ʧ��"), TEXT("��ʾ"), MB_OK);
		return;
	}

	//����ListView
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
		MessageBox(hwnd, TEXT("����ListView�ؼ�ʧ��"), TEXT("��ʾ"), MB_OK);
		return;
	}

	//�����б���ͼ�ؼ�����չ��ʽ
	ListView_SetExtendedListViewStyle(
		GetDlgItem(hwnd, IDC_LISTVIEWFILE),//�����յ���ʽ���ĵ��б���ͼ�ؼ��ľ��  
		LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);//ָ����չ�б���ͼ�ؼ��ķ�� 

	InitListViewColumn(hwnd, FALSE);
	SetLookMode(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVS_REPORT);

	//����������
    HWND hTool =CreateWindowEx(
		WS_EX_TOOLWINDOW, 
		TOOLBARCLASSNAME, //������������
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
		MessageBox(hwnd, TEXT("����������ʧ��"), TEXT("��ʾ"), MB_OK | MB_ICONWARNING);
		return;
	}
	SendMessage(//�ú�����ָ������Ϣ���͵�һ���������ڡ���Ӧ�ó����Ӧ�ó���֮�������Ϣ���ݵ���Ҫ�ֶ�֮һ
		hTool,//������Ϣ�Ĵ��ھ��
		TB_BUTTONSTRUCTSIZE,//ָ�������͵���Ϣ���������Ϣָ��TBBUTTON�ṹ�Ĵ�С
		(WPARAM)sizeof(TBBUTTON),//ִ�и��ӵ���Ϣ�ض���Ϣ
		0);//ָ�����ӵ���Ϣ�ض���Ϣ

	TBBUTTON	tbb[6];//�������еİ�ť
	

	/*
	TBADDBITMAP�ṹ�����˹�������ӵ�λͼ����Ϣ
	*/
	TBADDBITMAP tbab;//һ��������ťͼ���λͼ
	


	/*void ZeroMemory(PVOID Destination,SIZE_T Length);
	*������
	*Destination��ָ��һ��׼����0�������ڴ�����Ŀ�ʼ��ַ
	*Length��׼����0�������ڴ�����Ĵ�С�����ֽ�������
	*/
	ZeroMemory(&tbab, sizeof(TBADDBITMAP));
	

	tbab.hInst = HINST_COMMCTRL;
	tbab.nID   = IDB_STD_LARGE_COLOR;
	SendMessage(hTool, TB_ADDBITMAP, 6,(LPARAM)&tbab);

	ZeroMemory(tbb, sizeof(tbb));

	tbb[0].iBitmap = 3;//HIST_BACK;        //����
	tbb[0].fsState = TBSTATE_ENABLED ;//��ť״̬��־���ó�Ա�����ǹ�������ť״̬���г���ֵ�����
	tbb[0].fsStyle = TBSTYLE_BUTTON;//��ť��ʽ���ó�Ա�����ǹ�������ť��ʽ���г���ֵ�ļ���
	tbb[0].idCommand = ID_MENUBACK;//�밴ť������������ʶ������ѡ��ťʱ���˱�ʶ������WM_COMMAND��Ϣ��
	tbb[0].iString = (int)TEXT("����");//������İ�ť�ַ�������

	tbb[1].iBitmap = 4;//HIST_FORWARD;        //ǰ��
	tbb[1].fsState = TBSTATE_ENABLED ;
	tbb[1].fsStyle = TBSTYLE_BUTTON;
	tbb[1].idCommand = ID_MENUFOWARD;
	tbb[1].iString = (int)TEXT("ǰ��");


	tbb[2].iBitmap=STD_CUT;//����
	tbb[2].fsState=TBSTATE_ENABLED;//��ť״̬
	tbb[2].fsStyle = TBSTYLE_BUTTON;//��ť��ʽ
	tbb[2].idCommand =ID_MENUCUT;//��ť����ID
	tbb[2].iString = (int)TEXT("����");

	tbb[3].iBitmap=STD_COPY;//����
	tbb[3].fsState=TBSTATE_ENABLED;//��ť״̬
	tbb[3].fsStyle = TBSTYLE_BUTTON;//��ť��ʽ
	tbb[3].idCommand =ID_MENUCOPY;//��ť����ID
	tbb[3].iString = (int)TEXT("����");



	tbb[4].iBitmap=STD_DELETE;//ɾ��
	tbb[4].fsState=TBSTATE_ENABLED;//��ť״̬
	tbb[4].fsStyle = TBSTYLE_BUTTON;//��ť��ʽ
	tbb[4].idCommand =ID_MENUDELETE;//��ť����ID
	tbb[4].iString = (int)TEXT("ɾ��");

	tbb[5].iBitmap=STD_REPLACE;
	tbb[5].fsState=TBSTATE_ENABLED;
	tbb[5].fsStyle=TBSTYLE_BUTTON;
	tbb[5].idCommand=ID_MENUREFRESH;
	tbb[5].iString=(int)TEXT("ˢ��");


	SendMessage(hTool,
		TB_ADDBUTTONS,
		sizeof(tbb) / sizeof(TBBUTTON),
		(LPARAM)&tbb);



	  //����������(��ַ��)

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
		MessageBox(hwnd, TEXT("����������ʧ��"), TEXT("��ʾ"), MB_OK | MB_ICONWARNING);
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
	tbb2[0].iString = (int)TEXT("��ַ(&D)");
		
	SendMessage(hToolAddress,
		TB_ADDBUTTONS,
		sizeof(tbb2) / sizeof(TBBUTTON),
		(LPARAM)&tbb2);

 	//��ת
	g_hModule = LoadLibrary("riched20.dll");// LoadLibrary:��ָ����ģ����ص����ý��̵ĵ�ַ�ռ��С�

	//Ҫ����һ���ḻ�ı༭�ؼ�������CreateWindowEx������ָ���ḻ�ı༭�����ࡣ
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
		TEXT("��ת"),
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



	//����״̬��
	HWND hStatus;
	int statwidths[] = {200, 300, -1};

	hStatus = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		STATUSCLASSNAME,//������
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
	SendMessage(hStatus, SB_SETTEXT, 2, (LPARAM)TEXT("�ҵĵ���"));

	InitImageLIst(hwnd);
	AddFileInfoToListView(hwnd);      //��Ӵ�����Ϣ��LIstView��

}

//��2�����ÿؼ���λ��
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

	//�б���ͼ
	MoveWindow(GetDlgItem(hwnd, IDC_LISTVIEWFILE), 190, iToolHeight+iToolAddrHeight, LOWORD(lParam) - 190, iHeight,TRUE);
	//��״��ͼ
	MoveWindow(GetDlgItem(hwnd, IDC_TREEFILE), 0, iToolHeight+iToolAddrHeight, 190, iHeight, TRUE);

	RECT rcAddress;
	SendMessage(GetDlgItem(hwnd, IDC_TOOLADDRESS), TB_GETITEMRECT, (WPARAM)0, (LPARAM)&rcAddress);

	MoveWindow(GetDlgItem(GetDlgItem(hwnd, IDC_TOOLADDRESS), IDM_EDITPATH), rcAddress.right, rcAddress.top,
		rcClient.right-rcAddress.right-rcAddress.right, rcAddress.bottom - rcAddress.top, TRUE);

	//��ת
	MoveWindow(GetDlgItem(GetDlgItem(hwnd, IDC_TOOLADDRESS), ID_FILE_GOTO), rcClient.right - rcAddress.right,
		rcAddress.top, rcAddress.right-rcAddress.left, rcAddress.bottom - rcAddress.top, TRUE);

}


//��3����ʼ��Image_LIst
void InitImageLIst(HWND hwnd)
{
	g_ListViewImageListLarge = ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 100, 0);
	g_ListViewImageListSmall = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 100, 0);

	g_TreeViewImageList = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 100, 0);

	TreeView_SetImageList(GetDlgItem(hwnd, IDC_TREEFILE), g_TreeViewImageList, TVSIL_NORMAL);


	ListView_SetImageList(GetDlgItem(hwnd, IDC_LISTVIEWFILE), g_ListViewImageListLarge, LVSIL_NORMAL);
	ListView_SetImageList(GetDlgItem(hwnd, IDC_LISTVIEWFILE), g_ListViewImageListSmall, LVSIL_SMALL);

	DWORD dwStyles = GetWindowLong(GetDlgItem(hwnd, IDC_TREEFILE), GWL_STYLE);//��ȡ������ԭ���
    dwStyles |= TVS_EDITLABELS | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
    SetWindowLong(GetDlgItem(hwnd, IDC_TREEFILE), GWL_STYLE, dwStyles);//���÷��
}


//��4����ʼ��ListView�ؼ���
void InitListViewColumn(HWND hwnd, BOOL IsFile)
{
	HWND hwndListViewFile = GetDlgItem(hwnd, IDC_LISTVIEWFILE);//���ݼ̳й�ϵ�ĺ������ܣ����ش�����ָ������ID����Ԫ�صľ��
	DeleteAllColumn(hwnd);

	SendMessage(hwndListViewFile, 
		LVM_SETEXTENDEDLISTVIEWSTYLE, 
		0,
		LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES );//�����б�ķ��
	LVCOLUMN columnFile;
	columnFile.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;   //ָ��LVCOLUMN�ṹ�е�pszText��fmt��cx����Ч
	
	/*fmt
	���е��б���������ı��Ķ��롣�����еĶ��뷽ʽʼ��ΪLVCFMT_LEFT; �����ܸı䡣
	*/
	columnFile.fmt = LVCFMT_CENTER;
	
		
	//�б���ͼ�����е�һ�����
	columnFile.pszText = TEXT("����");
	columnFile.cx = 120;//��һ�еĿ��
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 0, (LPARAM)&columnFile);
	
	//�б���ͼ�����еڶ������/��С
	
	if (FALSE == IsFile)      //����
	{
		columnFile.pszText = TEXT("����");
	}
	else
	{
		columnFile.pszText = TEXT("��С");
	}
	columnFile.cx = 120; 
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 1, (LPARAM)&columnFile);

	//�б���ͼ�����е�����ܴ�С/����

	if (FALSE == IsFile)      //����
	{
		columnFile.pszText = TEXT("�ܴ�С");
	}
	else
	{
		columnFile.pszText = TEXT("����");
	}
	columnFile.cx = 120; 
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 2, (LPARAM)&columnFile);

	//�б���ͼ�����е�������ÿռ�/�޸�����
	
	if (FALSE == IsFile)      //����
	{
		columnFile.pszText = TEXT("���ÿռ�");
	}
	else
	{
		columnFile.pszText = TEXT("�޸�����");
	}
	columnFile.cx = 120; 
	SendMessage(hwndListViewFile, LVM_INSERTCOLUMN, 3, (LPARAM)&columnFile);
}









//��5���˵���
void InitMenuPopUp(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	//GetDlgItem���Ǹ��ݼ̳й�ϵ�ĺ������ܣ����ش�����ָ������ID����Ԫ�صľ��������ͨ�����صľ���Դ����ڵ���Ԫ�ؽ��в�����
	int iCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
	//��iCountֵ���ж���ʹ�˵����еĹ�����Ч������Ч��iCount>0 ? MF_ENABLED : MF_GRAYED��

	switch (lParam)
	{
	case 0:     //�ļ��˵�
		{
			/*EnableMenuItem����˼��������ֹ����ָ���Ĳ˵���Ŀ��
			����ԭ�ͣ�BOOL EnableMenuItem(
����                  HMENU hMenu, //�˵����
����				  UINT uIDEnableItem, //��������ֹ��һ���˵���Ŀ�ı�ʶ����
����				  UINT wEnable //
����				  );
            ������
			1���˵����
			2���˵���Ŀ�ı�ʶ��
			3���������������ֻ��ָ������������MF_BYCOMMAND��MF_BYPOSITION��MF_ENABLED��MF_DISABLED�Լ�MF_GRAYED
			  MF_BYCOMMAND ָ�����������Ѵ��ڵĲ˵��������ID�š���Ϊȱʡֵ��
              MF_BYPOSITION ָ�����������Ѵ��ڲ˵����λ�á���һ�����ڵ�λ����0��
              MF_DISABLED ʹ�˵�����Ч���Ա������ܱ�ѡ�񣬵�����ҡ�
			  MF_ENABLED ʹ�˵�����Ч���Ա����ܹ���ѡ�񣬲��ɴӱ�ҵ�״̬�лָ�������
              MF_GRAYED ʹ�˵�����Ч���Ա������ܱ�ѡ��ͬʱ��ҡ�


			*/
			EnableMenuItem((HMENU) wParam, ID_MENUCREATESHORTCUT, iCount>0 ? MF_ENABLED : MF_GRAYED);//������ݷ�ʽ
			EnableMenuItem((HMENU)wParam, ID_MENUOPEN, iCount>0 ? MF_ENABLED : MF_GRAYED);//��
			EnableMenuItem((HMENU)wParam, ID_MENURENAME, iCount>0 ? MF_ENABLED : MF_GRAYED);//������

			/*
			lstrcmp ���ִ�Сд; lstrcmpi �����ִ�Сд
			  lstrcmp(string1,string2)
			1)string1>string2:���� 1
			2)string1==string2:���� 0
			3)string1<string2:���� -1
			*/
			if (0 == lstrcmp(g_szAllPath, TEXT("")))      //����
			{
				EnableMenuItem((HMENU)wParam, ID_MENUDELETE, MF_GRAYED);
			}
			else
			{
				EnableMenuItem((HMENU)wParam, ID_MENUDELETE, iCount>0 ? MF_ENABLED : MF_GRAYED);
			}
		}
		break;
	case 1:     //�༭�˵�
		{
			EnableMenuItem((HMENU)wParam, ID_MENUCOPYTODIRECTORY, iCount>0 ? MF_ENABLED : MF_GRAYED);//���Ƶ��ļ���
			EnableMenuItem((HMENU)wParam, ID_MENUMOVETODIRECTORY, iCount>0 ? MF_ENABLED : MF_GRAYED);//�ƶ����ļ���
			
			if (0 == lstrcmp(g_szAllPath, TEXT("")))      //����
			{
				EnableMenuItem((HMENU)wParam, ID_MENUCUT, MF_GRAYED);//����
				EnableMenuItem((HMENU)wParam, ID_MENUCOPY, MF_GRAYED);//����
				EnableMenuItem((HMENU)wParam, ID_MENUPASTE, MF_GRAYED);//ճ��
			}
			else
			{
				/*
				��1��IsClipboardFormatAvailable(CF_TEXT); �жϼ������ϵ�����,
				�����ǰ���а����������NULL��β��ASCII�ַ����ı���ʽ,��ú�������ֵΪtrue,����Ϊfalse.

                ��2��Bool OpenClipboard(HWND hWndNewOwner);
����            ָ���������򿪵ļ��а�Ĵ��ھ��������NULL��ʾ��������ǰ����ÿ��ֻ����һ�����̴򿪲����ʡ�
               ÿ��һ�ξ�Ҫ�رգ��������������޷����ʼ��а塣

				*/
				if(IsClipboardFormatAvailable(CF_HDROP) && OpenClipboard(NULL))
				{
					EnableMenuItem((HMENU)wParam, ID_MENUPASTE, MF_ENABLED);//"ճ��"��Ч
				}
				else
				{
					EnableMenuItem((HMENU)wParam, ID_MENUPASTE, MF_GRAYED);//"ճ��"��Ч����ң�
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

//��6�����������Ĳ˵�
void InitContextMenu(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	if (GetDlgItem(hwnd, IDC_LISTVIEWFILE) == (HWND)wParam)
	{
		// LVM_GETSELECTEDCOUNT��ȷ���б���ͼ�ؼ�����ѡ��Ŀ������
		int iSelectedCount = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTEDCOUNT, 0, 0);
		HMENU hPopMenu = CreatePopupMenu();
		if (0 == iSelectedCount)       //û��ѡ����ʱ�һ������Ĳ˵�
		{
			NoIndexSelectedMenu(hwnd, hPopMenu, lParam);
		}
		else        //ѡ���ļ�
		{
			IndexSelectedMenu(hwnd, hPopMenu, lParam);
		}
	}
}

//��7��û��ѡ���κ���ʱ�һ������Ĳ˵�
void NoIndexSelectedMenu(HWND hwnd, HMENU hPopMenu, LPARAM lParam)
{
	//�鿴
	HMENU hPopSubLook = CreatePopupMenu();

	/*AppendMenu��һ�ֿ�����ָ���Ĳ˵���������ʽ�˵����Ӳ˵����ݲ˵���ĩβ׷��һ���²˵���ĺ�����
	������ԭ�͡�
	BOOL AppendMenu��
			hMenu hMenu��
			UINT uFlags��
			UINT uIDNewltem��
	        LPCTSTR lpNewltem��;
	��ʵ����
	1��uFlags=MF_STRING��ָ���˵�����һ�������ַ���������lpNewltem������4��ָ����ַ�����
	2��uFlags=MF_POPUP��ָ���˵���һ������ʽ�˵����Ӳ˵�������uIDNewltem������3������ʽ�˵�
	���Ӳ˵��ľ�����˱�־�������˵�������һ������ʽ�˵����ڲ˵��Ĳ˵���Ӳ˵����ݲ˵���һ�����֡�
	3��uFlags=MF_SEPARATOR����һ��ˮƽ�����ߡ��˱�־ֻ������ʽ�˵����Ӳ˵����ݲ˵�ʹ�á�
	�������߲��ܱ���ҡ���Ч�����������IpNewltem������3����uIDNewltem������4�����á�
	*/
	AppendMenu(hPopSubLook, MF_STRING, ID_MENUICON, TEXT("ͼ��(&N)"));//�鿴��ͼ��
	AppendMenu(hPopSubLook, MF_STRING, ID_MENULIST, TEXT("�б�(&L)"));//�鿴���б�
	AppendMenu(hPopSubLook, MF_STRING, ID_MENURESOURCE, TEXT("��ϸ��Ϣ(&D)"));//�鿴����ϸ��Ϣ
	AppendMenu(hPopMenu, MF_POPUP | MF_STRING, (UINT)hPopSubLook, TEXT("�鿴(&V)"));//���ò˵��� "�鿴"
	
	AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //"�鿴"��"����ͼ��"֮�� ����ָ���
	
	//����ͼ��
	HMENU hPopSubSortIcon = CreatePopupMenu();
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTNAME, TEXT("����(&N)"));//����ͼ�꡷����
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTSIZE, TEXT("��С(&S)"));//����ͼ�꡷��С
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTSTYPE, TEXT("����(&T)"));//����ͼ�꡷����
	AppendMenu(hPopSubSortIcon, MF_STRING, ID_MENUSORTWRITETIME, TEXT("�޸�ʱ��(&M)"));//����ͼ�꡷�޸�ʱ��
	
	AppendMenu(hPopMenu, MF_POPUP | MF_STRING, (UINT)hPopSubSortIcon, TEXT("����ͼ��(&I)"));//���ò˵��� "����ͼ��"
	
	AppendMenu(hPopMenu, MF_STRING, ID_MENUREFRESH, TEXT("ˢ��(&E)"));//���ò˵���  "ˢ��"
		
	AppendMenu(hPopMenu, MF_STRING, ID_MENUPASTE, TEXT("ճ��(&P)"));//���ò˵���  "ճ��"	

	if (0 == lstrcmp(g_szAllPath, TEXT("")))      //����
	{
		EnableMenuItem((HMENU)hPopMenu, ID_MENUPASTE, MF_GRAYED);//��ҳ����ʾ���Ǵ�����Ϣ�����Ҽ��Ĳ˵���"ճ��"��Ч
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
		
	AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //"ճ��"��"�½������ԣ�"֮�����ָ���
	
	//�½�
	HMENU hPopSubCreateNew = CreatePopupMenu();
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWDIRECTORY, TEXT("�ļ���(&F)"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWSHORTCUT, TEXT("��ݷ�ʽ(&S)"));
	
	AppendMenu(hPopSubCreateNew, MF_SEPARATOR, NULL,NULL);       //�ָ���
	
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWBMP, TEXT("BMPͼ��"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEWORD, TEXT("Microsoft Word �ĵ�"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEPPT, TEXT("Microsoft PowerPoint ��ʾ�ĸ�"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUNEWTEXT, TEXT("�ı��ĵ�"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEWAV, TEXT("��������"));
	AppendMenu(hPopSubCreateNew, MF_STRING, ID_MENUCREATEEXCEL, TEXT("Microsoft Excel ������"));
	
	if (0 != lstrcmp(g_szAllPath, TEXT("")))      //���Ǵ���
	{
		AppendMenu(hPopMenu, MF_POPUP | MF_STRING, (UINT)hPopSubCreateNew, TEXT("�½�(&W)"));
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //�ָ���
	}
	
	AppendMenu(hPopMenu, MF_STRING, ID_MENUATTRIBUTE, TEXT("����(&R)"));
	
	int nTopIndex;
	POINT point= {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	nTopIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_GETTOPINDEX, 0, 0);
	ScreenToClient(GetDlgItem(hwnd, IDC_LISTVIEWFILE), &point);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_SETCURSEL,nTopIndex + point.y / 16, 0); // ���Ҽ�ģ����� 
	
	TrackPopupMenuEx(hPopMenu,TPM_LEFTALIGN | TPM_TOPALIGN,
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hwnd, NULL);
	DestroyMenu(hPopSubLook);
	DestroyMenu(hPopSubSortIcon);
	DestroyMenu(hPopSubCreateNew);
	DestroyMenu(hPopMenu);
}

//��8����ѡ��ĳ��ʱ�һ������Ĳ˵�
void IndexSelectedMenu(HWND hwnd, HMENU hPopMenu, LPARAM lParam)
{
	LVITEM lv;
	ZeroMemory(&lv, sizeof(LVITEM));
	
	int iIndex = SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETSELECTIONMARK, 0, 0);
	lv.mask = LVIF_PARAM;
	lv.iSubItem = 3;
	lv.iItem = iIndex;
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LVM_GETITEM, 0, (LPARAM)(LV_ITEM FAR*)&lv);
	
	if (0 == lstrcmp(g_szAllPath, TEXT("")))      //����
	{
		AppendMenu(hPopMenu, MF_STRING, ID_MENUOPEN, TEXT("��(&O)"));//��
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //�ָ���
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCREATESHORTCUT, TEXT("������ݷ�ʽ(&S)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENURENAME, TEXT("������(&M)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //�ָ���
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUATTRIBUTE, TEXT("����(&R)"));
	}
	else                  //�ļ�(��)
	{
		AppendMenu(hPopMenu, MF_STRING, ID_MENUOPEN, TEXT("��(&O)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //�ָ���
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCUT, TEXT("����(&T)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCOPY, TEXT("����(&C)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //�ָ���
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUCREATESHORTCUT, TEXT("������ݷ�ʽ(&S)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENUDELETE, TEXT("ɾ��(&D)"));
		AppendMenu(hPopMenu, MF_STRING, ID_MENURENAME, TEXT("������(&M)"));
		
		AppendMenu(hPopMenu, MF_SEPARATOR, NULL,NULL);       //�ָ���
		
		AppendMenu(hPopMenu, MF_STRING, ID_MENUATTRIBUTE, TEXT("����(&R)"));
	}
	int nTopIndex;
	POINT point={GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	nTopIndex=SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_GETTOPINDEX,0,0);
	ScreenToClient(GetDlgItem(hwnd, IDC_LISTVIEWFILE),&point);
	SendMessage(GetDlgItem(hwnd, IDC_LISTVIEWFILE), LB_SETCURSEL, nTopIndex+point.y / 16, 0); // ���Ҽ�ģ����� 
	
	TrackPopupMenuEx(hPopMenu, TPM_LEFTALIGN | TPM_TOPALIGN,
		GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), hwnd, NULL);
	DestroyMenu(hPopMenu);
}

//��9���رմ���
void ExitWindow()
{
	ImageList_Destroy(g_ListViewImageListSmall);
	ImageList_Destroy(g_ListViewImageListLarge);
	ImageList_Destroy(g_TreeViewImageList);
	FreeLibrary(g_hModule);
	PostQuitMessage(0);
}

