#include <Windows.h>
#include <shobjidl.h>
#include "MainWindow.h"

#define WND_HEIGHT 430
#define WND_WIDTH 395
#define HOTKEY1  1002
#define HOTKEY_BUTTON 0x5A
#define NOTIFICATION_TRAY_ICON (WM_USER + 0x100)
#define TRAY_ICON_ID                     0x1234
#define ID_TRAY_EXIT_CONTEXT_MENU_ITEM      0x1238

HWND frgWnd, frgWnd2;
HMENU hMenu;

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

void AddTrayIcon(HWND hWnd) {
	NOTIFYICONDATA  nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = TRAY_ICON_ID;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uFlags = NIF_MESSAGE;
	nid.uCallbackMessage = NOTIFICATION_TRAY_ICON;

	Shell_NotifyIcon(NIM_ADD, &nid);
}

void AddPopIcon(HWND hWnd)
{
	POINT lpClickPoint;
	GetCursorPos(&lpClickPoint);

	hMenu = CreatePopupMenu();
	InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, ID_TRAY_EXIT_CONTEXT_MENU_ITEM, L"Exit");
	SetForegroundWindow(hWnd);
	TrackPopupMenu(hMenu,0, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int res;
	switch (uMsg)
	{
	case WM_CREATE:
		SetClipboardViewer(hWnd);
		AddTrayIcon(hWnd);
		RegisterHotKey(hWnd, HOTKEY1, MOD_SHIFT, HOTKEY_BUTTON);
		return 0;
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			ShowWindow(hWnd, SW_HIDE);
			//ShowWindow(frgWnd2, SW_SHOW);
		}
		return 0;
	case WM_HOTKEY:
		if (LOWORD(wParam) == HOTKEY1)
		{
			ShowWindowDefault(hWnd);
			SetForegroundWindow(hWnd);
		}
		break;
	case WM_DESTROY:
		UnregisterHotKey(hWnd, HOTKEY1);
		PostQuitMessage(0);
		return 0;
	case NOTIFICATION_TRAY_ICON:
		switch (lParam)
		{
		case WM_LBUTTONUP:
			ShowWindowDefault(hWnd);
			SetForegroundWindow(hWnd);
			return 0;
		case WM_RBUTTONUP:
			AddPopIcon(hWnd);
			return 0;
		}

		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TRAY_EXIT_CONTEXT_MENU_ITEM:
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		}
		
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DROPSHADOW;
	wcex.lpfnWndProc = MainWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"MainWindowClass";
	wcex.hIconSm = 0;

	if (!RegisterClassEx(&wcex))
	{
		return 0;
	}

	HWND hMainWindow = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, L"MainWindowClass", L"ClipBoard", WS_POPUPWINDOW | WS_VISIBLE, 100, 100, WND_WIDTH, WND_HEIGHT, NULL, NULL, hInstance, NULL);
	//SetClipboardViewer(hMainWindow);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) 
	{
		DispatchMessage(&msg);
	}

	return 0;

}

