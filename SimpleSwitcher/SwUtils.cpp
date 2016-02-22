#include "stdafx.h"
#include <Shellapi.h>

#include "SwUtils.h"
#include "SimpleSwitcher.h"


TStatus CheckAutoStartUser(bool& isAuto)
{
	isAuto = false;

	CAutoCloseHKey hg;
	SW_LSTATUS_RET(RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, 
		KEY_ALL_ACCESS,
		&hg));
	DWORD type = 0;
	if (RegQueryValueEx(hg, c_sRegRunValue, 0, &type, NULL, NULL) == ERROR_SUCCESS)
		isAuto = true;

	SW_RETURN_SUCCESS;
}
TStatus CheckService(bool& isAdmin)
{
	CAutoSCHandle hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	SW_WINBOOL_RET(hSCManager.IsValid());

	CAutoSCHandle hService = ::OpenService(hSCManager, c_sServiceName, SERVICE_QUERY_STATUS);
	isAdmin = hService.IsValid();

	SW_RETURN_SUCCESS;
}
TStatus RemoveWindowsRun()
{
	CAutoCloseHKey hg;
	SW_LSTATUS_RET(RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_ALL_ACCESS,
		&hg));
	SW_LSTATUS_RET(RegDeleteValue(hg, c_sRegRunValue));

	SW_RETURN_SUCCESS;
}

TStatus GetPath(std::wstring& sPath, TPathType type, TSWBit bit)
{
	TCHAR buf[32000];
	DWORD nSize = GetModuleFileName(NULL, buf, sizeof(buf));
	SW_WINBOOL_RET(nSize > 0);

	sPath = buf;

	if (type == PATH_TYPE_SELF_FOLDER)
	{
		size_t index = sPath.find_last_of(L"\\");
		if (index != std::string::npos)
			sPath.erase(index + 1);
	}
	if (type != PATH_TYPE_SELF_FOLDER)
	{
		size_t index = sPath.rfind(L"64.exe");
		if (index != std::string::npos)
			sPath.erase(index);
		index = sPath.rfind(L".exe");
		if (index != std::string::npos)
			sPath.erase(index);

		if (type == PATH_TYPE_DLL_NAME)
		{
			sPath += L"Hook";
		}

		if (bit == SW_BIT_64)
		{
			sPath += L"64";
		}

		if (type == PATH_TYPE_EXE_NAME)
		{
			sPath += L".exe";
		}
		else
		{
			sPath += L".dll";
		}

	}

	SW_RETURN_SUCCESS;
}

TStatus CreateService()
{
	CAutoSCHandle hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	SW_WINBOOL_RET(hSCManager.IsValid());

	CAutoSCHandle hService = ::OpenService(hSCManager, c_sServiceName, SERVICE_CHANGE_CONFIG);

	std::wstring sPath;
	GetPath(sPath, PATH_TYPE_EXE_NAME, SW_BIT_32);
	TChar buf[0x1000];
	swprintf_s(buf, L"%s %s", sPath.c_str(), c_sArgService);

	if (hService.IsInvalid())
	{
		hService = ::CreateService(
			hSCManager,
			c_sServiceName,
			c_sServiceName,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			buf,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);

		SW_WINBOOL_RET(hService.IsValid());

	}
	else
	{
		SW_WINBOOL_RET(ChangeServiceConfig(
			hService,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			buf,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			c_sServiceName));
	}

	SW_RETURN_SUCCESS;
}

TStatus AddWindowsRun()
{
	CAutoCloseHKey hg;
	SW_LSTATUS_RET(RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_ALL_ACCESS,
		&hg));

	std::wstring sPath;
	GetPath(sPath, PATH_TYPE_EXE_NAME, SW_BIT_32);
	TChar buf[0x1000];
	swprintf_s(buf, L"\"%s\" %s", sPath.c_str(), c_sArgAutostart);
	DWORD nSizeInBytes = (DWORD)(wcslen(buf) + 1) * sizeof(TCHAR);
	SW_LSTATUS_RET(RegSetValueEx(hg, c_sRegRunValue, 0, REG_SZ, (PBYTE)buf, nSizeInBytes));

	SW_RETURN_SUCCESS;
}

