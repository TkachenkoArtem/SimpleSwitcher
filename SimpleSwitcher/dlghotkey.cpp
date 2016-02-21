#include "stdafx.h"

#include "SwUtils.h"
#include "Settings.h"
#include "SwShared.h"

#include "SwGui.h"

SW_NAMESPACE(SwGui)

static DlgHotKeyData* g_hkdata = NULL;
static const int c_radioStart = IDC_RADIO_CLEAR;
static const int c_radioEnd = IDC_RADIO_TYPE;

LRESULT CALLBACK EditBreakProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam);


static void SetRadio(int id, HWND hwnd)
{
	CheckRadioButton(hwnd, c_radioStart, c_radioEnd, id);
}

LRESULT CALLBACK LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
		DWORD vkKey = kStruct->vkCode;
		//if (GetLogLevel() >= LOG_LEVEL_1)
		//{
		//	KeyState keyState = GetKeyState(wParam);
		//	SW_LOG_INFO_2(L"%S 0x%x", GetKeyStateName(keyState), vkKey);
		//}

		PostMessage(g_hkdata->hwndEditRevert, c_MSG_TypeHotKey, wParam, (WPARAM)vkKey);
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
}
void InitDialogHotKey(HWND hwnd, LPARAM lparam)
{
	SW_TSTATUS_LOG(CenterWindow(g_dlgData.hwndMainMenu, hwnd));

	g_hkdata = (DlgHotKeyData*)lparam;

	g_hkdata->hwnd = hwnd;

	std::wstring sKeyRevert;
	g_hkdata->keyDefault.ToString(sKeyRevert);
	SetDlgItemText(hwnd, IDC_RADIO_DEFAULT, sKeyRevert.c_str());

	KeyToDlg(g_hkdata->keyRevert, IDC_EDIT_BREAK, hwnd);

	g_hkdata->oldEditBreakProc = (WNDPROC)SetWindowLongPtr(
		GetDlgItem(hwnd, IDC_EDIT_BREAK),
		GWLP_WNDPROC,
		(LONG_PTR)EditBreakProc);

	SW_WINBOOL_LOG(g_hkdata->oldEditBreakProc != NULL);
	g_hkdata->hwndEditRevert = GetDlgItem(hwnd, IDC_EDIT_BREAK);

	
	if(g_hkdata->keyRevert == g_hkdata->keyDefault)
	{
		SetRadio(IDC_RADIO_DEFAULT, hwnd);
	}
	else if (g_hkdata->keyRevert.IsEmpty())
	{
		SetRadio(IDC_RADIO_CLEAR, hwnd);
	}
	else
	{
		SetRadio(IDC_RADIO_TYPE, hwnd);
	}

	CheckDlgButton(hwnd, IDC_CHECK_LEFTRIGHT, g_hkdata->keyRevert.GetLeftRightMode() ? BST_CHECKED : BST_UNCHECKED);
	SetFocus(GetDlgItem(hwnd, IDC_EDIT_BREAK));
	

	CAutoHMODULE hModUser;
	if (!IsWindowsVistaOrGreater())
	{
		hModUser = LoadLibrary(L"user32.dll");
	}
	g_hkdata->hHookKeyGlobal = ntapi::SetWindowsHookEx(WH_KEYBOARD_LL, &LowLevelKeyboardProc, hModUser, 0);
	SW_WINBOOL_LOG(g_hkdata->hHookKeyGlobal.IsValid());
}

void HandleExit(HWND hwnd, bool fOk)
{
	g_hkdata->fOk = fOk;
	g_hkdata->hHookKeyGlobal.Cleanup();

	EndDialog(hwnd, 0);
}

