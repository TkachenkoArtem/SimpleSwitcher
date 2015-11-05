#pragma once
#include <deque>
#include <vector>

enum  InputSendPos
{
	//INPUTSEND_FRONT,
	INPUTSEND_BACK
};
struct InputSender
{
private:
	//typedef std::deque<INPUT> TInputList;
	typedef std::vector<INPUT> TInputList;
	TInputList list;
public:

	TStatus Send()
	{
		if(list.empty())
			SW_RETURN_SUCCESS;
		SW_WINBOOL_RET(SendInput((UINT)list.size(), &list[0], sizeof(INPUT)) == list.size());
		SW_RETURN_SUCCESS;
	}
	void Clear()
	{
		list.clear();
	}
	TStatus Add(TKeyCode key, KeyState state, InputSendPos pos = INPUTSEND_BACK)
	{
		if(key == 0)
			SW_RETURN_SUCCESS;

		INPUT cur = {0};
		cur.type = INPUT_KEYBOARD;
		cur.ki.wVk = key;
		if (state == KEY_STATE_UP)
			cur.ki.dwFlags = KEYEVENTF_KEYUP;
		if(pos == INPUTSEND_BACK)
			list.push_back(cur);
		else
		{
			SW_TSTATUS_RET(SW_ERR_UNSUPPORTED);
		}
		SW_RETURN_SUCCESS;
	}
	TStatus AddDown(CHotKey& key, InputSendPos pos)
	{
		if(key.Size() == 0)
			SW_RETURN_SUCCESS;
		for(TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
		{
			SW_TSTATUS_RET(Add(*k, KEY_STATE_DOWN, pos));
		}
		SW_TSTATUS_RET(Add(key.ValueKey(), KEY_STATE_DOWN, pos));
		SW_RETURN_SUCCESS;
	}
	TStatus AddUp(CHotKey& key, InputSendPos pos = INPUTSEND_BACK)
	{
		if (key.Size() == 0)
			SW_RETURN_SUCCESS;
		SW_TSTATUS_RET(Add(key.ValueKey(), KEY_STATE_UP, pos));
		for (TKeyCode* k = key.ModsBegin(); k != key.ModsEnd(); ++k)
		{
			SW_TSTATUS_RET(Add(*k, KEY_STATE_UP, pos));
		}
		SW_RETURN_SUCCESS;
	}
	TStatus AddPress(CHotKey& key, InputSendPos pos = INPUTSEND_BACK)
	{
		SW_TSTATUS_RET(AddDown(key, pos));
		SW_TSTATUS_RET(AddUp(key, pos));
		SW_RETURN_SUCCESS;
	}
};