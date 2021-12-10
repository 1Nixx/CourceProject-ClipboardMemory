#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \
version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")   
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "UxTheme.lib")

#include <Windows.h>
#include <stdio.h>
#include <shobjidl.h>
#include <Winuser.h>
#include <uxtheme.h>
#include "resource.h"  
#include <commctrl.h>
#include <shlobj_core.h>
#include "Headers\ClipBoardTypes.h"
#include "Headers\ClipBoardProcessing.h"
#include "resource2.h"

#define WND_HEIGHT 430
#define WND_WIDTH 395
#define HOTKEY1  1002
#define HOTKEY_BUTTON 0x56

#define ID_TABCLASS 8

#define ID_TABCTRL 1
#define ID_LISTVIEW 2
#define BTN_EDIT 3
#define BTN_ADDTOFAV 4
#define BTN_DELITEM 5
#define BTN_DELALL 6
#define ID_EDIT 7

#define CLIPVOARD_FRM_AMOUNT 2

#define NOTIFICATION_TRAY_ICON (WM_USER + 0x100)
#define TRAY_ICON_ID 0x1234
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM 0x1238

HWND hMainWindow, hTab, hListView, hBtnEdit, hBtnAddToFav, hBtnDelItem, hBtnDelAll, hEdit;

NOTIFYICONDATA nid;
HMENU hMenu;

UINT auPriorityList[CLIPVOARD_FRM_AMOUNT] = { CF_UNICODETEXT, CF_HDROP };

HWND hWndNextViewer;

BOOL isEntryClipboardEmpty;

typedef struct _tabList {
	Node* list;
	wchar_t* filePath;
} tabList;

tabList *currentList;
tabList tabItemList[2];

LRESULT CALLBACK TabProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

BOOL ShowWindowDefault(HWND hWnd)
{
	MONITORINFO rcDesktop;
	HWND frgWnd;
	rcDesktop.cbSize = sizeof(MONITORINFO);
	frgWnd = GetForegroundWindow();
	GetMonitorInfo(MonitorFromWindow(frgWnd, MONITOR_DEFAULTTONEAREST), &rcDesktop);

	SetWindowPos(hWnd, NULL, rcDesktop.rcWork.right - WND_WIDTH - 10, rcDesktop.rcWork.bottom - WND_HEIGHT - 10, WND_WIDTH, WND_HEIGHT, NULL);

	return ShowWindow(hWnd, SW_SHOW);
}

