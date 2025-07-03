#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <string>
#include <vector>
#include "parameter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "driver/gpio.h"
#include <driver/uart.h>

// 常量定义
#define READ_DELAY_MS 10
#define SHORT_DELAY_MS 1
// 按钮常量定义
#define BUTTON_SELECT 0x0001
#define BUTTON_L3 0x0002
#define BUTTON_R3 0x0004
#define BUTTON_START 0x0008
#define BUTTON_PAD_UP 0x0010
#define BUTTON_PAD_RIGHT 0x0020
#define BUTTON_PAD_DOWN 0x0040
#define BUTTON_PAD_LEFT 0x0080
#define BUTTON_L2 0x0100
#define BUTTON_R2 0x0200
#define BUTTON_L1 0x0400
#define BUTTON_R1 0x0800
#define BUTTON_TRIANGLE 0x1000
#define BUTTON_CIRCLE 0x2000
#define BUTTON_CROSS 0x4000
#define BUTTON_SQUARE 0x8000

#define W_LX 0
#define W_LY 1
#define W_RX 2
#define W_RY 3
#define W_UP 0
#define W_DOWN 1
#define W_FORWARD 2
#define W_BACKWARD 3
#define W_RIGHT 4
#define W_LEFT 5
class WifiServer
{
public:
    // 初始化配置
    struct Config
    {
        const char *wifi_ssid;
        const char *wifi_pass;
        uart_port_t uart_port;
        gpio_num_t txd_pin;
        gpio_num_t rxd_pin;
        int web_port = 80;
        int max_clients = 5;
    };

    WifiServer(const Config &config);

    int get_reset(){return reset;}
    int get_ball(){return getBall;}
    double* get_catcherData(){return catcherData;}
    double* get_carData(){return carData;}

    double getUP(){return catcherData[W_UP];}
    double getDOWN(){return catcherData[W_DOWN];}
    double getLEFT(){return catcherData[W_LEFT];}
    double getRIGHT(){return catcherData[W_RIGHT];}
    double getFORWARD(){return catcherData[W_FORWARD];}
    double getBACKWARD(){return catcherData[W_BACKWARD];}

    double getLx(){return carData[W_LX];};
    double getLy(){return carData[W_LY];};
    double getRx(){return carData[W_RX];};
    double getRy(){return carData[W_RY];};
    double getPaw(){return pawData;}
    // 启动服务
    void start();
    void normlized();
    // 停止服务
    void stop();
    void print();
private:
    double carData[4];
    double catcherData[6];
    int reset;
    int getBall;
    double pawData;
    // 解析 GET 参数
    std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
    parse_get_params(const std::string &request);

    // WiFi 初始化
    void wifi_init_sta();

    // HTTP 服务器任务
    static void udp_server_task(void *arg);

    // 任务句柄
    TaskHandle_t server_task_handle = nullptr;

    // 配置
    Config config;

    // HTML 响应内容
    const char *html_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "    <title>MarsCar Web Server</title>"
        "</head>"
        "<body>"
        "    <h1>Hello from ESP32!</h1>"
        "    <p>这是一个运行在 ESP32 上的基本 Web 服务器。</p>"
        "</body>"
        "</html>";
};

#endif // WIFI_SERVER_H