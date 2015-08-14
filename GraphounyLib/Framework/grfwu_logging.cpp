#include "grfwu_logging.h"
GRAPHOUNY_NAMESPACE_FRAMEWORK {
	using namespace std;
	bool Logger::SetUp(const wstring& fileName)
	{
		m_sFileName = fileName;
		try
		{
			wcodec = new null_wcodecvt(1);
			locale wloc(locale::classic(), wcodec);
			File.imbue(wloc);
			File.open(fileName, ios_base::out | ios_base::binary);
			File << UTF_BOM;
		}
		catch (exception ex)
		{
#ifdef DEBUG
			throw ex;
#endif
			return false;
		}
		return true;
	}

	void Logger::Close()
	{
		if (!File) return;
		File.close();
	}

	void Logger::Save()
	{
		if (m_bShouldSave)
			File.flush();
	}

	void Logger::Say(const wstring& message)
	{
		if (m_bShouldSave)
			File << message.data();
	}
	void Logger::SayLn(const wstring& message)
	{
		if (m_bShouldSave)
			File << message.data() << null_wcodecvt::wendl;
	}

	void Logger::TurnOff()
	{
		Close();
		_wremove(m_sFileName.data());
		m_bShouldSave = false;
	}
} GRAPHOUNY_NAMESPACE_END