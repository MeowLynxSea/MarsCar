#ifndef MATRIX2F_H
#define MATRIX2F_H
#include "parameter.h"
class Vector2f;
class Matrix2f
{
public:
    Matrix2f(float fill = 0.f);
    Matrix2f(float m00, float m01,
             float m10, float m11);

    const float &operator()(int i, int j) const;
    float &operator()(int i, int j);

    float determinant();
    Matrix2f inverse(int &error,float epsilon = EPSSILON);

    void print();

    static float determinant2x2(float m00, float m01,
                                float m10, float m11);

    static Matrix2f ones();
    static Matrix2f identity();

private:
    float e[4];
};

// Scalar-Matrix multiplication
Matrix2f operator * ( float f, const Matrix2f& m );
Matrix2f operator * ( const Matrix2f& m, float f );

// Matrix-Vector multiplication
// 2x2 * 2x1 ==> 2x1
Vector2f operator * ( const Matrix2f& m, const Vector2f& v );

// Matrix-Matrix multiplication
Matrix2f operator * ( const Matrix2f& x, const Matrix2f& y );
#endif