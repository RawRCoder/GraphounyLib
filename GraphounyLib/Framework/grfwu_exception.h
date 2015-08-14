#pragma once
#include "grfw_shared.h"
#include <Windows.h>

GRAPHOUNY_NAMESPACE_FRAMEWORK
{
	class GrBasicException : public std::exception, public IPrintable
	{
	public:
		GrBasicException(std::wstring msg) 
			: m_bCausedByHRESULT(false)
			, m_wsMessage(msg) { }
		GrBasicException(HRESULT hr, std::wstring msg)
			: m_hResult(hr)
			, m_bCausedByHRESULT(true)
			, m_wsMessage(msg) { }
		virtual ~GrBasicException() {}

		std::wstring ToString() const override;
		HRESULT GetHResult() const { return m_hResult; }
		bool IsCausedByHResult() const { return m_bCausedByHRESULT; }
		std::wstring Message() const { return m_wsMessage; }
	private:
		HRESULT m_hResult;
		bool m_bCausedByHRESULT;
		std::wstring m_wsMessage;
	};

	class GrInitFailedException : public GrBasicException
	{
	public:
		GrInitFailedException(HRESULT hr, std::wstring msg) : GrBasicException(hr, msg) {}
		std::wstring ToString() const override;
	};

	static void ThrowOnFailed(HRESULT hr, const std::wstring& msg)
	{
		if (SUCCEEDED(hr))
			return;
		throw GrInitFailedException(hr, msg);
	}
}
GRAPHOUNY_NAMESPACE_END