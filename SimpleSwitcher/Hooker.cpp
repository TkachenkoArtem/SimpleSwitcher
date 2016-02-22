#include "stdafx.h"

#include "Hooker.h"
#include "Revert.h"
#include "Settings.h"
#include "SimpleSwitcher.h"
#include "CAutoCleanupWin.h"

TKeyType Hooker::GetCurKeyType()
{
	CHotKey key = m_curKey;
	if(key.Size() == 0)
	{
		return KEYTYPE_COMMAND_NO_CLEAR;
	}
	else if(key.Size() > 2)
	{
		return KEYTYPE_COMMAND_CLEAR;
	}
	else if (key.Size() == 2)
	{
		if (!key.HasMod(VK_SHIFT))
			return KEYTYPE_COMMAND_CLEAR;
	}

	switch (key.ValueKey())
	{
		case VK_CAPITAL:
		case VK_SCROLL:
		case VK_PRINT:
		case VK_NUMLOCK:
		case VK_INSERT:
			return KEYTYPE_COMMAND_NO_CLEAR;
		}
		switch (key.ValueKey())
		{
		case VK_RETURN:
		case VK_TAB:
		case VK_SPACE:
			return KEYTYPE_SPACE;
		case VK_BACK:
			return KEYTYPE_BACKSPACE;
	}

	if(SettingsGlobal().isDashSeparate)
	{
		if(key.ValueKey() == 189 && key.Size() == 1)
		{
			return KEYTYPE_SPACE;
		}
	}

	BYTE keyState[256] = { 0 };
	TCHAR sBufKey[0x100] = { 0 };
	int res = ToUnicodeEx(key.ValueKey(), m_curHookStruct->scanCode, keyState, sBufKey, 0x100, 0, NULL);
	if (res == 1)
	{
		return KEYTYPE_SYMBOL;
	}
	else
	{
		return KEYTYPE_COMMAND_CLEAR;
	}
}


void PrintHwnd(HWND hwnd, const TChar* name=L"name1")
{
	if(GetLogLevel() >= 2)
	{
		DWORD pid = 0;
		DWORD threadid = GetWindowThreadProcessId(hwnd, &pid);
		//SW_LOG_INFO_2(L"%s=%p, pid=%d threadid=%d", name, hwnd, pid, threadid);
	}
}
VOID CALLBACK SendAsyncProc(
	_In_  HWND hwnd,
	_In_  UINT uMsg,
	_In_  ULONG_PTR dwData,
	_In_  LRESULT lResult
	)
{
	SW_TSTATUS_LOG(HookerGlobal().DoneRevert());
}

void Hooker::NeedRevert(HotKeyType typeRevert)
{
	//SW_LOG_INFO_1(L"NeedRevert typeRevert=%d", typeRevert);

	TStatus stat = NeedRevertInt(typeRevert);
	if (SW_ERROR(stat))
	{
		SW_TSTATUS_LOG(stat, L"NeedRevert error");
		CompleteRevert();
	}
}

