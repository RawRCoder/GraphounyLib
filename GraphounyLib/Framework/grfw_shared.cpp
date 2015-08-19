#include <comdef.h>
#include "grfw_shared.h"
#include "grfwu_reporting.h"
#include "grfwu_logging.h"
#include "grfw_basicapplication.h"
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment( lib, "dxguid.lib")

GRAPHOUNY_NAMESPACE_FRAMEWORK{

	std::wstring Globals::LaunchPath()
	{ 
		if (!m_bLPGenerated)
		{
			auto drive = new wchar_t[260];
			auto name = &drive[4];
			GetModuleFileName(GetModuleHandle(nullptr), name, 256);
			auto temp = static_cast<LPCWSTR>(name);
			_wsplitpath_s(temp, drive, 3, name, 256, nullptr, 0, nullptr, 0);
			m_wsLaunchPath = lstrcatW(drive, static_cast<LPCWSTR>(name));
			SetCurrentDirectory(m_wsLaunchPath.data());
			m_bLPGenerated = true;
		}
		return m_wsLaunchPath;
	}

	
	void Log(std::wstring msg) { g_pGlobals->CurrentApp()->GetDefaultLogger()->Say(msg); }
	void LogLn(std::wstring msg) { g_pGlobals->CurrentApp()->GetDefaultLogger()->SayLn(msg); }
	void LogLn_HR(HRESULT hr, std::wstring msg)
	{
		_com_error err(hr);
		g_pGlobals->CurrentApp()->GetDefaultLogger()->SayLn(msg + L" " + err.ErrorMessage());
	}
	void ReportInfo(std::wstring title, std::wstring msg) { g_pGlobals->CurrentReporter()->ReportInfo(msg, title); }
	void ReportInfo(std::wstring msg) { ReportInfo(L"Info", msg); }
	void ReportWarning(std::wstring title, std::wstring msg) { g_pGlobals->CurrentReporter()->ReportWarning(msg, title); }
	void ReportWarning(std::wstring msg) { ReportWarning(L"Warning", msg); }
	void ReportError(std::wstring title, std::wstring msg) { g_pGlobals->CurrentReporter()->ReportError(msg, title); }
	void ReportError(std::wstring msg) { ReportError(L"Error", msg); }
	void ReportError_HR(HRESULT hr)
	{
		_com_error err(hr);
		ReportError(err.ErrorMessage());
	}
	void ReportError_HR(HRESULT hr, std::wstring msg)
	{
		_com_error err(hr);
		ReportError(msg + L" " + err.ErrorMessage());
	}
	void ReportError_HR(std::wstring title, HRESULT hr, std::wstring msg)
	{
		_com_error err(hr);
		ReportError(title, msg + L" " + err.ErrorMessage());
	}

	void SetDebugObjectName(ID3D12DeviceChild* obj, const std::string& name)
	{
		u32 l = name.length();
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, l, name.data());
	}

	Globals* g_pGlobals = new Globals();
} GRAPHOUNY_NAMESPACE_END