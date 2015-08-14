#pragma once
#include "..\gr_shared.h"
#include "..\Utils\gru_windows.h"
#include <string>

GRAPHOUNY_NAMESPACE_FRAMEWORK {
	class Logger;
	interface IReporter
	{
		virtual ~IReporter() {}

		virtual void ReportInfo(const std::wstring& message) = 0;
		virtual void ReportInfo(const std::wstring& message, const std::wstring& title) = 0;
		virtual void ReportWarning(const std::wstring& message) = 0;
		virtual void ReportWarning(const std::wstring& message, const std::wstring& title) = 0;
		virtual void ReportError(const std::wstring& message) = 0;
		virtual void ReportError(const std::wstring& message, const std::wstring& title) = 0;
	};


	class MessageBoxReporter : public IReporter
	{
	public:
		MessageBoxReporter(HWND hwnd, std::wstring defaultTitle);

		void ReportInfo(const std::wstring& message) override;
		void ReportInfo(const std::wstring& message, const std::wstring& title) override;
		void ReportWarning(const std::wstring& message) override;
		void ReportWarning(const std::wstring& message, const std::wstring& title) override;
		void ReportError(const std::wstring& message) override;
		void ReportError(const std::wstring& message, const std::wstring& title) override;
	private:
		HWND m_hWindow;
		std::wstring m_wsDefaultTitle;
	};

	class LoggerMessageBoxReporter : public IReporter
	{
	public:
		LoggerMessageBoxReporter(HWND hwnd, Logger* defaultTitle);

		void ReportInfo(const std::wstring& message) override;
		void ReportInfo(const std::wstring& message, const std::wstring& title) override;
		void ReportWarning(const std::wstring& message) override;
		void ReportWarning(const std::wstring& message, const std::wstring& title) override;
		void ReportError(const std::wstring& message) override;
		void ReportError(const std::wstring& message, const std::wstring& title) override;
	private:
		HWND m_hWindow;
		Logger* m_pLogger;
	};
} GRAPHOUNY_NAMESPACE_END