TStatus RemoveService()
{
	CAutoSCHandle hSCManager = ::OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	SW_WINBOOL_RET(hSCManager.IsValid());

	CAutoSCHandle hService = ::OpenService(hSCManager, c_sServiceName, DELETE);
	SW_WINBOOL_RET(hService.IsValid());
	SW_WINBOOL_RET(DeleteService(hService));

	SW_RETURN_SUCCESS;
}
TStatus GetUnElevatedToken(CAutoHandle& hToken)
{
	HWND hShellWnd = GetShellWindow();
	if(hShellWnd == NULL)
	{
		SW_TSTATUS_RET(SW_ERR_WND_NOT_FOUND, L"GetShellWindow() return NULL");
	}
	DWORD dwShellPID = 0;
	GetWindowThreadProcessId(hShellWnd, &dwShellPID);
	SW_WINBOOL_RET(dwShellPID != 0);

	CAutoHandle hShellProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwShellPID);
	SW_WINBOOL_RET(hShellProcess.IsValid());

	CAutoHandle hShellProcessToken;

	SW_WINBOOL_RET(OpenProcessToken(hShellProcess, TOKEN_DUPLICATE, &hShellProcessToken));

	const DWORD dwTokenRights = TOKEN_QUERY | TOKEN_ASSIGN_PRIMARY | TOKEN_DUPLICATE | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID;
	SW_WINBOOL_RET(DuplicateTokenEx(hShellProcessToken, dwTokenRights, NULL, SecurityImpersonation, TokenPrimary, &hToken));

	SW_RETURN_SUCCESS;
}
TStatus SwCreateProcess(SwCreateProcessParm& parm, CAutoHandle& hProc)
{
	TCHAR sExe[0x1000];
	sExe[0] = 0;

	if (parm.isOur)
	{
		std::wstring sPath;
		GetPath(sPath, PATH_TYPE_EXE_NAME, parm.bit);
		wcscpy_s(sExe, sPath.c_str());
	}
	else
	{
		wcscpy_s(sExe, parm.sExe);
	}

	TCHAR args[0x1000];
	args[0] = 0;
	if(parm.sCmd)
	{
		wcscpy_s(args, L" ");
		wcscat_s(args, parm.sCmd);
	}


	BOOL Res = FALSE;
	

	bool selfElevated = IsSelfElevated();
	CAutoHandle hToken;

	if(parm.mode == SW_CREATEPROC_DEFAULT)
	{
		if(IsWindowsVistaOrGreater())
		{
			if (parm.admin == SW_ADMIN_ON && !selfElevated)
			{
				parm.mode = SW_CREATEPROC_SHELLEXE;
			}
			else if (parm.admin == SW_ADMIN_OFF && selfElevated)
			{
				TStatus stat = GetUnElevatedToken(hToken);
				if (SW_SUCCESS(stat))
				{
					parm.hToken = hToken;
					parm.mode = SW_CREATEPROC_TOKEN;
				}
				else
				{
					SW_TSTATUS_LOG(stat);
				}
			}
		}
	}

	if (parm.mode == SW_CREATEPROC_DEFAULT)
	{
		parm.mode = SW_CREATEPROC_NORMAL;
	}

	if(parm.mode == SW_CREATEPROC_SHELLEXE)
	{
		SHELLEXECUTEINFO shExInfo = { 0 };
		shExInfo.cbSize = sizeof(shExInfo);
		shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shExInfo.hwnd = 0;
		shExInfo.lpVerb = (parm.admin == SW_ADMIN_ON) ? _T("runas") : 0;
		shExInfo.lpFile = sExe;
		shExInfo.lpParameters = args;
		shExInfo.lpDirectory = 0;
		shExInfo.nShow = parm.isHide ? SW_HIDE : SW_SHOW;
		shExInfo.hInstApp = 0;

		SW_WINBOOL_RET(ShellExecuteEx(&shExInfo) == TRUE);

		hProc = shExInfo.hProcess;
	}
	else if(parm.mode == SW_CREATEPROC_NORMAL || parm.mode == SW_CREATEPROC_AS_USER || parm.mode == SW_CREATEPROC_TOKEN)
	{
		STARTUPINFO         siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;

		ZeroMemory(&siStartupInfo, sizeof(siStartupInfo));
		ZeroMemory(&piProcessInfo, sizeof(piProcessInfo));

		siStartupInfo.cb = sizeof(siStartupInfo);
		if (parm.mode == SW_CREATEPROC_AS_USER)
		{
			
			Res = CreateProcessAsUser(
				parm.hToken,
				sExe,
				args,
				0,
				0,
				FALSE,
				CREATE_DEFAULT_ERROR_MODE,
				0,
				0,
				&siStartupInfo,
				&piProcessInfo);
		}
		else if(parm.mode == SW_CREATEPROC_NORMAL)
		{
			Res = CreateProcess(
				sExe,
				args,
				0,
				0,
				FALSE,
				CREATE_DEFAULT_ERROR_MODE,
				0,
				0,
				&siStartupInfo,
				&piProcessInfo);
		}
		else if(parm.mode == SW_CREATEPROC_TOKEN)
		{
			Res = ntapi::CreateProcessWithTokenW(
				parm.hToken,
				0,
				sExe,
				args,
				0,
				NULL,
				NULL,
				&siStartupInfo,
				&piProcessInfo);
		}
		SW_WINBOOL_RET(Res, L"Cant create proc %s %s", sExe, args);

		CloseHandle(piProcessInfo.hThread);
		hProc = piProcessInfo.hProcess;

	}
	SW_RETURN_SUCCESS;
}







