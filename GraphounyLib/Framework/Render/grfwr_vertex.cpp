#include "grfwr_vertex_types.h"

vertex_basic_s::vertex_basic_s(const v3f& _pos) 
	: pos(_pos) {}

vertex_colored_s::vertex_colored_s(const v3f& _pos, const rgba_s& _clr) 
	: vertex_basic_s(_pos), color(_clr) {}