TStatus Hooker::LowLevelKeyboardProcInt(KBDLLHOOKSTRUCT* k, WPARAM wParam)
{

	if(k->vkCode > 255)
	{
		//SW_LOG_INFO_2(L"k->vkCode > 255: %d", k->vkCode);
		SW_RETURN_SUCCESS;
	}

	m_curHookStruct = k;

	TKeyCode vkCode = (TKeyCode)k->vkCode;
	KeyState curKeyState = GetKeyState(wParam);

	if(IsOurInput())
		SW_RETURN_SUCCESS;

	if(curKeyState == KEY_STATE_UP)
	{
		if(!m_curKey.Remove(vkCode))
		{
			if (CHotKey::IsKnownMods(vkCode))
			{
				std::wstring s1;
				CHotKey::ToString(vkCode, s1);
				SW_TSTATUS_LOG(SW_ERR_UNKNOWN, L"Not found up for key %s", s1.c_str());
			}
		}
	}
	else if(curKeyState == KEY_STATE_DOWN)
	{
		m_curKey.Add(vkCode,  CHotKey::ADDKEY_ORDERED | CHotKey::ADDKEY_ENSURE_ONE_VALUEKEY);
	}
	else
		return SW_ERR_UNKNOWN;

	if (GetLogLevel() >= LOG_LEVEL_2)
	{
		std::wstring s1;
		CHotKey::ToString(vkCode, s1);
		SW_LOG_INFO_2(L"cur type %S: %s ", GetKeyStateName(curKeyState), s1.c_str());

		std::wstring s2;
		m_curKey.ToString(s2, true);
		SW_LOG_INFO_2(L"cur hot key: %s", s2.c_str());
	}

	if (curKeyState != KEY_STATE_DOWN)
		SW_RETURN_SUCCESS;

	int hotKeyIndex = 0;
	for (; hotKeyIndex < SW_ARRAY_SIZE(SettingsGlobal().hotKeys); ++hotKeyIndex)
	{
		CHotKey key = SettingsGlobal().hotKeys[hotKeyIndex].key;
		if (m_curKey == key)
		{
			break;
		}
	}

	if (hotKeyIndex != HotKey_SIZE)
	{
		NeedRevert((HotKeyType)hotKeyIndex);
		SW_RETURN_SUCCESS;
	}

	if (CHotKey::IsKnownMods(vkCode))
		SW_RETURN_SUCCESS;

	HandleSymbolDown();

	SW_RETURN_SUCCESS;
	
}
LRESULT CALLBACK Hooker::LowLevelKeyboardProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode == HC_ACTION)
	{
		KBDLLHOOKSTRUCT* kStruct = (KBDLLHOOKSTRUCT*)lParam;
		SW_TSTATUS_LOG(HookerGlobal().LowLevelKeyboardProcInt(kStruct, wParam));
	}	

	return CallNextHookEx(0, nCode, wParam, lParam);

}

