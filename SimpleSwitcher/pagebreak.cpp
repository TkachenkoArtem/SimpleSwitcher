#include "stdafx.h"

#include <shellapi.h>

#include "SwUtils.h"
#include "Settings.h"
#include "CAutoCOM.h"
#include "SwShedule.h"
#include "SwShared.h"

#include "SwGui.h"

SW_NAMESPACE(SwGui)


TStatus InitDialogPageBreak(HWND hwnd)
{
	KeyToDlg(HotKey_Revert, IDC_EDIT_BREAK, hwnd);
	KeyToDlg(HotKey_RevertCycle, IDC_EDIT_BREAK2, hwnd);
	KeyToDlg(Hotkey_RevertSel, IDC_EDIT_BREAK3, hwnd);

	SW_RETURN_SUCCESS;
}

BOOL CALLBACK HandleWMCommandPageBreak(WPARAM wParam, HWND hwnd)
{
	switch (LOWORD(wParam))
	{
	case IDC_BUTTON_SET:
		ChangeHotKey(HotKey_Revert, IDC_EDIT_BREAK, hwnd);
		return TRUE;
	case IDC_BUTTON_SET2:
		ChangeHotKey(HotKey_RevertCycle, IDC_EDIT_BREAK2, hwnd);
		return TRUE;
	case IDC_BUTTON_SET3:
		ChangeHotKey(Hotkey_RevertSel, IDC_EDIT_BREAK3, hwnd);
		return TRUE;

	default:
		return FALSE;
	}
}

SW_NAMESPACE_END