#ifndef CATCHPOINT_H
#define CATCHPOINT_H
#include "vector3f.h"
#include "vector2f.h"
#include "parameter.h"
#include "matrix2f.h"
#include "moveInfo.h"
#include "ps2_controller.h"
#include <string>
class CatchPoint
{
public:
    CatchPoint(const Vector3f &p=Vector3f(0,0,ARM_LENGTH_1+ARM_LENGTH_2)) : target3(p), o(Vector3f::ZERO)
    {
    };
    ~CatchPoint() = default;
    void move(const PS2Controller &ps2);
    bool solution(double Eps);
    void setSteerInfo();
    std::string getSteerInfo();
private:
    void calculateJacobi(Matrix2f &jacobi,Vector2f &f,Vector2f &sol);
    Vector3f target3;
    Vector2f target2;
    double angle_base;
    Vector2f angle;
    Vector3f o;

    MoveInfo steer_base;
    MoveInfo steer_1;
    MoveInfo steer_2;
};
#endif // CATCHPOINT_H