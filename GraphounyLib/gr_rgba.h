#pragma once
#include "gr_shared.h"

struct rgba_s
{
	f32 r, g, b, a;

	rgba_s() { }
	rgba_s(f32 grey) : r(grey), g(grey), b(grey), a(1.0f) { }
	rgba_s(f32 grey, f32 alpha) : r(grey), g(grey), b(grey), a(alpha) { }
	rgba_s(f32 _r, f32 _g, f32 _b) : r(_r), g(_g), b(_b), a(1.0f) { }
	rgba_s(f32 _r, f32 _g, f32 _b, f32 _a) : r(_r), g(_g), b(_b), a(_a) { }

	static const f32* to_floats(const rgba_s& a);
	static rgba_s from_rgba_bytes(const u32 value);
	u32 to_rgba_bytes() const;

	f32 get_brightness() const;
	u32 get_hue() const;
};

bool static operator ==(const rgba_s& a, const rgba_s& b);
bool static operator !=(const rgba_s& a, const rgba_s& b);

const rgba_s RGBA_BLACK{ 0, 0, 0, 1 };
const rgba_s RGBA_WHITE{ 1, 1, 1, 1 };
const rgba_s RGBA_RED{ 1, 0, 0, 1 };
const rgba_s RGBA_GREEN{ 0, 1, 0, 1 };
const rgba_s RGBA_BLUE{ 0, 0, 1, 1 };