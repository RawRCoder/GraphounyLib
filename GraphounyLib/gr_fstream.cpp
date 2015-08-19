#include "gr_fstream.h"
#include <fstream>
#include <algorithm>

FileInputStream::FileInputStream(std::ifstream* baseStream): m_pStream(baseStream)
{
	u64 pos = m_pStream->tellg();
	m_pStream->seekg(SEEK_END);
	m_iLength = m_pStream->tellg();
	m_pStream->seekg(pos);
}

FileInputStream::FileInputStream(std::ifstream* baseStream, u64 length) : m_pStream(baseStream), m_iLength(length)
{}

FileInputStream::FileInputStream(const std::string& path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
	m_iLength = file.tellg();
	file.close();

	m_pStream = new std::ifstream(path, std::ios::in | std::ios::binary);
	m_bOwnStream = true;
	if (m_pStream->operator!())
		throw std::exception("Fuck");
}

FileInputStream::FileInputStream(const std::wstring& path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
	m_iLength = file.tellg();
	file.close();

	m_pStream = new std::ifstream(path, std::ios::in | std::ios::binary);
	m_bOwnStream = true;
	if (m_pStream->operator!())
		throw std::exception("Fuck");
}

FileInputStream::~FileInputStream()
{
	if (m_pStream && m_bOwnStream)
	{
		m_pStream->close();
		delete m_pStream;
		m_pStream = nullptr;
		m_bOwnStream = false;
	}
}

bool FileInputStream::CanRead()const
{ return m_pStream && !m_pStream->operator!(); }

u64 FileInputStream::GetPosition()const
{ return m_pStream->tellg(); }

void FileInputStream::SetPosition(const u64 newPos)
{ m_pStream->seekg(newPos); }

void FileInputStream::SetPosition(std::ios::seek_dir dir, const i64 offset)
{
	m_pStream->seekg(offset, dir);
}

u8 FileInputStream::PeekByte()
{ return static_cast<u8>(m_pStream->peek()); }

u64 FileInputStream::ReadBytes(pointer buffer, u64 bytesToRead)
{
	if (EndOfStream())
		return 0;
	auto _bts2copy = std::min<u64>(bytesToRead, m_iLength - GetPosition());
	if (!_bts2copy)
		return 0;
	m_pStream->read(reinterpret_cast<char*>(buffer), bytesToRead);
	return _bts2copy;
}

bool FileInputStream::EndOfStream()const
{
	return !m_pStream || m_pStream->operator!();
}

void FileInputStream::Close()
{
	if (m_pStream && m_bOwnStream)
	{
		m_pStream->close();
		delete m_pStream;
		m_pStream = nullptr;
		m_bOwnStream = false;
	}
}