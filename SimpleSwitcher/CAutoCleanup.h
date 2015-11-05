#pragma once

template<class TType, class TTypeDescription>
class TemplateAutoClose
{
private:

	TType m_obj;

	typedef TemplateAutoClose<TType, TTypeDescription> TThis;
	void Set(const TType& other)
	{
		Cleanup();
		m_obj = other;
	}
	void ObjToNull() { TTypeDescription::ToNULL(m_obj); }

	TemplateAutoClose(const TThis& other);

public:

	TemplateAutoClose() { ObjToNull(); }
	TemplateAutoClose(const TType& obj) : m_obj(obj) { }
	~TemplateAutoClose() { Cleanup(); }

	const TType& operator=(const TType& obj)
	{
		Set(obj);
		return m_obj;
	}
	const TType& operator=(const TThis& other)
	{
		Set(other.m_obj);
		return m_obj;
	}

	bool IsValid() const { return TTypeDescription::IsValid(m_obj); }
	bool IsInvalid() const { return !IsValid(); }

	operator bool() const
	{
		return IsValid();
	}
	operator TType() const	{return m_obj;	}
	TType Get() const { return m_obj; }
	//void** PPVoid() {return (void**)&m_obj;}
	operator TType*() const	{	return &m_obj;	}
	TType* operator &()	{return &m_obj;	}
	TType operator->() { return m_obj; }
	void Cleanup()
	{
		if (IsValid())
		{
			TTypeDescription::CleanupFunction(m_obj);
			TTypeDescription::ToNULL(m_obj);
		}
	}
	TType Detach()
	{
		TType obj = m_obj;
		ObjToNull();
		return obj;
	}
};

#define MAKE_AUTO_CLEANUP(X_TYPE, X_FUNC, X_NULL, X_NAME)  class TCleaunDescription_ ## X_NAME {\
public: \
	static bool IsValid(const X_TYPE& obj)	{ return obj != X_NULL;} \
	static void ToNULL(X_TYPE& obj)	{ obj = X_NULL; } \
	static void CleanupFunction(const X_TYPE& obj) { X_FUNC(obj); } \
}; \
typedef TemplateAutoClose<X_TYPE, TCleaunDescription_ ## X_NAME> X_NAME;



MAKE_AUTO_CLEANUP(HANDLE, ::CloseHandle, NULL, CAutoHandle)
MAKE_AUTO_CLEANUP(HANDLE, ::CloseHandle, INVALID_HANDLE_VALUE, CAutoHandle2)
MAKE_AUTO_CLEANUP(HHOOK, ::UnhookWindowsHookEx, NULL, CAutoHHOOK)
MAKE_AUTO_CLEANUP(HKEY, ::RegCloseKey, NULL, CAutoCloseHKey)
MAKE_AUTO_CLEANUP(HWINEVENTHOOK, ::UnhookWinEvent, NULL, CAutoHWINEVENTHOOK)
MAKE_AUTO_CLEANUP(HMODULE, ::FreeLibrary, NULL, CAutoHMODULE)
MAKE_AUTO_CLEANUP(SC_HANDLE, ::CloseServiceHandle, NULL, CAutoSCHandle)
MAKE_AUTO_CLEANUP(LPVOID, ::LocalFree, NULL, CAutoWinMem)
MAKE_AUTO_CLEANUP(LPVOID, ::GlobalUnlock, NULL, CAutoGlobalLock)

class CAutoWinEvent
{
	BOOL SetEvent(HANDLE evt)
	{
		Cleanup();
		BOOL res = ::SetEvent(evt);
		if(res)
			m_evt = evt;
	}
	~CAutoWinEvent()
	{
		Cleanup();
	}
	void Cleanup()
	{
		if (m_evt)
		{
			ResetEvent(m_evt);
			m_evt = nullptr;
		}
	}
private:
	HANDLE m_evt = nullptr;
};

class CAutoWinMutexWaiter
{
public:
	CAutoWinMutexWaiter() {}
	CAutoWinMutexWaiter(HANDLE mtx) 
	{
		Wait(mtx);
	}
	DWORD Wait(HANDLE mtx, DWORD nTimeOut)
	{
		Cleanup();
		DWORD res = WaitForSingleObject(mtx, nTimeOut);
		if(res == WAIT_OBJECT_0)
			m_mtx = mtx;
		return res;
	}
	void Wait(HANDLE mtx)
	{
		Wait(mtx, INFINITE);
	}
	~CAutoWinMutexWaiter()
	{
		Cleanup();
	}
	void Cleanup()
	{
		if(m_mtx)
		{
			ReleaseMutex(m_mtx);
			m_mtx = nullptr;
		}
	}
private:
	HANDLE m_mtx = nullptr;
};








