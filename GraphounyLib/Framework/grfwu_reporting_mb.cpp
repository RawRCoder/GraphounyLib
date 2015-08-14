#include "grfwu_reporting.h"
#include "grfwu_logging.h"

GRAPHOUNY_NAMESPACE_FRAMEWORK{
	MessageBoxReporter::MessageBoxReporter(HWND hwnd, std::wstring defaultTitle)
	{
		m_hWindow = hwnd;
		m_wsDefaultTitle = defaultTitle;
	}

	void MessageBoxReporter::ReportInfo(const std::wstring& message)
	{
		ReportInfo(message, m_wsDefaultTitle);
	}

	void MessageBoxReporter::ReportInfo(const std::wstring& message, const std::wstring& title)
	{
		MessageBoxWS(m_hWindow, title, message, MB_OK | MB_ICONINFORMATION);
	}

	void MessageBoxReporter::ReportWarning(const std::wstring& message)
	{
		ReportWarning(message, m_wsDefaultTitle);
	}

	void MessageBoxReporter::ReportWarning(const std::wstring& message, const std::wstring& title)
	{
		MessageBoxWS(m_hWindow, title, message, MB_OK | MB_ICONWARNING);
	}

	void MessageBoxReporter::ReportError(const std::wstring& message)
	{
		ReportWarning(message, m_wsDefaultTitle);
	}

	void MessageBoxReporter::ReportError(const std::wstring& message, const std::wstring& title)
	{
		MessageBoxWS(m_hWindow, title, message, MB_OK | MB_ICONERROR);
	}
} GRAPHOUNY_NAMESPACE_END