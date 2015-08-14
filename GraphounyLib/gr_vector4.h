#pragma once
#include "gr_vector3.h"

template <class T>
struct _t_vector4
{
	T x, y, z, w;

	_t_vector4() {}
	_t_vector4(T x) : x(x) {}
	_t_vector4(T x, T y) : x(x), y(y) {}
	_t_vector4(T x, T y, T z) : x(x), y(y), z(z) {}
	_t_vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

	_t_vector4(_t_vector3<T> xyz, T w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
	_t_vector4(T x, _t_vector3<T> yzw) : x(x), y(yzw.y), z(yzw.z), w(yzw.w) {}

	_t_vector4(_t_vector2<T> xy, T z, T w) : x(xy.x), y(xy.y), z(z), w(w) {}
	_t_vector4(_t_vector2<T> xy, _t_vector2<T> zw) : x(xy.x), y(xy.y), z(zw.z), w(zw.w) {}
	_t_vector4(T x, T y, _t_vector2<T> zw) : x(x), y(y), z(zw.z), w(zw.w) {}

	inline T lengthSq() const { return x*x + y*y + z*z + w*w; }// const;
	inline T length() const { return sqrt(x*x + y*y + z*z + w*w); }// const;
	inline void add(const _t_vector4<T>& other) { x += other.x; y += other.y; z += other.z; w += other.w; }
	inline void sub(const _t_vector4<T>& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; }
	inline void mul(const _t_vector4<T>& other) { x *= other.x; y *= other.y; z *= other.z; w *= other.w; }
	inline void div(const _t_vector4<T>& other) { x /= other.x; y /= other.y; z /= other.z; w /= other.w; }
	inline void add(const T o) { x += o; y += o; z += o; w += o; }
	inline void sub(const T o) { x -= o; y -= o; z -= o; w -= o; }
	inline void mul(const T other) { x *= other; y *= other; z *= other; w *= other; }
	inline void div(const T other) { x /= other; y /= other; z /= other; w /= other; }
	inline void neg() { x = -x; y = -y; z = -z; w = -w; }
	inline void normalize() { div(length()); }
	inline _t_vector4<T> normalized() const { T l = length();  return _t_vector4<T>(x / l, y / l, z / l, w / l); }
	inline T dot(const _t_vector4<T>& other) const  { return x*other.x + y*other.y + z*other.z + w*other.w; }
	inline T angle_cos(const _t_vector4<T>& other)  const { return dot(other) / other.length() / length(); }
	inline T angle(const _t_vector4<T>& other) const  { return acos(angle_cos(other)); }
	inline T angle_sin(const _t_vector4<T>& other) const  { return sin(angle(other)); }

	inline _t_vector2<T>* xy() { return (_t_vector2<T>*)(&x); }
	inline _t_vector2<T>* yz() { return (_t_vector2<T>*)(&y); }
	inline _t_vector2<T>* zw() { return (_t_vector2<T>*)(&z); }

	inline _t_vector3<T>* xyz() { return (_t_vector3<T>*)(&x); }
	inline _t_vector3<T>* yzw() { return (_t_vector3<T>*)(&y); }

	inline _t_vector2<T> xy_c() const { return asv(_t_vector2<T>)x; }
	inline _t_vector2<T> yz_c() const  { return asv(_t_vector2<T>)y; }
	inline _t_vector2<T> zw_c()  const { return asv(_t_vector2<T>)z; }

	inline _t_vector3<T> xyz_c() const  { return asv(_t_vector3<T>)x; }
	inline _t_vector3<T> yzw_c() const  { return asv(_t_vector3<T>)y; }
};

typedef _t_vector4<i32> v4i;
typedef _t_vector4<f32> v4f;
typedef _t_vector4<f64> v4d;

template <class T>
static _t_vector4<T> operator + (const _t_vector4<T>& a, const _t_vector4<T>& b)
{
	return{ a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
}
template <class T>
static _t_vector4<T> operator - (const _t_vector4<T>& a, const _t_vector4<T>& b)
{
	return{ a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
}
template <class T>
static _t_vector4<T> operator * (const _t_vector4<T>& a, const _t_vector4<T>& b)
{
	return{ a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w };
}
template <class T>
static _t_vector4<T> operator / (const _t_vector4<T>& a, const _t_vector4<T>& b)
{
	return{ a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w };
}

template <class T>
static _t_vector4<T> operator + (const _t_vector4<T>& a, const T& b)
{
	return _t_vector4<T>(a.x + b, a.y + b, a.z + b, a.w + b);
}
template <class T>
static _t_vector4<T> operator - (const _t_vector4<T>& a, const T& b)
{
	return _t_vector4<T>(a.x - b, a.y - b, a.z - b, a.w - b);
}
template <class T>
static _t_vector4<T> operator * (const _t_vector4<T>& a, const T& b)
{
	return _t_vector4<T>(a.x * b, a.y * b, a.z * b, a.w * b);
}
template <class T>
static _t_vector4<T> operator / (const _t_vector4<T>& a, const T& b)
{
	return _t_vector4<T>(a.x / b, a.y / b, a.z / b, a.w / b);
}

template <class T>
static _t_vector4<T> operator + (const T& a, const _t_vector4<T>& b)
{
	return _t_vector4<T>(a + b.x, a + b.y, a + b.z, a + b.w);
}
template <class T>
static _t_vector4<T> operator - (const T& a, const _t_vector4<T>& b)
{
	return _t_vector4<T>(a - b.x, a - b.y, a - b.z, a - b.w);
}
template <class T>
static _t_vector4<T> operator * (const T& a, const _t_vector4<T>& b)
{
	return _t_vector4<T>(a * b.x, a * b.y, a * b.z, a * b.w);
}
template <class T>
static _t_vector4<T> operator / (const T& a, const _t_vector4<T>& b)
{
	return _t_vector4<T>(a / b.x, a / b.y, a / b.z, a / b.w);
}
template <class T>
static _t_vector4<T> operator ! (const _t_vector4<T>& b)
{
	return b.normalized();
}
template <class T>
static _t_vector4<T> operator - (const _t_vector4<T>& b)
{
	return _t_vector4<T>(-b.x, -b.y, -b.z, -b.w);
}

const v4f V4F_ZERO = v4f(0, 0, 0, 0);

template <class T>
static bool operator == (const _t_vector4<T>& a, const _t_vector4<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}
template <class T>
static bool operator != (const _t_vector4<T>& a, const _t_vector4<T>& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}