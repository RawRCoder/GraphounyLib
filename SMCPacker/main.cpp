#include <tuple>
#include <Utils/gru_strutils.h>
#include <gr_shared.h>
#include <iostream>
#include <fstream>
#include <Utils/gru_filesystem.h>
#include <codecvt>
#include <utils/gru_lineparse.h>
#include <boost/algorithm/string/replace.hpp>
#include <formats/smc/grf_smc_system.h>
using namespace std;

wstring g_wsOutputFile;

struct entry_info_s
{
	wstring m_wsSourceFileName;
	wstring m_wsName;
	u64 m_iLength;

	entry_info_s() {}
	entry_info_s(wstring s) : m_wsSourceFileName(s), m_wsName(s)
	{
		m_iLength = GetFileSize(m_wsSourceFileName);
	}
	entry_info_s(wstring fn, wstring s) : m_wsSourceFileName(fn), m_wsName(s)
	{
		m_iLength = GetFileSize(m_wsSourceFileName);
	}
};
Array<entry_info_s> g_aEntries;


void push_file(wstring path, wstring name)
{	
	wcout << L"Found file '" << path << L"' as '" << name << L"'\n";
	g_aEntries.Add(entry_info_s(path, name));
}
void push_dir(wstring path, wstring prefix)
{
	if (path.length() < 1)
		return;
	boost::replace_all(path, L"/", L"\\");
	if (path[path.length() - 1] != L'\\')
		path += L'\\';
	boost::replace_all(prefix, L"\\", L"/");
	if (prefix[prefix.length() - 1] != L'/')
		prefix += L'/';
	while (prefix[0] == L'\\')
	{
		prefix = prefix.substr(1);
		if (prefix.length() < 1)
			return;
	}

	std::tuple<wstring, wstring> arg(path, prefix);

	IterateFilesAndSubdirsInDirectory(path, [](const wstring& _fileName, pointer obj)
	{
		std::tuple<wstring, wstring> _arg = *(tuple<wstring, wstring>*)obj;
		auto _pref = get<1>(_arg);
		auto _basepath = get<0>(_arg);
		auto _name = _pref / _fileName.substr(_basepath.length());
		if (IsPathAFile(_fileName))
			push_file(_fileName, _name);
		else if (IsPathADirectory(_fileName))
			push_dir(_fileName, _name);
		return true;
	}, &arg);
}

void run_file(wstring wsFileName)
{
	wifstream fs(wsFileName, ios_base::in);
	if (!fs)
	{
		wcout << L"Failed to open file\n";
		return;
	}
	fs.imbue(locale(locale::empty(), new codecvt_utf8<wchar_t>));
	wstring line;
	while (getline(fs, line))
	{		
		lineparse_s lp(line);
		auto arg0 = lp.GetArgumentOrDefault(0);
		auto arg1 = lp.GetArgumentOrDefault(1);

		if (!lp.m_wsCommand.compare(L"#target"))
		{
			g_wsOutputFile = arg0;
			continue;
		}
		if (!lp.m_wsCommand.compare(L"#push"))
		{
			if (IsPathAFile(arg0))
				push_file(arg0, arg1);
			else if (IsPathADirectory(arg0))
				push_dir(arg0, arg1);
			continue;
		}
	}
	fs.close();
}

void pack()
{
	ofstream f(g_wsOutputFile, ios_base::out | ios_base::binary);
	f << 'S' << 'M' << 'C';
	Write_u32(&f, 1);
	Array<wchar_t> namebuffer;
	for (u32 i = 0; i < g_aEntries.Count(); ++i)
	{
		auto n = g_aEntries.Get(i).m_wsName;
		namebuffer.AddRange(n.c_str(), n.length());
	}
	Write_u32(&f, namebuffer.Count()*2);
	f.write((char*)namebuffer.ToFullArray(), namebuffer.Count() * 2);
	Write_u32(&f, g_aEntries.Count());
	u64 offsetData = 0;
	u32 offsetName = 0;
	for (u32 i = 0; i < g_aEntries.Count();++i)
	{
		auto entry = g_aEntries[i];
		Write_u32(&f, 0);
		Write_u16(&f, 0);
		Write_u16(&f, entry.m_wsName.length());
		Write_u32(&f, offsetName);
		Write_u32(&f, 0);
		Write_u32(&f, 0);
		Write_u32(&f, 0);
		Write_u32(&f, entry.m_iLength);
		Write_u64(&f, offsetData);

		offsetName += entry.m_wsName.length();
		offsetData += entry.m_iLength;
	}

	for (u32 i = 0; i < g_aEntries.Count(); ++i)
	{
		auto entry = g_aEntries[i];
		wcout << L"Adding '" << entry.m_wsName << L"' ...\n";
		WriteFileToStream(&f, entry.m_wsSourceFileName);
	}

	f.close();
	wcout << L"Done!\n";
}

i32 wmain(i32 argc, wchar_t *argv[], wchar_t *envp[])
{
	if (argc < 1)
		return 0;

	auto wsFileName = wstring(argv[1]);
	if (PathExists(wsFileName))
	{
		run_file(wsFileName);
		wcout << L"Writing '" << g_wsOutputFile << L"' ...\n";
		pack();

		//TEST
		/*Graphouny::Formats::SMC::SMCReaderSystem sys{};
		sys.AddAllSMCFromDirectory(L"D:/Projects/GraphounyLib/Debug/tools");
		Graphouny::Formats::SMC::smcs_info inf{};
		if (!sys.GetEntry(L"cuba-old-client/bin/client.exe", &inf))
		{
			wcout << L"Shiet\n";
		}*/
	}
	else
		wcout << L"Specified file does not exist\n";
	return 0;
}
