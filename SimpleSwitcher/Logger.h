#pragma once
#include <fstream>
#include <clocale>
#include "Errors.h"


class SwLogger
{
public:
	static SwLogger&  Get() 
	{
		static SwLogger logger;
		return logger;
	}
	//template<class T>
	//SwLogger& operator<<(const T& sMessage)
	//{
	//	if(fp)
	//	{
	//		
	//	}
	//	return *this;
	//}
	void VFormat(const TChar* format, va_list args)
	{
		if(fp)
			vfwprintf_s(fp, format, args);
	}
	//void VFormat(const char* format, va_list args)
	//{
	//	if (fp)
	//		vfprintf_s(fp, format, args);
	//}
	void Format(const TChar* Format, ...)
	{
		va_list alist;
		va_start(alist, Format);
		VFormat(Format, alist);
		va_end(alist);
	}
	//void Format(const char* Format, ...)
	//{
	//	va_list alist;
	//	va_start(alist, Format);
	//	VFormat(Format, alist);
	//	va_end(alist);
	//}
	void EndLine()
	{
		if(fp)
		{
			fwprintf_s(fp, L"\n");
			fflush(fp);
		}
		//fout << std::endl;
	}
	~SwLogger()
	{
		if(fp)
		{
			fclose(fp);
		}
	}
private:
	//std::wofstream fout;
	FILE* fp = NULL;
	FILE* fp_char = NULL;
	SwLogger()
	{
		//std::setlocale(LC_ALL, "en_US.UTF-8");
		//std::locale::global(std::locale("en_US.UTF-8"));
		//if (GetCurrentProcessId() == 3176)		MessageBox(0, L"1", L"1", 0);
		TChar folder[0x1000];
		if (!GetModuleFileNameEx(GetCurrentProcess(), GetCurrentModule(), folder, sizeof(folder)))
			return;
		//if (GetCurrentProcessId() == 3176)		MessageBox(0, L"1", L"1", 0);
		TChar* sLast = wcsrchr(folder, L'\\');
		if(sLast)
			*sLast = 0;
		//if (GetCurrentProcessId() == 3176)		MessageBox(0, L"1", L"1", 0);
		wcscat_s(folder, L"\\log");
		CreateDirectory(folder, NULL);
		TChar base[512];
		base[0] = 0;
		GetModuleBaseName(GetCurrentProcess(), NULL, base, sizeof(base));
		//wcscat_s(base, GetCommandLine());
		TChar sLogPath[0x1000];
		swprintf_s(sLogPath, L"%s\\%s(%d)_%u.log", folder, base, GetCurrentProcessId(), GetTickCount());
		//if (GetCurrentProcessId() == 3176)	MessageBox(0, folder, folder, 0);
		//_wfopen_s(&fp, sLogPath, L"wt, ccs=UTF-8");
		fp = _wfsopen(sLogPath, L"wt, ccs=UTF-8", _SH_DENYNO);

		//fout.open(sLogPath, std::fstream::in | std::fstream::out | std::fstream::app);
	}

};

inline SwLogger& SwLoggerGlobal() {return SwLogger::Get();}

enum TLogLevel
{
	LOG_LEVEL_0 = 0,  // No log
	LOG_LEVEL_1 = 1,  // log only place error occupies and error handles
	LOG_LEVEL_2 = 2,  // completely stack trace, debug message and so.

	LOG_LEVEL_ERROR = LOG_LEVEL_1,
};

inline TLogLevel GetLogLevel()
{
#ifdef _DEBUG
	return LOG_LEVEL_2;
#else
	return LOG_LEVEL_0;
#endif
}

//template<class TFormat>
inline void __SW_LOG_FORMAT_V__(const TChar* format, va_list alist)
{
	SwLoggerGlobal().VFormat(format, alist);
}
inline void __SW_LOG_TIME()
{
	SYSTEMTIME ST;
	::GetLocalTime(&ST);
	SwLoggerGlobal().Format(
		L"%02u/%02u:%02u:%02u.%03u-%05u ",
		(TUInt32)ST.wDay, 
		(TUInt32)ST.wHour, 
		(TUInt32)ST.wMinute, 
		(TUInt32)ST.wSecond, 
		(TUInt32)ST.wMilliseconds,
		GetCurrentThreadId());
}
//template<class TFormat>
inline void __SW_LOG_FORMAT__(const TChar* Format, ...)
{
	va_list alist;
	va_start(alist, Format);
	__SW_LOG_FORMAT_V__(Format, alist);
	va_end(alist);
}
//template<class TFormat>
inline void SW_LOG_INFO_DEBUG(const TChar* Format, ...)
{
	if (GetLogLevel() >= LOG_LEVEL_2)
	{
		__SW_LOG_TIME();
		va_list alist;
		va_start(alist, Format);
		SwLoggerGlobal().VFormat(Format, alist);
		va_end(alist);
		SwLoggerGlobal().EndLine();
	}
}

