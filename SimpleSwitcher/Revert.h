#pragma once
#include "KeysStruct.h"
#include "InputSender.h"

#include "ShareMemory.h"
#include "SwShared.h"

#include "Hooker.h"

TStatus SendKeys(SendData& sendData);
enum CLEARMODS
{
	CLEARMODS_RESTORE,
	CLEARMODS_NORESTORE,
	CLEARMODS_NOCLEAR,
};

inline TStatus SendOurInput(InputSender& sender)
{
	HookerGlobal().SendOurInputFromHost(sender);

	SW_RETURN_SUCCESS;
}
inline TStatus ClearMods()
{
	BYTE buf[256] = { 0 };
	SW_WINBOOL_RET(SetKeyboardState(buf));

	SW_RETURN_SUCCESS;
}

inline TStatus RestoreMods(CHotKey key)
{
	SW_LOG_INFO_DEBUG(L"RestoreMods for key %s", key.ToString().c_str());

	BYTE buf[256];// = {0};
	SW_WINBOOL_RET(GetKeyboardState(buf));

	CHotKey keyAdded;
	for (TKeyCode k : key)
	{
		if (!CHotKey::IsKnownMods(k))
			continue;
		SHORT res = GetAsyncKeyState(k);
		SW_LOG_INFO_DEBUG(L"GetAsyncKeyState for key %s = 0x%x", CHotKey::ToString(k).c_str(), res);
		if (res & 0x8000)
		{
			if (k >= SW_ARRAY_SIZE(buf))
				SW_TSTATUS_RET(SW_ERR_BUFFER_TOO_SMALL);

			buf[k] &= 0x8000;
			keyAdded.Add(k);

			SW_LOG_INFO_DEBUG(L"Add key %s to down after input", CHotKey::ToString(k).c_str());

		}
	}

	SW_WINBOOL_RET(SetKeyboardState(buf));

	SW_RETURN_SUCCESS;
}
