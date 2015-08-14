#pragma once
#include "..\gr_shared.h"
#include "..\Utils\gru_uniout.h"
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <sstream>

GRAPHOUNY_NAMESPACE_FRAMEWORK {
	class Logger
	{
	public:
		Logger() { }
		~Logger() { Save(); Close(); }
		bool SetUp(const std::wstring& fileName);

		void Say(const std::wstring& message);
		void SayLn(const std::wstring& message);

		void Save();
		void Close();

		void TurnOff();

	private:
		bool m_bShouldSave = true;
		std::wstring m_sFileName;
		std::wofstream File;
		null_wcodecvt* wcodec = nullptr;
	};

	std::wstring static FormatTime(boost::posix_time::ptime now)
	{
		using namespace boost::posix_time;
		static std::locale loc(std::wcout.getloc(), new wtime_facet(L"%Y%m%d_%H%M%S"));

		std::basic_stringstream<wchar_t> wss;
		wss.imbue(loc);
		wss << now;
		return wss.str();
	}

	std::wstring static FormLogNumber()
	{
		using namespace boost::posix_time;
		return FormatTime(second_clock::universal_time());
	}
} GRAPHOUNY_NAMESPACE_END