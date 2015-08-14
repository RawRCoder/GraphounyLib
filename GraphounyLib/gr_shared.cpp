#include "gr_shared.h"
#include <boost/filesystem.hpp>


std::wstring operator/(const std::wstring& s1, const std::wstring& s2)
{
	namespace fs = boost::filesystem;
	auto path = fs::path(s1) / fs::path(s2);
	return path.c_str();
}

f32 randf(f32 LO, f32 HI)
{
	return (LO + static_cast <f32>(rand()) / (static_cast<f32>(RAND_MAX / (HI - LO))));
}

f64 randd(f64 LO, f64 HI)
{
	return (LO + static_cast <f64>(rand()) / (static_cast<f64>(RAND_MAX / (HI - LO))));
}

i32 randi(i32 LO, i32 HI)
{
	return static_cast<i32>(randf(static_cast<f32>(LO), static_cast<f32>(HI)));
}