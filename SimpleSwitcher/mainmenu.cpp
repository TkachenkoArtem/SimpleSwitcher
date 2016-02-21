#include "stdafx.h"

#include <shellapi.h>
#include "Commctrl.h"

#include "SwUtils.h"
#include "Settings.h"
#include "CAutoCOM.h"
#include "SwShedule.h"
#include "SwShared.h"

#include "SwGui.h"

SW_NAMESPACE(SwGui)

LRESULT CALLBACK DlgProcMainMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

DialogData g_dlgData;

void DeleteNotifyIcon()
{
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize = sizeof(nid);
	nid.uID = c_nTrayIconId;
	nid.hWnd = g_dlgData.hwndMainMenu;
	SW_WINBOOL_LOG(Shell_NotifyIcon(NIM_DELETE, &nid));
}

void ExitMainMenu()
{
	if (SettingsGlobal().isAddToTray)
	{
		if (SettingsGlobal().isEnabled)
		{
			SW_TSTATUS_LOG(CommonDataGlobal().procMonitor.Stop());
		}
		DeleteNotifyIcon();
	}

	PostQuitMessage(0);
}

void HandleExitGui()
{
	if (SettingsGlobal().isAddToTray)
	{
		ShowWindow(g_dlgData.hwndMainMenu, SW_HIDE);
	}
	else
	{
		ExitMainMenu();
	}
}

void ShowGui()
{
	ShowWindow(g_dlgData.hwndMainMenu, SW_SHOW);
	SetForegroundWindow(g_dlgData.hwndMainMenu);
}
void ShowPopupMenu()
{
	HWND hwnd = g_dlgData.hwndMainMenu;

	POINT curPoint;
	GetCursorPos(&curPoint);

	MENUITEMINFO inf = { 0 };
	inf.cbSize = sizeof(inf);
	inf.fState = SettingsGlobal().isEnabled ? MF_CHECKED : MFS_UNCHECKED;
	inf.fMask = MIIM_STATE;
	SW_WINBOOL_LOG(SetMenuItemInfo(g_dlgData.hPopupMenu, ID_TRAYITEM_ENABLE, false, &inf));

	SetForegroundWindow(hwnd);

	UINT clicked = TrackPopupMenu(
		g_dlgData.hPopupMenu,
		TPM_RETURNCMD | TPM_NONOTIFY,
		curPoint.x,
		curPoint.y,
		0,
		hwnd,
		NULL);
	if (clicked == ID_TRAYITEM_EXIT)
	{
		ExitMainMenu();
	}
	else if (clicked == ID_TRAYITEM_SHOW)
	{
		ShowGui();
	}
	else if (clicked == ID_TRAYITEM_ENABLE)
	{
		PageMainHandleEnable();
	}
	else if (clicked == ID_TRAYITEM_ABOUT)
	{
		ShowAbout();
	}
}
void HandleNotifyIcon(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_RBUTTONUP:
		ShowPopupMenu();
		break;
	case WM_LBUTTONUP:
		if(IsWindowVisible(hwnd))
		{
			ShowWindow(hwnd, SW_HIDE);
		}
		else
		{
			ShowGui();
		}
		break;
	default:
		break;
	}
}
TStatus InitPage(LPCTSTR templ, DLGPROC dlgProc, HWND* hwndStorage, HWND hwndMain)
{
	HWND hwnd = CreateDialog(
		CommonDataGlobal().hInst,
		templ,
		g_dlgData.hwndMainMenu,
		dlgProc);

	SW_WINBOOL_RET(hwnd != NULL);
	*hwndStorage = hwnd;

	RECT rect;
	SW_WINBOOL_RET(GetWindowRect(hwnd, &rect));
	int widthPage = rect.right - rect.left;
	SW_WINBOOL_RET(GetClientRect(hwndMain, &rect));
	int widthMain = rect.right - rect.left;
	int x = widthMain - widthPage;
	SetWindowPos(hwnd, 0, x, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);

	SW_RETURN_SUCCESS;
}

