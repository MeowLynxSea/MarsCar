#include "catchPoint.h"
#include "esp_random.h"
#include "parameter.h"
#include "matrix2f.h"
#include "vector2f.h"
#include "ps2_controller.h"

void CatchPoint::reset()
{
    lastTry = true;
    pawState = 0;
    angleBase = PI / 2;
    angle = Vector2f(PI / 2 + PI / 12, 0);
    target3 = Vector3f(0, ARM_LENGTH_1 * std::cos(angle[0]) + ARM_LENGTH_2 * std::cos(angle[1] + angle[0] - PI / 2),
                       ARM_LENGTH_1 * std::sin(angle[0]) + ARM_LENGTH_2 * std::sin(angle[1] + angle[0] - PI / 2));
    target2 = Vector2f((target3.xy().length()), target3.z());
    setSteerInfo(angle, 1000);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void CatchPoint::getBall()
{
    lastTry = true;
    pawState = 1; // TODO
    angleBase = PI*1.4;
    angle = Vector2f(1.7, -0.37);
    double y = ARM_LENGTH_1 * std::cos(angle[0]) + ARM_LENGTH_2 * std::cos(angle[1] + angle[0] - PI / 2);
    target3 = Vector3f(std::cos(angleBase) * y, std::sin(angleBase) * y,
                       ARM_LENGTH_1 * std::sin(angle[0]) + ARM_LENGTH_2 * std::sin(angle[1] + angle[0] - PI / 2));
    target2 = Vector2f((target3.xy().length()), target3.z());

    setSteerInfo(angle, 1000);
    vTaskDelay(100 / portTICK_PERIOD_MS);
}
bool CatchPoint::move(const PS2Controller &ps2)
{
    if (ps2.Button(BUTTON_START))
    {
        reset();
        return false;
    }
    if (ps2.Button(BUTTON_CIRCLE) && pawState < 1)
        pawState += PAW_SPEED;
    else if (ps2.Button(BUTTON_SQUARE) && pawState > 0)
        pawState -= PAW_SPEED;

    Vector3f speed = Vector3f::ZERO;
    double r = 1;
    if (!lastTry)
        r = 5;
    if (ps2.Button(BUTTON_PAD_UP))
        speed[1] += 1;
    if (ps2.Button(BUTTON_PAD_DOWN))
        speed[1] -= 1;
    if (ps2.Button(BUTTON_PAD_RIGHT))
        speed[0] += 1;
    if (ps2.Button(BUTTON_PAD_LEFT))
        speed[0] -= 1;
    if (ps2.Button(BUTTON_TRIANGLE))
        speed[2] += 1;
    if (ps2.Button(BUTTON_CROSS))
        speed[2] -= 1;
    if (speed.length() > EPSSILON)
        speed = speed.normalized() * CATCHER_SPEED * r;

    Vector3f temp = target3 + speed;
    if (solution(temp, 1e-2))
    {
        target3 += speed;
        target2 = Vector2f((target3.xy().length()), target3.z());
        return true;
    }
    // target3.print();
    return false;
}

bool CatchPoint::move(WifiServer &wifi)
{
    if (wifi.get_reset())
    {
        reset();
        return false;
    }
    if (wifi.get_ball())
    {
        getBall();
        return false;
    }
    if (wifi.getPaw() > 0.5 && pawState < 1)
        pawState += (wifi.getPaw()-0.5)*PAW_SPEED;
    else if (wifi.getPaw() < 0.5 && pawState > 0)
        pawState += ((wifi.getPaw()-0.5))*PAW_SPEED;

    Vector3f speed = Vector3f::ZERO;
    speed[1] = wifi.getFORWARD() + wifi.getBACKWARD();
    speed[0] = wifi.getRIGHT() + wifi.getLEFT();
    speed[2] = wifi.getUP() + wifi.getDOWN();

    // if (speed.length() > EPSSILON)
    //  speed = speed.normalized() * WIFI_CATCHER_SPEED;

    Vector3f temp = target3 + speed;
    if (temp.length() > ARM_LENGTH_1 + ARM_LENGTH_2 - 0.1)
        temp = temp.normalized() * (ARM_LENGTH_1 + ARM_LENGTH_2 - 0.1);
    // speed.print();
    // target3.print();
    if (solution(temp, 1e-2))
    {
        target3 = temp;
        target2 = Vector2f((target3.xy().length()), target3.z());

        return true;
    }
    return false;
}

bool CatchPoint::solution(Vector3f &tar, double Eps = 1e-2)
{
    bool success = false;
    bool success2 = false;
    angleBase = tar.xy().angleWithX();

    Vector2f sol;
    // 牛顿迭代50次,尝试10次不同初值
    for (int now = 0; now < 10; now++)
    {
        // 在此时值扰动
        uint32_t random_integer = esp_random();
        double random_double = (double)random_integer * 2.0 / UINT32_MAX - 1;

        sol[0] = random_double * PI * 0.01;
        random_integer = esp_random();
        random_double = (double)random_integer / UINT32_MAX;
        sol[1] = random_double * PI * 0.01;
        sol += angle;

        Matrix2f jacobi;
        Vector2f f;
        double eps;
        // 找到一个解
        success = false;
        for (int i = 0; i < 50; i++)
        {
            int zero;
            calculateJacobi(jacobi, f, sol);
            Vector2f last_sol = sol;
            sol = sol - jacobi.inverse(zero) * f;
            eps = (sol - last_sol).length();

            if (eps < Eps)
            {
                success = true;
                break;
            }
            if (zero)
                break;
        }
        // 判断解可行性
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

            Vector2f theta = target2.angleWithX();
            Vector2f sol_2 = 2 * theta - sol;

            sol[1] = sol[1] - sol[0] + PI / 2;
            sol_2[1] = sol_2[1] - sol_2[0] + PI / 2;

            if ((angle - sol).squaredLength() > (angle - sol_2).squaredLength())
                sol = sol_2;
            // printf("%.2f ", (angle - sol).length());
            if ((angle - sol).length() < 1)
            {
                success2 = true;
                angle = sol;
                setSteerInfo(angle);
                break;
            }
        }
    }

    // printf("sol: ");
    // sol.print();

    // printf("target: ");
    // tar.print();

    if (!success2)
    {
        printf("target: ");
        tar.print();
        printf("无解！ ");
    }
    // printf("\n");

    return success2;
}
void CatchPoint::calculateJacobi(Matrix2f &jacobi, Vector2f &f, Vector2f &sol)
{
    jacobi = Matrix2f(-ARM_LENGTH_1 * std::sin(sol[0]), -ARM_LENGTH_2 * std::sin(sol[1]),
                      ARM_LENGTH_1 * std::cos(sol[0]), ARM_LENGTH_2 * std::cos(sol[1]));
    f = Vector2f(ARM_LENGTH_1 * std::cos(sol[0]) + ARM_LENGTH_2 * std::cos(sol[1]) - target2.x(),
                 ARM_LENGTH_1 * std::sin(sol[0]) + ARM_LENGTH_2 * std::sin(sol[1]) - target2.y());
}
void CatchPoint::setSteerInfo(Vector2f &sol, int Speed)
{
    double pwm = 1020 + (angleBase * 1.0f / PI) * 1000;
    steerBase.setInfo(CATCHER_1, (int)pwm, Speed);
    pwm = 850 + (sol[0] * 1.0f / PI) * 1300;
    steer_1.setInfo(CATCHER_2, (int)pwm, Speed);
    pwm = 850 + (sol[1] * 1.0f / PI) * 1300;
    steer_2.setInfo(CATCHER_3, (int)pwm, Speed);

    pwm = 1500 + (pawState - 0.5) * 900;
    steerPaw.setInfo(CATCHER_4, (int)pwm, Speed);
}
std::string CatchPoint::getSteerInfo()
{
    std::string result = steerBase.getInfo() + steer_1.getInfo() + steer_2.getInfo() + steerPaw.getInfo();
    return result;
}