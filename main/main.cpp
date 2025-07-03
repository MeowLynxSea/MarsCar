#include "ps2_controller.h"
#include "moveInfo.h"
#include "parameter.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "uartTool.h"
#include "catchPoint.h"
#include "wifiServer.h"
#include <iostream>
extern "C" void app_main()
{
    //PS2Controller ps2(GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_15, GPIO_NUM_23);
    //ps2.launch();
    CatchPoint catcher;
    UartTool uartTool(UART_NUM_2, GPIO_NUM_17, GPIO_NUM_16);

    WifiServer::Config config;
    config.wifi_ssid = "ahxbxa";
    config.wifi_pass = "12345678";
    config.uart_port = UART_PORT;
    config.txd_pin = TXD;
    config.rxd_pin = RXD;
    WifiServer server(config);
    server.start();
    
    while (1)
    {
        // ps2.updateStatic();
        WheelMoveInfo wheel(WHEEL_FL, WHEEL_FR, WHEEL_BL, WHEEL_BR,server.getLx(), server.getLy());
        std::string cmd1 = wheel.getWheelInfo();

        SteerMoveInfo steer(STEER_FL, STEER_FR, STEER_BL, STEER_BR, server.getRx(), server.getRy());
        std::string cmd2 = steer.getSteerInfo();

        std::string cmd3 = "";
        catcher.move(server);
            cmd3 = catcher.getSteerInfo();

        uartTool.sendData("Move", cmd1+cmd2+cmd3);
       // std::cout<<" "<<cmd3<<std::endl;
        
        vTaskDelay(10 / portTICK_PERIOD_MS); // 延
    }
}