void ShowPage(HWND hwnd)
{
	if(g_dlgData.hwndCurPageShow)
	{
		ShowWindow(g_dlgData.hwndCurPageShow, SW_HIDE);
		UpdateWindow(g_dlgData.hwndCurPageShow);
	}
	g_dlgData.hwndCurPageShow = hwnd;
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
}

TStatus InitDialogMainMenu(HWND hwnd)
{
	SW_TSTATUS_RET(g_dlgData.Init(hwnd));
	g_dlgData.hwndMainMenu = hwnd;

	SetDlgItemText(hwnd, IDC_BUTTON_CLOSE, GetMessageById(AM_CLOSE));

	{
		TChar buf[512];
		wcscpy_s(buf, c_sProgramName);
		wcscat_s(buf, L" ");
		wcscat_s(buf, c_sVersion);
		if (IsSelfElevated())
			wcscat_s(buf, L" - Administrator");
		SetWindowText(hwnd, buf);
	}

	g_dlgData.hIcon = (HICON)LoadImage(
		CommonDataGlobal().hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXICON),
		GetSystemMetrics(SM_CYICON),
		0);
	g_dlgData.hIconBig = (HICON)LoadImage(
		CommonDataGlobal().hInst,
		MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON,
		256,
		256,
		0);

	if (g_dlgData.hIcon)
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_dlgData.hIcon.Get());
	if (g_dlgData.hIconBig)
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_dlgData.hIconBig.Get());

	SW_TSTATUS_RET(InitPage(
		MAKEINTRESOURCE(GetDialogById(SD_MAINMENU)),
		(DLGPROC)DlgProcPageMain, 
		&g_dlgData.hwndPageMain, hwnd));

	SW_TSTATUS_RET(InitPage(
		MAKEINTRESOURCE(GetDialogById(SD_LAY)),
		(DLGPROC)DlgProcPageLay,
		&g_dlgData.hwndPageLay, hwnd));

	SW_TSTATUS_RET(InitPage(
		MAKEINTRESOURCE(GetDialogById(SD_ADV)),
		(DLGPROC)DlgProcPageAdv,
		&g_dlgData.hwndPageAdv, hwnd));

	SW_TSTATUS_RET(InitPage(
		MAKEINTRESOURCE(IDD_DIALOG_PAGE_LANG),
		(DLGPROC)DlgProcPageLang,
		&g_dlgData.hwndPageLang, hwnd));

	SendDlgItemMessage(hwnd, IDC_LIST_MAIN_MENU, LB_ADDSTRING, 0, (WPARAM)GetMessageById(AM_COMMON));
	SendDlgItemMessage(hwnd, IDC_LIST_MAIN_MENU, LB_ADDSTRING, 0, (WPARAM)GetMessageById(AM_2));
	SendDlgItemMessage(hwnd, IDC_LIST_MAIN_MENU, LB_ADDSTRING, 0, (WPARAM)GetMessageById(AM_3));
	SendDlgItemMessage(hwnd, IDC_LIST_MAIN_MENU, LB_ADDSTRING, 0, (WPARAM)GetMessageById(AM_LANG));

	ShowPage(g_dlgData.hwndPageMain);

	g_dlgData.hwndLeftPanel = GetDlgItem(hwnd, IDC_LIST_MAIN_MENU);

	SW_RETURN_SUCCESS;
}
BOOL CALLBACK HandleSelChange(HWND hwnd)
{
	if(g_dlgData.hwndLeftPanel == hwnd)
	{
		LRESULT res = SendMessage(hwnd, LB_GETCURSEL, 0, 0);
		if (res != LB_ERR)
		{
			if(res == 0)
			{
				ShowPage(g_dlgData.hwndPageMain);
			}
			else if(res == 1)
			{
				ShowPage(g_dlgData.hwndPageAdv);
			}
			else if (res == 2)
			{
				ShowPage(g_dlgData.hwndPageLay);
			}
			else if (res == 3)
			{
				ShowPage(g_dlgData.hwndPageLang);
			}
		}
		return TRUE;
	}
	return FALSE;
}
BOOL CALLBACK MainMenuHandleWMCommand(WPARAM wParam, HWND hwnd, LPARAM lparam)
{
	switch (LOWORD(wParam))
	{
	case IDOK:
	case IDCANCEL:
	case IDC_BUTTON_CLOSE:
		HandleExitGui();
		return TRUE;
	}

	switch (HIWORD(wParam))
	{
	case LBN_SELCHANGE:
		return HandleSelChange((HWND)lparam);
	}

	return FALSE;
}
LRESULT CALLBACK DlgProcMainMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (CommonDlgPageProcess(hwnd, msg, wParam, lParam))
	{
		return TRUE;
	}

	switch (msg)
	{
	case WM_TRAYICON:
		HandleNotifyIcon(hwnd, wParam, lParam);
		return TRUE;
	case WM_INITDIALOG:
		InitDialogMainMenu(hwnd);
		return TRUE;
	case WM_COMMAND:
		return MainMenuHandleWMCommand(wParam, hwnd, lParam);
	case WM_CLOSE:
		HandleExitGui();
		return TRUE;
	}
	return FALSE;
}
TStatus InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd = CreateDialog(
		hInstance,
		MAKEINTRESOURCE(IDD_DIALOG_MAIN_MENU),
		0,
		(DLGPROC)DlgProcMainMenu);
	SW_WINBOOL_RET(hWnd != NULL);
	g_dlgData.hwndMainMenu = hWnd;
	CommonDataGlobal().hWndGuiMain = hWnd;

	{
		CAutoWinMutexWaiter waiter(G_SwSharedMtx());
		G_SwSharedBuf().hwndGui = (TUInt64)hWnd;
	}

	if (ntapi::ChangeWindowMessageFilterEx)
	{
		SW_WINBOOL_LOG(ntapi::ChangeWindowMessageFilterEx(hWnd, c_MSG_Quit, MSGFLT_ALLOW, 0));
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SW_RETURN_SUCCESS;
}
TStatus StartCycle(_In_ HINSTANCE hInstance, _In_ int nCmdShow, int& retFromWnd)
{
	retFromWnd = 0;
	MSG msg;

	SW_TSTATUS_RET(InitInstance(hInstance, nCmdShow));

	BOOL bRet;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (msg.message == WM_SYSKEYDOWN)
		{
			int k = 0;
		}
		if (bRet == -1)
		{
			// handle the error and possibly exit
			SW_WINBOOL_RET(FALSE);
		}
		if(msg.message == c_MSG_Quit)
		{
			ExitMainMenu();
			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);	
	}

	retFromWnd = (int)msg.wParam;

	SW_RETURN_SUCCESS;
}

