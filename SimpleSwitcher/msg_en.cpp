#include "stdafx.h"
#include "resource.h"

void InitMessages_Eng()
{
	ZeroMemory(c_messages, sizeof(c_messages));

	c_messages[AM_COMMON] = L"Common";
	c_messages[AM_2] = L"Additional";
	c_messages[AM_3] = L"Layout change";
	c_messages[AM_4] = L"Alt modifier is not supported due to the nature of Windows";
	c_messages[AM_5] = L"[Not setted]";
	c_messages[AM_6] = L" (windows requires a reboot)";
	c_messages[AM_7] = L"Section 'Scancode Map' already exists";
	c_messages[AM_ERROR] = L"Error";
	c_messages[AM_LANG] = L"Settings";
	c_messages[AM_ENG] = L"English";
	c_messages[AM_RUS] = L"Russian";
	c_messages[AM_NEED_REST] = L"Need restart program";
	c_messages[AM_VERS] = L"Version: ";
	c_messages[AM_CaptAb] = L"About - SimpleSwitcher";
	c_messages[AM_CLOSE] = L"Close";

	c_messages[AM_SHOW] = L"Show";
	c_messages[AM_ENABLE] = L"Enable";
	c_messages[AM_ABOUT] = L"About";
	c_messages[AM_EXIT] = L"Exit";
}

void InitDialogs_Eng()
{
	ZeroMemory(c_dialogs, sizeof(c_dialogs));

	c_dialogs[SD_HOTKEY] = IDD_DIALOG_HOTKEY;
	c_dialogs[SD_LAY] = IDD_DIALOG_PAGE_CHLAY;
	c_dialogs[SD_ADV] = IDD_DIALOG_PAGEADV;
	c_dialogs[SD_MAINMENU] = IDD_DIALOG_PAGEMAIN;
}