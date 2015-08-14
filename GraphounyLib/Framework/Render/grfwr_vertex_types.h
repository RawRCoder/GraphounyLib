#pragma once
#include "../../gr_vector3.h"
#include "../../gr_rgba.h"

struct vertex_basic_s
{
	v3f pos;

	vertex_basic_s() {}
	vertex_basic_s(const v3f& _pos);
};


struct vertex_colored_s : vertex_basic_s
{
	rgba_s color;

	vertex_colored_s() : vertex_basic_s() {}
	vertex_colored_s(const v3f& _pos, const rgba_s& _clr);
};