void InitLang()
{
	if (SettingsGlobal().idLang != SLANG_UNKNOWN)
		return;
	
	TChar sLocName[1000];
	int size = GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, sLocName, sizeof(sLocName));
	if (size > 0)
	{
		if (boost::algorithm::istarts_with(sLocName, L"ru-"))
		{
			SettingsGlobal().idLang = SLANG_RUS;
			SettingsGlobal().Save();
		}
	}
	if (SettingsGlobal().idLang == SLANG_UNKNOWN)
	{
		SettingsGlobal().idLang = SLANG_ENG;
		SettingsGlobal().Save();
	}

	InitializeLang(SettingsGlobal().idLang);
}

TStatus APIENTRY StartGui(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow)
{
	InitLang();

	SW_TSTATUS_RET(SwShared::Global().Init());

	HWND hwnd_run = NULL;
	{
		CAutoWinMutexWaiter w(G_SwSharedMtx());
		hwnd_run = (HWND)G_SwSharedBuf().hwndGui;
	}

	if (hwnd_run && nCmdShow == SW_SHOW)
	{
		PostMessage(hwnd_run, c_MSG_Quit, 0, 0);
	}

	//SettingsGlobal().Load();
	InitializeLang(SettingsGlobal().idLang);

	CAutoCOMInitialize autoCom;

	int retWnd = 0;
	TStatus stat = StartCycle(hInstance, nCmdShow, retWnd);

	{
		CAutoWinMutexWaiter w(G_SwSharedMtx());
		G_SwSharedBuf().hwndGui = NULL;
	}

	SW_TSTATUS_RET(stat);


	SW_RETURN_SUCCESS;
}

SW_NAMESPACE_END