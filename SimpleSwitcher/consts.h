#pragma once
#include "targetver.h"


static const UINT c_MSG_TypeHotKey = 0xBFFF - 37;
static const UINT c_MSG_SettingsChanges = 0xBFFF - 36;
static const UINT c_MSG_Quit = 0xBFFF - 35;
static const UINT WM_SWRevert = 0xBFFF - 34;

static const UINT c_msgRevertID = WM_NULL;
static const WPARAM c_msgWParm = (WPARAM)0x9F899984;
static const WPARAM c_msgLParm = (WPARAM)0x34729EC9;

static const TCHAR c_sClassName32[] = L"C01D72259X32";
static const TCHAR c_sClassName64[] = L"C01D72259X64";
static const TCHAR c_sClassNameGUI[] = L"C03a3bb9a011";

#ifdef _WIN64
static const TCHAR* c_sClassName = c_sClassName64;
#else
static const TCHAR* c_sClassName = c_sClassName32;
#endif 

static const TChar c_sArgServiceOFF[] = L"/SheduleRemove";
static const TChar c_sArgServiceON[] = L"/SheduleAdd";
static const TChar c_sArgStarter[] = L"/startMonitor";
static const TChar c_sArgHook32[] = L"/hook32";
static const TChar c_sArgHook64[] = L"/hook64";
static const TChar c_sArgService[] = L"/service";
static const TChar c_sArgAutostart[] = L"/autostart";
static const TChar c_sArgCapsRemapAdd[] = L"/CapsRemapAdd";
static const TChar c_sArgCapsRemapRemove[] = L"/CapsRemapRemove";

enum 
{
	SW_CLIENT_PUTTEXT = 0x2,
	SW_NOT_BACKSPACE   = 0x4,
	SW_CLIENT_SetLang = 0x8,
	SW_CLIENT_CTRLC = 0x10,
};

enum HotKeyType
{
	HotKey_Revert = 0,
	HotKey_RevertCycle,
	Hotkey_RevertSel,
	HotKey_RevertAdv,
	HotKey_RevertCycleAdv,

	HotKey_ChangeLayoutCycle,

	HotKey_ChangeSetLayout_1,
	HotKey_ChangeSetLayout_2,
	HotKey_ChangeSetLayout_3,

	HotKey_CapsGenerate,

	HotKey_SIZE,

	HotKey_RevertFromClip,
};

enum
{
	SendTop_NeedRestore = 0x10000,
};

static const int c_nCommonWaitProcess = 5000;
static const int c_nCommonWaitMtx = 30000;

static const TChar c_sSiteLink[] = L"simpleswitcher.ru";

static const TChar c_sProgramName[] = SW_PROGRAM_NAME_L;

static const TChar c_sVersion[] = SW_VERSION_L;