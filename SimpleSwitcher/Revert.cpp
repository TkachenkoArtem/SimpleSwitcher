#include "stdafx.h"
#include "Revert.h"


TStatus SendKeys(SendData& sendData)
{
	//SW_LOG_INFO_2(L"SendInputShared revert");
	InputSender inputSender;
	CHotKey backspace(VK_BACK);
	if (sendData.type != Hotkey_RevertSel)
	{
		for (CHotKey& key : sendData.keyList)
		{
			SW_TSTATUS_RET(inputSender.AddPress(backspace));
		}
	}
	for (CHotKey& key : sendData.keyList)
	{
		SW_TSTATUS_RET(inputSender.AddPress(key));
	}

	SW_TSTATUS_RET(SendOurInput(inputSender));


	SW_RETURN_SUCCESS;
}

