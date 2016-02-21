#include "stdafx.h"

#include "SwUtils.h"
#include "Settings.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)

static HKL g_laylist[20] = {0};
static int g_laySize = 0;
void FillLayList()
{
	g_laySize = GetKeyboardLayoutList(SW_ARRAY_SIZE(g_laylist), g_laylist);
}
void FillCombo(HWND hwnd, int id, int indexSettings)
{
	HKL saved = SettingsGlobal().hkl_lay[indexSettings];
	bool found = false;
	for (int i = 0; i < g_laySize; i++)
	{
		HKL cur = g_laylist[i];
		WORD langid = LOWORD(cur);
		TCHAR buf[512]; buf[0] = 0;
		int flag = IsWindowsVistaOrGreater() ? LOCALE_SNAME : LOCALE_SLANGUAGE;
		int len = GetLocaleInfo(MAKELCID(langid, SORT_DEFAULT), flag, buf, 512);
		SW_WINBOOL_LOG(len != 0);
		SendDlgItemMessage(hwnd, id, CB_ADDSTRING, 0, (LPARAM)buf);
		if (saved == cur)
		{
			found = true;
			SendDlgItemMessage(hwnd, id, CB_SETCURSEL, (WPARAM)i, (LPARAM)0);
		}
	}
	if(!found)
	{
		SendDlgItemMessage(hwnd, id, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	

}
void ComboChanged(HWND hwnd, int id)
{
	int cur = (int)SendDlgItemMessage(hwnd, id, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	if (cur != CB_ERR)
	{
		int index = 0;
		if (id == IDC_COMBO_K2)
			index = SettingsGui::SW_HKL_1;
		else if (id == IDC_COMBO_K3)
			index = SettingsGui::SW_HKL_2;
		else if (id == IDC_COMBO_K4)
			index = SettingsGui::SW_HKL_3;
		else
			return;
		SettingsGlobal().hkl_lay[index] = g_laylist[cur];
		SettingsGlobal().Save();
		PostMsgSettingChanges();
	}
}
LRESULT CALLBACK DlgProcPageLay(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (CommonDlgPageProcess(hwnd, msg, wParam, lParam))
	{
		return TRUE;
	}

	if (msg == WM_INITDIALOG)
	{
		KeyToDlg(HotKey_ChangeLayoutCycle, IDC_EDIT_K1, hwnd);
		KeyToDlg(HotKey_ChangeSetLayout_1, IDC_EDIT_K2, hwnd);
		KeyToDlg(HotKey_ChangeSetLayout_2, IDC_EDIT_K3, hwnd);
		KeyToDlg(HotKey_ChangeSetLayout_3, IDC_EDIT_K4, hwnd);

		FillLayList();
		FillCombo(hwnd, IDC_COMBO_K2, SettingsGui::SW_HKL_1);
		FillCombo(hwnd, IDC_COMBO_K3, SettingsGui::SW_HKL_2);
		FillCombo(hwnd, IDC_COMBO_K4, SettingsGui::SW_HKL_3);

		return FALSE;
	}
	else if (msg == WM_CTLCOLORSTATIC)
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_K1 || id == IDC_EDIT_K2 || id == IDC_EDIT_K3 || id == IDC_EDIT_K4)
			return HandleCtlColor(hwnd, lParam, wParam);
		return TRUE;
	}
	else if (msg == WM_COMMAND)
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			int id = LOWORD(wParam);
			ComboChanged(hwnd, id);
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K1)
		{
			ChangeHotKey(HotKey_ChangeLayoutCycle, IDC_EDIT_K1, hwnd);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K2)
		{
			if(ChangeHotKey(HotKey_ChangeSetLayout_1, IDC_EDIT_K2, hwnd))
			{
				ComboChanged(hwnd, IDC_COMBO_K2);
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K3)
		{
			if(ChangeHotKey(HotKey_ChangeSetLayout_2, IDC_EDIT_K3, hwnd))
			{
				ComboChanged(hwnd, IDC_COMBO_K3);
			}
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_K4)
		{
			if(ChangeHotKey(HotKey_ChangeSetLayout_3, IDC_EDIT_K4, hwnd))
			{
				ComboChanged(hwnd, IDC_COMBO_K4);
			}
			return TRUE;
		}
	}
	return FALSE;
}

SW_NAMESPACE_END