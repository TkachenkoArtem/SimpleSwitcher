#include "stdafx.h"
#include "Settings.h"
#include "SwUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>



SettingsGui::SettingsGui()
{
	std::wstring sCurFolder;
	GetPath(sCurFolder, PATH_TYPE_SELF_FOLDER, GetSelfBit());

	m_sIniPath = sCurFolder + L"settings.ini";

	hotKeys[HotKey_Revert].def = CHotKey(VK_PAUSE);

	hotKeys[HotKey_RevertAdv].def = CHotKey(c_capsRemap);
	hotKeys[HotKey_RevertAdv].fFillFromDef = false;

	hotKeys[HotKey_RevertCycleAdv].def = CHotKey(c_capsRemap, VK_SHIFT);
	hotKeys[HotKey_RevertCycleAdv].fFillFromDef = false;

	hotKeys[HotKey_ChangeLayoutCycle].def = CHotKey(VK_LSHIFT, VK_RSHIFT);
	hotKeys[HotKey_ChangeLayoutCycle].fFillFromDef = false;

	hotKeys[HotKey_ChangeSetLayout_1].def = CHotKey().Add(VK_LSHIFT).Add(VK_RSHIFT).SetLeftRightMode(true);
	hotKeys[HotKey_ChangeSetLayout_1].fFillFromDef = false;

	hotKeys[HotKey_ChangeSetLayout_2].def = CHotKey().Add(VK_RSHIFT).Add(VK_LSHIFT).SetLeftRightMode(true);
	hotKeys[HotKey_ChangeSetLayout_2].fFillFromDef = false;

	hotKeys[HotKey_ChangeSetLayout_3].def = CHotKey();
	hotKeys[HotKey_ChangeSetLayout_3].fFillFromDef = false;

	hotKeys[HotKey_CapsGenerate].def = CHotKey(c_capsRemap, VK_CONTROL);
	hotKeys[HotKey_CapsGenerate].fFillFromDef = false;

	hotKeys[HotKey_RevertCycle].def = CHotKey(VK_SHIFT, VK_PAUSE);

	hotKeys[Hotkey_RevertSel].def = CHotKey(VK_SHIFT, VK_SCROLL);
}


SettingsGui::~SettingsGui()
{
}

void SettingsGui::Load()
{


	boost::property_tree::wptree pt;
	std::wifstream file(m_sIniPath);
	boost::property_tree::ini_parser::read_ini(file, pt);

	isMonitorAdmin = pt.get_optional<bool>(L"monitorAdmin").get_value_or(bool(false));
	isAddToTray = pt.get_optional<bool>(L"addToTray").get_value_or(bool(true));
	isTryOEM2 = pt.get_optional<bool>(L"OEM2").get_value_or(bool(false));
	isDashSeparate = pt.get_optional<bool>(L"isDashSeparate").get_value_or(bool(false));
	//isAltCapsGen = pt.get_optional<bool>(L"AltCapsGen").get_value_or(bool(false));
	isEnabledSaved = pt.get_optional<bool>(L"enable").get_value_or(bool(false));
	fHookDll = pt.get_optional<bool>(L"fHookDll").get_value_or(bool(true));
	fDbgMode = pt.get_optional<bool>(L"fDbgMode").get_value_or(bool(false));
	bootTime = pt.get_optional<TUInt64>(L"boottime").get_value_or(0);
	capsRemapApply = pt.get_optional<TKeyCode>(L"capsRemapApply").get_value_or(0);
	idLang = (SwLang) pt.get_optional<int>(L"idLang").get_value_or(int(SLANG_UNKNOWN));

	for (int i = 0; i < SW_ARRAY_SIZE(hotKeys); ++i)
	{
		std::wstring sName = L"hotkey";
		sName += std::to_wstring(i);
		CHotKeySet& set = hotKeys[i];
		TUInt64 def = set.fFillFromDef ? set.def.AsUInt64() : 0; 
		set.key.AsUInt64() = pt.get_optional<TUInt64>(sName).get_value_or(def);
	}
	for (int i = 0; i < SW_ARRAY_SIZE(hkl_lay); ++i)
	{
		HKL& cur = hkl_lay[i];
		std::wstring sName = L"hkl";
		sName += std::to_wstring(i);
		cur = (HKL)pt.get_optional<TUInt64>(sName).get_value_or(0);
	}

	SetLogLevelBySettings();

}
void SettingsGui::Save()
{
	boost::property_tree::wptree pt;

	pt.put(L"monitorAdmin", isMonitorAdmin);
	pt.put(L"addToTray", isAddToTray);
	pt.put(L"OEM2", isTryOEM2);
	pt.put(L"fHookDll", fHookDll);
	pt.put(L"fDbgMode", fDbgMode);
	pt.put(L"isDashSeparate", isDashSeparate);
	//pt.put(L"AltCapsGen", isAltCapsGen);
	pt.put(L"enable", isEnabledSaved);
	pt.put(L"boottime", bootTime);
	pt.put(L"capsRemapApply", capsRemapApply);
	pt.put(L"idLang", (int)idLang);

	for (int i = 0; i < SW_ARRAY_SIZE(hotKeys); ++i)
	{
		std::wstring sName = L"hotkey";
		sName += std::to_wstring(i);

		pt.put(sName, hotKeys[i].key.AsUInt64());
	}

	for (int i = 0; i < SW_ARRAY_SIZE(hkl_lay); ++i)
	{
		HKL& cur = hkl_lay[i];
		std::wstring sName = L"hkl" + std::to_wstring(i);
		pt.put(sName, (TUInt64)cur);
	}

	std::wofstream file(m_sIniPath);

	boost::property_tree::ini_parser::write_ini(file, pt);
}
