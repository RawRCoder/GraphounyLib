#include "grfwr_vertex_types.h"

vertex_basic_s::vertex_basic_s(const v3f& _pos) 
	: pos(_pos) {}

vertex_colored_s::vertex_colored_s(const v3f& _pos, const rgba_s& _clr) 
	: vertex_basic_s(_pos), color(_clr) {}

vertex_textured_s::vertex_textured_s(const v3f& _pos, const v2f& _tc)
	: vertex_basic_s(_pos), texcoord(_tc) {}


vertex_CT_s::vertex_CT_s(const v3f& _pos, const v2f& _tc)
	: vertex_basic_s(_pos), color(RGBA_WHITE), texcoord(_tc) {}

vertex_CT_s::vertex_CT_s(const v3f& _pos, const rgba_s& _c, const v2f& _tc)
	: vertex_basic_s(_pos), color(_c), texcoord(_tc) {}


vertex_CTN_s::vertex_CTN_s(const v3f& _pos, const v2f& _tc, const v3f& _n)
	: vertex_CT_s(_pos,_tc), normal(_n) {}

vertex_CTN_s::vertex_CTN_s(const v3f& _pos, const rgba_s& _c, const v2f& _tc, const v3f& _n)
	: vertex_CT_s(_pos, _c, _tc), normal(_n) {}


vertex_CN_s::vertex_CN_s(const v3f& _pos, const v3f& _n)
	: vertex_colored_s(_pos, RGBA_WHITE), normal(_n) {}

vertex_CN_s::vertex_CN_s(const v3f& _pos, const rgba_s& _c, const v3f& _n)
	: vertex_colored_s(_pos, _c), normal(_n) {}