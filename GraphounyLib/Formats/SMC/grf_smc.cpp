#include "grf_smc.h"
#include "../../Utils/gru_filesystem.h"
#include "../../Framework/grfwu_exception.h"
using namespace std;

GRAPHOUNY_NAMESPACE_FORMATS{ namespace SMC {
	SMCReader::~SMCReader()
	{
		Close();
		if (m_pNames) delete[] m_pNames;
	}

	void SMCReader::Open(std::wstring path, bool close)
	{
		if (m_bOpened)
			return;
		if (!PathExists(path))
		{
			throw Framework::GrBasicException(L"File not found / unavaliable");
			return;
		}
		if (m_pStream)
			delete m_pStream;
		m_wsFileName = path;
		m_pStream = new ifstream(m_wsFileName.data(), ios_base::in | ios_base::binary);
		m_bOpened = true;
		m_aEntries.Clear();
		if (m_pNames) delete[] m_pNames;

		// Проверим магию
		char buffer[4];
		m_pStream->read(buffer, 3);
		buffer[3] = 0;
		if (strcmp(buffer, "SMC"))
		{
			Close();
			throw Framework::GrBasicException(L"Unknown SMC format");
		}

		// Проверим версию
		auto v = Read_u32(m_pStream);
		if (v != m_iVersion)
		{
			Close();
			throw Framework::GrBasicException(L"Unsupported SMC version (" + to_wstring(v) + L" vs " + to_wstring(m_iVersion) + L")");
		}

		// Читаем размер секции наименований
		m_iNamesSize = Read_u32(m_pStream);
		if (m_iNamesSize > 256 * 1024 * 1024) // 256 MiB max
		{
			Close();
			throw Framework::GrBasicException(L"Name section too large (256 MiB max, got " + to_wstring(m_iNamesSize) + L" bytes)");
		}		

		// Считаем всю секцию имён
		m_pNames = new wchar_t[m_iNamesSize / (sizeof(wchar_t)/sizeof(char))];
		m_pStream->read((char*)m_pNames, m_iNamesSize);

		// Считаем все записи
		hash<wstring> hasher;
		u32 iEntriesCount = Read_u32(m_pStream);
		for (u32 i = 0; i < iEntriesCount; ++i)
		{
			smc_entry_s entry{};
			entry.m_iVersion = Read_u32(m_pStream);
			entry.m_iContentType = Read_u16(m_pStream);
			entry.m_iNameLength = Read_u16(m_pStream);
			entry.m_iNameOffset = Read_u32(m_pStream);
			entry.m_iReserved[0] = Read_u32(m_pStream);
			entry.m_iReserved[1] = Read_u32(m_pStream);
			entry.m_iReserved[2] = Read_u32(m_pStream);
			entry.m_iLength = Read_u32(m_pStream);
			entry.m_iOffset = Read_u64(m_pStream);
			auto name = UnsafeGetEntryName(entry);
			entry.m_iNameHash = hasher(name);
			m_aEntries.Add(entry);
		}

		m_iDataOffset = m_pStream->tellg();
		m_bLoaded = true;
		if (close) Close();
	}

	bool SMCReader::Reopen()
	{
		if (!IsLoaded()) return false;
		if (IsOpened()) return true;
		if (m_pStream)
			delete m_pStream;
		m_pStream = new ifstream(m_wsFileName.data());
		m_pStream->seekg(m_iDataOffset);
		m_bOpened = true;
		return true;
	}

	void SMCReader::Close()
	{
		if (m_pStream)
		{
			m_pStream->close();
			delete m_pStream;
			m_bOpened = false;
			m_pStream = nullptr;
		}
	}

	smc_entry_s SMCReader::GetEntryByIndex(u32 id)
	{
		return m_aEntries.Get(id);
	}

	u32 SMCReader::GetAllEntries(Array<smc_entry_s>* target)
	{
		if (target)
		{
			for (u32 i = 0; i < m_aEntries.Count(); ++i)
				target->Add(m_aEntries.Get(i));
		}
		return m_aEntries.Count();
	}

	u32 SMCReader::GetAllEntriesErrors(Array<std::wstring>* target)
	{
		u32 cnt = 0;
		for (u32 i = 0; i < m_aEntries.Count(); ++i)
		{
			auto entry = m_aEntries.Get(i);
			if (entry.m_iNameOffset >= m_iNamesSize/2 || entry.m_iNameOffset + entry.m_iNameLength >= m_iNamesSize/2)
			{
				++cnt;
				if (target) target->Add(L"An entry[" + to_wstring(i) + L"]`s name section is out of range");
			}
			if (entry.m_iOffset >= m_iDataSize || entry.m_iOffset + entry.m_iLength >= m_iDataSize)
			{
				++cnt;
				if (target) target->Add(L"An entry[" + to_wstring(i) + L"]`s data section is out of range");
			}
		}
		return cnt;
	}

	bool SMCReader::GetEntryDesc(const wstring& name, smc_entry_s* pEntry)
	{
		smc_entry_s* pe = nullptr;
		if (!UnsafeGetEntryDesc(name, &pe))
			return false;
		*pEntry = *pe;
		return true;
	}

	bool SMCReader::UnsafeGetEntryDesc(const wstring& name, smc_entry_s** pEntry)
	{
		hash<wstring> hasher;
		for (u32 i = 0; i < m_aEntries.Count(); ++i)
		{
			auto e = m_aEntries[i];
			auto ename = UnsafeGetEntryName(e);

			if (hasher(ename) != e.m_iNameHash)
				continue;
			if (ename.compare(name))
				continue;
			if (pEntry)
				*pEntry = &m_aEntries[i];
			return true;
		}
		return false;
	}

	wstring SMCReader::GetEntryName(const smc_entry_s& entry)
	{
		if (entry.m_iNameOffset >= m_iNamesSize/2 || entry.m_iNameOffset + entry.m_iNameLength >= m_iNamesSize/2)		
			throw Framework::GrBasicException(L"An entry`s name section is out of range");
		
		/*if (entry.m_iOffset >= m_iDataSize || entry.m_iOffset + entry.m_iLength >= m_iDataSize)		
			throw Framework::GrBasicException(L"An entry`s data section is out of range");*/
		
		return UnsafeGetEntryName(entry);
	}

	wstring SMCReader::GetEntryNameByIndex(u32 id)
	{
		if (id > m_aEntries.Count())
			throw Framework::GrBasicException(
			L"Index out of range (" + to_wstring(id) + L"vs range 0.." + to_wstring(m_aEntries.Count() - 1) + L")");
		return UnsafeGetEntryNameByIndex(id);
	}

	wstring SMCReader::UnsafeGetEntryName(const smc_entry_s& e)
	{
		wchar_t* dest = new wchar_t[e.m_iNameLength];
		memcpy_s(dest, 2*e.m_iNameLength, m_pNames + e.m_iNameOffset, 2*e.m_iNameLength);
		return wstring(dest, e.m_iNameLength);
	}

	wstring SMCReader::UnsafeGetEntryNameByIndex(u32 id)
	{
		return UnsafeGetEntryName(m_aEntries[id]);
	}

	ifstream* SMCReader::ReadEntry(const std::wstring& name)
	{
		smc_entry_s entry;
		return ReadEntry(name, &entry);
	}

	ifstream* SMCReader::ReadEntry(const std::wstring& name, smc_entry_s* pEntry)
	{
		if (!GetEntryDesc(name, pEntry))
			return nullptr;
		return UnsafeReadEntry(*pEntry);
	}

	ifstream* SMCReader::UnsafeReadEntry(const smc_entry_s& e)
	{
		if (!IsLoaded())
			return nullptr;
		if (!IsOpened())
			Reopen();
		u64 offset = m_iDataOffset + e.m_iOffset;
		m_pStream->seekg(offset);
		return m_pStream;
	}

	smc_entry_s* SMCReader::UnsafeGetEntryByIndex(u32 id)
	{
		return &m_aEntries[id];
	}
}} GRAPHOUNY_NAMESPACE_END