BOOL MainWindowCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HFONT hFont;
	HWND hText;

	hFont = CreateFont(-19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tahoma");
	hText = CreateWindow(WC_STATIC, L"Буфер Обмена", WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP, 10, 10, 165, 25, hWnd, NULL, NULL, NULL);
	SendMessage(hText, WM_SETFONT, (WPARAM)hFont, 0);

	hTab = CreateWindow(WC_TABCONTROL, NULL, WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER, 8, 40, 380, 386, hWnd, ID_TABCTRL, NULL, NULL);
	hFont = CreateFont(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tahoma");
	SendMessage(hTab, WM_SETFONT, (WPARAM)hFont, 0);

	//Add Tabs to Tabs Control
	TC_ITEM tabItem;
	tabItem.mask = TCIF_TEXT | TCIF_STATE;

	int count = TabCtrl_GetItemCount(hTab);
	tabItem.pszText = L"Хранилище";
	TabCtrl_InsertItem(hTab, count, &tabItem);

	count = TabCtrl_GetItemCount(hTab);
	tabItem.pszText = L"Избранное";
	TabCtrl_InsertItem(hTab, count, &tabItem);

	hListView = CreateWindowEx(NULL, WC_LISTVIEW, NULL, WS_CHILD | WS_VISIBLE| WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL, 2, 23, 375, 230, hTab, ID_LISTVIEW, NULL, NULL);
	ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);
	SetWindowTheme(hListView, L"Explorer", NULL);

	//Add columns in ListView Control
	LVCOLUMNA tvColumn;
	tvColumn.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	tvColumn.pszText = L"Заголовок";
	tvColumn.cx = 220;
	tvColumn.iSubItem = 0;
	ListView_InsertColumn(hListView, 0, &tvColumn);

	tvColumn.pszText = L"Дата";
	tvColumn.cx = 100;
	tvColumn.iSubItem = 1;
	ListView_InsertColumn(hListView, 1, &tvColumn);

	tvColumn.pszText = L"Тип";
	tvColumn.cx = 55;
	tvColumn.iSubItem = 2;
	ListView_InsertColumn(hListView, 2, &tvColumn);

	//Add buttons
	hBtnEdit = CreateWindow(WC_BUTTON, L"Редактировать", WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, 2, 255, 90, 25, hTab, BTN_EDIT, NULL, NULL);

	hBtnAddToFav = CreateWindow(WC_BUTTON, L"В Избранное", WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, 95, 255, 75, 25, hTab, BTN_ADDTOFAV, NULL, NULL);

	hBtnDelItem = CreateWindow(WC_BUTTON, L"Удалить", WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, 173, 255, 75, 25, hTab, BTN_DELITEM, NULL, NULL);

	hBtnDelAll = CreateWindow(WC_BUTTON, L"Очистить всё", WS_CHILD | WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, 293, 255, 83, 25, hTab, BTN_DELALL, NULL, NULL);

	hEdit = CreateWindow(WC_EDIT, NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_BORDER, 3, 285, 372, 90, hTab, ID_EDIT, NULL, NULL);
	hFont = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Tahoma");
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

	SetWindowSubclass(hTab, TabProc, ID_TABCLASS, 0);

	//wchar_t *folderPath;
	//int a = SHGetKnownFolderPath(&FOLDERID_RoamingAppData, KF_FLAG_DEFAULT, NULL, &folderPath);
	
	tabItemList[1].filePath = "dataFav.bin";
	tabItemList[0].filePath = "dataReg.bin";
	tabItemList[0].list = GetListTermFromFile(tabItemList[0].filePath);

	hWndNextViewer = SetClipboardViewer(hWnd);
}

BOOL AddTrayIcon(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON_ID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uFlags = NIF_MESSAGE | NIF_ICON;
	nid.hIcon = LoadIcon(((CREATESTRUCT*)lParam)->hInstance, MAKEINTRESOURCE(IDI_ICON1));
	nid.uCallbackMessage = NOTIFICATION_TRAY_ICON;

	hMenu = CreatePopupMenu();
	InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, L"Exit");

	Shell_NotifyIcon(NIM_ADD, &nid);
}