LRESULT CALLBACK Hooker::LowLevelMouseProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	)
{
	if (nCode < 0)
		return  CallNextHookEx(NULL, nCode, wParam, lParam);

	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_MOUSEMOVE:
			break;
		default:
			HookerGlobal().ClearAllWords();
			break;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

TStatus Hooker::Init()
{
	ClearAllWords();

	SW_RETURN_SUCCESS;

}
void Hooker::ClearAllWords()
{
	//SW_LOG_INFO_2(L"ClearsKeys");

	{
		std::unique_lock<std::recursive_mutex > lock(m_mtxKeyList);
		m_wordList.clear();
		ClearCycleRevert();

		CHotKey curCopy = m_curKey;
		for (TKeyCode* k = curCopy.ModsBegin(); k != curCopy.ModsEnd(); ++k)
		{
			if (GetAsyncKeyState(*k) & 0x8000)
			{
			}
			else
			{
				//SW_LOG_INFO_2(L"Up key %s because GetAsyncKeyState", CHotKey::ToString(*k).c_str());
				m_curKey.Remove(*k);
			}
		}
	}
}

void Hooker::AddKeyToList(TKeyType type)
{
	std::unique_lock<std::recursive_mutex> lock(m_mtxKeyList);	

	ClearCycleRevert();

	if (m_wordList.size() >= c_nMaxLettersSave)
	{
		m_wordList.pop_front();
	}

	TKeyHookInfo key2(m_curKey, type);
	m_wordList.push_back(key2);
}
TStatus Hooker::FillKeyToRevert(TKeyListShared& keyList, HotKeyType typeRevert)
{
	std::unique_lock<std::recursive_mutex> lock(m_mtxKeyList);

	if (m_nCurrentRevertCycle == -1)
		SW_RETURN_SUCCESS;
	if (m_CycleRevertList.empty())
		SW_RETURN_SUCCESS;

	CycleRevert curRevertInfo;

	if (typeRevert == HotKey_Revert)
	{
		if (m_nCurrentRevertCycle > 0)
		{
			m_nCurrentRevertCycle -= 1;
			curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
			m_nCurrentRevertCycle = 0;
		}
		else
		{
			curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
			m_nCurrentRevertCycle = 1;
			if (m_nCurrentRevertCycle >= (int)m_CycleRevertList.size())
				m_nCurrentRevertCycle = 0;
		}
	}
	else
	{
		curRevertInfo = m_CycleRevertList[m_nCurrentRevertCycle];
		++m_nCurrentRevertCycle;
		if (m_nCurrentRevertCycle >= (int)m_CycleRevertList.size())
			m_nCurrentRevertCycle = 0;
	}

	if (curRevertInfo.nIndexWordList == -1)
		SW_RETURN_SUCCESS;
	if (m_wordList.empty())
		SW_RETURN_SUCCESS;

	for (int i = curRevertInfo.nIndexWordList; i < (int)m_wordList.size(); ++i)
	{
		keyList.Add(m_wordList[i].key);
	}

	SW_RETURN_SUCCESS;
}
TStatus Hooker::GenerateCycleRevertList()
{
	bool isNeedLangChange = true;

	std::unique_lock<std::recursive_mutex> lock(m_mtxKeyList);

	m_CycleRevertList.clear();

	int countWords = 0;
	if (!m_wordList.empty())
	{
		for (int i = (int)m_wordList.size() - 1; i >= 0; --i)
		{
			if (SettingsGlobal().isTryOEM2 && 
				m_wordList[i].key.ValueKey() == VK_OEM_2
				&& (i == m_wordList.size() - 1 || m_wordList[i + 1].type == KEYTYPE_SPACE)
				&& m_CycleRevertList.empty()
				)
			{
				CycleRevert cycleRevert = { i, m_CycleRevertList.empty() };
				m_CycleRevertList.push_back(cycleRevert);
				if (++countWords >= c_maxWordRevert)
					break;
			}
			else if (m_wordList[i].type != KEYTYPE_SPACE && (i == 0 || m_wordList[i - 1].type == KEYTYPE_SPACE))
			{
				CycleRevert cycleRevert = { i, m_CycleRevertList.empty() };
				m_CycleRevertList.push_back(cycleRevert);
				if (++countWords >= c_maxWordRevert)
					break;
			}

		}
	}

	if(m_CycleRevertList.size() > 1)
	{
		m_CycleRevertList.push_back(m_CycleRevertList.back());
		m_CycleRevertList.back().fNeedLanguageChange = true;
	}

	if(m_CycleRevertList.empty())
	{
		CycleRevert cycleRevert = { -1, true };
		m_CycleRevertList.push_back(cycleRevert);
	}

	m_nCurrentRevertCycle = 0;

	SW_RETURN_SUCCESS;
}

void Hooker::ClearCycleRevert()
{
	//SW_LOG_INFO_2(L"ClearCycleRevert");
	{
		std::unique_lock<std::recursive_mutex > lock(m_mtxKeyList);
		m_CycleRevertList.clear();
		m_nCurrentRevertCycle = -1;
	}
	
}

void CALLBACK Hooker::WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
	HookerGlobal().ChangeForeground(hwnd);
}

TStatus Hooker::ClipboardChangedInt()
{
	DWORD dwTime = GetTickCount() - m_dwLastCtrlCReqvest;
	//SW_LOG_INFO_2(L"ClipboardChangedInt dwTime=%u", dwTime);
	if (dwTime < 500)
	{
		if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
			SW_RETURN_SUCCESS;
		CAutoClipBoard clip;
		SW_TSTATUS_RET(clip.Open());
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		CAutoGlobalLock lock = GlobalLock(hData);
		SW_WINBOOL_RET(lock.IsValid());
		TCHAR* sText = (TCHAR*)lock.Get();
		//SW_LOG_INFO_2(L"buffer=%s", sText);

		HKL layouts[10];
		int count = GetKeyboardLayoutList(10, layouts);
		SW_WINBOOL_RET(count != 0);
		
		SwZeroMemory(m_sendData);

		//SW_LOG_INFO_2(L"layout=%u", m_layoutTopWnd);
		for(int i = 0; i < (int)wcslen(sText); ++i)
		{
			TCHAR c = sText[i];
			if (c == L'\r')
				continue;
			SHORT res = VkKeyScanEx(c, m_layoutTopWnd);
			if (res == -1)
			{
				for(int i = 0; i < count; ++i)
				{
					if(layouts[i] != m_layoutTopWnd)
						res = VkKeyScanEx(c, layouts[i]);
					if(res != -1)
						break;
				}
			}
			if(res == -1)
			{
				SW_TSTATUS_LOG(SW_ERR_UNKNOWN, L"Cant scan char %c", c);
				continue;
			}
			BYTE mods = HIBYTE(res);
			BYTE code = LOBYTE(res);

			//SW_LOG_INFO_2(L"found vkcode=%u mods=%u", code, mods);
			CHotKey key;
			key.Add(code);
			if(TestFlag(mods, 0x1))
				key.Add(VK_SHIFT);
			m_sendData.keyList.Add(key);
		}
		m_sendData.type = Hotkey_RevertSel;
		SW_TSTATUS_RET(StartRevert(SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang, HKL_NEXT));
	}

	SW_RETURN_SUCCESS;
}

