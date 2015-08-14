#pragma once

class Endianess
{
public:
	bool IsLittleEndian();
	bool IsBigEndian();
private:
	bool m_bBigEndian = false;
	bool m_bEndianessGenerated = false;
};

static Endianess g_endianess;