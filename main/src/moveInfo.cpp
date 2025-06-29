#include "moveInfo.h"
#include "parameter.h"
#include <iostream>
#include <string>
std::string MoveInfo::getInfo()
{
    char *Pwm = new char[5];
    char *T = new char[5];
    sprintf(Pwm, "%04d", pwm);
    sprintf(T, "%04d", t);
    std::string result = "#" + std::string(index) + "P" + std::string(Pwm) + "T" + std::string(T) + "!";
    delete[] Pwm;
    delete[] T;
    return result;
}
void MoveInfo::setInfo(std::string index, int pwm, int t)
{
    this->index = index;
    this->pwm = pwm;
    this->t = t;
}

//================轮子==============//
WheelMoveInfo::WheelMoveInfo(std::string fl, std::string fr, std::string bl, std::string br, int lx, int ly)
{
    auto abs = [](auto x)
    { return x > 0 ? x : -x; };
    auto max = [](auto x, auto y)
    { return x > y ? x : y; };

    double x = lx * 1.0 - 255.0 / 2;
    double y = ly * 1.0 - 255.0 / 2;
    double leftRatio, rightRatio;
    if (x < 0 && y > 0)
    {
        leftRatio = x + y;
        rightRatio = max(-x, y);
    }
    else if (x < 0 && y < 0)
    {
        leftRatio = -abs(x - y);
        if (x < y)
            rightRatio = -x + 2 * y;
        else
            rightRatio = y;
    }
    else if (x > 0 && y > 0)
    {
        leftRatio = max(x, y);
        rightRatio = y - x;
    }
    else
    {
        rightRatio = -abs(x + y);
        if (x + y < 0)
            leftRatio = y;
        else
            leftRatio = x + 2 * y;
    }

    front_left.setInfo(fl, 1500 + leftRatio * MAX_WHEEL_SPEED, FRAME_TIME);
    back_left.setInfo(fl, 1500 + leftRatio * MAX_WHEEL_SPEED, FRAME_TIME);

    front_right.setInfo(fr, 1500 + rightRatio * MAX_WHEEL_SPEED, FRAME_TIME);
    back_right.setInfo(br, 1500 + rightRatio * MAX_WHEEL_SPEED, FRAME_TIME);
}
std::string WheelMoveInfo::getWheelInfo()
{
    std::string result = front_left.getInfo() + front_right.getInfo() + back_left.getInfo() + back_right.getInfo();
    return result;
}

//==================底盘舵机============//
std::string SteerMoveInfo::getSteerInfo()
{
    std::string result = front_left.getInfo() + front_right.getInfo() + back_left.getInfo() + back_right.getInfo();
    return result;
}