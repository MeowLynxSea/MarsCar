#ifndef VECTOR3F_H
#define VECTOR3F_H
#include "vector2f.h"
class Vector3f
{
public:
    static const Vector3f ZERO;
    static const Vector3f UP;
    static const Vector3f RIGHT;
    static const Vector3f FORWARD;

    Vector3f(float f = 0.f);
    Vector3f(float x, float y, float z);
    Vector3f(const Vector3f &rv);

    // assignment operators
    Vector3f &operator=(const Vector3f &rv);

    // returns the ith element
    const float &operator[](int i) const;
    float &operator[](int i);

    float &x();
    float &y();
    float &z();

    float x() const;
    float y() const;
    float z() const;
    Vector2f xy()const;

    float length() const;
    float squaredLength() const;

    void normalize();
    Vector3f normalized() const;

    Vector3f &operator+=(const Vector3f &v);
    Vector3f &operator-=(const Vector3f &v);
    Vector3f &operator*=(float f);
    Vector3f &operator/=(float f);

    void print() const;

private:
    float e[3];
};

// component-wise operators
Vector3f operator+(const Vector3f &v0, const Vector3f &v1);
Vector3f operator-(const Vector3f &v0, const Vector3f &v1);
float operator*(const Vector3f &v0, const Vector3f &v1);
Vector3f operator^(const Vector3f &v0, const Vector3f &v1);

Vector3f operator-(const Vector3f &v);

// multiply and divide by scalar
Vector3f operator*(float f, const Vector3f &v);
Vector3f operator*(const Vector3f &v, float f);
Vector3f operator/(const Vector3f &v, float f);

bool operator==(const Vector3f &v0, const Vector3f &v1);
bool operator!=(const Vector3f &v0, const Vector3f &v1);
#endif