#include "stdafx.h"

#include "SwUtils.h"
#include "SettingsGui.h"

#include "SwGui.h"

#include <string>        
#include <locale>
#include <algorithm>

SW_NAMESPACE(SwGui)

void HandleCapsRemap(HWND hwnd)
{
	bool check = (SendDlgItemMessage(hwnd, IDC_CHECK_CAPSREMAP, BM_GETCHECK, 0, 0) == BST_CHECKED);
	if(check)
	{
		BufScanMap etalon;
		etalon.GenerateCapsBytesRemap();

		BufScanMap readed;
		readed.FromRegistry();

		if(etalon != readed)
		{
			if(readed.size > 0)
			{
				MessageBox(hwnd, L"Section 'Scancode Map' already exists", L"Error", MB_ICONERROR);
				return;
			}
			if(IsSelfElevated())
			{
				SW_TSTATUS_LOG(etalon.ToRegistry());
			}
			else
			{
				SW_TSTATUS_LOG(SwCreateProcessOurWaitFinished(
					c_sArgCapsRemapAdd,
					SW_BIT_32,
					SW_ADMIN_ON));
			}
		}
	}
	else
	{
		if (IsSelfElevated())
		{
			SW_TSTATUS_LOG(BufScanMap::RemoveRegistry());
		}
		else
		{
			SW_TSTATUS_LOG(SwCreateProcessOurWaitFinished(
				c_sArgCapsRemapRemove,
				SW_BIT_32,
				SW_ADMIN_ON));
		}
	}
}
void UpdateRemapApply()
{
	TUInt64 curTime = GetBootTime();
	BufScanMap buf;
	buf.FromRegistry();
	TKeyCode inreg = buf.RemapedKey();
	TKeyCode saved = SettingsGlobal().capsRemapApply;
	if ((curTime != SettingsGlobal().bootTime && saved != inreg) || curTime == 0)
	{
		SettingsGlobal().capsRemapApply = buf.RemapedKey();
		SettingsGlobal().bootTime = curTime;
		SettingsGlobal().Save();
	}
}
void UpdateCapsRemap(HWND hwnd)
{
	GetBootTime();
	BufScanMap buf;
	buf.FromRegistry();
	std::wstring capsVal;
	capsVal += L"CapsLock = ";
	std::wstring capsNewName;
	CHotKey::ToString(buf.RemapedKey(), capsNewName);
	capsVal += capsNewName;
	if (SettingsGlobal().capsRemapApply != buf.RemapedKey())
	{
		capsVal += L" (windows requires a reboot)";
	}
	SetDlgItemText(hwnd, IDC_STATIC_CAPSVAL, capsVal.c_str());
	BufScanMap etalon;
	etalon.GenerateCapsBytesRemap();
	CheckDlgButton(hwnd, IDC_CHECK_CAPSREMAP, buf == etalon ? BST_CHECKED : BST_UNCHECKED);
}
LRESULT CALLBACK DlgProcPageAdv(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG)
	{
		KeyToDlg(HotKey_RevertAdv, IDC_EDIT_BREAK, hwnd);
		KeyToDlg(HotKey_RevertCycleAdv, IDC_EDIT_BREAK2, hwnd);
		KeyToDlg(HotKey_CapsGenerate, IDC_EDIT_BREAK3, hwnd);

		CheckDlgButton(hwnd, IDC_CHECK_OEM2, SettingsGlobal().isTryOEM2 ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hwnd, IDC_CHECK_DASH, SettingsGlobal().isDashSeparate ? BST_CHECKED : BST_UNCHECKED);

		UpdateRemapApply();
		UpdateCapsRemap(hwnd);

		return FALSE;
	}
	else if(msg == WM_CTLCOLORSTATIC)
	{
		int id = GetDlgCtrlID((HWND)lParam);
		if (id == IDC_EDIT_BREAK || id == IDC_EDIT_BREAK2 || id == IDC_EDIT_BREAK3 || id == IDC_EDIT_BREAK4 )
			return HandleCtlColor(hwnd, lParam, wParam);
		return TRUE;
	}
	else if (msg == WM_COMMAND)
	{
		if (LOWORD(wParam) == IDC_BUTTON_SET)
		{
			ChangeHotKey(HotKey_RevertAdv, IDC_EDIT_BREAK, hwnd);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_SET2)
		{
			ChangeHotKey(HotKey_RevertCycleAdv, IDC_EDIT_BREAK2, hwnd);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_BUTTON_SET3)
		{
			ChangeHotKey(HotKey_CapsGenerate, IDC_EDIT_BREAK3, hwnd, false);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_CHECK_CAPSREMAP)
		{
			HandleCapsRemap(hwnd);
			UpdateCapsRemap(hwnd);
			return TRUE;
		}
		else if (LOWORD(wParam) == IDC_CHECK_DASH)
		{
				SettingsGlobal().isDashSeparate = (SendDlgItemMessage(hwnd, IDC_CHECK_DASH, BM_GETCHECK, 0, 0) == BST_CHECKED);
				SettingsGlobal().SaveAndPostMsg();
		}
		else if (LOWORD(wParam) == IDC_CHECK_OEM2)
		{
			SettingsGlobal().isTryOEM2 = (SendDlgItemMessage(hwnd, IDC_CHECK_OEM2, BM_GETCHECK, 0, 0) == BST_CHECKED);
			SettingsGlobal().SaveAndPostMsg();
		}
	}
	return FALSE;
}

SW_NAMESPACE_END