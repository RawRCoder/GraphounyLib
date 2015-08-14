#include "grfwu_exception.h"
#include <comdef.h>

GRAPHOUNY_NAMESPACE_FRAMEWORK {
	std::wstring GrBasicException::ToString() const
	{
		if (m_bCausedByHRESULT)
		{
			_com_error err(m_hResult);
			return L"Exception: " + m_wsMessage + L" (hresult = " + std::to_wstring(m_hResult) + L";" + err.ErrorMessage() + L")";
		}
		return L"Exception: " + m_wsMessage;
	}

	std::wstring GrInitFailedException::ToString() const
	{
		return L"Init Failed " + GrBasicException::ToString();
	}

} GRAPHOUNY_NAMESPACE_END