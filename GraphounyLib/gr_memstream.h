#pragma once
#include "gr_stream.h"

class InputMemoryStream : public InputStream
{
public:
	InputMemoryStream(const pointer pData, u64 size);

	bool CanRead() const  override { return true; }
	bool CanWrite() const  override { return false; }
	bool CanPeekByte()  const override { return true; }
	bool CanSetPosition()  const override { return true; }

	u64 GetPosition()  const override { return true; }
	void SetPosition(const u64 newPos) override { m_iPosition = newPos; }
	void SetPosition(std::ios::seek_dir dir, const i64 offset) override;
	u8 PeekByte() override;
	u64 ReadBytes(pointer buffer, u64 bytesToRead) override;
	bool EndOfStream()  const override { return m_iPosition >= m_iLength; }
	u64 GetLength() override { return m_iLength; }
private:
	const pointer m_pData;
	const u64 m_iLength;
	u64 m_iPosition = 0;
};