LRESULT CALLBACK Hooker::CBTProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	//SW_LOG_INFO_2(L"fff=10");
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Hooker::ChangeForeground(HWND hwnd)
{
	//SW_LOG_INFO_2(L"Now foreground hwnd=0x%x", hwnd);
	DWORD procId = 0;
	DWORD threadid = GetWindowThreadProcessId(hwnd, &procId);
	if (threadid != m_dwIdThreadForeground && procId != m_dwIdProcoreground)
	{
		//SW_LOG_INFO_2(L"threadid=%d, procId=%d", threadid, procId);
		ClearAllWords();
	}
	m_dwIdThreadForeground = threadid;
	m_dwIdProcoreground = procId;
}
bool Hooker::IsOurInput()
{
	if (m_fOurSend)
		return true;

	return false;
}
void Hooker::HandleSymbolDown()
{
	TKeyType type = GetCurKeyType();
	switch (type)
	{
	case KEYTYPE_BACKSPACE:
	{
		{
			std::unique_lock<std::recursive_mutex> lock(m_mtxKeyList);
			if (!m_wordList.empty())
				m_wordList.pop_back();
		}
							  break;
	}
	case KEYTYPE_SYMBOL:
	case KEYTYPE_SPACE:
	{
						  AddKeyToList(type);
						  break;
	}
	case KEYTYPE_COMMAND_NO_CLEAR:
		break;
	default:
		ClearAllWords();
		break;
	}
}

TStatus Hooker::StartRevert(TUInt32 flags, TUInt64 lay)
{

	m_sendData.lay = (TUInt64)lay;
	m_sendData.flags = flags;
	m_sendData.hwndFocused = m_hwndTop;

	DoneRevert();

	SW_RETURN_SUCCESS;
}