#define SW_RETURN_SUCCESS {return SW_ERR_SUCCESS; }
inline bool SW_SUCCESS(TStatus stat) { return stat == SW_ERR_SUCCESS; }
inline bool SW_ERROR(TStatus stat) { return !SW_SUCCESS(stat); }

//struct SwErrBase
//{
//	operator bool() const
//	{
//		return IsErr();
//	}
//};

struct WinErrBOOL
{
	BOOL res;
	WinErrBOOL(BOOL r): res(r)  {}
	void Log()	
	{
		DWORD dwErr = GetLastError();
		__SW_LOG_FORMAT__(L"WinErr=%d ", dwErr);

		//SwLoggerGlobal() << L"fff";

		CAutoWinMem lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		TChar* sMessage = (TChar*)lpMsgBuf.Get();
		__SW_LOG_FORMAT__(L"%s", sMessage);

		//SwLoggerGlobal() << L"fff";
	}
	operator bool() const { return res == FALSE; }
	TStatus ToTStatus()	{	return SW_ERR_WINAPI;	}
};

struct WinErrDwordWait
{
	DWORD res;
	WinErrDwordWait(DWORD r) : res(r)  {}
	void Log()	{
		CAutoWinMem lpMsgBuf;
		DWORD dwErr = GetLastError();
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		TChar* sMessage = (TChar*)lpMsgBuf.Get();
		__SW_LOG_FORMAT__(L"Wait result=%d LastErr=%d %s", res, dwErr, sMessage);
	 }
	operator bool() const { return res != WAIT_OBJECT_0; }
	TStatus ToTStatus()	{ return SW_ERR_WAIT_PROCESS; }
};

struct SwErrTStatus
{
	TStatus res;
	SwErrTStatus(TStatus r): res(r) {}
	void Log()	{ __SW_LOG_FORMAT__(L"TStatus=%s(%d)", c_StatusNames[res], res); }
	operator bool() const { return SW_ERROR(res); }
	TStatus ToTStatus()	{ return res; }
};

struct WinErrLSTATUS
{
	LSTATUS res;
	WinErrLSTATUS(LSTATUS r) : res(r) {}
	void Log()	{ __SW_LOG_FORMAT__(L"LSTATUS=%d", res); }
	bool IsError() const { return res != ERROR_SUCCESS; }
	operator bool() const { return IsError();  }
	TStatus ToTStatus()	{ return SW_ERR_WINAPI; }
};

struct WinErrHRESULT
{
	HRESULT res;
	WinErrHRESULT(HRESULT r) : res(r) {}
	void Log()	{ __SW_LOG_FORMAT__(L"HResult=%d(0x%x)", res, res); }
	operator bool() const { return FAILED(res); }
	TStatus ToTStatus()	{ return SW_ERR_HRESULT; }
};




template<class T>
inline void __Log_Err_Common(T err, const char* file, int line, const wchar_t* format = nullptr, ...)
{
	if(GetLogLevel() < LOG_LEVEL_2)
		return;
	__SW_LOG_TIME();
	err.Log();
	__SW_LOG_FORMAT__(L" %S(%d) ", file, line);
	if (format)
	{
		va_list alist;
		va_start(alist, format);
		__SW_LOG_FORMAT_V__(format, alist);
		va_end(alist);
	}
	SwLoggerGlobal().EndLine();
}

#define _LOG_ERR(ERR, ...) __Log_Err_Common(ERR, __FILE__, __LINE__, __VA_ARGS__)

#define  _SW_ERR_RET(ClassName, X, ...) {if (ClassName __res = (X)) { _LOG_ERR(__res, __VA_ARGS__); return __res.ToTStatus(); } }
#define  _SW_ERR_LOG(ClassName, X, ...) {if (ClassName __res = (X))   _LOG_ERR(__res, __VA_ARGS__); }


#define SW_WINBOOL_RET(X, ...) _SW_ERR_RET(WinErrBOOL, X, __VA_ARGS__)
#define SW_WINBOOL_LOG(X, ...) _SW_ERR_LOG(WinErrBOOL, X, __VA_ARGS__)

#define SW_TSTATUS_RET(X, ...) _SW_ERR_RET(SwErrTStatus, X, __VA_ARGS__)
#define SW_TSTATUS_LOG(X, ...) _SW_ERR_LOG(SwErrTStatus, X, __VA_ARGS__)

#define SW_LSTATUS_RET(X, ...) _SW_ERR_RET(WinErrLSTATUS, X, __VA_ARGS__)
#define SW_LSTATUS_LOG(X, ...) _SW_ERR_LOG(WinErrLSTATUS, X, __VA_ARGS__)

#define SW_HRESULT_RET(X, ...) _SW_ERR_RET(WinErrHRESULT, X, __VA_ARGS__)
#define SW_HRESULT_LOG(X, ...) _SW_ERR_LOG(WinErrHRESULT, X, __VA_ARGS__)

#define SW_WAITDWORD_RET(X, ...) _SW_ERR_RET(WinErrDwordWait, X, __VA_ARGS__)
#define SW_WAITDWORD_LOG(X, ...) _SW_ERR_LOG(WinErrDwordWait, X, __VA_ARGS__)
