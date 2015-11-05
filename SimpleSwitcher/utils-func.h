#pragma once

TStatus IsElevated(HANDLE hProc, bool& res);

inline bool IsElevated(HANDLE hProc)
{
	bool fRet;
	SW_TSTATUS_LOG(IsElevated(hProc, fRet));

	return fRet;
}

TStatus GetTokenNormalUser(CAutoHandle& token);

bool IsElevated(DWORD procId);

inline bool IsSelfElevated()
{
	return IsElevated(GetCurrentProcess());
}

TStatus GetFocusWindowByAttach(HWND& hwnd, HWND top);

inline TStatus GetFocusWindow(HWND& hwndFocused)
{
	hwndFocused = NULL;
	GUITHREADINFO gui = { 0 };
	gui.cbSize = sizeof(gui);
	SW_WINBOOL_LOG(GetGUIThreadInfo(0, &gui));

	hwndFocused = gui.hwndFocus;

	if (hwndFocused == NULL)
		hwndFocused = GetForegroundWindow();

	if (hwndFocused == NULL)
	{
		SW_TSTATUS_RET(SW_ERR_WND_NOT_FOUND, L"Cant found focused window");
	}

	SW_RETURN_SUCCESS;

}
inline TStatus CenterWindow(HWND hwndMain, HWND hwndClient)
{
	RECT rect;
	SW_WINBOOL_RET(GetWindowRect(hwndClient, &rect));
	int wDlg = rect.right - rect.left;
	int hDlg = rect.bottom - rect.top;
	SW_WINBOOL_RET(GetWindowRect(hwndMain, &rect));
	int wMain = rect.right - rect.left;
	int hMain = rect.bottom - rect.top;
	int widthMain = rect.right - rect.left;
	int x = (wMain - wDlg) / 2;
	int y = (hMain - hDlg) / 2;
	SetWindowPos(hwndClient, 0, x + rect.left, y + rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
	SW_RETURN_SUCCESS;
}
inline TUInt64 GetBootTime()
{
	typedef struct _SYSTEM_TIME_OF_DAY_INFORMATION
	{
		LARGE_INTEGER BootTime;
		LARGE_INTEGER CurrentTime;
		LARGE_INTEGER TimeZoneBias;
		ULONG CurrentTimeZoneId;
	} SYSTEM_TIME_OF_DAY_INFORMATION, *PSYSTEM_TIME_OF_DAY_INFORMATION;

	SYSTEM_TIME_OF_DAY_INFORMATION SysTimeInfo;

	TUInt64 res = 0;

	__try
	{

		NTSTATUS stat = ntapi::NtQuerySystemInformation(
			SystemTimeOfDayInformation,
			&SysTimeInfo,
			sizeof(SysTimeInfo),
			0);
		if(NT_SUCCESS(stat))
		{
			res = SysTimeInfo.BootTime.QuadPart;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		
	}
	return res;
}




