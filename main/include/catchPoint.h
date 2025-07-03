#ifndef CATCHPOINT_H
#define CATCHPOINT_H
#include "vector3f.h"
#include "vector2f.h"
#include "parameter.h"
#include "matrix2f.h"
#include "moveInfo.h"
#include "ps2_controller.h"
#include <string>
#include <math.h>
#include "wifiServer.h"
class CatchPoint
{
public:
    CatchPoint() : o(Vector3f::ZERO)
    {
        lastTry=true;
        angleBase=PI/2;
        angle=Vector2f(PI/2,0);
        target3=Vector3f(0,ARM_LENGTH_1*std::cos(angle[0])+ARM_LENGTH_2*std::cos(angle[1]+angle[0]-PI/2),
        ARM_LENGTH_1*std::sin(angle[0])+ARM_LENGTH_2*std::sin(angle[1]+angle[0]-PI/2));
        target2 = Vector2f((target3.xy().length()), target3.z());
    };
    ~CatchPoint() = default;
    bool move(const PS2Controller &ps2);
    bool move(WifiServer &wifi);
    bool solution(Vector3f &tar,double Eps);
    void setSteerInfo(Vector2f &sol,int Speed=CATCHER_TIME);
    std::string getSteerInfo();
    void reset();
    void getBall();
private:
    void calculateJacobi(Matrix2f &jacobi,Vector2f &f,Vector2f &sol);
    Vector3f target3;
    Vector2f target2;
    double angleBase;
    double pawState;//0~1
    Vector2f angle;
    Vector3f o;
    
    MoveInfo steerBase;
    MoveInfo steer_1;
    MoveInfo steer_2;
    MoveInfo steerPaw;

    bool lastTry;//上次检测是否为无解
};
#endif // CATCHPOINT_H