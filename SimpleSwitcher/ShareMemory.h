#pragma once

//static const TChar c_ShareMemoryNameObj[] = ;

//static const size_t c_ShareMemorySize = 1024;



template<class T>
class CShareMemory
{
public:

	CShareMemory(){}

	TStatus Init(const TChar* name, bool& exist)
	{
		m_Mapping = CreateFileMapping(
			INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			(DWORD)sizeof(T),
			name);
		exist = (GetLastError() == ERROR_ALREADY_EXISTS);

		SW_WINBOOL_RET(m_Mapping.IsValid());

		m_buf = (PBYTE)MapViewOfFile(
			m_Mapping,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			sizeof(T));
		SW_WINBOOL_RET(m_buf != NULL);

		SW_RETURN_SUCCESS;
	}


	T& Get() { return *(T*)m_buf; }
	T* operator->() { return (T*)m_buf; }
private:
	PBYTE m_buf = NULL;
	CAutoHandle m_Mapping;



};