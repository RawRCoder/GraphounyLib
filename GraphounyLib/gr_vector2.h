#pragma once
#include "gr_shared.h"

template <class T>
struct _t_vector2
{
	T x, y;

	_t_vector2() {}
	_t_vector2(T x) : x(x) {}
	_t_vector2(T x, T y) : x(x), y(y) {}

	//template <class T>
	inline T lengthSq()  const { return x*x + y*y; }// const;
	inline T length()  const { return sqrt(x*x + y*y); }// const;
	inline void add(const _t_vector2<T>& other) { x += other.x; y += other.y; }
	inline void sub(const _t_vector2<T>& other) { x -= other.x; y -= other.y; }
	inline void mul(const _t_vector2<T>& other) { x *= other.x; y *= other.y; }
	inline void div(const _t_vector2<T>& other) { x /= other.x; y /= other.y; }
	inline void add(const T other) { x += other; y += other; }
	inline void sub(const T other) { x -= other; y -= other; }
	inline void mul(const T other) { x *= other; y *= other; }
	inline void div(const T other) { x /= other; y /= other; }
	inline void neg() { x = -x; y = -y; }
	inline void normalize() { div(length()); }
	inline _t_vector2<T> normalized() const  { T l = length();  return _t_vector2<T>(x / l, y / l); }
	inline T dot(const _t_vector2<T>& other)  const { return x*other.x + y*other.y; }// const;
	inline T angle_cos(const _t_vector2<T>& other)  const { return dot(other) / other.length() / length(); }
	inline T angle(const _t_vector2<T>& other) const  { return acos(angle_cos(other)); }
	inline T angle_sin(const _t_vector2<T>& other) const  { return sin(angle(other)); }
};

typedef _t_vector2<i32> v2i;
typedef _t_vector2<f32> v2f;
typedef _t_vector2<f64> v2d;
typedef _t_vector2<f32> float2;

// VectorVector operations:
template <class T>
static _t_vector2<T> operator + (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a.x + b.x, a.y + b.y);
}
template <class T>
static _t_vector2<T> operator - (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a.x - b.x, a.y - b.y);
}
template <class T>
static _t_vector2<T> operator * (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a.x * b.x, a.y * b.y);
}
template <class T>
static _t_vector2<T> operator / (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a.x / b.x, a.y / b.y);
}

// VectorScalar operations:
template <class T>
static _t_vector2<T> operator + (const _t_vector2<T>& a, const T& b)
{
	return _t_vector2<T>(a.x + b, a.y + b);
}
template <class T>
static _t_vector2<T> operator - (const _t_vector2<T>& a, const T& b)
{
	return _t_vector2<T>(a.x - b, a.y - b);
}
template <class T>
static _t_vector2<T> operator * (const _t_vector2<T>& a, const T& b)
{
	return _t_vector2<T>(a.x * b, a.y * b);
}
template <class T>
static _t_vector2<T> operator / (const _t_vector2<T>& a, const T& b)
{
	return _t_vector2<T>(a.x / b, a.y / b);
}

// ScalarVector operations:
template <class T>
static _t_vector2<T> operator + (const T& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a + b.x, a + b.y);
}
template <class T>
static _t_vector2<T> operator - (const T& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a - b.x, a - b.y);
}
template <class T>
static _t_vector2<T> operator * (const T& a, const _t_vector2<T>& b)
{
	return _t_vector2<T>(a * b.x, a * b.y);
}
template <class T>
static _t_vector2<T> operator / (const T& b, const _t_vector2<T>& a)
{
	return _t_vector2<T>(a / b.x, a / b.y);
}

template <class T>
// Dot product
static T operator | (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return a.dot(b);
}
template <class T>
// Normalized version of a vector
static _t_vector2<T> operator !(const _t_vector2<T>& a)
{
	return a.normalized();
}

const v2f V2F_ZERO =	v2f(0, 0);
const v2f V2F_RIGHT =	v2f(1, 0);
const v2f V2F_LEFT =	v2f(-1, 0);
const v2f V2F_UP =		v2f(0, 1);
const v2f V2F_DOWN =	v2f(0, -1);


template <class T>
static bool operator == (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return a.x == b.x && a.y == b.y;
}
template <class T>
static bool operator != (const _t_vector2<T>& a, const _t_vector2<T>& b)
{
	return a.x != b.x || a.y != b.y;
}