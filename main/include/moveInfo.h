#ifndef MOVEINFO_H
#define MOVEINFO_H
#include <string>
class MoveInfo
{
public:
    MoveInfo() = default;
    MoveInfo(std::string i, int p, int t) : index(i), pwm(t), t(t) {};
    ~MoveInfo() = default;

    std::string getInfo();
    void setInfo(std::string i,int p,int t);
private:
    std::string index;
    int pwm;
    int t;
};
class WheelMoveInfo : public MoveInfo
{
public:
    WheelMoveInfo() = default;
    WheelMoveInfo(std::string fl, std::string fr, std::string bl, std::string br, int lx, int ly);
    WheelMoveInfo(std::string fl, int pwm_fl, int t_fl, std::string fr, int pwm_fr, int t_fr,
                  std::string bl, int pwm_bl, int t_bl, std::string br, int pwm_br, int t_br)
        : front_left(fl, pwm_fl, t_fl), front_right(fr, pwm_fr, t_fr),
          back_left(bl, pwm_bl, t_bl), back_right(fl, pwm_br, t_br) {};
    ~WheelMoveInfo() = default;

    std::string getWheelInfo();
private:
    MoveInfo front_left;
    MoveInfo front_right;
    MoveInfo back_left;
    MoveInfo back_right;
};

class SteerMoveInfo : public MoveInfo
{
public:
    SteerMoveInfo() = default;
    SteerMoveInfo(std::string fl, std::string fr, std::string bl, std::string br, int rx, int ry);
    SteerMoveInfo(std::string fl, int pwm_fl, int t_fl, std::string fr, int pwm_fr, int t_fr,
                  std::string bl, int pwm_bl, int t_bl, std::string br, int pwm_br, int t_br)
        : front_left(fl, pwm_fl, t_fl), front_right(fr, pwm_fr, t_fr),
          back_left(bl, pwm_bl, t_bl), back_right(fl, pwm_br, t_br){};
    ~SteerMoveInfo() = default;

    std::string getSteerInfo();

private:
    MoveInfo front_left;
    MoveInfo front_right;
    MoveInfo back_left;
    MoveInfo back_right;
};

#endif // MOVEINFO_H