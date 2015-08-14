#pragma once
#include "..\..\gr_shared.h"
#include "../../Utils/gru_array.h"
#include <fstream>

GRAPHOUNY_NAMESPACE_FORMATS { namespace SMC {

	struct smc_entry_s
	{
		u32 m_iVersion;

		u16 m_iContentType;
		u16 m_iNameLength;

		u32 m_iNameOffset;
		u32 m_iReserved[3];
		u32 m_iLength;
		u64 m_iOffset;

		u64 m_iNameHash;

		smc_entry_s() {}
	};

	class SMCReader
	{
	public:
		const u32 m_iVersion = 1;

		~SMCReader();
		void Open(std::wstring path, bool close = false);
		bool Reopen();
		void Close();
		inline std::wstring GetFilePath() const { return m_wsFileName; }
		inline bool IsOpened() const { return m_bOpened; }
		inline bool IsLoaded() const { return m_bLoaded; }
		inline u64 GetPosition() { return m_pStream->tellg(); }
		inline std::ifstream* GetStream() { return m_bOpened ? m_pStream : nullptr; }
		smc_entry_s GetEntryByIndex(u32 id);
		u32 GetAllEntries(Array<smc_entry_s>* target = nullptr);
		u32 GetAllEntriesErrors(Array<std::wstring>* target = nullptr);

		bool GetEntryDesc(const std::wstring& name, smc_entry_s* pEntry = nullptr);
		std::wstring GetEntryName(const smc_entry_s& myEntry);
		std::wstring GetEntryNameByIndex(u32 id);
		std::wstring UnsafeGetEntryName(const smc_entry_s& myEntry);
		std::wstring UnsafeGetEntryNameByIndex(u32 id);

		std::ifstream* ReadEntry(const std::wstring& name);
		std::ifstream* ReadEntry(const std::wstring& name, smc_entry_s* pEntry);
		std::ifstream* UnsafeReadEntry(const smc_entry_s& name);
		smc_entry_s* UnsafeGetEntryByIndex(u32 id);
	private:
		bool m_bOpened = false;
		bool m_bLoaded = false;
		std::ifstream* m_pStream = nullptr;
		Array<smc_entry_s> m_aEntries;
		wchar_t* m_pNames = nullptr;
		u32 m_iNamesSize;
		u64 m_iDataSize;
		u64 m_iDataOffset;
		std::wstring m_wsFileName;

		bool UnsafeGetEntryDesc(const std::wstring& name, smc_entry_s** pEntry = nullptr);
	};

}} GRAPHOUNY_NAMESPACE_END