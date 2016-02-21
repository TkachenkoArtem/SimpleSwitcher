#pragma once

#include "swlocal.h"

typedef unsigned char TKeyCode;


inline const TChar* TKeyCodeName(TKeyCode key)
{
	switch (key)
	{
	case 0x41:			return L"A";
	case 0x42:			return L"B";
	case 0x43:			return L"C";
	case 0x44:			return L"D";
	case 0x45:			return L"E";
	case 0x46:			return L"F";
	case 0x47:			return L"G";
	case 0x48:			return L"H";
	case 0x49:			return L"I";
	case 0x4A:			return L"J";
	case 0x4B:			return L"K";
	case 0x4C:			return L"L";
	case 0x4D:			return L"M";
	case 0x4E:			return L"N";
	case 0x4F:			return L"O";
	case 0x50:			return L"P";
	case 0x51:			return L"Q";
	case 0x52:			return L"R";
	case 0x53:			return L"S";
	case 0x54:			return L"T";
	case 0x55:			return L"U";
	case 0x56:			return L"V";
	case 0x57:			return L"W";
	case 0x58:			return L"X";
	case 0x59:			return L"Y";
	case 0x5A:			return L"Z";
			
	case VK_SHIFT: return L"Shift";
	case VK_LSHIFT: return L"LShift";
	case VK_RSHIFT: return L"RShift";

	case VK_CONTROL: return L"Ctrl";
	case VK_LCONTROL: return L"LCtrl";
	case VK_RCONTROL: return L"RCtrl";

	case VK_MENU: return L"Alt";
	case VK_LMENU: return L"LAlt";
	case VK_RMENU: return L"RAlt";

	case VK_LWIN: return L"LWin";
	case VK_RWIN: return L"RWin";

	case VK_ESCAPE: return L"Esc";

	case VK_F1: return L"F1";
	case VK_F2: return L"F2";
	case VK_F3: return L"F3";
	case VK_F4: return L"F4";
	case VK_F5: return L"F5";
	case VK_F6: return L"F6";
	case VK_F7: return L"F7";
	case VK_F8: return L"F8";
	case VK_F9: return L"F9";
	case VK_F10: return L"F10";
	case VK_F11: return L"F11";
	case VK_F12: return L"F12";
	case VK_F13: return L"F13";
	case VK_F14: return L"F14";
	case VK_F15: return L"F15";
	case VK_F16: return L"F16";
	case VK_F17: return L"F17";
	case VK_F18: return L"F18";
	case VK_F19: return L"F19";
	case VK_F20: return L"F20";
	case VK_F21: return L"F21";
	case VK_F22: return L"F22";
	case VK_F23: return L"F23";
	case VK_F24: return L"F24";

	case VK_INSERT:	return L"Insert";
	case VK_DELETE:	return L"Delete";

	case VK_HOME:	return L"Home";
	case VK_END:	return L"End";

		//case VK_PGUP:	return "PgUp";

	case VK_PAUSE:	return L"Break";
	case VK_PRINT:	case VK_SNAPSHOT:	return L"PrintScreen";
	case VK_CANCEL:	return L"Cancel";

	case VK_SCROLL:	return L"ScrollLock";
	case VK_CAPITAL:	return L"CapsLock";

	case VK_NUMLOCK: return L"NumLock";

	case VK_SPACE: return L"Space";

	case VK_BACK: return L"Backspace";
	case VK_RETURN: return L"Enter";
	case VK_TAB: return L"Tab";

	default:return NULL;
	}
}

