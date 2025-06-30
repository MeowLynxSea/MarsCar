#include "vector3f.h"
#include "parameter.h"
#include <stdio.h>
#include <math.h>
//////////////////////////////////////////////////////////////////////////
// Public
//////////////////////////////////////////////////////////////////////////

// static
const Vector3f Vector3f::ZERO = Vector3f(0, 0, 0);
const Vector3f Vector3f::UP = Vector3f(0, 1, 0);
const Vector3f Vector3f::RIGHT = Vector3f(1, 0, 0);
const Vector3f Vector3f::FORWARD = Vector3f(0, 0, -1);

Vector3f::Vector3f(float f)
{
    e[0] = f;
    e[1] = f;
    e[2] = f;
}

Vector3f::Vector3f(float x, float y, float z)
{
    e[0] = x;
    e[1] = y;
    e[2] = z;
}
Vector3f::Vector3f(const Vector3f &rv)
{
    e[0] = rv[0];
    e[1] = rv[1];
    e[2] = rv[2];
}

Vector3f &Vector3f::operator=(const Vector3f &rv)
{
    if (this != &rv)
    {
        e[0] = rv[0];
        e[1] = rv[1];
        e[2] = rv[2];
    }
    return *this;
}
const float &Vector3f::operator[](int i) const
{
    return e[i];
}

float &Vector3f::operator[](int i)
{
    return e[i];
}
float &Vector3f::x()
{
    return e[0];
}

float &Vector3f::y()
{
    return e[1];
}

float &Vector3f::z()
{
    return e[2];
}

float Vector3f::x() const
{
    return e[0];
}

float Vector3f::y() const
{
    return e[1];
}

float Vector3f::z() const
{
    return e[2];
}
Vector2f Vector3f::xy() const
{
    return Vector2f(e[0], e[1]);
}
float Vector3f::length() const
{
    return std::sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
}

float Vector3f::squaredLength() const
{
    return (
        e[0] * e[0] +
        e[1] * e[1] +
        e[2] * e[2]);
}

void Vector3f::normalize()
{
    float norm = length();
    e[0] /= norm;
    e[1] /= norm;
    e[2] /= norm;
}

Vector3f Vector3f::normalized() const
{
    float norm = length();
    return Vector3f(
        e[0] / norm,
        e[1] / norm,
        e[2] / norm);
}
void Vector3f::print() const
{
    printf("< %.4f, %.4f, %.4f >\n",
           e[0], e[1], e[2]);
}

Vector3f &Vector3f::operator+=(const Vector3f &v)
{
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
}

Vector3f &Vector3f::operator-=(const Vector3f &v)
{
    e[0] -= v.e[0];
    e[1] -= v.e[1];
    e[2] -= v.e[2];
    return *this;
}

Vector3f &Vector3f::operator*=(float f)
{
    e[0] *= f;
    e[1] *= f;
    e[2] *= f;
    return *this;
}

Vector3f &Vector3f::operator/=(float f)
{
    if (f != 0)
    {
        e[0] /= f;
        e[1] /= f;
        e[2] /= f;
    }
    return *this;
}

Vector3f operator+(const Vector3f &v0, const Vector3f &v1)
{
    return Vector3f(v0[0] + v1[0], v0[1] + v1[1], v0[2] + v1[2]);
}

Vector3f operator-(const Vector3f &v0, const Vector3f &v1)
{
    return Vector3f(v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2]);
}

float operator*(const Vector3f &v0, const Vector3f &v1)
{
    return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

Vector3f operator^(const Vector3f &v0, const Vector3f &v1)
{
    return Vector3f(v0[1] * v1[2] - v0[2] * v1[1], -v0[0] * v1[2] + v0[2] * v1[0], v0[0] * v1[1] - v0[1] * v1[0]);
}

Vector3f operator-(const Vector3f &v)
{
    return Vector3f(-v[0], -v[1], -v[2]);
}

Vector3f operator*(float f, const Vector3f &v)
{
    return Vector3f(v[0] * f, v[1] * f, v[2] * f);
}

Vector3f operator*(const Vector3f &v, float f)
{
    return Vector3f(v[0] * f, v[1] * f, v[2] * f);
}

Vector3f operator/(const Vector3f &v, float f)
{
    return Vector3f(v[0] / f, v[1] / f, v[2] / f);
}

bool operator==(const Vector3f &v0, const Vector3f &v1)
{
    return (v0 - v1).length() < EPS;
}

bool operator!=(const Vector3f &v0, const Vector3f &v1)
{
    return !(v0 == v1);
}