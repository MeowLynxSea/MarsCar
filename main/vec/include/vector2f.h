#ifndef VECTOR2F_H
#define VECTOR2F_H
#include <math.h>
#include "parameter.h"
class Vector2f
{
public:
    static const Vector2f ZERO;

    Vector2f(float f = 0.f);
    Vector2f(float x, float y);
    Vector2f(const Vector2f &rv);

    // assignment operators
    Vector2f &operator=(const Vector2f &rv);

    // returns the ith element
    const float &operator[](int i) const;
    float &operator[](int i);

    float &x();
    float &y();
    float x() const;
    float y() const;

    float length() const;
    float squaredLength() const;

    void normalize();
    Vector2f normalized() const;

    Vector2f &operator+=(const Vector2f &v);
    Vector2f &operator-=(const Vector2f &v);
    Vector2f &operator*=(float f);
    Vector2f &operator/=(float f);

    void print() const;
    double angleWithX()
    {
        double angle = std::atan2(e[1], e[0]);
        if (angle < -PI/2)
            angle += 2 * PI;
        return angle;
    }

private:
    float e[2];
};

// component-wise operators
Vector2f operator+(const Vector2f &v0, const Vector2f &v1);
Vector2f operator-(const Vector2f &v0, const Vector2f &v1);
double operator*(const Vector2f &v0, const Vector2f &v1);
double operator^(const Vector2f &v0, const Vector2f &v1);

Vector2f operator-(const Vector2f &v);

// multiply and divide by scalar
Vector2f operator*(float f, const Vector2f &v);
Vector2f operator*(const Vector2f &v, float f);
Vector2f operator/(const Vector2f &v, float f);

bool operator==(const Vector2f &v0, const Vector2f &v1);
bool operator!=(const Vector2f &v0, const Vector2f &v1);
#endif