BOOL ShowPopMenu(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT lpClickPoint;
	GetCursorPos(&lpClickPoint);

	SetForegroundWindow(hWnd);
	TrackPopupMenu(hMenu, 0, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
}

BOOL ShowControlsWnd(HWND hWndя, int tabId)
{
	switch (tabId)
	{
	case 0:
		ShowWindow(hBtnAddToFav, SW_SHOW);
		MoveWindow(hBtnDelItem, 173, 255, 75, 25, TRUE);
		break;
	case 1:
		ShowWindow(hBtnAddToFav, SW_HIDE);
		MoveWindow(hBtnDelItem, 95, 255, 75, 25, TRUE);
		break;
	}
	SetWindowText(hEdit, L"");
}

BOOL EnableControls(HWND hWnd, int tabId)
{
	EnableWindow(hBtnEdit, FALSE);
	EnableWindow(hBtnDelItem, FALSE);
	EnableWindow(hBtnAddToFav, FALSE);
	if (currentList->list == NULL)
		EnableWindow(hBtnDelAll, FALSE);
	else
		EnableWindow(hBtnDelAll, TRUE);
	EnableWindow(hEdit, FALSE);
}

BOOL UpdateListView()
{
	ListView_DeleteAllItems(hListView);

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;

	int itemIndex = 0;
	Node* currItem = currentList->list;
	while (currItem)
	{
		lvItem.iItem = itemIndex;

		lvItem.iSubItem = 0;
		lvItem.pszText = currItem->value.data;
		ListView_InsertItem(hListView, &lvItem);

		lvItem.iSubItem = 1;
		lvItem.pszText = currItem->value.dateTime;
		ListView_SetItem(hListView, &lvItem);

		lvItem.iSubItem = 2;
		lvItem.pszText = currItem->value.type;
		ListView_SetItem(hListView, &lvItem);
		
		itemIndex++;
		currItem = currItem->next;
	}
	
}

BOOL TabSelChange(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int tabId = TabCtrl_GetCurSel(hTab);

	currentList = &tabItemList[tabId];
	if (currentList->list == NULL)
		currentList->list = GetListTermFromFile(currentList->filePath);

	UpdateListView();	

	ShowControlsWnd(hWnd, tabId);
	EnableControls(hWnd, tabId);

	SetFocus(hListView);
	if (currentList->list)
		ListView_SetItemState(hListView, 0, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

BOOL TabSelChanging(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int tabId = TabCtrl_GetCurSel(((LPNMHDR)lParam)->hwndFrom);

	if (tabId == 1)
		UnloadList(&currentList->list);

	return FALSE;
}

//Attention canbe bugs with memory
BOOL BtnEditClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int textLength = GetWindowTextLength(hEdit);
	wchar_t *editText = calloc(1, textLength*sizeof(wchar_t)+ sizeof(wchar_t));
	int retValue = GetWindowText(hEdit, editText, textLength*sizeof(wchar_t));

	int selectedItem = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	if (selectedItem != -1)
	{
		CLIPBOARDDATA* dataTerm = GetTerm(&currentList->list, currentList->filePath, selectedItem + 1);
		dataTerm->data = editText;
		dataTerm->dataLength = wcslen(editText) * 2;
		EditTerm(&currentList->list, currentList->filePath, selectedItem + 1, dataTerm);
		EnableWindow(hBtnEdit, FALSE);
	}
	UpdateListView();
	SetFocus(hListView);
	if (currentList->list)
		ListView_SetItemState(hListView, selectedItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
}

BOOL BtnAddFavClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int selectedItem = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	if (selectedItem != -1)
	{
		CLIPBOARDDATA* selectedData = GetTerm(&currentList->list, currentList->filePath, selectedItem + 1);
		AddTerm(&tabItemList[1].list, tabItemList[1].filePath, selectedData);
		SetFocus(hListView);
		if (currentList->list)
			ListView_SetItemState(hListView, selectedItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}

//Fix: button enable if delete last record
BOOL BtnDelItemClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int selectedItem = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	if (selectedItem != -1)
	{
		DeleteTerm(&currentList->list, currentList->filePath, selectedItem + 1);
		UpdateListView();
		SetFocus(hListView);
		EnableControls(hWnd, 0);
		if (currentList->list)
			ListView_SetItemState(hListView, selectedItem, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}
}

BOOL BtnDelAllClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ClearTerms(&currentList->list, currentList->filePath);
	UpdateListView();
	EnableControls(hWnd, 0);
}

BOOL ListViewItemClick(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int selectedItem = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

	if (selectedItem != -1)
	{
		EnableWindow(hEdit, TRUE);
		wchar_t* editText = GetTerm(&currentList->list, currentList->filePath, selectedItem + 1)->data;
		SendMessage(hEdit, WM_SETTEXT, NULL, editText);
		//SetWindowText(hEdit, editText);
		
		EnableWindow(hBtnDelItem, TRUE);
		EnableWindow(hBtnAddToFav, TRUE);
	}
	else
	{
		EnableWindow(hEdit, FALSE);
		EnableWindow(hBtnEdit, FALSE);
		EnableWindow(hBtnDelItem, FALSE);
		EnableWindow(hBtnAddToFav, FALSE);
	}
}

BOOL ListViewItemChoosen(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int selectedItem = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
	if (selectedItem != -1)
	{
		CLIPBOARDDATA* selectedData = GetTerm(&currentList->list, currentList->filePath, selectedItem + 1);

		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, selectedData->dataLength + sizeof(wchar_t));
		LPTSTR lptstrCopy = GlobalLock(hMem);
		memcpy(lptstrCopy, selectedData->data, selectedData->dataLength);
		lptstrCopy[selectedData->dataLength / sizeof(wchar_t)] = NULL;
		GlobalUnlock(hMem);

		if (OpenClipboard(hWnd))
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMem);
			isEntryClipboardEmpty = FALSE;
			CloseClipboard();
		}
		ShowWindow(hMainWindow, SW_HIDE);
	}
}

