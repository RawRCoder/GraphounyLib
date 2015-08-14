#include "gr_endianess.h"
#include "gr_shared.h"

bool Endianess::IsLittleEndian() { return !IsBigEndian(); }
bool Endianess::IsBigEndian()
{
	if (!m_bEndianessGenerated)
	{
		union {
			u32 val;
			u8 c[4];
		} u;
		u.val = 0x01;
		m_bBigEndian = 0x01 == u.c[3];
		m_bEndianessGenerated = true;
	}
	return m_bBigEndian;
}