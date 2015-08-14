#include <gr_shared.h>
#include <Windows.h>
#include <ctime>
#include "DemoApplication.h"

inline void InitRandom()
{
	srand((u32)time(nullptr));
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	InitRandom();
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// �������������� ������
	auto app = new DemoApplication();
	app->m_Config.Set(L"#TestKey", L"TextValue", true);
	app->Run(hInstance, cmdShow, L"�������� Demo");
	delete app;
	return 0;
}