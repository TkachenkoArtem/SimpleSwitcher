#include "stdafx.h"
#include "utils-func.h"

TStatus IsElevated(HANDLE hProc, bool& res)
{
	res = false;

	if (!IsWindowsVistaOrGreater())
		SW_RETURN_SUCCESS;

	if (!hProc)
		SW_TSTATUS_RET(SW_ERR_INVALID_PARAMETR);

	CAutoHandle hToken;
	SW_WINBOOL_RET(OpenProcessToken(hProc, TOKEN_QUERY, &hToken));

	TOKEN_ELEVATION Elevation;
	DWORD cbSize = sizeof(TOKEN_ELEVATION);
	SW_WINBOOL_RET(GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize));
	res = Elevation.TokenIsElevated != 0;

	SW_RETURN_SUCCESS;
}

bool IsElevated(DWORD procId)
{
	if(!IsWindowsVistaOrGreater())
		return false;

	CAutoHandle hProc = OpenProcess(
		PROCESS_QUERY_LIMITED_INFORMATION,
		FALSE,
		procId);
	if (hProc.IsValid())
		return IsElevated(hProc);
	else
	{
		SW_WINBOOL_LOG(FALSE);
		return false;
	}
}

TStatus GetFocusWindowByAttach(HWND& hwnd, HWND top)
{
	DWORD procId;
	DWORD activeThreadId = GetWindowThreadProcessId(top, &procId);
	SW_WINBOOL_RET(AttachThreadInput(activeThreadId, GetCurrentThreadId(), TRUE));
	hwnd = GetFocus();
	SW_WINBOOL_RET(AttachThreadInput(activeThreadId, GetCurrentThreadId(), FALSE));

	SW_RETURN_SUCCESS;
}

TStatus GetTokenNormalUser(CAutoHandle& token)
{
	// todo
	SW_RETURN_SUCCESS;
}
