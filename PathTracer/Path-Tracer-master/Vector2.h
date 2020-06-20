#pragma once
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#define MAXN 1000000

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f) { }
	Vector2(float x, float y) : x(x), y(y) { }

	void Scramble();

	Vector2& operator=(const Vector2 &vec) { x = vec.x; y = vec.y; return *this; }

public:
	float x, y;

};

inline Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs) { return Vector2(lhs.x-rhs.x, lhs.y-rhs.y); }

inline Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs) { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }

inline Vector2 operator*(const Vector2 &v, float s)
{
	return Vector2(v.x*s, v.y*s);
}

inline Vector2 operator*(float s, const Vector2 &v)
{
	return Vector2(v.x*s, v.y*s);
}

inline void Vector2::Scramble()
{


        x = rand() % (MAXN + 1) / (float)(MAXN + 1);
        y = rand() % (MAXN + 1) / (float)(MAXN + 1);
}
