#include "stdafx.h"

#include "SwGui.h"

namespace SwGui {

void InitDlgAbout(HWND hwnd)
{
	SetWindowText(hwnd, GetMessageById(AM_CaptAb));

	SetDlgItemText(hwnd, IDC_EDIT_LINK, c_sSiteLink);
	SetDlgItemText(hwnd, IDC_EDIT_LABEL, c_sProgramName);
	SendDlgItemMessage(hwnd, IDC_EDIT_LABEL,  WM_SETFONT, (WPARAM)g_dlgData.fontLabel.Get(), 0);

	{
		TChar buf[1024];
		wcscpy_s(buf, GetMessageById(AM_VERS));
		wcscat_s(buf, c_sVersion);
		wcscat_s(buf, L"\r\n");
		wcscat_s(buf, L"Date: 15.01.2015");
		SetDlgItemText(hwnd, IDC_EDIT_VERS, buf);
	}

	if (g_dlgData.hIcon)
	{
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)g_dlgData.hIcon.Get());
	}
	if (g_dlgData.hIconBig)
	{
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)g_dlgData.hIconBig.Get());
	}
}
LRESULT CALLBACK DlgProcAbout(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		InitDlgAbout(hwnd);
		return TRUE;
	case WM_COMMAND:
		if (wParam == IDCANCEL)
		{
			EndDialog(hwnd, 1);
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hwnd, 1);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}
void ShowAbout()
{
	INT_PTR ret = DialogBox(
		NULL,
		MAKEINTRESOURCE(IDD_DIALOG_ABOUT),
		g_dlgData.hwndMainMenu,
		(DLGPROC)DlgProcAbout);
	SW_WINBOOL_LOG(ret != -1);
}


}