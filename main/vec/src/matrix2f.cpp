#include "matrix2f.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include "vector2f.h"
Matrix2f::Matrix2f(float fill)
{
    for (int i = 0; i < 4; ++i)
    {
        e[i] = fill;
    }
}

Matrix2f::Matrix2f(float m00, float m01,
                   float m10, float m11)
{
    e[0] = m00;
    e[1] = m10;

    e[2] = m01;
    e[3] = m11;
}


const float &Matrix2f::operator()(int i, int j) const
{
    return e[j * 2 + i];
}

float &Matrix2f::operator()(int i, int j)
{
    return e[j * 2 + i];
}

float Matrix2f::determinant()
{
    return Matrix2f::determinant2x2(
        e[0], e[2],
        e[1], e[3]);
}

Matrix2f Matrix2f::inverse(int &error, float epsilon)
{
    error=1;
    float determinant = e[0] * e[3] - e[2] * e[1];

    bool isSingular = (fabs(determinant) < epsilon);
    if (isSingular)
        return Matrix2f();
    else
    {
        error=0;
        float reciprocalDeterminant = 1.0f / determinant;
        return Matrix2f(
            e[3] * reciprocalDeterminant, -e[2] * reciprocalDeterminant,
            -e[1] * reciprocalDeterminant, e[0] * reciprocalDeterminant);
    }
}

void Matrix2f::print()
{
    printf("[ %.4f %.4f ]\n[ %.4f %.4f ]\n",
           e[0], e[2],
           e[1], e[3]);
}

// static
float Matrix2f::determinant2x2(float m00, float m01,
                               float m10, float m11)
{
    return (m00 * m11 - m01 * m10);
}

// static
Matrix2f Matrix2f::ones()
{
    Matrix2f m;
    for (int i = 0; i < 4; ++i)
        m.e[i] = 1;

    return m;
}

// static
Matrix2f Matrix2f::identity()
{
    Matrix2f m;

    m(0, 0) = 1;
    m(1, 1) = 1;

    return m;
}

Matrix2f operator*(float f, const Matrix2f &m)
{
    Matrix2f output;

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            output(i, j) = f * m(i, j);
    return output;
}

Matrix2f operator*(const Matrix2f &m, float f)
{
    return f * m;
}

Vector2f operator*(const Matrix2f &m, const Vector2f &v)
{
    Vector2f output(0, 0);

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            output[i] += m(i, j) * v[j];

    return output;
}

Matrix2f operator*(const Matrix2f &x, const Matrix2f &y)
{
    Matrix2f product; // zeroes

    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            for (int k = 0; k < 2; ++k)
                product(i, k) += x(i, j) * y(j, k);

    return product;
}
