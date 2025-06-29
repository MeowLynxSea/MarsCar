#include "ps2_controller.h"
#include "moveInfo.h"
#include "parameter.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "uartTool.h"
extern "C" void app_main()
{
    PS2Controller ps2(GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_15,GPIO_NUM_23);
    UartTool uartTool(UART_NUM_2,GPIO_NUM_17,GPIO_NUM_16);
    ps2.launch();
    
    while(1)
    {
        WheelMoveInfo wheel(WHEEL_FL,WHEEL_FR,WHEEL_BL,WHEEL_BR,ps2.getLx(),ps2.getLy());
        std::string cmd= wheel.getWheelInfo();
        uartTool.sendData("WheelMove",cmd);       
        vTaskDelay(1 / portTICK_PERIOD_MS);                  //延
    }
}