TStatus Hooker::NeedRevertInt(HotKeyType typeRevert)
{
	SwZeroMemory(m_sendData);

	m_sendData.key = m_curKey;
	m_sendData.type = typeRevert;


	if(typeRevert == HotKey_CapsGenerate)
	{
		InputSender inputSender;
		inputSender.Add(VK_CAPITAL, KEY_STATE_DOWN);
		inputSender.Add(VK_CAPITAL, KEY_STATE_UP);
		SW_TSTATUS_LOG(SendOurInput(inputSender));
		CompleteRevert();
		SW_RETURN_SUCCESS;
	}

	SW_TSTATUS_RET(AnalizeTopWnd());


	if (typeRevert == HotKey_ChangeLayoutCycle)
	{

		SW_TSTATUS_RET(StartRevert(SW_CLIENT_SetLang, HKL_NEXT));
		SW_RETURN_SUCCESS;
	}

	if(typeRevert == HotKey_ChangeSetLayout_1 || typeRevert == HotKey_ChangeSetLayout_2 || typeRevert == HotKey_ChangeSetLayout_3)
	{
		HKL hkl = 0;
		if (typeRevert == HotKey_ChangeSetLayout_1)
			hkl = SettingsGlobal().hkl_lay[SettingsGui::SW_HKL_1];
		else if (typeRevert == HotKey_ChangeSetLayout_2)
			hkl = SettingsGlobal().hkl_lay[SettingsGui::SW_HKL_2];
		else if (typeRevert == HotKey_ChangeSetLayout_3)
			hkl = SettingsGlobal().hkl_lay[SettingsGui::SW_HKL_3];


		SW_TSTATUS_RET(StartRevert(SW_CLIENT_SetLang, (TUInt64)hkl));

		SW_RETURN_SUCCESS;
	}


	if (typeRevert == Hotkey_RevertSel)
	{
		m_dwLastCtrlCReqvest = GetTickCount();
		SW_TSTATUS_RET(StartRevert(SW_CLIENT_CTRLC));
		SW_RETURN_SUCCESS;
	}
	else if (typeRevert == HotKey_Revert || typeRevert == HotKey_RevertAdv)
	{
		typeRevert = HotKey_Revert;
	}
	else if(typeRevert == HotKey_RevertCycle || typeRevert == HotKey_RevertCycleAdv)
	{
		typeRevert = HotKey_RevertCycle;
	}
	else
	{
		SW_TSTATUS_RET(SW_ERR_UNKNOWN, L"Unknown typerevert %d", typeRevert);
	}

	bool isNeedLangChange = true;
	{
		std::unique_lock<std::recursive_mutex> lock(m_mtxKeyList);
		if (m_nCurrentRevertCycle == -1)
		{
			SW_TSTATUS_RET(GenerateCycleRevertList());
		}
		if (typeRevert == HotKey_Revert)
		{
			isNeedLangChange = true;
		}
		else
		{
			isNeedLangChange = m_CycleRevertList[m_nCurrentRevertCycle].fNeedLanguageChange;
		}
	}

	SW_TSTATUS_RET(FillKeyToRevert(m_sendData.keyList, typeRevert));
	SW_TSTATUS_RET(StartRevert(SW_CLIENT_PUTTEXT | SW_CLIENT_SetLang, isNeedLangChange ? HKL_NEXT : 0));

	SW_RETURN_SUCCESS;
}
TStatus Hooker::DoneRevert()
{
	SendData& sendData = m_sendData;

	if (sendData.fRequestComplite == 0)
	{

		//SW_LOG_INFO_2(L"SendKeysFromHost...");
		if (TestFlag(sendData.flags, SW_CLIENT_SetLang))
		{
			if (sendData.lay)
				PostMessage(m_hwndTop, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)m_sendData.lay);
		}

		if (TestFlag(sendData.flags, SW_CLIENT_PUTTEXT))
		{
			SW_TSTATUS_RET(ClearModsBySend(m_curKey));
			SW_TSTATUS_RET(SendKeys(sendData));
		}

		if (TestFlag(sendData.flags, SW_CLIENT_CTRLC))
		{
			SW_TSTATUS_RET(ClearModsBySend(m_curKey));

			CHotKey ctrlc(VK_CONTROL, 67);
			InputSender inputSender;
			inputSender.AddPress(ctrlc);

			SW_TSTATUS_RET(SendOurInput(inputSender));
		}
	}

	CompleteRevert();

	SW_RETURN_SUCCESS;
}


void Hooker::Deinit()
{

}

TStatus Hooker::AnalizeTopWnd()
{
	HWND hwndFocused;
	SW_TSTATUS_RET(GetFocusWindow(hwndFocused));
	PrintHwnd(hwndFocused, L"hwndFocused");

	m_dwIdThreadTopWnd = GetWindowThreadProcessId(hwndFocused, 0);
	m_layoutTopWnd = GetKeyboardLayout(m_dwIdThreadTopWnd);
	m_hwndTop = hwndFocused;

	SW_RETURN_SUCCESS;
}

TStatus Hooker::ClearModsBySend(CHotKey key)
{
	InputSender sender;

	for (TKeyCode k : key)
	{
		if (!CHotKey::IsKnownMods(k))
			continue;
		SHORT res = GetAsyncKeyState(k);
		if (res & 0x8000)
		{
			SW_TSTATUS_RET(sender.Add(k, KEY_STATE_UP));
		}
	}

	SW_TSTATUS_RET(SendOurInput(sender));

	SW_RETURN_SUCCESS;
}

TStatus Hooker::SendOurInputFromHost(InputSender& sender)
{
	CAutoCounter autoBool(m_fOurSend);
	SW_TSTATUS_RET(sender.Send());

	SW_RETURN_SUCCESS;
}