BOOL ListViewDeleteAllItems(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (currentList->list == NULL)
		EnableWindow(hBtnDelAll, FALSE);
	else
		EnableWindow(hBtnDelAll, TRUE);
}

BOOL EditChange(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EnableWindow(hBtnEdit, TRUE);
}

BOOL WindowShow(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (CountClipboardFormats() != 0)
		isEntryClipboardEmpty = FALSE;
	else
		isEntryClipboardEmpty = TRUE;

	ChangeClipboardChain(hWnd, hWndNextViewer);

	TabCtrl_SetCurFocus(hTab, 0);
	TabSelChange(hWnd, uMsg, wParam, lParam);
}

BOOL WindowHide(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	hWndNextViewer = SetClipboardViewer(hWnd);
	ListView_DeleteAllItems(hListView); 
}

BOOL HotKeyPressed(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ShowWindowDefault(hWnd);
	SetForegroundWindow(hWnd);
}

BOOL ClipBoardChainChange(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if ((HWND)wParam == hWndNextViewer)
		hWndNextViewer = (HWND)lParam;
	else if (hWndNextViewer != NULL)
		SendMessage(hWndNextViewer, uMsg, wParam, lParam);
}

#define TIMESTRSIZE 17

void AddAdditionalInfoToClipboard(CLIPBOARDDATA* buffer, void* data, int dataLen, int dataElSize, wchar_t* typeStr)
{
	buffer->dataLength = dataLen * dataElSize;
	buffer->data = calloc(dataLen + 1, dataElSize);
	memcpy(buffer->data, data, buffer->dataLength);

	buffer->type = calloc(wcslen(typeStr) + 1, sizeof(wchar_t));
	memcpy(buffer->type, typeStr, wcslen(typeStr) * 2);

	SYSTEMTIME time;
	GetLocalTime(&time);

	wchar_t* timeStr = calloc(TIMESTRSIZE, sizeof(wchar_t));
	swprintf(timeStr, TIMESTRSIZE, L"%02d.%02d.%d %02d:%02d", time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute);

	buffer->dateTime = timeStr;
}

BOOL ClipBoardUnicodeTextRead(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HGLOBAL hglb;
	LPTSTR    lptstr;

	if (!OpenClipboard(hWnd)) return;
	hglb = GetClipboardData(CF_UNICODETEXT);
	if (hglb != NULL)
	{
		lptstr = GlobalLock(hglb);
		if (lptstr != NULL)
		{
			CLIPBOARDDATA dataToAdd;

			wchar_t* typeStr = L"Text";
			AddAdditionalInfoToClipboard(&dataToAdd, lptstr, wcslen(lptstr), sizeof(wchar_t), typeStr);

			AddTerm(&tabItemList[0].list, tabItemList[0].filePath, &dataToAdd);
			GlobalUnlock(hglb);
		}
	}
	CloseClipboard();
}

BOOL ClipBoardFileDropRead(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDROP hFile;

	if (!OpenClipboard(hWnd)) return;

	hFile = GetClipboardData(CF_HDROP);
	int fileAmount = DragQueryFile((HDROP)hFile, 0xFFFFFFFF, (LPSTR)NULL, 0);

	for (int i = 0; i < fileAmount; i++)
	{
		int pathLen = DragQueryFile(hFile, i, NULL, 0);
		wchar_t* filePathStr = calloc(pathLen + 1, sizeof(wchar_t));
		DragQueryFile((HDROP)hFile, i, filePathStr, pathLen + 1);

		CLIPBOARDDATA dataToAdd;
		wchar_t* typeStr = L"Path";
		AddAdditionalInfoToClipboard(&dataToAdd, filePathStr, wcslen(filePathStr), sizeof(wchar_t), typeStr);

		AddTerm(&tabItemList[0].list, tabItemList[0].filePath, &dataToAdd);
	}

	CloseClipboard();
}

