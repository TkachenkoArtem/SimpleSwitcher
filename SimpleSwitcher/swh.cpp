#include "stdafx.h"
#include "swh.h"

HMODULE GetCurrentModule()
{
	// NB: XP+ solution!
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);

	return hModule;
}

bool IsWindows64()
{
	BOOL bIsWow64 = FALSE;

	typedef BOOL(APIENTRY *LPFN_ISWOW64PROCESS)
		(HANDLE, PBOOL);

	LPFN_ISWOW64PROCESS fnIsWow64Process;

	HMODULE module = GetModuleHandle(_T("kernel32"));
	const char funcName[] = "IsWow64Process";
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)
		GetProcAddress(module, funcName);

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),
			&bIsWow64))
			throw std::exception("Unknown error");
	}
	return bIsWow64 != FALSE;
}
