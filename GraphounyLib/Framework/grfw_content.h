#pragma once
#include "..\gr_shared.h"
#include <string>
#include "../formats/smc/grf_smc_system.h"

GRAPHOUNY_NAMESPACE_FRAMEWORK{
	class ContentHandle
	{
	public:
		ContentHandle(Formats::SMC::smcs_info smcsinfo);
		ContentHandle(const std::wstring& wsFileName);
		~ContentHandle();

		std::ifstream* GetStream();
		inline bool IsFromSMC() const { return m_bRealFile; }
		inline bool IsClosed() const { return m_bClosed; }
		inline u64 GetSize() const { return m_iSize; }
		inline std::wstring GetFileName() const { return m_wsFileName; }
	private:
		std::wstring m_wsFileName;
		u64 m_iSize;
		std::ifstream* m_pStream;
		bool m_bRealFile;
		Formats::SMC::smcs_info m_SMCSInfo;
		bool m_bClosed = false;
	};

	class ContentManager
	{
	public:
		virtual ~ContentManager();

		ContentManager(std::wstring binPath);

		void Init();

		inline std::wstring GetBasePath() const { return m_wsBaseDirectory; }
		inline std::wstring GetBinPath() const { return m_wsBinaryDirectory; }
		virtual std::wstring GetPath_LogsDir() const { return m_wsBaseDirectory / L"logs"; }
		virtual std::wstring GetPath_ShadersDir() const { return m_wsBaseDirectory / L"shaders"; }
		virtual std::wstring GetPath_TexturesDir() const { return m_wsBaseDirectory / L"textures"; }
		virtual std::wstring GetPath_3dDir() const { return m_wsBaseDirectory / L"3d"; }

		bool FileExists(std::wstring fileName);
		ContentHandle* ReadFile(std::wstring fileName);
		bool CloseFile(ContentHandle*& hFile);

		inline std::wstring MakePath(std::wstring fileName) const { return m_wsBaseDirectory / fileName; }

	protected:
		std::wstring m_wsBaseDirectory;
	private:
		bool RemoveHandle(std::wstring fileName);
		inline void RegisterHandle(std::wstring fileName, ContentHandle* pch) { m_dictHandles.Set(fileName, pch); }

		std::wstring m_wsBinaryDirectory;
		Formats::SMC::SMCReaderSystem* m_pSystem = new Formats::SMC::SMCReaderSystem();
		Dictionary<std::wstring, ContentHandle*> m_dictHandles { Comparator<std::wstring> };
	};
}GRAPHOUNY_NAMESPACE_END