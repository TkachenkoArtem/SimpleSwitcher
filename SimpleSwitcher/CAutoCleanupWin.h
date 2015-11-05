#pragma once

class CAutoClipBoard
{
public:
	CAutoClipBoard() {}
	TStatus Open(HWND hwnd = NULL)
	{
		m_stat = OpenClipboard(hwnd);
		SW_WINBOOL_RET(m_stat);
		SW_RETURN_SUCCESS;
	}
	void Cleanup()
	{
		if (m_stat)
			CloseClipboard();
		m_stat = FALSE;
	}
	~CAutoClipBoard()
	{
		Cleanup();
	}
private:
	BOOL m_stat = FALSE;
};

class CAutoHotKey
{
public:
	BOOL Register(HWND hwnd, int id, UINT mods, DWORD key)
	{
		Cleanup();
		m_hwnd = hwnd;
		m_id = id;
		m_res = RegisterHotKey(hwnd, id, mods, key);
		return m_res;
	}
	~CAutoHotKey()
	{
		Cleanup();
	}
	void Cleanup()
	{
		if (m_res)
		{
			UnregisterHotKey(m_hwnd, m_id);
			m_res = FALSE;
		}
	}
private:
	BOOL m_res = FALSE;
	HWND m_hwnd;
	int m_id;
};

class CAutoThreadAttach
{
public:
	CAutoThreadAttach(){}
	TStatus Attach(DWORD id)
	{
		threadId = id;
		res = AttachThreadInput(threadId, GetCurrentThreadId(), TRUE);
		SW_WINBOOL_RET(res);
		SW_RETURN_SUCCESS;
	}
	~CAutoThreadAttach()
	{
		if(res)
		{
			SW_WINBOOL_LOG(AttachThreadInput(threadId, GetCurrentThreadId(), FALSE));
			res = 0;
		}
	}
private:
	BOOL res = false;
	DWORD threadId = 0;
};


