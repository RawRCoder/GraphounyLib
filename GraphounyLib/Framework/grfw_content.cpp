#include "grfw_shared.h"
#include "grfw_content.h"
#include "../Utils/gru_filesystem.h"

GRAPHOUNY_NAMESPACE_FRAMEWORK{
	ContentManager::~ContentManager()
	{
		if (m_pSystem) delete m_pSystem;
		for (auto i = m_dictHandles.GetIterator(); !i.Finished(); ++i)
		{
			auto handle = i();
			if (handle)
				delete handle;
		}
	}

	ContentManager::ContentManager(std::wstring binPath)
		: m_wsBinaryDirectory(binPath)
	{
		m_wsBaseDirectory = m_wsBinaryDirectory / L"..";
	}

	void ContentManager::Init()
	{
		m_pSystem->AddAllSMCFromDirectory(m_wsBaseDirectory);
	}

	bool ContentManager::FileExists(std::wstring fileName)
	{
		auto p = MakePath(fileName);
		if (PathExists(p))
			return true;
		Formats::SMC::smcs_info inf;
		if (m_pSystem->GetEntry(fileName, &inf))
			return true;
		return false;
	}

	ContentHandle* ContentManager::ReadFile(std::wstring fileName)
	{
		auto p = MakePath(fileName);
		ContentHandle* ch = nullptr;
		if (m_dictHandles.TryGet(p, &ch))
		{
			if (ch && !ch->IsClosed())
				return ch;
		}
		else if (m_dictHandles.TryGet(fileName, &ch))
		{
			if (ch && !ch->IsClosed())
				return ch;
		}
		if (PathExists(p))
		{
			ch = new ContentHandle(p);
			if (ch->GetStream()->operator!())
				ch = nullptr;
			if (ch)
				RegisterHandle(p, ch);
		}
		else {
			Formats::SMC::smcs_info inf;
			if (m_pSystem->GetEntry(fileName, &inf))
			{
				ch = new ContentHandle(inf);
				if (ch)
					RegisterHandle(fileName, ch);
			}
		}
		return ch;
	}

	bool ContentManager::CloseFile(ContentHandle*& hFile)
	{
		if (!hFile)
			return true;
		if (!RemoveHandle(hFile->GetFileName()))
			return false;
		hFile = nullptr;
		return true;
	}

	bool ContentManager::RemoveHandle(std::wstring fileName)
	{
		return m_dictHandles.RemoveDelete(fileName);
	}

	ContentHandle::ContentHandle(Formats::SMC::smcs_info smcsinfo) : m_SMCSInfo(smcsinfo)
	{
		m_iSize = m_SMCSInfo.m_entry.m_iLength;
		m_bRealFile = false;
		m_wsFileName = smcsinfo.m_wsEntryName;
	}

	ContentHandle::ContentHandle(const std::wstring& wsFileName)
	{
		m_wsFileName = wsFileName;
		m_pStream = new std::ifstream(wsFileName);
		m_iSize = GetFileSize(wsFileName);
		m_bRealFile = true;
	}

	ContentHandle::~ContentHandle()
	{
		if (IsClosed())
			return;
		if(!IsFromSMC())
		{
			m_pStream->close();
			delete m_pStream;
			m_bClosed = true;
			m_pStream = nullptr;
		}
	}

	std::ifstream* ContentHandle::GetStream()
	{
		if (IsClosed())
			return nullptr;
		if (IsFromSMC())
			return m_SMCSInfo.ReadEntry();
		return m_pStream;
	}
}GRAPHOUNY_NAMESPACE_END