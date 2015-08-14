#pragma once
#include "gr_vector2.h"

template <class T>
struct _t_vector3
{
	T x, y, z;

	_t_vector3() {}
	_t_vector3(T x) : x(x) {}
	_t_vector3(T x, T y) : x(x), y(y) {}
	_t_vector3(T x, T y, T z) : x(x), y(y), z(z) {}

	inline T lengthSq()  const { return x*x + y*y + z*z; }// const;
	inline T length() const  { return sqrt(x*x + y*y + z*z); }// const;
	inline void add(const _t_vector3<T>& other) { x += other.x; y += other.y;  z += other.z; }
	inline void sub(const _t_vector3<T>& other) { x -= other.x; y -= other.y; z -= other.z; }
	inline void mul(const _t_vector3<T>& other) { x *= other.x; y *= other.y; z *= other.z; }
	inline void div(const _t_vector3<T>& other) { x /= other.x; y /= other.y; z /= other.z; }
	inline void add(const T other) { x += other; y += other; z += other; }
	inline void sub(const T other) { x -= other; y -= other; z -= other; }
	inline void mul(const T other) { x *= other; y *= other; z *= other; }
	inline void div(const T other) { x /= other; y /= other; z /= other; }
	inline void neg() { x = -x; y = -y; z = -z; }
	inline void normalize() { div(length()); }
	inline _t_vector3<T> normalized() const { T l = length();  return _t_vector3<T>(x / l, y / l, z / l); }
	inline T dot(const _t_vector3<T>& other)  const { return x*other.x + y*other.y + z*other.z; }// const;
	inline T angle_cos(const _t_vector3<T>& other)  const { return dot(other) / other.length() / length(); }
	inline T angle(const _t_vector3<T>& other) const  { return acos(angle_cos(other)); }
	inline T angle_sin(const _t_vector3<T>& other)  const { return sin(angle(other)); }

	inline _t_vector3<T> cross(const _t_vector3<T>& o) const { return _t_vector3(y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x); }// const;

	inline _t_vector2<T>* xy() { return (_t_vector2<T>*)&x; }
	inline _t_vector2<T>* yz() { return (_t_vector2<T>*)&y; }
	inline _t_vector2<T> xy_c() const { return *((_t_vector2<T>*)&x); }
	inline _t_vector2<T> yz_c() const { return *((_t_vector2<T>*)&y); }
};


typedef _t_vector3<i32> v3i;
typedef _t_vector3<f32> v3f;
typedef _t_vector3<f64> v3d;


template <class T>
static _t_vector3<T> operator + (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return{ a.x + b.x, a.y + b.y, a.z + b.z };
}
template <class T>
static _t_vector3<T> operator - (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return{ a.x - b.x, a.y - b.y, a.z - b.z };
}
template <class T>
static _t_vector3<T> operator * (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return{ a.x * b.x, a.y * b.y, a.z * b.z };
}
template <class T>
static _t_vector3<T> operator / (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return{ a.x / b.x, a.y / b.y, a.z / b.z };
}

template <class T>
static _t_vector3<T> operator + (const _t_vector3<T>& a, const T& b)
{
	return _t_vector3<T>(a.x + b, a.y + b, a.z + b);
}
template <class T>
static _t_vector3<T> operator - (const _t_vector3<T>& a, const T& b)
{
	return _t_vector3<T>(a.x - b, a.y - b, a.z - b);
}
template <class T>
static _t_vector3<T> operator * (const _t_vector3<T>& a, const T& b)
{
	return _t_vector3<T>(a.x * b, a.y * b, a.z * b);
}
template <class T>
static _t_vector3<T> operator / (const _t_vector3<T>& a, const T& b)
{
	return _t_vector3<T>(a.x / b, a.y / b, a.z / b);
}

template <class T>
static _t_vector3<T> operator + (const T& a, const _t_vector3<T>& b)
{
	return _t_vector3<T>(a + b.x, a + b.y, a + b.y);
}
template <class T>
static _t_vector3<T> operator - (const T& a, const _t_vector3<T>& b)
{
	return _t_vector3<T>(a - b.x, a - b.y, a - b.y);
}
template <class T>
static _t_vector3<T> operator * (const T& a, const _t_vector3<T>& b)
{
	return _t_vector3<T>(a * b.x, a * b.y, a * b.z);
}
template <class T>
static _t_vector3<T> operator / (const T& a, const _t_vector3<T>& b)
{
	return _t_vector3<T>(a / b.x, a / b.y, a / b.y);
}
template <class T>
static _t_vector3<T> operator - (const _t_vector3<T>& b)
{
	return _t_vector3<T>(-b.x, -b.y, -b.z);
}


template <class T>
// Dot product
static T operator | (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return a.dot(b);
}

template <class T>
// Cross product
static _t_vector3<T> operator & (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return a.cross(b);
}


template <class T>
// Normalized version of a vector
static _t_vector3<T> operator !(const _t_vector3<T>& a)
{
	return a.normalized();
}

const v3f V3F_ZERO		= v3f(0, 0, 0);
const v3f V3F_FORWARD	= v3f(0, 0, -1);
const v3f V3F_BACKWARD	= v3f(0, 0, 1);
const v3f V3F_RIGHT		= v3f(1, 0, 0);
const v3f V3F_LEFT		= v3f(-1, 0, 0);
const v3f V3F_UP		= v3f(0, 1, 0);
const v3f V3F_DOWN		= v3f(0, -1, 0);

template <class T>
static bool operator == (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}
template <class T>
static bool operator != (const _t_vector3<T>& a, const _t_vector3<T>& b)
{
	return a.x != b.x || a.y != b.y || a.z != b.z;
}