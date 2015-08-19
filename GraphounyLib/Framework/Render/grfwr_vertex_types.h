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

struct vertex_textured_s : vertex_basic_s
{
	v2f texcoord;

	vertex_textured_s() : vertex_basic_s() {}
	vertex_textured_s(const v3f& _pos, const v2f& _tc);
};

struct vertex_CT_s : vertex_basic_s //Colored+Textured
{
	rgba_s color;
	v2f texcoord;

	vertex_CT_s() : vertex_basic_s() {}
	vertex_CT_s(const v3f& _pos, const v2f& _tc);
	vertex_CT_s(const v3f& _pos, const rgba_s& _c, const v2f& _tc);
};

struct vertex_CTN_s : vertex_CT_s //Colored+Textured+Normal
{
	v3f normal;

	vertex_CTN_s() : vertex_CT_s() {}
	vertex_CTN_s(const v3f& _pos, const v2f& _tc, const v3f& _n);
	vertex_CTN_s(const v3f& _pos, const rgba_s& _c, const v2f& _tc, const v3f& _n);
};


struct vertex_CN_s : vertex_colored_s //Colored+Normal
{
	v3f normal;

	vertex_CN_s() : vertex_colored_s() {}
	vertex_CN_s(const v3f& _pos, const v3f& _n);
	vertex_CN_s(const v3f& _pos, const rgba_s& _c, const v3f& _n);
};