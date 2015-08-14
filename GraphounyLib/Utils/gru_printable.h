#pragma once
#include <string>

struct IPrintable
{
	virtual std::wstring ToString() const = 0;
};