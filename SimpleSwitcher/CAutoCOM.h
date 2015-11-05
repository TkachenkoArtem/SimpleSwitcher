#pragma once
#include "Objbase.h"

class CAutoCOMInitialize
{
public:
	CAutoCOMInitialize()
	{
		m_result = CoInitializeEx(0, COINIT_MULTITHREADED);
	}
	~CAutoCOMInitialize()
	{
		if (m_result == S_OK)
		{
			CoUninitialize();
		}
	}
private:
	HRESULT m_result = NULL;
};



