#include "grfw_shared.h"
#include "grfw_basicapplication.h"
#include "grfw_content.h"
#include "grfwu_reporting.h"
#include "grfwu_logging.h"
#include "grfwu_exception.h"
#include <ctime>
#include "..\resource.h"

GRAPHOUNY_NAMESPACE_FRAMEWORK{
	BasicApplication::~BasicApplication()
	{
		// Wait for the GPU to be done with all resources.
		if (m_hFenceEvent)
			CloseHandle(m_hFenceEvent);

		// Close logger
		if (m_pLogger)
		{
			m_pLogger->Close();
			delete m_pLogger;
		}
	}
		

	void BasicApplication::Run(HINSTANCE hInst, i32 cmdShow, std::wstring title)
	{
		try
		{
			Init_PreInit();
			LogLn(L"[DBG]: Engine started!");
			Init_LoadConfig(GetConfigPath());
			Init_CreateWindow(hInst, cmdShow, title);
			Init_DirectX12();

			Initialize();
			LoadContent();
		}
		catch (GrBasicException ex)
		{
			auto rep = g_pGlobals->CurrentReporter();
			if (!rep) 
				throw ex;
			rep->ReportError(ex.ToString());
		}		

		auto oClock = clock();
		MSG msg = { 0 };
		f32 m_fTargetDelay = 1.0f / 60.0f;
		m_timeLastUpdate = time(nullptr);
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				auto now = time(nullptr);
				f32 delta = static_cast<f32>(clock() - oClock) / 1000.0f;
				oClock = clock();

				Update(delta);
				m_timeLastUpdate = time(nullptr);
				Render_Before();
				Render(delta);
				Render_After();
			}
		}

		LogLn(L"[DBG]: Engine closed!");
	}

	void BasicApplication::SetIcon(std::wstring path)
	{
		auto hIcon = LoadImage(m_hWindowsInstance, path.data(), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
		if (hIcon) {
			//Change both icons to the same icon handle.
			SendMessage(m_hWindowHandle, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(m_hWindowHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			//This will ensure that the application icon gets changed too.
			SendMessage(GetWindow(m_hWindowHandle, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(GetWindow(m_hWindowHandle, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}
	}

	void BasicApplication::SetIcon(i32 IDI)
	{
		auto hIcon = LoadIcon(m_hWindowsInstance, MAKEINTRESOURCE(IDI));
		if (hIcon) {
			//Change both icons to the same icon handle.
			SendMessage(m_hWindowHandle, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(m_hWindowHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

			//This will ensure that the application icon gets changed too.
			SendMessage(GetWindow(m_hWindowHandle, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			SendMessage(GetWindow(m_hWindowHandle, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}
	}

	void BasicApplication::Init_PreInit()
	{
		// Init content manager
		m_pContentManager = InitContentManager();

		// Init logger
		m_pLogger = new Logger();
		m_pLogger->SetUp(m_pContentManager->GetPath_LogsDir() / GetLogFileName());

		// Init reporter
		m_pReporter = new LoggerMessageBoxReporter(nullptr, m_pLogger);

		// Set up globals
		g_pGlobals->RegisterApp(this, m_pReporter);

		// Load all SMC files (initialize filesystem)
		m_pContentManager->Init();
	}

	void BasicApplication::Init_LoadConfig(std::wstring filename)
	{
		// TODO: Load config file
	}

	void BasicApplication::Init_CreateWindow(HINSTANCE hInst, i32 cmdShow, std::wstring title)
	{
		// Запоминаем hInstance
		m_hWindowsInstance = hInst;

		m_pWindowFullRect = new RECT();
		m_pWindowClientRect = new RECT();

		// Создаём и регистрируем класс окна
		WNDCLASSEX wndClass = { 0 };
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = &WindowProcessor;
		wndClass.hInstance = hInst;
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = WND_CLASSNAME;

		if (!RegisterClassEx(&wndClass))
			throw GrBasicException(L"Failed to register window class");		

		RECT wRect = { 0, 0, (i32)GetTCFGD(u32, "width", 800), (i32)GetTCFGD(u32, "height", 600) };
		AdjustWindowRect(&wRect, WS_OVERLAPPEDWINDOW, FALSE);

		m_hWindowHandle = CreateWindow(WND_CLASSNAME, title.data(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			wRect.right - wRect.left,
			wRect.bottom - wRect.top,
			nullptr, nullptr, hInst, nullptr);

		if (!m_hWindowHandle)
			throw GrBasicException(L"Failed to create window");
		m_bWindowIsCreated = true;

		ShowWindow(m_hWindowHandle, cmdShow);
		UpdateWindow(m_hWindowHandle);

		GetWindowRect(m_hWindowHandle, m_pWindowFullRect);
		GetClientRect(m_hWindowHandle, m_pWindowClientRect);
		UpdateMouseData(true);
	}

	ContentManager* BasicApplication::InitContentManager()
	{
		auto pContent = new ContentManager(g_pGlobals->LaunchPath());
		return pContent;
	}

	std::wstring BasicApplication::GetLogFileName() const
	{ return L"grlog_" + FormLogNumber() + L".log"; }

	std::wstring BasicApplication::GetConfigPath() const
	{ return m_pContentManager->GetBasePath() / L"settings.cfg"; }
	
	void BasicApplication::UpdateMouseData(bool firstTime)
	{
		if (!firstTime)
		{
			m_MouseData.m_pntPrevCursorLocation = m_MouseData.m_pntCursorLocation;
			m_MouseData.m_vCursorPrevPosPercents = m_MouseData.m_vCursorPosPercents;
		}
		GetCursorPos(&(m_MouseData.m_pntCursorLocation));
		auto loc = m_MouseData.m_pntCursorLocation;
		auto crect = *(m_pWindowClientRect);
		auto pos = m_MouseData.m_vCursorPosClient = v2f((f32)(loc.x - crect.left), (f32)(loc.y - crect.top));
		auto crectf = v2f(GetWidthF(), GetHeightF());
		m_MouseData.m_vCursorPosPercents = pos / crectf;
		m_MouseData.m_bIsInClient = pos.x >= 0 && pos.y >= 0 && pos.x <= 1 && pos.y <= 1;
		if (firstTime)
		{
			m_MouseData.m_pntPrevCursorLocation = m_MouseData.m_pntCursorLocation;
			m_MouseData.m_vCursorPrevPosPercents = m_MouseData.m_vCursorPosPercents;
		}
		m_MouseData.m_vCursorDelta = m_MouseData.m_vCursorPosPercents - m_MouseData.m_vCursorPrevPosPercents;
	}

	LRESULT BasicApplication::WindowProcessor(HWND hwnd, u32 message, WPARAM wParam, LPARAM lParam)
	{
		auto app = g_pGlobals->CurrentApp();
		PAINTSTRUCT paintStruct;
		HDC hDC;
		//	eKeyCodes KeyIndex = static_cast<eKeyCodes>(0x00);
		//	wchar_t buffer[1];

		GetKeyState(0);
		switch (message)
		{
		case WM_PAINT:
			hDC = BeginPaint(hwnd, &paintStruct);
			EndPaint(hwnd, &paintStruct);
			GetWindowRect(hwnd, app->m_pWindowFullRect);
			GetClientRect(hwnd, app->m_pWindowClientRect);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_MOUSEWHEEL:
			app->OnMouseWheel((f32)GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA);
			break;

		case WM_MOUSEMOVE:
			app->UpdateMouseData();
			app->OnMouseMove();
			break;

		case WM_KEYDOWN:
			app->OnKeyPressed(KeyEventArgs(wParam));
			break;
		case WM_KEYUP:
			app->OnKeyReleased(KeyEventArgs(wParam));
			break;
		case WM_CHAR:
			app->OnChar((wchar_t)wParam);
			break;

		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}

		return 0;
	}

} GRAPHOUNY_NAMESPACE_END