BOOL CALLBACK HandleWMCommandHotKey(WPARAM wParam, HWND hwnd)
{
	if (HIWORD(wParam) == BN_CLICKED)
	{
		switch (LOWORD(wParam))
		{
		case IDC_RADIO_CLEAR:
			g_hkdata->keyRevert.Clear();
			KeyToDlg(g_hkdata->keyRevert, IDC_EDIT_BREAK, hwnd);
			return TRUE;
		case IDC_RADIO_DEFAULT:
			g_hkdata->keyRevert = g_hkdata->keyDefault;
			KeyToDlg(g_hkdata->keyRevert, IDC_EDIT_BREAK, hwnd);
			CheckDlgButton(hwnd, IDC_CHECK_LEFTRIGHT, g_hkdata->keyRevert.GetLeftRightMode() ? BST_CHECKED : BST_UNCHECKED);
			return TRUE;
		case IDC_RADIO_TYPE:
			g_hkdata->keyRevert.Clear();
			SetDlgItemText(hwnd, IDC_EDIT_BREAK, L"Press hotkey ...");
			return TRUE;
		case IDC_BUTTON_OK:
			HandleExit(hwnd, true);
			return TRUE;
		case IDC_BUTTON_CANCEL:
			HandleExit(hwnd, false);
			return TRUE;
		case IDC_CHECK_LEFTRIGHT:
			g_hkdata->keyRevert.SetLeftRightMode(SendDlgItemMessage(hwnd, IDC_CHECK_LEFTRIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED);
			KeyToDlg(g_hkdata->keyRevert, IDC_EDIT_BREAK, g_hkdata->hwnd);
			break;
		}
	}

	return FALSE;
}
LRESULT CALLBACK HandleCtlColor(HWND hwnd, LPARAM lParam, WPARAM wParam)
{
	HDC hdcEdit = (HDC)wParam;
	SetTextColor(hdcEdit, g_dlgData.colEditTxt); // Text color
	SetBkColor(hdcEdit, g_dlgData.colEditBg);
	return (LRESULT)g_dlgData.hEditBrush.Get();
}

LRESULT CALLBACK EditBreakProc(HWND hEdit, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == c_MSG_TypeHotKey)
	{
		TKeyCode vkCode = (TKeyCode)lParam;
		KeyState state = GetKeyState(wParam);
		if (state != KEY_STATE_DOWN)
			return 0;
		if(CHotKey::Normalize(vkCode) == VK_MENU && g_hkdata->fAltDisable)
		{
			MessageBox(g_hkdata->hwnd, GetMessageById(AM_4), SW_PROGRAM_NAME_L, MB_ICONINFORMATION);
		}
		else
		{
			if (g_hkdata->keyRevert.HasKey(vkCode, true))
			{
				g_hkdata->keyRevert.Remove(vkCode);
			}
			else
			{
				g_hkdata->keyRevert.Add(vkCode, CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
			}
			SetRadio(IDC_RADIO_TYPE, g_hkdata->hwnd);
			g_hkdata->keyRevert.SetLeftRightMode(SendDlgItemMessage(g_hkdata->hwnd, IDC_CHECK_LEFTRIGHT, BM_GETCHECK, 0, 0) == BST_CHECKED);
			KeyToDlg(g_hkdata->keyRevert, IDC_EDIT_BREAK, g_hkdata->hwnd);

		}
		return 0;
	}
	return CallWindowProc(g_hkdata->oldEditBreakProc, hEdit, msg, wParam, lParam);
}

LRESULT CALLBACK DlgProcHotKey(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CTLCOLORSTATIC:
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_BREAK)
			return HandleCtlColor(hwnd, lParam, wParam);
		break;
	}
	case WM_INITDIALOG:
		InitDialogHotKey(hwnd, lParam);
		return FALSE;
	case WM_COMMAND:
		return HandleWMCommandHotKey(wParam, hwnd);
	}
	return FALSE;
}
bool ChangeHotKey(HotKeyType type, int dlgId, HWND hwnd, bool fAltDisable)
{
	DlgHotKeyData data;
	data.keyRevert = SettingsGlobal().hotKeys[type].key;
	data.keyDefault = SettingsGlobal().hotKeys[type].def;
	data.fAltDisable = fAltDisable;
	DialogBoxParam(
		CommonDataGlobal().hInst,
		MAKEINTRESOURCE(GetDialogById(SD_HOTKEY)),
		CommonDataGlobal().hWndGuiMain,
		(DLGPROC)DlgProcHotKey,
		(LPARAM)&data);
	if (data.fOk)
	{
		SettingsGlobal().hotKeys[type].key = data.keyRevert;
		KeyToDlg(data.keyRevert, dlgId, hwnd);
		SettingsGlobal().SaveAndPostMsg();
		return true;
	}
	return false;
}

SW_NAMESPACE_END