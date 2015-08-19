#pragma once
#include "gr_stream.h"

class FileInputStream : public InputStream
{
public:
	FileInputStream(std::ifstream* baseStream);
	FileInputStream(std::ifstream* baseStream, u64 length);
	FileInputStream(const std::string& sPath);
	FileInputStream(const std::wstring& wsPath);

	~FileInputStream();

	bool CanRead() const override;
	bool CanWrite() const override { return false; }
	bool CanPeekByte() const override { return CanRead(); }
	bool CanSetPosition() const override { return CanRead(); }

	u64 GetPosition() const override;
	void SetPosition(const u64 newPos) override;
	void SetPosition(std::ios::seek_dir dir, const i64 offset) override;
	u8 PeekByte() override;
	u64 ReadBytes(pointer buffer, u64 bytesToRead) override;
	bool EndOfStream() const override;
	void Close() override;
	u64 GetLength() override { return m_iLength; }
private:
	std::ifstream* m_pStream;
	u64 m_iLength;
	bool m_bOwnStream = false;
};

