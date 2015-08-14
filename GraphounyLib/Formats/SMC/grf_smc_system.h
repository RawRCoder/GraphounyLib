#pragma once
#include "grf_smc.h"
#include "../../Utils/gru_dictionary.h"

GRAPHOUNY_NAMESPACE_FORMATS{ namespace SMC {

	struct smcs_info
	{
		std::wstring m_wsSMCPath;
		std::wstring m_wsEntryName;
		smc_entry_s m_entry;

		smcs_info(){}

		smcs_info(SMCReader* r, smc_entry_s entry, std::wstring path);
		void set(SMCReader* r, smc_entry_s entry, std::wstring path);

		inline std::ifstream* ReadEntry() { return m_pReader->UnsafeReadEntry(m_entry); }
	private:
		SMCReader* m_pReader;
	};

	class SMCReaderSystem
	{
	public:
		SMCReaderSystem() {}
		~SMCReaderSystem();

		void CloseAll();

		void AddSMCFile(std::wstring filePath);
		void AddAllSMCFromDirectory(std::wstring filePath);
		bool RemoveSMC(std::wstring path);
		bool CloseSMC(std::wstring path);
		bool IsSMCOpened(std::wstring path);
		bool IsSMCLoaded(std::wstring path);

		inline u32 CountOpenedSMC() { return m_iOpened; }
		inline u32 CountLoadedSMC() { return m_iLoaded; }
		inline u32 CountSMC() { return m_dFiles.Count(); }

		bool GetEntry(std::wstring name, smcs_info* outEntryInfo);
		bool GetEntry(std::wstring smcName, std::wstring name, smcs_info* outEntryInfo);
	private:
		Dictionary<std::wstring, SMCReader*> m_dFiles{ Comparator<std::wstring> };
		u32 m_iOpened = 0;
		u32 m_iLoaded = 0;
	};
} }GRAPHOUNY_NAMESPACE_END