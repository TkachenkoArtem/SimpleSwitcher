#include "stdafx.h"
#include "SwShedule.h"

#include "CAutoCOM.h"

#include <comdef.h>
#define _WIN32_DCOM

#include <taskschd.h>
# pragma comment(lib, "taskschd.lib")

#include "atlbase.h"

#include "SimpleSwitcher.h"

#include <Shellapi.h>


static const LPCWSTR c_wszTaskName = SW_PROGRAM_NAME_L L"Task";


TStatus CheckTaskShedule(bool& isShedule)
{
	isShedule = false;

	if (!IsWindowsVistaOrGreater())
		SW_RETURN_SUCCESS;

	CComPtr<ITaskService> pService;

	SW_HRESULT_RET(CoCreateInstance(
		CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(void**)&pService));

	SW_HRESULT_RET(pService->Connect(
		_variant_t(),
		_variant_t(),
		_variant_t(),
		_variant_t()));

	CComPtr<ITaskFolder> pRootFolder;

	SW_HRESULT_RET(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));

	CComPtr<IRegisteredTask> swtask;
	HRESULT res = pRootFolder->GetTask(_bstr_t(c_wszTaskName), &swtask);
	if(res == S_OK)
		isShedule = true;

	SW_RETURN_SUCCESS;
}
TStatus CreateTaskShedule()
{
	std::wstring sPath;
	GetPath(sPath, PATH_TYPE_EXE_NAME, SW_BIT_32);

	TChar args[0x1000];

	swprintf_s(args, L" /create /sc onlogon /F /tn %s /rl highest /tr \"%s %s\"", c_wszTaskName, sPath.c_str(), c_sArgAutostart);

	SwCreateProcessParm parm;
	ZeroMemory(&parm, sizeof(parm));
	parm.sCmd = args;
	parm.sExe = L"schtasks";
	parm.mode = SW_CREATEPROC_SHELLEXE;
	parm.isHide = true;
	CAutoHandle hProc;
	SW_TSTATUS_RET(SwCreateProcess(parm, hProc));
	SW_WAITDWORD_RET(WaitForSingleObject(hProc, c_nCommonWaitProcess));


	SW_RETURN_SUCCESS;
}
//TStatus CreateTaskShedule2()
//{
//	CComPtr<ITaskService> pService;
//
//	SW_HRESULT_RET(CoCreateInstance(
//		CLSID_TaskScheduler,
//		NULL,
//		CLSCTX_INPROC_SERVER,
//		IID_ITaskService,
//		(void**)&pService));
//
//	HRESULT res =(CoInitializeSecurity(
//		NULL,
//		-1,
//		NULL,
//		NULL,
//		RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
//		RPC_C_IMP_LEVEL_IMPERSONATE,
//		NULL,
//		0,
//		NULL));
//
//	SW_HRESULT_RET(pService->Connect(
//		_variant_t(),
//		_variant_t(),
//		_variant_t(),
//		_variant_t()));
//
//	CComPtr<ITaskFolder> pRootFolder;
//
//	SW_HRESULT_RET(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));
//
//	pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);
//
//	CComPtr<ITaskDefinition> pTask;
//	SW_HRESULT_RET(pService->NewTask(0, &pTask));
//
//	pService.Release();
//
//	CComPtr<IRegistrationInfo> pRegInfo;
//	SW_HRESULT_RET(pTask->get_RegistrationInfo(&pRegInfo));
//
//	CComPtr<ITaskSettings> pSettings;
//	SW_HRESULT_RET(pTask->get_Settings(&pSettings));
//
//	SW_HRESULT_RET(pSettings->put_StartWhenAvailable(VARIANT_TRUE));
//
//	pSettings.Release();
//
//	CComPtr<ITriggerCollection> pTriggerCollection;
//	SW_HRESULT_RET(pTask->get_Triggers(&pTriggerCollection));
//
//	CComPtr<ITrigger> pTrigger;
//	SW_HRESULT_RET(pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger));
//	pTriggerCollection.Release();
//
//	CComPtr<ILogonTrigger> pLogonTrigger;
//	SW_HRESULT_RET(pTrigger->QueryInterface(
//		IID_ILogonTrigger, 
//		(void**)&pLogonTrigger));
//	pTrigger.Release();
//
//	SW_HRESULT_RET(pLogonTrigger->put_Id(_bstr_t(L"Trigger1")));
//
//	TChar buf[0x1000];
//	ULONG size = 0x1000;
//	//SW_WINBOOL_RET(GetUserNameEx(NameFullyQualifiedDN, buf, &size));
//	BOOL res1 = GetUserName(buf, &size);
//	//BYTE* buf2;
//	//NET_API_STATUS stat = NetUserGetInfo(NULL, buf, 1, &buf2);
//	//PUSER_INFO_11 inf = (PUSER_INFO_11)buf2;
//
//	//SW_HRESULT_RET(pLogonTrigger->put_UserId(_bstr_t(buf)));
//
//	CComPtr<IActionCollection> pActionCollection;
//	SW_HRESULT_RET(pTask->get_Actions(&pActionCollection));
//
//	CComPtr<IAction> pAction;
//	SW_HRESULT_RET(pActionCollection->Create(TASK_ACTION_EXEC, &pAction));
//
//	CComPtr<IExecAction> pExecAction;
//	SW_HRESULT_RET(pAction->QueryInterface(
//		IID_IExecAction, (void**)&pExecAction));
//
//	std::wstring sPath;
//	GetPath(sPath, PATH_TYPE_EXE_NAME, SW_BIT_32);
//	swprintf_s(buf, L"%s %s", sPath.c_str(), c_sArgMonitorOn);
//	SW_HRESULT_RET(pExecAction->put_Path(_bstr_t(buf)));
//
//	CComPtr<IRegisteredTask> pRegisteredTask;
//	SW_HRESULT_RET(pRootFolder->RegisterTaskDefinition(
//		_bstr_t(wszTaskName),
//		pTask,
//		TASK_CREATE_OR_UPDATE,
//		_variant_t(L"Builtin\\Administrators"),
//		_variant_t(),
//		TASK_LOGON_GROUP,
//		_variant_t(L""),
//		&pRegisteredTask));
//
//	SW_RETURN_SUCCESS;
//}

TStatus RemoveTaskShedule()
{
	CComPtr<ITaskService> pService;

	SW_HRESULT_RET(CoCreateInstance(
		CLSID_TaskScheduler,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITaskService,
		(void**)&pService));

	SW_HRESULT_RET(pService->Connect(
		_variant_t(),
		_variant_t(),
		_variant_t(),
		_variant_t()));

	CComPtr<ITaskFolder> pRootFolder;

	SW_HRESULT_RET(pService->GetFolder(_bstr_t(L"\\"), &pRootFolder));

	SW_HRESULT_RET(pRootFolder->DeleteTask(_bstr_t(c_wszTaskName), 0));

	SW_RETURN_SUCCESS;
}
