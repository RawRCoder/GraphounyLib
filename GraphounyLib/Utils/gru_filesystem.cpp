#define _CRT_SECURE_NO_WARNINGS 1
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include "gru_filesystem.h"
#include "..\gr_endianess.h"
#include <Windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include "gru_strutils.h"

using namespace std;

bool PathExists(const wstring& path)
{
	wstring p = path;
	boost::replace_all(p, L"/", L"\\");
	return boost::filesystem::exists(p);
}

u64 GetFileSize(const wstring& path)
{
	wstring p = path;
	boost::replace_all(p, L"/", L"\\");
	if (p[p.length() - 1] == L'\\')
		p = p.substr(0, p.length() - 1);
	struct _stat64 buf;
	if (_wstat64(p.data(), &buf) == 0)
		return buf.st_size;
	return 0;
}

bool IsPathADirectory(const wstring& path)
{
	wstring p = path;
	boost::replace_all(p, L"/", L"\\");
	if (p[p.length() - 1] == L'\\')
		p = p.substr(0, p.length() - 1);
	struct _stat64 s;
	if (_wstat64(p.data(), &s) == 0)
		if (s.st_mode & S_IFDIR)
			return true;
	return false;
}

bool IsPathAFile(const wstring& path)
{
	wstring p = path;
	boost::replace_all(p, L"/", L"\\");
	if (p[p.length() - 1] == L'\\')
		p = p.substr(0, p.length() - 1);
	struct _stat64 s;
	if (_wstat64(p.data(), &s) == 0)
		if (s.st_mode & S_IFREG)
			return !IsPathADirectory(p);
	return false;
}

wstring RelativizePath(const wstring& fullPath, const wstring& basePath)
{
	// Очень наивно
	return fullPath.substr(basePath.length());
}

void IterateFilesInDirectory(const std::wstring& directory, Array<std::wstring>::DConstIterateT func, pointer obj)
{
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const wstring file_name = file_data.cFileName;
		const auto full_file_name = directory / file_name;
		const auto is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;
		
		if (!func(full_file_name, obj))
			break;

	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
}

void IterateFilesAndSubdirsInDirectory(const std::wstring& directory, Array<std::wstring>::DConstIterateT func, pointer obj)
{
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + L"/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const wstring file_name = file_data.cFileName;
		const auto full_file_name = directory / file_name;

		if (file_name[0] == '.')
			continue;

		if (!func(full_file_name, obj))
			break;

	} while (FindNextFile(dir, &file_data));

	FindClose(dir);
}

void WriteFileToStream(std::ostream* sOut, std::wstring wsName, u32 bufferSize)
{
	ifstream sIn(wsName, ios_base::in | ios_base::binary);
	char* buffer = new char[bufferSize];
	u64 totalBytes = GetFileSize(wsName);
	u64 bytesCopied = 0;
	u64 copyNow = min<u64>(bufferSize, totalBytes - bytesCopied);
	while (copyNow > 0)
	{
		sIn.read(buffer, copyNow);
		sOut->write(buffer, copyNow);
		bytesCopied += copyNow;
		copyNow = min<u64>(bufferSize, totalBytes - bytesCopied);
	}
	delete[] buffer;
	sIn.close();
}

template <class T>
T ReadGeneric(istream* src, bool srcIsBE)
{
	static const u8 Tsize = sizeof(T);
	static const u8 TsizeH = sizeof(T) / 2;
	char buffer[Tsize];
	if (!src->read(buffer, Tsize))
	{
#ifdef _DEBUG
		wcerr << L"ReadGeneric<"<< to_wstring(Tsize)<<L"> Error: " << str2wstr(strerror(errno)) << L"\n";
#endif
		return 0;
	}

	// Надо перевернуть?
	if (srcIsBE != g_endianess.IsBigEndian())
	{
		for (auto i = 0; i < TsizeH; ++i)
		{
			char c = buffer[i];
			buffer[i] = buffer[Tsize - i - 1];
			buffer[Tsize - i - 1] = c;
		}
	}
	T value = asv(T)buffer;
	return value;
}

template <class T>
void WriteGeneric(std::ostream* t, T value, bool BE)
{
	static const u8 Tsize = sizeof(T);
	char buffer[Tsize];
	char* src = (char*)&value;

	// Надо перевернуть?
	if (BE != g_endianess.IsBigEndian())
	{
		for (auto i = 0; i < Tsize; ++i)		
			buffer[Tsize - i - 1] = src[i];
		t->write(buffer, Tsize);
		return;
	}
	t->write(src, Tsize);
}