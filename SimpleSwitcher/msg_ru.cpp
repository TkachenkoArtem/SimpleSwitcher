#include "stdafx.h"
#include "resource.h"

void InitMessages_Ru()
{
	ZeroMemory(c_messages, sizeof(c_messages));

	c_messages[AM_COMMON] = L"Общие";
	c_messages[AM_2] = L"Дополнительно";
	c_messages[AM_3] = L"Смена раскладки";
	c_messages[AM_4] = L"Модификатор Alt не поддерживается из-за особенностей Windows";
	c_messages[AM_5] = L"[Не задано]";
	c_messages[AM_6] = L" (требуется перезагрузка Windows)";
	c_messages[AM_7] = L"Раздел 'Scancode Map' уже существует";
	c_messages[AM_ERROR] = L"Ошибка";
	c_messages[AM_LANG] = L"Настройки";
	c_messages[AM_ENG] = L"Английский";
	c_messages[AM_RUS] = L"Русский";
	c_messages[AM_NEED_REST] = L"Требуется перезапуск программы";
	c_messages[AM_VERS] = L"Версия: ";
	c_messages[AM_CaptAb] = L"О программе - SimpleSwitcher";
	c_messages[AM_CLOSE] = L"Закрыть";

	c_messages[AM_SHOW] = L"Показать";
	c_messages[AM_ENABLE] = L"Включить";
	c_messages[AM_ABOUT] = L"О программе";
	c_messages[AM_EXIT] = L"Выход";

}

void InitDialogs_Ru()
{
	ZeroMemory(c_dialogs, sizeof(c_dialogs));

	c_dialogs[SD_HOTKEY] = IDD_DIALOG_HOTKEY_RU;
	c_dialogs[SD_LAY] = IDD_DIALOG_PAGE_CHLAY_RU;
	c_dialogs[SD_ADV] = IDD_DIALOG_PAGEADV_RU;
	c_dialogs[SD_MAINMENU] = IDD_DIALOG_PAGEMAIN_RU;
}