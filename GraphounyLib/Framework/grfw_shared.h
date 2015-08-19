#pragma once
#include "..\gr_shared.h"
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <stdarg.h>
#include "..\Utils\gru_printable.h"


GRAPHOUNY_NAMESPACE_FRAMEWORK{
	class BasicApplication;
	class Logger;
	interface IReporter;

	class Globals
	{
	public:
		inline void RegisterApp(BasicApplication* app, IReporter* rep)
		{
			m_pCurrentApp = app;
			m_pDefaultReporter = rep;
		}

		inline BasicApplication* CurrentApp() { return m_pCurrentApp; }
		inline IReporter* CurrentReporter() { return m_pDefaultReporter; }
		std::wstring LaunchPath();
	private:
		BasicApplication* m_pCurrentApp = nullptr;
		IReporter* m_pDefaultReporter = nullptr;
		std::wstring m_wsLaunchPath;
		bool m_bLPGenerated = false;
	};
	extern Globals* g_pGlobals;

	extern inline void Log(std::wstring msg);
	extern inline void LogLn(std::wstring msg);
	extern inline void LogLn_HR(HRESULT hr, std::wstring msg);
	extern inline void ReportInfo(std::wstring title, std::wstring msg);
	extern inline void ReportInfo(std::wstring msg);
	extern inline void ReportWarning(std::wstring title, std::wstring msg);
	extern inline void ReportWarning(std::wstring msg);
	extern inline void ReportError(std::wstring title, std::wstring msg);
	extern inline void ReportError(std::wstring msg);
	extern inline void ReportError_HR(HRESULT hr);
	extern inline void ReportError_HR(HRESULT hr, std::wstring msg);
	extern inline void ReportError_HR(std::wstring title, HRESULT hr, std::wstring msg);

	extern void SetDebugObjectName(ID3D12DeviceChild* obj, const std::string& name);

} GRAPHOUNY_NAMESPACE_END