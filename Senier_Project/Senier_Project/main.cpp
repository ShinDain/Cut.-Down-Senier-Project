#include "SenierProjectApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE previnstance, PSTR cmdLine, int showCmd)
{
	// 메모리 누수 탐지
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	if (S_OK != CoInitializeEx(NULL, COINIT_MULTITHREADED))
		return 0;

	try
	{
		SenierProjectApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;
		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR_Failed", MB_OK);
		return 0;
	}

	CoUninitialize();

}