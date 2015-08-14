#pragma once
#include "gr_shared.h"
#include <xmmintrin.h>

union u128
{	
	u64 _u64[2];
	i64 _i64[2];
	f64 _f64[2];
	u32 _u32[4];
	i32 _i32[4];
	f32 _f32[4];
	u16 _u16[8];
	i16 _i16[8];
	u8 _u8[16];
	i8 _i8[16];	
};
bool static operator ==(u128 a, u128 b)
{
	return a._u64[0] == b._u64[0] && a._u64[1] == b._u64[1];
}
bool static operator !=(u128 a, u128 b)
{
	return a._u64[0] != b._u64[0] || a._u64[1] != b._u64[1];
}