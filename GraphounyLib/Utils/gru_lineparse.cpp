#include "gru_lineparse.h"
using namespace std;

lineparse_s::lineparse_s(wstring s)
{
	auto inQuotes = false;
	auto quoteChar = L'\0';
	auto c = L'\0';
	auto inEscape = false;
	wchar_t pc;
	auto readingCommand = true;
	wstring buffer = L"";
	for (auto i = 0; i < s.length(); ++i)
	{
		pc = c;
		c = s[i];
		if (c == '\\')
		{
			if (pc == c)
			{
				inEscape = false;
				buffer += c;
				c = '\0';
			}
			inEscape = true;
			continue;
		}
		if (inEscape)
		{
			switch (c)
			{
			case L'0':
				c = L'\0';
				buffer += c;
				continue;
			case L'\'':
			case L'\"':
				buffer += c;
				continue;
			case L'n':
				c = L'\n';
				buffer += c;
				continue;
			case L'r':
				c = L'\r';
				buffer += c;
				continue;
			case L't':
				c = L'\t';
				buffer += c;
				continue;
			}
			buffer += '\\';
			inEscape = false;
		}
		if (!inQuotes)
		{
			if (c == L' ' || c == L'\t')
			{
				if (readingCommand)
				{
					m_wsCommand = buffer;
					readingCommand = false;
				}
				else
					m_aArguments.Add(buffer);
				buffer = L"";
				continue;
			}
			if (!inEscape && (c == L'\'' || c == L'"'))
			{
				quoteChar = c;
				inQuotes = true;
				continue;
			}
			buffer += c;
		}
		else // in quotes
		{
			if (!inEscape && (c == quoteChar))
			{
				inQuotes = false;
				continue;
			}
			buffer += c;
		}
	}
	if (buffer.length() > 0)
	{
		if (readingCommand)
			m_wsCommand = buffer;
		else
			m_aArguments.Add(buffer);
	}
}

wstring lineparse_s::GetArgumentOrDefault(u32 id, wstring defValue)
{
	if (id >= m_aArguments.Count())
		return defValue;
	return m_aArguments[id];
}