class CHotKey
{
	
public:
	CHotKey():value(0){}
	CHotKey(TKeyCode key)
	{
		Clear().Add(key);
	}
	CHotKey(TKeyCode key1, TKeyCode key2)
	{
		Clear().Add(key1).Add(key2);
	}
	CHotKey(TKeyCode key1, TKeyCode key2, TKeyCode key3)
	{
		Clear().Add(key1).Add(key2).Add(key3);
	}
	enum 
	{
		ADDKEY_NORMAL = 0,
		ADDKEY_ORDERED = 0x1,
		ADDKEY_ENSURE_ONE_VALUEKEY = 0x2,
	};
	CHotKey& Add(TKeyCode key, int flags = ADDKEY_NORMAL)
	{
		for (TKeyCode k : *this)
		{
			if (CompareKeys(k, key, true))
			{
				if (IsCommonMods(k) && !IsCommonMods(key))
				{
					// rewrite common key
					Remove(k);
					break;
				}
				else
				{
					// already exists
					return *this;
				}
			}
		}

		if (TestFlag(flags, ADDKEY_ENSURE_ONE_VALUEKEY) && !CHotKey::IsKnownMods(key))
		{
			RemoveAllNoMods();
		}

		if (TestFlag(flags, ADDKEY_ORDERED))
		{
			if (size < c_MAX)
				++size;
			for (int i = size - 1; i > 0; i--)
			{
				keys[i] = keys[i - 1];
			}
			keys[0] = key;
		}
		else
		{
			if (size == 0)
			{
				keys[size++] = key;
			}
			else
			{
				if (IsKnownMods(keys[0]))
				{
					InsertMods(keys[0]);
					keys[0] = key;
				}
				else
				{
					InsertMods(key);
				}
			}
		}
		return *this;
	}
	bool Remove(TKeyCode key)
	{
		if(size == 0)
			return false;
		bool found = false;
		for (int i = 0; i < size; i++)
		{
			if(CompareKeys(key, keys[i], true))
			{
				found = true;
				keys[i] = 0;
				continue;
			}
			if(found)
			{
				keys[i-1] = keys[i];
			}
		}
		if(found)
			--size;
		return found;
		
	}
	void RemoveAllNoMods()
	{
		TKeyCode k = 0;
		do
		{
			k = 0;
			for(TKeyCode cur: *this)
			{
				if(!IsKnownMods(cur))
				{
					k = cur;
					break;
				}
			}
			if(k)
			{
				Remove(k);
			}
		}while (k);
	}
	bool HasKey(TKeyCode key, bool leftrightCheck = false)
	{
		for(TKeyCode k : *this)
		{
			if(CompareKeys(k, key, leftrightCheck))
				return true;
		}
		return false;
	}
	bool HasAnyMod()
	{
		for (int i = 0; i < size; ++i)
		{
			if(IsKnownMods(keys[i]))
				return true;
		}
		return false;
	}
	TKeyCode At(int i) {return keys[i];}
	bool HasMod(TKeyCode k, bool leftrightCheck = false)
	{
		if(size <= 1)
			return false;
		for(int i = 1; i < size; ++i)
		{
			if (CompareKeys(k, keys[i], leftrightCheck))
				return true;
		}
		return false;
	}
	bool Compare(CHotKey& other, bool ignOrderValueKey = false)
	{
		if(size != other.size)
			return false;
		if(size == 0 || other.size == 0)
			return false;
		bool fCheckLeftRight = leftRightDifferene == 1 || other.leftRightDifferene == 1;
		if (ignOrderValueKey || ignoreOrderValueKey || other.ignoreOrderValueKey)
		{
			return CompareIgnoreOrder(keys, other.keys, size, fCheckLeftRight);
		}
		else
		{
			if (!CompareKeys(keys[0], other.keys[0], fCheckLeftRight))
				return false;
			return CompareIgnoreOrder(keys + 1, other.keys + 1, size - 1, fCheckLeftRight);
		}
	}
	bool IsEmpty(){return Size() == 0;}
	TKeyCode ValueKey()	{		return keys[0];	}
	TKeyCode* begin() {return keys;}
	TKeyCode* end() {return keys+size;}
	TKeyCode* ModsBegin()	{		return keys + 1;	}
	TKeyCode* ModsEnd(){		return keys + 1 + (size > 0 ? size-1 : 0);	}
	TUInt8 Size() {return size;}
	//TKeyCode operator[] (int i) { return keys[i]; }
	void ToString(std::wstring& s)
	{
		ToString(s, leftRightDifferene);
	}
	std::wstring ToString()
	{
		std::wstring s;
		ToString(s, leftRightDifferene);
		return s;
	}
	void ToString(std::wstring& s, bool leftrightDiff)
	{
		if(size == 0)
		{
			s = GetMessageById(AM_5);
		}
		else
		{
			for(int i = size - 1; i >= 0; --i)
			{
				TKeyCode k = !leftrightDiff ? Normalize(keys[i]) : keys[i];
				ToString(k, s);
				if(i != 0)
					s += L" + ";
			}
		}
	}
	static std::wstring ToString(TKeyCode key)
	{
		std::wstring s;
		ToString(key, s);
		return s;
	}
	static void ToString(TKeyCode key, std::wstring& s)
	{
		const wchar_t* sName = TKeyCodeName(key);
		if (sName)
		{
			s += sName;
		}
		else
		{
			s += L"VK_";
			s += std::to_wstring(key);
		}
	}
	TUInt64& AsUInt64() { return value; }
	static bool IsKnownMods(TKeyCode key)
	{
		switch (Normalize(key))
		{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
		case VK_LWIN:
			return true;
		}
		return false;
	}
	static bool IsRightMod(TKeyCode key)
	{
		switch (key)
		{
		case VK_RSHIFT:
		case VK_RCONTROL:
		case VK_RMENU:
		case VK_RWIN:
			return true;
		}
		return false;
	}
	CHotKey& Clear(){value = 0; return *this;}
	bool operator== ( CHotKey& other) {return Compare(other);}
	bool operator!= ( CHotKey& other) { return !(*this == other); }
	static TKeyCode Normalize(TKeyCode key)
	{
		switch (key)
		{
		case VK_RSHIFT:
		case VK_LSHIFT:
			return VK_SHIFT;
		case VK_RCONTROL:
		case VK_LCONTROL:
			return VK_CONTROL;
		case VK_RMENU:
		case VK_LMENU:
			return VK_MENU;
		case VK_RWIN:
			return VK_LWIN;
		default:
			return key;
		}
	}
	CHotKey& SetLeftRightMode(bool checkLeftRight)
	{
		leftRightDifferene = checkLeftRight;
		return *this;
	}
	bool GetLeftRightMode()
	{
		return leftRightDifferene;
	}
private:
	bool CompareIgnoreOrder(TKeyCode* list1, TKeyCode* list2, int size, bool checkLeftRight)
	{
		for(int i = 0; i < size; ++i)
		{
			TKeyCode k = list1[i];
			bool found = false;
			for (int j = 0; j < size; ++j)
			{
				if (CompareKeys(k, list2[j], checkLeftRight))
				{
					found = true;
					break;
				}
			}
			if(!found)
				return false;
		}
		return true;
	}
	void InsertMods(TKeyCode key)
	{
		keys[size++] = key;
		if(size >= c_MAX)
			size = c_MAX - 1;
	}


	bool CompareKeys(TKeyCode k1, TKeyCode k2, bool checkLeftRight)
	{
		TKeyCode k1norm = Normalize(k1);
		TKeyCode k2norm = Normalize(k2);

		if(k1norm != k2norm)
			return false;

		if(checkLeftRight)
		{
			if (IsCommonMods(k1) || IsCommonMods(k2))
				return true;
			return k1 == k2;
		}

		return true;
	}
	bool IsCommonMods(TKeyCode key)
	{
		switch (key)
		{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
			return true;
		}
		return false;
	}
	static const int c_MAX = 6;
	union
	{
		TUInt64 value;
		struct 
		{
			struct
			{
				TUInt8 ignoreOrderValueKey : 1;
				TUInt8 leftRightDifferene : 1;
			};
			TKeyCode size;
			TKeyCode keys[c_MAX];
		};
	};
};