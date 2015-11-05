#pragma once

#define SW_NAMESPACE(X) namespace X {
#define SW_NAMESPACE_END }

// Base functions

#define SW_ARRAY_SIZE(V)		(sizeof(V) / sizeof(V[0]))
#define SW_ARRAY_END(V)		(V + sizeof(V) / sizeof(V[0]))
#define SW_STRING_LENGTH(V)	(sizeof(V) / sizeof(V[0]) - 1)
#define SW_ARRAY_ZEROEND(V)	(V[(sizeof(V) / sizeof(V[0])) - 1] = 0)

typedef unsigned __int64 TUInt64;
typedef signed   __int64 TInt64;
typedef unsigned __int32 TUInt32;
typedef signed   __int32 TInt32;
typedef unsigned __int16 TUInt16;
typedef signed   __int16 TInt16;
typedef unsigned __int8  TUInt8;
typedef signed   __int8  TInt8;

enum TSWBit
{
	SW_BIT_32,
	SW_BIT_64,
};

typedef TCHAR TChar;

template <typename T1, typename T2>
inline bool TestMask(T1 V, T2 M)
{
	return M == (V & M);
}

template <typename T1, typename T2>
inline bool TestMaskAny(T1 V, T2 M)
{
	return (V & M) != 0;
}

template <typename T1, typename T2>
inline bool TestFlag(T1 V, T2 F)
{
	return (V & F) != 0;
}

template <typename T>
inline void SetFlag(T& V, TUInt32 F)
{
	V |= F;
}

HMODULE GetCurrentModule();

inline bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
	OSVERSIONINFOEXW osvi = { sizeof(osvi), 0, 0, 0, 0, { 0 }, 0, 0 };
	DWORDLONG        const dwlConditionMask = VerSetConditionMask(
		VerSetConditionMask(
		VerSetConditionMask(
		0, VER_MAJORVERSION, VER_GREATER_EQUAL),
		VER_MINORVERSION, VER_GREATER_EQUAL),
		VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

	osvi.dwMajorVersion = wMajorVersion;
	osvi.dwMinorVersion = wMinorVersion;
	osvi.wServicePackMajor = wServicePackMajor;

	return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

inline bool
IsWindowsVistaOrGreater()
{
	return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}


bool IsWindows64();

inline bool IsSelf64()
{
#ifdef _WIN64
	return true;
#elif _WIN32
	return false;
#else
	!!ERROR!!
#endif
}

template<class T>
void SwZeroMemory(T& t)
{
	ZeroMemory(&t, sizeof(T));
}