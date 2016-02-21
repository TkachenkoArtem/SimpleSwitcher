#include "stdafx.h"

#include "SwUtils.h"
#include "Settings.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)

void LangChanged(HWND hwnd, int id)
{
	int cur = (int)SendDlgItemMessage(hwnd, id, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (cur != CB_ERR)
	{
		SettingsGlobal().idLang = (SwLang)cur;
		InitializeLang(SettingsGlobal().idLang);
		SettingsGlobal().Save();
		int res = MessageBox(hwnd, GetMessageById(AM_NEED_REST), SW_PROGRAM_NAME_L, MB_ICONINFORMATION | MB_OKCANCEL);
		if (res == IDOK)
		{
			PostQuitMessage(0);
			CAutoHandle hProc32;
			SW_TSTATUS_LOG(SwCreateProcessOur(0, SW_BIT_32, SW_ADMIN_SELF, hProc32));
		}
	}
}

LRESULT CALLBACK DlgProcPageLang(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (CommonDlgPageProcess(hwnd, msg, wParam, lParam))
	{
		return TRUE;
	}

	if (msg == WM_INITDIALOG)
	{
		SendDlgItemMessage(hwnd, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)GetMessageById(AM_RUS));
		SendDlgItemMessage(hwnd, IDC_COMBO_LANG, CB_ADDSTRING, 0, (LPARAM)GetMessageById(AM_ENG));

		SendDlgItemMessage(hwnd, IDC_COMBO_LANG, CB_SETCURSEL, (WPARAM)SettingsGui::Global().idLang, (LPARAM)0);

		CheckDlgButton(hwnd, IDC_CHECK_DBG_MODE, SettingsGlobal().fDbgMode ? BST_CHECKED : BST_UNCHECKED);

		return FALSE;
	}
	else if (msg == WM_COMMAND)
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int id = LOWORD(wParam);
			LangChanged(hwnd, id);
		}
		else if (LOWORD(wParam) == IDC_CHECK_DBG_MODE)
		{
			SettingsGlobal().fDbgMode = (SendDlgItemMessage(hwnd, IDC_CHECK_DBG_MODE, BM_GETCHECK, 0, 0) == BST_CHECKED);
			SettingsGlobal().SaveAndPostMsg();
			SettingsGlobal().SetLogLevelBySettings();
		}
	}
	return FALSE;
}

SW_NAMESPACE_END