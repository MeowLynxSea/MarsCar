#include "vector2f.h"
#include "parameter.h"
#include <stdio.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Vector2f Vector2f::ZERO = Vector2f(0, 0);
Vector2f::Vector2f(float f)
{
    e[0] = f;
    e[1] = f;
}

Vector2f::Vector2f(float x, float y)
{
    e[0] = x;
    e[1] = y;
}
Vector2f::Vector2f(const Vector2f &rv)
{
    e[0] = rv[0];
    e[1] = rv[1];
}

Vector2f &Vector2f::operator=(const Vector2f &rv)
{
    if (this != &rv)
    {
        e[0] = rv[0];
        e[1] = rv[1];
    }
    return *this;
}
const float &Vector2f::operator[](int i) const
{
    return e[i];
}

float &Vector2f::operator[](int i)
{
    return e[i];
}
float &Vector2f::x()
{
    return e[0];
}

float &Vector2f::y()
{
    return e[1];
}

float Vector2f::x() const
{
    return e[0];
}

float Vector2f::y() const
{
    return e[1];
}

float Vector2f::length() const
{
    return std::sqrt(e[0] * e[0] + e[1] * e[1]);
}

float Vector2f::squaredLength() const
{
    return (
        e[0] * e[0] +
        e[1] * e[1] );
}

void Vector2f::normalize()
{
    float norm = length();
    e[0] /= norm;
    e[1] /= norm;
}

Vector2f Vector2f::normalized() const
{
    float norm = length();
    return Vector2f(
        e[0] / norm,
        e[1] / norm);
}
void Vector2f::print() const
{
    printf("< %.4f, %.4f >\n",
           e[0], e[1]);
}

Vector2f &Vector2f::operator+=(const Vector2f &v)
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    return *this;
}

Vector2f &Vector2f::operator-=(const Vector2f &v)
{
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    return *this;
}

Vector2f &Vector2f::operator*=(float f)
{
    e[0] *= f;
    e[1] *= f;
    return *this;
}

Vector2f &Vector2f::operator/=(float f)
{
    if (f != 0)
    {
        e[0] /= f;
        e[1] /= f;
    }
    return *this;
}

Vector2f operator+(const Vector2f &v0, const Vector2f &v1)
{
    return Vector2f(v0[0] + v1[0], v0[1] + v1[1]);
}

Vector2f operator-(const Vector2f &v0, const Vector2f &v1)
{
    return Vector2f(v0[0] - v1[0], v0[1] - v1[1]);
}

double operator*(const Vector2f &v0, const Vector2f &v1)
{
    return v0[0] * v1[0] + v0[1] * v1[1];
}

double operator^(const Vector2f &v0, const Vector2f &v1)
{
    return v0[0]*v1[1]-v0[1]*v1[0];
}

Vector2f operator-(const Vector2f &v)
{
    return Vector2f(-v[0], -v[1]);
}

Vector2f operator*(float f, const Vector2f &v)
{
	return Vector2f(v[0] * f, v[1] * f);
}

Vector2f operator*(const Vector2f &v, float f)
{
	return Vector2f(v[0] * f, v[1] * f);
}

Vector2f operator/(const Vector2f &v, float f)
{
	return Vector2f(v[0] / f, v[1] / f);
}


bool operator==(const Vector2f &v0, const Vector2f &v1)
{
	return (v0-v1).length()<EPS;
}

bool operator!=(const Vector2f &v0, const Vector2f &v1)
{
	return !(v0 == v1);
}