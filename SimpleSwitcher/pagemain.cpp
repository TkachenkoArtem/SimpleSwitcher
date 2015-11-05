#include "stdafx.h"

#include <shellapi.h>


#include "SwUtils.h"
#include "SettingsGui.h"
#include "CAutoCOM.h"
#include "SwShedule.h"
#include "SwShared.h"

#include "SwGui.h"

namespace SwGui{


void UpdateAddToTray(HWND hwnd);

void UpdateAdmin(HWND hwnd)
{
	CheckDlgButton(hwnd, IDC_CHECK_ADMIN, SettingsGlobal().isMonitorAdmin ? BST_CHECKED : BST_UNCHECKED);
}
void UpdateEnabled(HWND hwnd)
{
	TSWCheckRunRes res = CommonDataGlobal().procMonitor.CheckRunning();
	if(res.found)
	{
		if(SettingsGlobal().isMonitorAdmin != res.admin)
		{
			SW_TSTATUS_LOG(CommonDataGlobal().procMonitor.Stop());
			res = CommonDataGlobal().procMonitor.CheckRunning();
		}
	}

	SettingsGlobal().isEnabled = res.found;

	CheckDlgButton(hwnd, IDC_CHECK_ENABLE, SettingsGlobal().isEnabled ? BST_CHECKED : BST_UNCHECKED);
}
void UpdateAutoStart(HWND hwnd)
{
	bool isAdmin = false;
	bool isUser = false;
	SW_TSTATUS_LOG(CheckAutoStartUser(isUser));
	SW_TSTATUS_LOG(CheckTaskShedule(isAdmin));

	if(SettingsGlobal().isMonitorAdmin)
	{
		if(isUser)
		{
			SW_TSTATUS_LOG(RemoveWindowsRun());
		}
		SettingsGlobal().isAddToAutoStart = isAdmin;
	}
	else
	{
		// try disable admin
		if(isAdmin && IsSelfElevated())
		{
			SW_TSTATUS_LOG(RemoveTaskShedule());
			SW_TSTATUS_LOG(CheckTaskShedule(isAdmin));
		}

		SettingsGlobal().isAddToAutoStart = isAdmin || isUser;
		if(isAdmin && isUser)
		{
			SW_TSTATUS_LOG(RemoveWindowsRun());
		}

	}

	CheckDlgButton(hwnd, IDC_CHECK_AUTOSTART, SettingsGlobal().isAddToAutoStart ? BST_CHECKED : BST_UNCHECKED);
}



TStatus InitDialogPageMain(HWND hwnd)
{
	if (!IsWindowsVistaOrGreater())
	{
		SettingsGlobal().isMonitorAdmin = false;
		EnableWindow(GetDlgItem(hwnd, IDC_CHECK_ADMIN), FALSE);

	}

	UpdateAdmin(hwnd);
	UpdateEnabled(hwnd);
	UpdateAutoStart(hwnd);
	UpdateAddToTray(hwnd);

	if (SettingsGlobal().isEnabledSaved != SettingsGlobal().isEnabled)
	{
		PageMainHandleEnable();
		UpdateEnabled(hwnd);
	}

	SW_RETURN_SUCCESS;
}

TStatus ServiceOn()
{
	if (IsSelfElevated())
	{
		SW_TSTATUS_RET(CreateTaskShedule());
	}
	else
	{
		SW_TSTATUS_RET(SwCreateProcessOurWaitFinished(
			c_sArgServiceON,
			SW_BIT_32,
			SW_ADMIN_ON));
	}
	SW_RETURN_SUCCESS;
}
TStatus ServiceOff()
{
	if (IsSelfElevated())
	{
		SW_TSTATUS_RET(RemoveTaskShedule());
	}
	else
	{
		SW_TSTATUS_RET(SwCreateProcessOurWaitFinished(
			c_sArgServiceOFF,
			SW_BIT_32,
			SW_ADMIN_ON));
	}
	SW_RETURN_SUCCESS;
}
void PageMainHandleEnable()
{
	HWND hwnd = g_dlgData.hwndPageMain;
	if (SettingsGlobal().isEnabled)
	{
		SW_TSTATUS_LOG(CommonDataGlobal().procMonitor.Stop());
	}
	else
	{
		SW_TSTATUS_LOG(CommonDataGlobal().procMonitor.EnsureStarted(
			SettingsGlobal().isMonitorAdmin ? SW_ADMIN_ON : SW_ADMIN_OFF
		));
	}

	UpdateEnabled(hwnd);

	SettingsGlobal().isEnabledSaved = SettingsGlobal().isEnabled;
	SettingsGlobal().Save();
}
void UpdateAddToTray(HWND hwnd)
{
	CheckDlgButton(hwnd, IDC_CHECK_ADDTOTRAY, SettingsGlobal().isAddToTray ? BST_CHECKED : BST_UNCHECKED);
	if (SettingsGlobal().isAddToTray)
	{
		// Add a Shell_NotifyIcon notificaion
		NOTIFYICONDATA nid = { 0 };
		nid.cbSize = sizeof(nid);
		nid.uID = c_nTrayIconId;
		nid.hWnd = g_dlgData.hwndMainMenu;
		nid.uFlags = NIF_ICON | NIF_MESSAGE;
		nid.hIcon = g_dlgData.hIcon;
		nid.uCallbackMessage = WM_TRAYICON;
		SW_WINBOOL_LOG(Shell_NotifyIcon(NIM_ADD, &nid));
	}
	else
	{
		DeleteNotifyIcon();
	}
}
void HandleAutostart(HWND hwnd)
{
	bool isAdmin = false;
	bool isUser = false;
	SW_TSTATUS_LOG(CheckTaskShedule(isAdmin));
	CheckAutoStartUser(isUser);

	if(SettingsGlobal().isAddToAutoStart)
	{
		if(isUser)
		{
			SW_TSTATUS_LOG(RemoveWindowsRun());
		}
		if(isAdmin)
		{
			SW_TSTATUS_LOG(ServiceOff());
		}
	}
	else
	{
		if (SettingsGlobal().isMonitorAdmin)
		{
			if(isUser)
			{
				SW_TSTATUS_LOG(RemoveWindowsRun());
			}
			if(!isAdmin)
			{
				SW_TSTATUS_LOG(ServiceOn());
			}
		}
		else
		{
			if(!isUser)
			{
				SW_TSTATUS_LOG(AddWindowsRun());
			}
			if(isAdmin)
			{
				SW_TSTATUS_LOG(ServiceOff());
			}
		}
	}

	UpdateAutoStart(hwnd);
}

void HandleAdmin(HWND hwnd)
{
	SettingsGlobal().isMonitorAdmin = (SendDlgItemMessage(hwnd, IDC_CHECK_ADMIN, BM_GETCHECK, 0, 0) == BST_CHECKED);
	SettingsGlobal().Save();
	UpdateAdmin(hwnd);
	UpdateEnabled(hwnd);
	UpdateAutoStart(hwnd);
}


void HandleAddToTray(HWND hwnd)
{
	SettingsGlobal().isAddToTray = (SendDlgItemMessage(hwnd, IDC_CHECK_ADDTOTRAY, BM_GETCHECK, 0, 0) == BST_CHECKED);
	SettingsGlobal().Save();
	UpdateAddToTray(hwnd);
}

BOOL CALLBACK HandleWMCommand(WPARAM wParam, HWND hwnd)
{
	switch (LOWORD(wParam))
	{
	case IDC_CHECK_ENABLE:
		PageMainHandleEnable();
		return TRUE;
	case IDC_CHECK_AUTOSTART:
		HandleAutostart(hwnd);
		return TRUE;
	case IDC_CHECK_ADMIN:
		HandleAdmin(hwnd);
		return TRUE;
	case IDC_CHECK_ADDTOTRAY:
		HandleAddToTray(hwnd);
		return TRUE;
	default:
		return FALSE;
	}
}


LRESULT CALLBACK DlgProcPageMain(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_RBUTTONUP:
			ShowPopupMenu();
			return TRUE;
		case WM_INITDIALOG:
			InitDialogPageMain(hwnd);
			InitDialogPageBreak(hwnd);
			return TRUE;
		case WM_COMMAND:
			if(HandleWMCommand(wParam, hwnd))
				return TRUE;
			if(HandleWMCommandPageBreak(wParam, hwnd))
				return TRUE;
			break;
		// page break
		case WM_CTLCOLORSTATIC:
		{
			int id = GetDlgCtrlID((HWND)lParam);
			if (id == IDC_EDIT_BREAK || id == IDC_EDIT_BREAK2 || id == IDC_EDIT_BREAK3)
				return HandleCtlColor(hwnd, lParam, wParam);
			break;
		}
	}
	return FALSE;
}

















}











