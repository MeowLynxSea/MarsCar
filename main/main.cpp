#include "ps2_controller.h"
#include "moveInfo.h"
#include "parameter.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "uartTool.h"
#include "catchPoint.h"
#include <iostream>
extern "C" void app_main()
{
    PS2Controller ps2(GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_15, GPIO_NUM_23);
    ps2.launch();
    CatchPoint catcher;
    UartTool uartTool(UART_NUM_2, GPIO_NUM_17, GPIO_NUM_16);
    while (1)
    {
        //ps2.updateStatic();
        WheelMoveInfo wheel(WHEEL_FL, WHEEL_FR, WHEEL_BL, WHEEL_BR, ps2.getLx(), ps2.getLy());
        std::string cmd1 = wheel.getWheelInfo();

        SteerMoveInfo steer(STEER_FL, STEER_FR, STEER_BL, STEER_BR, ps2.getRx(), ps2.getRy());
        std::string cmd2 = steer.getSteerInfo();

        catcher.move(ps2);
        std::string cmd3="";
        if (catcher.solution(1e-2))
        {
            std::string cmd3 = catcher.getSteerInfo();
            std::cout<<cmd3<<std::endl;
       }
        //printf("update!\n");
        uartTool.sendData("Move",cmd1+cmd2+cmd3);
        // uartTool.sendData("SteerMove","#013P1500T0100!");
        
        vTaskDelay(10 / portTICK_PERIOD_MS); // 延
    }
}