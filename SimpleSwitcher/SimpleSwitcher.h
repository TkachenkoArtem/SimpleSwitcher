#pragma once

#include "resource.h"

#include "CAutoProcMonitor.h"


class Hooker;

struct CommonData
{
	HINSTANCE hInst;
	TSWBit curModeBit;
	HWND hWndMonitor;
	HWND hWndGuiMain;
	Hooker* hooker = 0;
	CAutoProcMonitor procMonitor;
	CommonData() : procMonitor(c_sClassName32, c_sArgHook32, SW_BIT_32, false) {}
	static CommonData& Global()
	{
		static CommonData data;
		return data;
	}
};
inline CommonData& CommonDataGlobal() { return CommonData::Global(); }
//enum TStartMode
//{
//	START_MODE_x32,
//	START_MODE_x64
//};

TStatus StartMonitor(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow,
	TSWBit bit);



TStatus StartService(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR lpCmdLine,
	_In_ int nCmdShow);

LRESULT CALLBACK GetMsgProc(
	_In_  int nCode,
	_In_  WPARAM wParam,
	_In_  LPARAM lParam
	);












