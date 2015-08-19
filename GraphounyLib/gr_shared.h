#pragma once

#define STRY( name ) # name

#define GRAPHOUNY_NAMESPACE(NAME) namespace Graphouny { namespace NAME
#define GRAPHOUNY_NAMESPACE_FRAMEWORK namespace Graphouny { namespace Framework
#define GRAPHOUNY_NAMESPACE_FORMATS namespace Graphouny { namespace Formats
#define GRAPHOUNY_NAMESPACE_END }
#include <string>

typedef unsigned char byte;
typedef signed char sbyte;
#if defined(_MSC_VER)
typedef unsigned __int8		u8;
typedef signed __int8		i8;
typedef unsigned __int16	u16;
typedef signed __int16  	i16;
typedef unsigned __int32	u32;
typedef signed __int32		i32;
typedef unsigned __int64	u64;
typedef signed __int64		i64;
#else
typedef unsigned char			u8;
typedef signed char				i8;
typedef unsigned short int		u16;
typedef signed short int		i16;
typedef unsigned long int		u32;
typedef signed long int			i32;
typedef unsigned long long int	u64;
typedef signed long long int	i64;
#endif
typedef float f32;
typedef double f64;
typedef void* pointer;

extern std::wstring operator /(const std::wstring& s1, const std::wstring& s2);
static f32 randf(f32 LO, f32 HI);
static f64 randd(f64 LO, f64 HI);
static i32 randi(i32 LO, i32 HI);

#define IsInBounds(mn, value, mx)		((value >= mn) && (value <= mx))
#define IsNotInBounds(mn, value, mx)	((value < mn) || (value > mx))
#define Saturate(mn, value, mx)			max(mn, min(value, mx))

#define TEMPLATE_DELEGATE(TS, T, NAME, ARGS) template ##TS using NAME = T(*)ARGS
#define DELEGATE(T, NAME, ARGS)			typedef T (*##NAME)##ARGS
#define PREDICATE(NAME, ARGS)			DELEGATE(bool, NAME, ARGS)
#define PROCEDURE(NAME, ARGS)			DELEGATE(void, NAME, ARGS)

#define FLIP_INT(c) (((c >> 24) & 0xff) + (((c >> 16) & 0xff) << 8 ) + (((c >> 8) & 0xff) << 16) + ((c & 0xff) << 24))

#define asv(T) *(T*)&
#define asr(T) (T*)&

template <class T> 
static u64 GetHashCode(const T& value)
{
	std::hash<T> h;
	return h(value);
}

#define M_OFFSET(addr, member) (((u64)&(addr->member)) - ((u64)addr))