#pragma once
#include "gr_shared.h"

class Stream
{
public:
	Stream() {}
	virtual ~Stream() {};
	virtual bool CanRead() const = 0;
	virtual bool CanWrite() const = 0;
	virtual bool CanSetPosition() const = 0;

	virtual u64 GetPosition() const = 0;
	virtual void SetPosition(const u64 newPos) = 0;
	virtual void SetPosition(std::ios::seek_dir dir, const i64 offset) = 0;

	virtual void Close() {};
};

class InputStream : public Stream
{
public:
	InputStream() {}
	virtual ~InputStream() {};

	virtual bool CanPeekByte() const = 0;
	virtual u64 GetLength() = 0;

	virtual u8 PeekByte() = 0;
	virtual u64 ReadBytes(pointer buffer, u64 bytesToRead) = 0;

	template <typename T> T Read()
	{
		T value;
		return (ReadBytes(reinterpret_cast<u8*>(&value), sizeof(T)) != sizeof(T)) ? 0 : value;
	}
	template <typename T> bool Read(T* outValue)
	{
		T value;
		if (ReadBytes(reinterpret_cast<pointer>(&value), sizeof(T)) != sizeof(T))
			return false;
		*outValue = value;
		return true;
	}

	virtual bool EndOfStream() const = 0;
protected:
	bool m_bCanRead;
};