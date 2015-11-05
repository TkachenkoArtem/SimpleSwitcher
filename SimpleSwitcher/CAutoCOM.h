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

//#define MAKE_AUTO_CLEANUP_COM(X_TYPE, X_NAME)  class TCleaunDescription_ ## X_NAME {\
//public: \
//	static bool IsValid(X_TYPE* obj)	{ return obj != NULL; } \
//	static void ToNULL(X_TYPE*& obj)	{ obj = NULL; } \
//	static void CleanupFunction(X_TYPE* obj) { obj->Release(); } \
//}; \
//	typedef TemplateAutoClose<X_TYPE*, TCleaunDescription_ ## X_NAME> X_NAME;

