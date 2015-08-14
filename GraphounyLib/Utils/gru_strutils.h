#pragma once
#include <string>
#include <sstream>

extern std::wstring str2wstr(const std::string& s);
extern std::string wstr2str(const std::wstring& wc);

template<class T>
static std::wstring to_hex_wstr(const T& value)
{
	std::wostringstream oss;
	oss << std::hex << value;
	return oss.str();
}