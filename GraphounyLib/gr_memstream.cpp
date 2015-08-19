#include "gr_memstream.h"
#include <algorithm>

InputMemoryStream::InputMemoryStream(const pointer pData, u64 size)
	:m_pData(pData), m_iLength(size)
{}

void InputMemoryStream::SetPosition(std::ios::seek_dir dir, const i64 offset)
{
	switch (dir)
	{
	case SEEK_END:
		m_iPosition = m_iLength + offset;
		break;
	case SEEK_CUR:
		if ((i64)m_iPosition < offset)
			m_iPosition = 0;
		else
			m_iPosition += offset;
		break;
	case SEEK_SET:
		m_iPosition = 0 + offset;
		break;
	}
}

u8 InputMemoryStream::PeekByte()
{
	if (EndOfStream())
		return 0;
	return ((const u8*)m_pData)[m_iPosition];
}

u64 InputMemoryStream::ReadBytes(pointer buffer, u64 bytesToRead)
{
	if (EndOfStream())
		return 0;
	auto _bts2copy = std::min<u64>(bytesToRead, m_iLength - m_iPosition);
	if (!_bts2copy)
		return 0;
	memcpy_s(buffer, _bts2copy, m_pData, _bts2copy);
	m_iPosition += _bts2copy;
	return _bts2copy;
}