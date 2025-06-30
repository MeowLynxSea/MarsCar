#include "catchPoint.h"
#include "esp_random.h"
#include "parameter.h"
#include "matrix2f.h"
#include "vector2f.h"
#include "ps2_controller.h"
void CatchPoint::move(const PS2Controller &ps2)
{
    Vector3f speed = Vector3f::ZERO;
    if (ps2.Button(BUTTON_PAD_UP))
        speed[1] += 1;
    if (ps2.Button(BUTTON_PAD_DOWN))
        speed[1] -= 1;
    if (ps2.Button(BUTTON_PAD_RIGHT))
        speed[0] -= 1;
    if (ps2.Button(BUTTON_PAD_LEFT))
        speed[0] += 1;
    if (ps2.Button(BUTTON_TRIANGLE))
        speed[2] += 1;
    if (ps2.Button(BUTTON_CROSS))
        speed[2] -= 1;
    if (speed.length() > EPS)
        speed = speed.normalized() * CATCHER_SPEED;
    target3 += speed;
    target2 = Vector2f((target3.xy().length()), target3.z());
    // target3.print();
}

bool CatchPoint::solution(double Eps = 1e-2)
{
    bool success = false;

    angle_base = target3.xy().angleWithX();

    // 牛顿迭代20次,尝试10次不同初值
    for (int now = 0; now < 10; now++)
    {
        uint32_t random_integer = esp_random();
        double random_double = (double)random_integer / UINT32_MAX;
        Vector2f sol;
        sol[0] = random_double * 2 * PI;
        random_integer = esp_random();
        random_double = (double)random_integer / UINT32_MAX;
        sol[1] = random_double * 2 * PI;

        Matrix2f jacobi;
        Vector2f f;
        double eps;
        for (int i = 0; i < 50; i++)
        {
            int zero;
            calculateJacobi(jacobi, f, sol);
            Vector2f last_sol = sol;
            sol = sol - jacobi.inverse(zero) * f;
            eps = (sol - last_sol).length();
            if (zero)
                break;
            if (eps < Eps)
            {
                success = true;
                break;
            }
        }

        if (success)
        {
            while (sol[0] > PI)
                sol[0] -= 2 * PI;
            while (sol[0] < -PI)
                sol[0] += 2 * PI;
            while (sol[1] > PI)
                sol[1] -= 2 * PI;
            while (sol[1] < -PI)
                sol[1] += 2 * PI;

            sol[1] = sol[1] - sol[0] + PI / 2;
            angle = sol;
            setSteerInfo();
            break;
        }
    }

    if (success)
        printf("有解! ");
    else
        printf("当前钩爪无解！\n");

    return success;
}
void CatchPoint::calculateJacobi(Matrix2f &jacobi, Vector2f &f, Vector2f &sol)
{
    jacobi = Matrix2f(-ARM_LENGTH_1 * std::sin(sol[0]), -ARM_LENGTH_2 * std::sin(sol[1]),
                      ARM_LENGTH_1 * std::cos(sol[0]), ARM_LENGTH_2 * std::cos(sol[1]));
    f = Vector2f(ARM_LENGTH_1 * std::cos(sol[0]) + ARM_LENGTH_2 * std::cos(sol[1]) - target2.x(),
                 ARM_LENGTH_1 * std::sin(sol[0]) + ARM_LENGTH_2 * std::sin(sol[1]) - target2.y());
}
void CatchPoint::setSteerInfo()
{
    double pwm = 1020 + (angle_base * 1.0f / PI) * 980;
    steer_base.setInfo(CATCHER_1, (int)pwm, CATCHER_TIME);
    pwm = 850 + (angle[0] * 1.0f / PI) / 1300;
    steer_1.setInfo(CATCHER_2, (int)pwm, CATCHER_TIME);
    // TODO:
    pwm = 0;
    steer_2.setInfo(CATCHER_3, (int)pwm, CATCHER_TIME);
}
std::string CatchPoint::getSteerInfo()
{
    std::string result = steer_base.getInfo() + steer_1.getInfo() + steer_2.getInfo();
    return result;
}