BOOL ClipBoardDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (isEntryClipboardEmpty == FALSE)
	{
		isEntryClipboardEmpty = TRUE;
		return;
	}
	UINT uFormat = GetPriorityClipboardFormat(&auPriorityList, CLIPVOARD_FRM_AMOUNT);
	switch (uFormat)
	{
	case CF_UNICODETEXT:
		ClipBoardUnicodeTextRead(hWnd, uMsg, wParam, lParam);
		break;
	case CF_HDROP:
		ClipBoardFileDropRead(hWnd, uMsg, wParam, lParam);
		break;
	}
}

LRESULT CALLBACK TabProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case LVN_ITEMCHANGED:
			ListViewItemClick(hWnd, uMsg, wParam, lParam);
			break;
		case NM_DBLCLK:
			ListViewItemChoosen(hWnd, uMsg, wParam, lParam);
			break;
		case NM_RETURN:
			ListViewItemChoosen(hWnd, uMsg, wParam, lParam);
			break;
		case LVN_DELETEALLITEMS:
			ListViewDeleteAllItems(hWnd, uMsg, wParam, lParam);
			break;
		}
		break;
	case WM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			switch (LOWORD(wParam))
			{
			case BTN_EDIT:
				BtnEditClick(hWnd, uMsg, wParam, lParam);
				break;
			case BTN_ADDTOFAV:
				BtnAddFavClick(hWnd, uMsg, wParam, lParam);
				break;
			case BTN_DELITEM:
				BtnDelItemClick(hWnd, uMsg, wParam, lParam);
				break;
			case BTN_DELALL:
				BtnDelAllClick(hWnd, uMsg, wParam, lParam);
				break;
			}
			break;
		case EN_CHANGE:
			EditChange(hWnd, uMsg, wParam, lParam);
			break;
		}
		return 0;
	case WM_NCDESTROY:
		RemoveWindowSubclass(hWnd, TabProc, ID_TABCLASS);
		return 0;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		MainWindowCreate(hWnd, uMsg, wParam, lParam);
		AddTrayIcon(hWnd, uMsg, wParam, lParam);
		RegisterHotKey(hWnd, HOTKEY1, MOD_ALT, HOTKEY_BUTTON);
		return 0;
	case WM_NOTIFY:	
		switch (((LPNMHDR)lParam)->code)
		{
		case TCN_SELCHANGE:
			return TabSelChange(hWnd, uMsg, wParam, lParam);
		case TCN_SELCHANGING:
			return TabSelChanging(hWnd, uMsg, wParam, lParam);
		}
		break;
	case NOTIFICATION_TRAY_ICON:
		switch (lParam)
		{
		case WM_LBUTTONUP:
			HotKeyPressed(hWnd, uMsg, wParam, lParam);
			return 0;
		case WM_RBUTTONUP:
			ShowPopMenu(hWnd, uMsg, wParam, lParam);
			return 0;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TRAY_EXIT_CONTEXT_MENU_ITEM:
			SendMessage(hWnd, WM_DESTROY, NULL, NULL);
		}
	case WM_HOTKEY:
		if (LOWORD(wParam) == HOTKEY1)
			HotKeyPressed(hWnd, uMsg, wParam, lParam);
		break;
	case WM_CHANGECBCHAIN:
		ClipBoardChainChange(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_DRAWCLIPBOARD:
		ClipBoardDraw(hWnd, uMsg, wParam, lParam);
		SendMessage(hWndNextViewer, uMsg, wParam, lParam);
		break;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
			ShowWindow(hMainWindow, SW_HIDE);
		return 0;
	case WM_SHOWWINDOW:
		if (wParam == TRUE)
			WindowShow(hWnd, uMsg, wParam, lParam);
		else
			WindowHide(hWnd, uMsg, wParam, lParam);
		return 0;
	case WM_DESTROY:
		UnregisterHotKey(hWnd, HOTKEY1);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

ATOM RegisterMyClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DROPSHADOW;
	wcex.lpfnWndProc = MainWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindowClass";
	wcex.hIconSm = 0;

	return RegisterClassEx(&wcex);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);
	RegisterMyClass(hInstance);

	hMainWindow = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"MainWindowClass", L"ClipBoard", WS_POPUPWINDOW, 500, 300, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		//if (!IsDialogMessage(hTab, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}