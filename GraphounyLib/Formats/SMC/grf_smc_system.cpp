#include "grf_smc_system.h"
#include "Shlwapi.h"
#include "..\..\Utils\gru_filesystem.h"
#include "../../Framework/grfwu_exception.h"
#pragma comment(lib, "Shlwapi.lib")

using namespace std;

GRAPHOUNY_NAMESPACE_FORMATS{ namespace SMC{
	SMCReaderSystem::~SMCReaderSystem()
	{
		CloseAll();
		m_dFiles.IterateAll([](const wstring& path, SMCReader*& reader, pointer obj)
		{
			delete reader;
		});
	}

	void SMCReaderSystem::CloseAll()
	{
		m_dFiles.IterateAll([](const wstring& path, SMCReader*& reader, pointer obj)
		{
			reader->Close();
		});
	}

	void SMCReaderSystem::AddSMCFile(wstring filePath)
	{
		if (m_dFiles.ContainsKey(filePath))
			return;
		auto reader = new SMCReader();
		reader->Open(filePath, true);
		m_dFiles.Set(filePath, reader);
	}

	void SMCReaderSystem::AddAllSMCFromDirectory(wstring path)
	{
		IterateFilesInDirectory(path, [](const wstring& fname, pointer obj)
		{
			const auto _this = (SMCReaderSystem* const)obj;
			auto ext = PathFindExtension(fname.c_str());
			if (StrCmpW(ext, L".smc"))
				return true;
			try
			{
				_this->AddSMCFile(fname);
			}
			catch (Framework::GrBasicException ex)
			{}
			return true;
		}, this);
	}

	bool SMCReaderSystem::RemoveSMC(wstring path)
	{
		return m_dFiles.RemoveDelete(path);
	}

	bool SMCReaderSystem::CloseSMC(wstring path)
	{
		SMCReader* reader;
		if (!m_dFiles.TryGet(path, &reader))
			return false;
		reader->Close();
		return true;
	}

	bool SMCReaderSystem::IsSMCOpened(wstring path)
	{
		SMCReader* reader;
		if (!m_dFiles.TryGet(path, &reader))
			return false;
		return reader->IsOpened();
	}

	bool SMCReaderSystem::IsSMCLoaded(wstring path)
	{
		SMCReader* reader;
		if (!m_dFiles.TryGet(path, &reader))
			return false;
		return reader->IsLoaded();
	}

	bool SMCReaderSystem::GetEntry(wstring name, smcs_info* outEntryInfo)
	{
		for (auto it = m_dFiles.GetIterator(); !it.Finished(); ++it)
		{
			auto reader = it();
			if (!reader->IsLoaded())
				continue;
			smc_entry_s entry{};
			if (!reader->GetEntryDesc(name, &entry))
				continue;
			if (outEntryInfo)
				*outEntryInfo = smcs_info(reader, entry, it.GetKey());
			return true;
		}
		return false;
	}

	bool SMCReaderSystem::GetEntry(wstring smcName, wstring name, smcs_info* outEntryInfo)
	{
		SMCReader* reader;
		if (!m_dFiles.TryGet(smcName, &reader))
			return false;
		if (!reader->IsLoaded())
			return false;
		smc_entry_s entry{};
		if (!reader->GetEntryDesc(name, &entry))
			return false;
		if (outEntryInfo)
			*outEntryInfo = smcs_info(reader, entry, smcName);
		return true;
	}

	smcs_info::smcs_info(SMCReader* r, smc_entry_s entry, wstring path) : m_wsSMCPath(path), m_entry(entry), m_pReader(r)
	{
		if (!m_pReader) return;
		m_wsEntryName = m_pReader->UnsafeGetEntryName(entry);
	}

	void smcs_info::set(SMCReader* r, smc_entry_s entry, wstring path)
	{
		m_wsSMCPath = path;
		m_entry = entry;
		m_pReader = r;
		if (m_pReader)
			m_wsEntryName = m_pReader->UnsafeGetEntryName(entry);
	}
} }GRAPHOUNY_NAMESPACE_END