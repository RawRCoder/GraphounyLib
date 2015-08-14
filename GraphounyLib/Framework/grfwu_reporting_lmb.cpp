#include "grfwu_reporting.h"
#include "grfwu_logging.h"

GRAPHOUNY_NAMESPACE_FRAMEWORK{
	LoggerMessageBoxReporter::LoggerMessageBoxReporter(HWND hwnd, Logger* logger)
	{
		m_hWindow = hwnd;
		m_pLogger = logger;
	}

	void LoggerMessageBoxReporter::ReportInfo(const std::wstring& message)
	{
		m_pLogger->SayLn(L"[INF]: " + message);
	}

	void LoggerMessageBoxReporter::ReportInfo(const std::wstring& message, const std::wstring& title)
	{
		ReportInfo(message);
		MessageBoxWS(m_hWindow, title, message, MB_OK | MB_ICONINFORMATION);
	}

	void LoggerMessageBoxReporter::ReportWarning(const std::wstring& message)
	{
		m_pLogger->SayLn(L"[WRN]: " + message);
	}

	void LoggerMessageBoxReporter::ReportWarning(const std::wstring& message, const std::wstring& title)
	{
		ReportWarning(message);
		MessageBoxWS(m_hWindow, title, message, MB_OK | MB_ICONWARNING);
	}

	void LoggerMessageBoxReporter::ReportError(const std::wstring& message)
	{
		m_pLogger->SayLn(L"[ERR]: " + message);
	}

	void LoggerMessageBoxReporter::ReportError(const std::wstring& message, const std::wstring& title)
	{
		ReportError(message);
		MessageBoxWS(m_hWindow, title, message, MB_OK | MB_ICONERROR);
	}
} GRAPHOUNY_NAMESPACE_END