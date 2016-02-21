// SimpleLayout.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "SimpleSwitcher.h"

#include "Hooker.h"
#include "SwUtils.h"
#include "Settings.h"
#include "CAutoCleanupWin.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct HotKeyHold
{
	CAutoHotKey autoHot;
	CHotKey saved;
};
struct HotKeyHolders
{
	HotKeyHold holders[HotKey_SIZE];
};
TStatus SetHotKey(CHotKey key, CAutoHotKey& autokey, int idHotKey)
{
	if(key.IsEmpty())
	{
		autokey.Cleanup();
	}
	else
	{
		UINT fsModifiers = 0;
		std::wstring s;
		key.ToString(s);
		SW_LOG_INFO_1(L"Set hotkey %s", s.c_str());
		if (key.HasMod(VK_MENU))
			fsModifiers |= MOD_ALT;
		if (key.HasMod(VK_CONTROL))
			fsModifiers |= MOD_CONTROL;
		if (key.HasMod(VK_SHIFT))
			fsModifiers |= MOD_SHIFT;
		SW_WINBOOL_RET(autokey.Register(CommonDataGlobal().hWndMonitor, idHotKey, fsModifiers, key.ValueKey()));
	}
	SW_RETURN_SUCCESS;
}
TStatus ResetAllHotKey(HotKeyHolders& keys)
{
	for(int i = 0; i < HotKey_SIZE; ++i)
	{
		HotKeyHold& hold =  keys.holders[i];
		CHotKeySet& set = SettingsGlobal().hotKeys[i];
		if(hold.saved != set.key)
		{
			TStatus stat = SetHotKey(set.key, hold.autoHot, i);
			if (SW_SUCCESS(stat))
			{
				hold.saved = set.key;
			}
		}
	}

	SW_RETURN_SUCCESS;
}


TStatus StartCycle(_In_ HINSTANCE hInstance,_In_ int nCmdShow)
{

	WNDCLASSEX wcex = { 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = DefWindowProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = c_sClassName;

	SW_WINBOOL_RET(RegisterClassEx(&wcex) != 0);

	HWND hWnd = CreateWindow(
		c_sClassName,
		L"Title",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	SW_WINBOOL_RET(hWnd != NULL);

	CommonDataGlobal().hWndMonitor = hWnd;

	HotKeyHolders hotKeys;

	if (CommonDataGlobal().curModeBit == SW_BIT_32)
	{
		SW_TSTATUS_RET(ResetAllHotKey(hotKeys));
	}

	if (ntapi::AddClipboardFormatListener)
	{
		SW_WINBOOL_LOG(ntapi::AddClipboardFormatListener(hWnd));
	}

	if (ntapi::ChangeWindowMessageFilterEx)
	{
		SW_WINBOOL_LOG(ntapi::ChangeWindowMessageFilterEx(hWnd, c_MSG_Quit, MSGFLT_ALLOW, 0));
		SW_WINBOOL_LOG(ntapi::ChangeWindowMessageFilterEx(hWnd, c_MSG_SettingsChanges, MSGFLT_ALLOW, 0));
	}

	MSG msg;
	while (true)
	{
		BOOL bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet == 0)
			break;

		if (bRet == -1)
			SW_WINBOOL_RET(FALSE);

		if(msg.message == WM_HOTKEY)
		{
			//SW_LOG_INFO_2(L"WM_HOTKEY id=%d", msg.wParam);

		}
		else if (msg.message == c_MSG_SettingsChanges)
		{
			if (CommonDataGlobal().curModeBit == SW_BIT_32)
			{
				SettingsGlobal().Load();
				SW_TSTATUS_LOG(ResetAllHotKey(hotKeys));
			}
		}
		else if (msg.message == c_MSG_Quit)
		{
			PostQuitMessage(0);
		}
		else if (msg.message == WM_SWRevert)
		{
			HotKeyType type = (HotKeyType)msg.wParam;
			HookerGlobal().NeedRevert((HotKeyType)type);
		}
		else if (msg.message == WM_CLIPBOARDUPDATE)
		{
			HookerGlobal().CliboardChanged();
		}
		else
		{
			//TranslateMessage(&msg);
			//DispatchMessage(&msg);
		}
	}

	SW_RETURN_SUCCESS;
}
struct HookHandles
{
	CAutoHHOOK hHookKeyGlobal;
	CAutoHHOOK hHookGetMessage;
	CAutoHHOOK hHookCallWndProc;
	CAutoHHOOK hHookMouseGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobal;
	CAutoHWINEVENTHOOK hHookEventGlobalSwitchDesk;
};

static void CALLBACK WinEventProc_SwitchDesk(
	HWINEVENTHOOK hWinEventHook,
	DWORD event,
	HWND hwnd,
	LONG idObject,
	LONG idChild,
	DWORD dwEventThread,
	DWORD dwmsEventTime
	)
{
	HookerGlobal().ClearAllWords();
}

TStatus HookGlobal(HookHandles& handles)
{
	SW_TSTATUS_RET(HookerGlobal().Init());
	CAutoHMODULE hModUser;
	if (!IsWindowsVistaOrGreater())
	{
		hModUser = LoadLibrary(L"user32.dll");
		//SW_LOG_INFO_2(L"hModUser=%p", hModUser.Get());
	}
	handles.hHookKeyGlobal = ntapi::SetWindowsHookEx(WH_KEYBOARD_LL, &Hooker::LowLevelKeyboardProc, hModUser, 0);
	SW_WINBOOL_RET(handles.hHookKeyGlobal.IsValid());

#ifndef _DEBUG
	handles.hHookMouseGlobal = ntapi::SetWindowsHookEx(WH_MOUSE_LL, &Hooker::LowLevelMouseProc, hModUser, 0);
	SW_WINBOOL_RET(handles.hHookMouseGlobal.IsValid());
#endif

	handles.hHookEventGlobal = ntapi::SetWinEventHook(
		EVENT_SYSTEM_FOREGROUND,
		EVENT_SYSTEM_FOREGROUND,
		NULL,
		&Hooker::WinEventProc,
		0, 0,
		WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
	SW_WINBOOL_LOG(handles.hHookEventGlobal.IsValid());

	SW_RETURN_SUCCESS;
}
TStatus InitHooks(HookHandles& handles, TSWBit bit)
{
	if (bit == SW_BIT_32)
	{
		SW_TSTATUS_RET(HookGlobal(handles));
	}



	SW_RETURN_SUCCESS;
}
TStatus StartMonitor(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow,
	TSWBit bit)
{

	CommonDataGlobal().curModeBit = bit;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CAutoHandle mtxHook;
	if(bit == SW_BIT_32)
	{
		mtxHook = CreateMutex(NULL, FALSE, c_mtxHook32);
		SW_WINBOOL_RET(mtxHook.IsValid());
		CAutoWinMutexWaiter waiter;
		SW_WAITDWORD_RET(waiter.Wait(mtxHook, 0));
	}

	//SettingsGlobal().Load();

	Hooker hooker;
	CommonDataGlobal().hooker = &hooker;
	HookHandles hookHandles;
	SW_TSTATUS_RET(InitHooks(hookHandles, bit));

	SW_TSTATUS_RET(StartCycle(hInstance, nCmdShow));

	SW_RETURN_SUCCESS;
}





