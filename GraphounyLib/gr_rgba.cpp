#include "gr_128.h"
#include "gr_rgba.h"
#include <algorithm>

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

rgba_s rgba_s::from_rgba_bytes(const u32 value)
{
	union { u32 v32; u8 v8[4]; } v;
	v.v32 = value;
	return rgba_s{ v.v8[0] / 255.0f, v.v8[1] / 255.0f, v.v8[2] / 255.0f, v.v8[3] / 255.0f };
}

u32 rgba_s::to_rgba_bytes() const
{
	union { u32 v32; u8 v8[4]; } value;
	value.v8[0] = static_cast<u8>(r*255.0f);
	value.v8[1] = static_cast<u8>(g*255.0f);
	value.v8[2] = static_cast<u8>(b*255.0f);
	value.v8[3] = static_cast<u8>(a*255.0f);
	return value.v32;
}

f32 rgba_s::get_brightness() const
{
	return 0.2126f*r + 0.7152f*g + 0.0722f*b;
}

u32 rgba_s::get_hue() const
{
	f32 cmin = std::min<f32>(std::min<f32>(r, g), b);
	f32 cmax = std::max<f32>(std::max<f32>(r, g), b);

	f32 hue = 0.0f;
	if (cmax == r)
		hue = (g - b) / (cmax - cmin);
	else if (cmax == g)
		hue = 2.0f + (b - r) / (cmax - cmin);
	else
		hue = 4.0f + (r - g) / (cmax - cmin);

	hue *= 60;
	while (hue < 0) 
		hue += 360;

	return static_cast<u32>(round(hue));
}