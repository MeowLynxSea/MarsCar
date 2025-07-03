#ifndef PARAMETER_H
#define PARAMETER_H
#include <string>

#define UART_PORT UART_NUM_2
#define TXD GPIO_NUM_17
#define RXD GPIO_NUM_16

static const int MAX_WHEEL_STEER_SPEED = 100;
static const int MAX_WHEEL_SPEED = 1000;
static const int MAX_STEER_PWM = 700;
static const double CATCHER_SPEED = 0.1;
static const double WIFI_CATCHER_SPEED = 0.15;
static const double PAW_SPEED=0.01;

static const std::string WHEEL_FL="001";
static const std::string WHEEL_FR="002";
static const std::string WHEEL_BL="003";
static const std::string WHEEL_BR="004";

static const std::string STEER_FL="011";
static const std::string STEER_FR="012";
static const std::string STEER_BL="013";
static const std::string STEER_BR="014";

static const std::string CATCHER_1="021";
static const std::string CATCHER_2="022";
static const std::string CATCHER_3="023";
static const std::string CATCHER_4="024";

static const int CATCHER_TIME= 3;
static const int FRAME_TIME= 100;
static const double EPSSILON=1e-4;

static const double H=0;
static const double ARM_LENGTH_1=15;
static const double ARM_LENGTH_2=20;

static const double PI=3.1415926535;

#endif //PARAMETER_H