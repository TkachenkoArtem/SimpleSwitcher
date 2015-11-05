#include "stdafx.h"
#include "SimpleSwitcher.h"
#include "SwUtils.h"
#include "SettingsGui.h"
#include "SwShedule.h"
#include "CAutoCOM.h"
#include "SwGui.h"

TStatus SetShedule(bool add)
{
	CAutoCOMInitialize autoCom;
	if(add)
	{
		SW_TSTATUS_RET(CreateTaskShedule());
	}
	else
	{
		SW_TSTATUS_RET(RemoveTaskShedule());
	}

	SW_RETURN_SUCCESS;
		
}

//TStatus MonitorOn(TSWAdmin admin);

TStatus HandleAutostart(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine);

TStatus MainInt(LPTSTR lpCmdLine, HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
	CommonDataGlobal().hInst = hInstance;

	SW_LOG_INFO_DEBUG(L"Start %s. Elevated=%d", *lpCmdLine ? lpCmdLine : L"GUI", IsSelfElevated());

	if (wcscmp(lpCmdLine, c_sArgHook32) == 0)
	{
		SW_TSTATUS_RET(StartMonitor(hInstance, hPrevInstance, lpCmdLine, nCmdShow, SW_BIT_32));
	}
	else if (wcscmp(lpCmdLine, c_sArgHook64) == 0)
	{
		SW_TSTATUS_RET(StartMonitor(hInstance, hPrevInstance, lpCmdLine, nCmdShow, SW_BIT_64));
	}
	else if (wcscmp(lpCmdLine, c_sArgStarter) == 0)
	{
		CommonDataGlobal().procMonitor.EnsureStarted(SW_ADMIN_SELF);
	}
	else if (wcscmp(lpCmdLine, c_sArgAutostart) == 0)
	{
		HandleAutostart(hInstance, hPrevInstance, lpCmdLine);

	}
	else if (wcscmp(lpCmdLine, c_sArgServiceOFF) == 0)
	{
		SW_TSTATUS_RET(SetShedule(false));
	}
	else if (wcscmp(lpCmdLine, c_sArgServiceON) == 0)
	{
		SW_TSTATUS_RET(SetShedule(true));
	}
	else if (wcscmp(lpCmdLine, c_sArgCapsRemapAdd) == 0)
	{
		BufScanMap buf;
		buf.GenerateCapsBytesRemap();
		SW_TSTATUS_LOG(buf.ToRegistry());
	}
	else if (wcscmp(lpCmdLine, c_sArgCapsRemapRemove) == 0)
	{
		SW_TSTATUS_LOG(BufScanMap::RemoveRegistry());
	}
	else
	{	
		if (IsSelf64())
		{
			CAutoHandle hProc32;
			SW_TSTATUS_RET(SwCreateProcessOur(0, SW_BIT_32, SW_ADMIN_SELF, hProc32));
		}
		else
		{
			SW_TSTATUS_RET(SwGui::StartGui(hInstance, hPrevInstance, lpCmdLine, SW_SHOW));
		}
	}

	SW_RETURN_SUCCESS;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{


	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	TStatus status;
	status = MainInt(lpCmdLine, hInstance, hPrevInstance, nCmdShow);
	SW_TSTATUS_LOG(status);

	return status;
	
}

TStatus HandleAutostart(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine)
{
	SettingsGlobal().Load();

	TSWAdmin admin = SW_ADMIN_SELF;

	if (IsSelfElevated() && !SettingsGlobal().isMonitorAdmin)
	{
		admin = SW_ADMIN_OFF;
	}

	SW_TSTATUS_LOG(CommonDataGlobal().procMonitor.EnsureStarted(admin));

	if (SettingsGlobal().isAddToTray)
	{
		SW_TSTATUS_LOG(SwGui::StartGui(hInstance, hPrevInstance, lpCmdLine, SW_HIDE));
	}

	SW_RETURN_SUCCESS;
}

