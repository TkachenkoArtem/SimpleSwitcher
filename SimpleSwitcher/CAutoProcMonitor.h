
struct TSWCheckRunRes
{
	bool found = false;
	bool admin = false;
};

class CAutoProcMonitor
{
public:
	~CAutoProcMonitor()
	{
		if(m_autoClose)
		{
			SW_TSTATUS_LOG(Stop());
		}
	}

	CAutoProcMonitor(const TChar* sWndName, const TChar* sCmd, TSWBit bit,  bool autoClose) :
		m_sWndName(sWndName), 
		m_sCmd(sCmd), 
		m_autoClose(autoClose),
		m_bit(bit)
	{
	}
	TStatus Stop()
	{
		HWND hwnd = FindWindow(m_sWndName, NULL);

		if(!hwnd)
			SW_RETURN_SUCCESS;

		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		DWORD desireAccess = SYNCHRONIZE;
		CAutoHandle hProc = OpenProcess(SYNCHRONIZE, FALSE, pid);
		SW_WINBOOL_RET(hProc.IsValid());
		PostMessage(hwnd, c_MSG_Quit, 0, 0);
		SW_WAITDWORD_RET(WaitForSingleObject(hProc, c_nCommonWaitProcess));

		SW_RETURN_SUCCESS;
	}
	TSWCheckRunRes CheckRunning()
	{
		TSWCheckRunRes res;

		HWND hwnd = FindWindow(m_sWndName, NULL);

		if (!hwnd)
			return res;

		res.found = true;

		DWORD pid = 0;
		GetWindowThreadProcessId(hwnd, &pid);
		if (IsElevated(pid))
			res.admin = true;

		return res;
	}
	TStatus EnsureStarted(TSWAdmin admin)
	{
		TSWCheckRunRes res = CheckRunning();

		if(res.found)
		{
			bool fCheckAdmin = (admin == SW_ADMIN_ON);
			if(admin == SW_ADMIN_SELF)
			{
				fCheckAdmin = IsSelfElevated();
			}

			if(fCheckAdmin != res.admin)
			{
				SW_TSTATUS_RET(Stop());
			}
			else
			{
				SW_RETURN_SUCCESS;
			}
		}

		// hack for WaitInputIdle
		if (m_bit == SW_BIT_32 && admin != SW_ADMIN_SELF)
		{
			bool selfAdmin = IsSelfElevated();
			if ((!selfAdmin && admin == SW_ADMIN_ON) || selfAdmin && admin == SW_ADMIN_OFF)
			{
				SW_TSTATUS_RET(SwCreateProcessOurWaitFinished(
					c_sArgStarter,
					m_bit,
					admin));
				SW_RETURN_SUCCESS;
			}
		}

		SW_TSTATUS_RET(SwCreateProcessOurWaitIdle(
			m_sCmd,
			m_bit,
			admin));

		SW_RETURN_SUCCESS;
	}
private:
	const TChar* m_sWndName = 0;
	const TChar* m_sCmd = 0;
	TSWBit m_bit = SW_BIT_32;
	bool m_autoClose = false;

};