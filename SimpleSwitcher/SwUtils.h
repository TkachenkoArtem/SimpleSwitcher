#pragma once

#include "consts.h"
#include "CHotKey.h"

const static TChar c_sRegRunValue[] = SW_PROGRAM_NAME_L;
const static TChar c_sServiceName[] = SW_PROGRAM_NAME_L L"Service";

enum TSWAdmin
{
	SW_ADMIN_ON,
	SW_ADMIN_OFF,
	SW_ADMIN_SELF,
};

inline TSWBit GetSelfBit()
{
	return IsSelf64() ? SW_BIT_64 : SW_BIT_32;
}

enum TStopRes
{
	STOP_RES_OK,
	STOP_RES_NOT_FOUND,
	STOP_RES_CANT_ADMIN,
	STOP_RES_NOT_WIN64,
};

enum SwCreateProcFunc
{
	SW_CREATEPROC_DEFAULT = 0,

	SW_CREATEPROC_NORMAL,
	SW_CREATEPROC_AS_USER,
	SW_CREATEPROC_TOKEN,
	SW_CREATEPROC_SHELLEXE ,
};
struct SwCreateProcessParm
{
	SwCreateProcFunc mode;
	const TChar* sExe;
	bool isOur;
	const TChar* sCmd;
	TSWAdmin admin;
	TSWBit bit;
	HANDLE hToken;
	bool isHide;
};


TStatus SwCreateProcess(SwCreateProcessParm& parm, CAutoHandle& hProc);
inline TStatus SwCreateProcessOur(const TCHAR* sCmd, TSWBit bit, TSWAdmin admin, CAutoHandle& hProc)
{
	SwCreateProcessParm parm;
	ZeroMemory(&parm, sizeof(parm));
	parm.isOur = true;
	parm.sCmd = sCmd;
	parm.bit = bit;
	parm.admin =admin;
	SW_TSTATUS_RET(SwCreateProcess(parm, hProc));

	SW_RETURN_SUCCESS;
}
inline TStatus SwCreateProcessOurWaitIdle(const TCHAR* sCmd, TSWBit bit, TSWAdmin admin)
{
	CAutoHandle hProc;
	SwCreateProcessParm parm;
	ZeroMemory(&parm, sizeof(parm));
	parm.isOur = true;
	parm.sCmd = sCmd;
	parm.bit = bit;
	parm.admin = admin;

	SW_TSTATUS_RET(SwCreateProcess(parm, hProc));
	SW_WAITDWORD_RET(WaitForInputIdle(hProc, c_nCommonWaitProcess));

	SW_RETURN_SUCCESS;
}
inline TStatus SwCreateProcessOurWaitFinished(const TCHAR* sCmd, TSWBit bit, TSWAdmin admin)
{
	CAutoHandle hProc;
	SwCreateProcessParm parm;
	ZeroMemory(&parm, sizeof(parm));
	parm.isOur = true;
	parm.sCmd = sCmd;
	parm.bit = bit;
	parm.admin = admin;
	SW_TSTATUS_RET(SwCreateProcess(parm, hProc));

	SW_WAITDWORD_RET(WaitForSingleObject(hProc, c_nCommonWaitProcess));

	SW_RETURN_SUCCESS;
}

enum TPathType
{
	PATH_TYPE_SELF_FOLDER,
	PATH_TYPE_EXE_NAME,
	PATH_TYPE_DLL_NAME,
};
TStatus GetPath(std::wstring& sPath, TPathType type, TSWBit bit);


TStatus AddWindowsRun();
TStatus RemoveWindowsRun();

TStatus CreateService();
TStatus RemoveService();

TStatus CheckAutoStartUser(bool& isUser);
TStatus CheckService(bool& isAdmin);
static const TKeyCode c_capsRemap = 0x87;
struct BufScanMap
{
	static const DWORD c_size = 128;
	BYTE buf[c_size];
	DWORD size = 0;
	BufScanMap() { ZeroMemory(buf, c_size); }
	TKeyCode RemapedKey()
	{
		if (size == 0)
		{
			return VK_CAPITAL;
		}
		else
		{
			BYTE scanCode = buf[12];
			if(scanCode == 0)
			{
				return VK_CAPITAL;
			}
			SHORT virtcode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);
			return (TKeyCode)virtcode;
		}
	}
	void FromRegistry()
	{
		CAutoCloseHKey hg;
		LSTATUS stat = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout",
			0,
			KEY_READ,
			&hg);
		if (stat == ERROR_SUCCESS)
		{
			DWORD type = 0;
			size = c_size;
			LONG res = RegQueryValueEx(hg, L"Scancode Map", 0, &type, buf, &size);
			if (res != 0 || type != REG_BINARY)
			{
				size = 0;
			}
			int k = 0;
		}
	}
	bool operator==(BufScanMap& other)
	{
		if (size != other.size)
			return false;
		for (DWORD i = 0; i < size; ++i)
		{
			if (buf[i] != other.buf[i])
				return false;
		}
		return true;
	}
	bool operator!=(BufScanMap& other) { return !(*this == other); }
	void GenerateCapsBytesRemap()
	{
		size = 20;
		ZeroMemory(buf, c_size);
		buf[8] = 2;
		UINT a = MapVirtualKey(c_capsRemap, MAPVK_VK_TO_VSC);
		buf[12] = (BYTE)a;
		buf[14] = (BYTE)MapVirtualKey(VK_CAPITAL, MAPVK_VK_TO_VSC);
	}
	TStatus ToRegistry()
	{
		CAutoCloseHKey hg;
		SW_LSTATUS_RET(RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout",
			0,
			KEY_ALL_ACCESS,
			&hg));

		SW_LSTATUS_RET(RegSetValueEx(hg, L"Scancode Map", 0, REG_BINARY, buf, size));
		SW_RETURN_SUCCESS;
	}
	static TStatus RemoveRegistry()
	{
		CAutoCloseHKey hg;
		SW_LSTATUS_RET(RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout",
			0,
			KEY_ALL_ACCESS,
			&hg));
		SW_LSTATUS_RET(RegDeleteValue(hg, L"Scancode Map"));
		SW_RETURN_SUCCESS;

	}
};



