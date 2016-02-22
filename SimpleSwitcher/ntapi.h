#pragma once
#include <winternl.h>
namespace ntapi
{

	template<typename TFnPtr>
	TFnPtr GetProcAddressEx(const char* sFnName, const char* sDllName, TFnPtr)
	{
		HMODULE hDll = GetModuleHandleA(sDllName);
		if (hDll == NULL)
		{
			UINT oldErrMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
			hDll = LoadLibraryA(sDllName);
			SetErrorMode(oldErrMode);
		}
		if (hDll == NULL) return NULL;
		return (TFnPtr)GetProcAddress(hDll, sFnName);
	}

	#define GET_PROC_ADDRESS_EX_NTDLL(FnName) GetProcAddressEx(#FnName, "ntdll.dll", FnName);
	#define GET_PROC_ADDRESS_EX_Advapi32(FnName) GetProcAddressEx(#FnName, "Advapi32.dll", FnName);
	#define GET_PROC_ADDRESS_EX_User32(FnName) GetProcAddressEx(#FnName, "User32.dll", FnName);
	#define GET_PROC_ADDRESS_EX_Kernel32(FnName) GetProcAddressEx(#FnName, "Kernel32.dll", FnName);

	BOOL
	(NTAPI * const CreateProcessWithTokenW)
		(
		_In_        HANDLE hToken,
		_In_        DWORD dwLogonFlags,
		_In_opt_    LPCWSTR lpApplicationName,
		_Inout_opt_ LPWSTR lpCommandLine,
		_In_        DWORD dwCreationFlags,
		_In_opt_    LPVOID lpEnvironment,
		_In_opt_    LPCWSTR lpCurrentDirectory,
		_In_        LPSTARTUPINFOW lpStartupInfo,
		_Out_       LPPROCESS_INFORMATION lpProcessInformation
		) = GET_PROC_ADDRESS_EX_Advapi32(CreateProcessWithTokenW);

	BOOL
		(NTAPI * const AddClipboardFormatListener)
		(_In_ HWND hwnd) = GET_PROC_ADDRESS_EX_User32(AddClipboardFormatListener);

	NTSTATUS
		(NTAPI * const NtQuerySystemInformation)
	(
		_In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
		_Inout_    PVOID SystemInformation,
		_In_       ULONG SystemInformationLength,
		_Out_opt_  PULONG ReturnLength
	) = GET_PROC_ADDRESS_EX_NTDLL(NtQuerySystemInformation);

	BOOL 
	(WINAPI * const ChangeWindowMessageFilterEx)
	(
		_In_         HWND hWnd,
		_In_         UINT message,
		_In_         DWORD action,
		_Inout_opt_  PCHANGEFILTERSTRUCT pChangeFilterStruct
	) = GET_PROC_ADDRESS_EX_User32(ChangeWindowMessageFilterEx);

	HHOOK
		(WINAPI * const SetWindowsHookEx)
		(
		_In_  int idHook,
		_In_  HOOKPROC lpfn,
		_In_  HINSTANCE hMod,
		_In_  DWORD dwThreadId
		) = GET_PROC_ADDRESS_EX_User32(SetWindowsHookExW);

	HWINEVENTHOOK
		(WINAPI * const SetWinEventHook)
		(
		_In_  UINT eventMin,
		_In_  UINT eventMax,
		_In_  HMODULE hmodWinEventProc,
		_In_  WINEVENTPROC lpfnWinEventProc,
		_In_  DWORD idProcess,
		_In_  DWORD idThread,
		_In_  UINT dwflags
		) = GET_PROC_ADDRESS_EX_User32(SetWinEventHook);

	int (WINAPI * const GetLocaleInfoEx)
		(
		_In_opt_  LPCWSTR lpLocaleName,
		_In_      LCTYPE  LCType,
		_Out_opt_ LPWSTR  lpLCData,
		_In_      int     cchData
		) = GET_PROC_ADDRESS_EX_Kernel32(GetLocaleInfoEx);

}