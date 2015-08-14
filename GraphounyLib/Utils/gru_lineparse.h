#pragma once
#include "..\gr_shared.h"
#include "gru_array.h"

struct lineparse_s
{
	std::wstring m_wsCommand;
	Array<std::wstring> m_aArguments;

	lineparse_s(std::wstring s);
	std::wstring GetArgumentOrDefault(u32 id, std::wstring defValue = L"");
};