#pragma once

#include "ShareMemory.h"
#include "Settings.h"
#include "SwShared.h"

#include <deque>
#include <list>
#include <condition_variable>

#include "CAutoCleanupWin.h"
#include "InputSender.h"
#include "SwSyncVal.h"


class Hooker
{
private:

	struct TKeyHookInfo
	{
		CHotKey key;
		TKeyType type;
		TKeyHookInfo(CHotKey k, TKeyType t) : key(k), type(t) {}
		TKeyHookInfo() {}
	};
	typedef std::deque<TKeyHookInfo> TWordList;
	typedef std::vector<CHotKey> TKeyToRevert;

public:
	Hooker()  	{}
	~Hooker() 	{ Deinit();}

	static LRESULT CALLBACK LowLevelKeyboardProc(_In_  int nCode,_In_  WPARAM wParam,_In_  LPARAM lParam);
	static LRESULT CALLBACK Hooker::LowLevelMouseProc(_In_  int nCode,	_In_  WPARAM wParam,_In_  LPARAM lParam	);
	static LRESULT CALLBACK CBTProc(_In_  int nCode, _In_  WPARAM wParam, _In_  LPARAM lParam);
	static void CALLBACK WinEventProc(
		HWINEVENTHOOK hWinEventHook,
		DWORD event,
		HWND hwnd,
		LONG idObject,
		LONG idChild,
		DWORD dwEventThread,
		DWORD dwmsEventTime
		);
	
	void ClearAllWords();
	void ClearCycleRevert();
	void NeedRevert(HotKeyType typeRevert);
	bool IsOurInput();
	TStatus NeedRevertInt(HotKeyType typeRevert);
	void ThreadInputSender();
	TStatus AnalizeTopWnd();
	void CliboardChanged() { SW_TSTATUS_LOG(ClipboardChangedInt()); }
	TStatus ClipboardChangedInt();
	void ChangeForeground(HWND hwnd);
	TKeyType GetCurKeyType();
	TStatus SendOurInputFromHost(InputSender& sender);
	TStatus LowLevelKeyboardProcInt(KBDLLHOOKSTRUCT* k, WPARAM wParam);
	TStatus ClearModsBySend(CHotKey key);
	void HandleSymbolDown();
	void AddKeyToList(TKeyType type);
	void Deinit();
	TStatus Init();
	TStatus DoneRevert();
	void CompleteRevert()
	{
		//SW_LOG_INFO_2(L"CompleteRevert");
		m_fRevertInProgress = false;
	}

	bool m_fTryUseHookDll = false;

private:

	bool m_fRevertInProgress = false;
	DWORD m_dwLastCtrlCReqvest = 0;

	DWORD m_dwIdThreadForeground = -1;
	DWORD m_dwIdProcoreground = -1;

	DWORD m_dwIdThreadTopWnd = 0;
	HKL m_layoutTopWnd = 0;
	HWND m_hwndTop = 0;

	SendData m_sendData;

	struct CycleRevert
	{
		int nIndexWordList;
		bool fNeedLanguageChange;
	};
	typedef std::vector<CycleRevert> TCycleRevertList;

	TStatus GenerateCycleRevertList();
	TStatus FillKeyToRevert(TKeyListShared& keyList, HotKeyType typeRevert);
	TStatus StartRevert(TUInt32 flags, TUInt64 lay = 0);

	static const int c_maxWordRevert = 10;

	std::recursive_mutex  m_mtxKeyList;
	TWordList m_wordList;
	TCycleRevertList m_CycleRevertList;
	int m_nCurrentRevertCycle = -1;

	TSyncVal32 m_fOurSend = 0;
	CHotKey m_curKey;
	KBDLLHOOKSTRUCT* m_curHookStruct = 0;

};

inline Hooker& HookerGlobal() {return *CommonDataGlobal().hooker;}










