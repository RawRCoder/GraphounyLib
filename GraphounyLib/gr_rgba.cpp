#include "gr_128.h"
#include "gr_rgba.h"

bool operator==(const rgba_s& a, const rgba_s& b)
{
	return asv(u128)a == asv(u128)b;
}

bool operator!=(const rgba_s& a, const rgba_s& b)
{
	return asv(u128)a != asv(u128)b;
}

const f32* rgba_s::to_floats(const rgba_s& a)
{
	return reinterpret_cast<const f32*>(&a);
}