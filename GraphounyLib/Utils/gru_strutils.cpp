#define _CRT_SECURE_NO_WARNINGS
#include "gru_strutils.h"

std::wstring str2wstr(const std::string& s)
{
	std::wstring wc(s.length(), L'#');
	mbstowcs(&wc[0], s.data(), s.length());
	return wc;
}

std::string wstr2str(const std::wstring& wc)
{
	std::string s(wc.length(), L'#');
	wcstombs(&s[0], wc.data(), wc.length());
	return s;
}