#pragma once

#include "CAutoCleanupGUI.h"
#include "SimpleSwitcher.h"
#include "SettingsGui.h"

namespace SwGui
{
	TStatus APIENTRY StartGui(
		_In_ HINSTANCE hInstance,
		_In_opt_ HINSTANCE hPrevInstance,
		_In_ LPTSTR lpCmdLine,
		_In_ int nCmdShow);

	void ShowAbout();
	void PageMainHandleEnable();
	LRESULT CALLBACK DlgProcPageMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void ShowPopupMenu();
	void DeleteNotifyIcon();
	LRESULT CALLBACK HandleCtlColor(HWND hwnd, LPARAM lParam, WPARAM wParam);
	LRESULT CALLBACK DlgProcHotKey(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	TStatus InitDialogPageBreak(HWND hwnd);
	BOOL CALLBACK HandleWMCommandPageBreak(WPARAM wParam, HWND hwnd);
	void HandleExitGui();
	LRESULT CALLBACK DlgProcPageAdv(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK DlgProcPageLay(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



	const static int c_nTrayIconId = 6332;
	const static int WM_TRAYICON = WM_USER + 100;
	const static int ID_TRAYITEM_EXIT = 3000;
	const static int ID_TRAYITEM_SHOW = 3001;
	const static int ID_TRAYITEM_ENABLE = 3002;
	const static int ID_TRAYITEM_ABOUT = 3003;

	struct DlgHotKeyData
	{
		CHotKey keyRevert;
		CHotKey keyDefault;

		bool fOk;

		WNDPROC oldEditBreakProc = NULL;
		HWND hwndEditRevert = NULL;
		CAutoHHOOK hHookKeyGlobal;
		HWND hwnd;

		bool fAltDisable = true;

	};
	bool ChangeHotKey(HotKeyType type, int dlgId, HWND hwnd, bool fAltDisable=true);

	inline void KeyToDlg(CHotKey key, int dlgId, HWND hwnd)
	{
		std::wstring sKeyRevert;
		key.ToString(sKeyRevert);
		SetDlgItemText(hwnd, dlgId, sKeyRevert.c_str());
	}
	inline void KeyToDlg(HotKeyType type, int dlgId, HWND hwnd)
	{
		std::wstring sKeyRevert;
		CHotKey key = SettingsGlobal().hotKeys[type].key;
		key.ToString(sKeyRevert);
		SetDlgItemText(hwnd, dlgId, sKeyRevert.c_str());
	}

	struct DialogData
	{
		CAutoCloseHBRUSH hEditBrush;
		const static COLORREF colEditBg = 0xffffff;
		const static COLORREF colEditTxt = 0x000000;
		TStatus Init(HWND hwnd)
		{
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = colEditBg;
			lb.lbHatch = 0;
			hEditBrush = CreateBrushIndirect(&lb);
			SW_WINBOOL_RET(hEditBrush.IsValid());

			hPopupMenu = CreatePopupMenu();
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAYITEM_SHOW, TEXT("Показать"));
			AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);
			AppendMenu(hPopupMenu, MF_CHECKED, ID_TRAYITEM_ENABLE, TEXT("Включить"));
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAYITEM_ABOUT, TEXT("О программе"));
			AppendMenu(hPopupMenu, MF_SEPARATOR, 0, 0);
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAYITEM_EXIT, TEXT("Выход"));

			{
				NONCLIENTMETRICS metrics;
				metrics.cbSize = sizeof(metrics);
				::SystemParametersInfo(
					SPI_GETNONCLIENTMETRICS,
					sizeof(metrics),
					&metrics, 0);
				LOGFONTW font = metrics.lfMessageFont;
				font.lfHeight = -20;
				fontLabel = ::CreateFontIndirect(&font);
			}

			SW_RETURN_SUCCESS;
		}
		HWND hwndLeftPanel = NULL;

		HWND hwndMainMenu = NULL;
		HWND hwndPageMain = NULL;
		HWND hwndPageBreak = NULL;
		HWND hwndPageAdv = NULL;
		HWND hwndPageLay = NULL;

		HWND hwndCurPageShow = NULL;

		CAutoHandleIcon hIcon;
		CAutoHandleIcon hIconBig;
		CAutoHMENU hPopupMenu;
		CAutoHFONT fontLabel;
	};

	extern DialogData g_dlgData;

}