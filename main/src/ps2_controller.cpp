#include "ps2_controller.h"
#include "freertos/FreeRTOS.h"
#include "esp_timer.h"
#include <string.h>
PS2Controller::PS2Controller(gpio_num_t dat_pin, gpio_num_t cmd_pin,
                             gpio_num_t sel_pin, gpio_num_t clk_pin)
{
    DAT = dat_pin;
    CMD = cmd_pin;
    SEL = sel_pin;
    CLK = clk_pin;

    // 配置GPIO
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << dat_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << cmd_pin) | (1ULL << sel_pin) | (1ULL << clk_pin);
    io_conf.mode = GPIO_MODE_OUTPUT;
    gpio_config(&io_conf);

    // 初始化引脚状态
    gpio_set_level(CMD, 1);
    gpio_set_level(CLK, 1);
    gpio_set_level(SEL, 1);

    // 初始化内部状态
    memset(data, 0, sizeof(data));
    last_buttons = 0;
    buttons = 0;
    last_read = 0;
    read_delay = 1; // 1ms读取一次
    controller_type = 0;
    en_Rumble = false;
    en_Pressures = false;
}
uint8_t PS2Controller::sendAndReceive(uint8_t const &byte) const
{
    uint8_t tmp = 0;
    for (int i = 0; i < 8; i++)
    {
        // 设置命令引脚的状态
        gpio_set_level(CMD, (byte >> i) & 0x01);

        // 拉低时钟信号开始传输
        gpio_set_level(CLK, 0);
        vTaskDelay(SHORT_DELAY_MS / portTICK_PERIOD_MS);

        // 读取数据引脚的状态
        if (gpio_get_level(DAT))
            tmp |= (1 << i);

        // 拉高时钟信号结束当前位的传输
        gpio_set_level(CLK, 1);
        vTaskDelay(SHORT_DELAY_MS / portTICK_PERIOD_MS);
    }

    // 释放命令引脚
    gpio_set_level(CMD, 1);
    vTaskDelay(SHORT_DELAY_MS / portTICK_PERIOD_MS);

    return tmp;
}

bool PS2Controller::readState(bool motor1, uint8_t motor2)
{
    int64_t now = esp_timer_get_time() / 1000; // 转换为毫秒
    int64_t elapsed = now - last_read;

    // 如果超过1.5秒没有读取，重新配置手柄
    if (elapsed > 1500)
    {
        reconfig();
        printf("重新配置完成");
    }
    // 等待下一次读取时间
    if (elapsed < read_delay)
        vTaskDelay((read_delay - elapsed) / portTICK_PERIOD_MS);

    // 准备发送数据
    uint8_t data_to_send[9] = {0x01, 0x42, 0,
                               (uint8_t)(motor1 ? 0x01 : 0x00),
                               (uint8_t)((motor2 * 0xFF) / 255),
                               0, 0, 0, 0};
    uint8_t extra_data[12] = {0};
    bool success = false;

    // 尝试最多5次读取
    for (int attempt = 0; attempt < 5; attempt++)
    {
        gpio_set_level(CMD, 1);
        gpio_set_level(CLK, 1);

        // 选择手柄开始通信
        gpio_set_level(SEL, 0);
        vTaskDelay(SHORT_DELAY_MS / portTICK_PERIOD_MS);

        // 发送和接收9个字节
        for (int i = 0; i < 9; i++)
            data[i] = sendAndReceive(data_to_send[i]);

        // 如果收到0x79，继续读取额外数据
        if (data[1] == 0x79)
            for (int i = 0; i < 12; i++)
                data[i + 9] = sendAndReceive(extra_data[i]);

        // 结束通信
        gpio_set_level(SEL, 1);

        // 检查响应是否有效
        if ((data[1] & 0xF0) == 0x70)
        {
            success = true;
            break;
        }

        // 重新配置手柄
        reconfig();
        vTaskDelay(read_delay / portTICK_PERIOD_MS);
    }
    // 更新状态
    if (!success)
        read_delay = (read_delay < 10) ? read_delay + 1 : 10;

    last_buttons = buttons;
    buttons = ((uint16_t)data[4] << 8) | data[3];
    last_read = esp_timer_get_time() / 1000;
    return success;
}
void PS2Controller::sendCommandString(uint8_t *cmd, int length) const
{
    for (int i = 0; i < length; i++)
        sendAndReceive(cmd[i]);
}
void PS2Controller::reconfig()
{
    config(en_Pressures, en_Rumble);
}
int PS2Controller::config(bool pressures, bool rumble)
{
    int error = 1;
    for (int i = 0; i < 10; i++)
    {
        gpio_set_level(CMD, 1);
        gpio_set_level(CLK, 1);

        // 选择手柄
        gpio_set_level(SEL, 0);
        vTaskDelay(SHORT_DELAY_MS / portTICK_PERIOD_MS);
        // 发送和接收配置命令
        uint8_t temp[9];
        uint8_t cmd[9] = {0x01, 0x45, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
        for (int j = 0; j < 9; j++)
            temp[j] = sendAndReceive(cmd[j]);
        // 结束通信
        gpio_set_level(SEL, 1);
        controller_type = temp[3];

        // TODO: 发送更多配置命令(待测试)
        uint8_t config_cmd2[9] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
        sendCommandString(config_cmd2, 9);

        if (rumble)
        {
            uint8_t rumble_cmd[5] = {0x01, 0x4D, 0x00, 0x00, 0x01};
            sendCommandString(rumble_cmd, 5);
            en_Rumble = true;
        }

        if (pressures)
        {
            uint8_t pressure_cmd[9] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
            sendCommandString(pressure_cmd, 9);
            en_Pressures = true;
        }
        uint8_t config_cmd3[9] = {0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A};
        sendCommandString(config_cmd3, 9);

        readState(false, 0);

        // 检查配置是否成功
        if (pressures && data[1] == 0x79)
        {
            error = 0;
            break;
        }
        if (data[1] == 0x73)
        {
            error = 0;
            break;
        }
    }
    // 检查最终配置状态
    if (data[1] == 0x41 || data[1] == 0x42 ||
        data[1] == 0x73 || data[1] == 0x79)
    {
        error = 0;
    }
    return error;
}

// 按钮状态检测
bool PS2Controller::Button(uint16_t button) const
{
    return !(buttons & button);
}

bool PS2Controller::NewButtonState(uint16_t button) const
{
    return (last_buttons ^ buttons) & button;
}

bool PS2Controller::ButtonPressed(uint16_t button) const
{
    return NewButtonState(button) && Button(button);
}

bool PS2Controller::ButtonReleased(uint16_t button) const
{
    return NewButtonState(button) && !(last_buttons & button);
}
// 手柄主循环
void PS2Controller::ps2Task()
{
    int error = config(true, true);
    if (error) {
        printf("Error configuring controller\n");
        while (1) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    
    printf("Found Controller, configured successful\n");
    
    // 震动测试
    if (en_Rumble) {
        printf("Vibrating controller for 1 second...\n");
        readState(true, 255);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        //read_gamepad(&ps2, false, 0);
    }
    
    printf("Controller is ready. Press START + SELECT together to exit.\n");
    while (1) {
        readState( true, 255);
        // 读取手柄数据
        if (!readState(false, 0)) {
            vTaskDelay(READ_DELAY_MS / portTICK_PERIOD_MS);
            continue;
        }
        
        // 检查是否同时按下START和SELECT键
        if (ButtonPressed(BUTTON_START) && ButtonPressed(BUTTON_SELECT)) {
            printf("START and SELECT pressed together. Exiting...\n");
            break;
        }
        
        // 打印按钮状态
        if (Button(BUTTON_SELECT)) printf("SELECT pressed\n");
        if (Button(BUTTON_START)) printf("START pressed\n");
        if (Button(BUTTON_PAD_UP)) printf("UP pressed\n");
        if (Button(BUTTON_PAD_DOWN)) printf("DOWN pressed\n");
        if (Button(BUTTON_PAD_LEFT)) printf("LEFT pressed\n");
        if (Button(BUTTON_PAD_RIGHT)) printf("RIGHT pressed\n");
        if (Button(BUTTON_TRIANGLE)) printf("TRIANGLE pressed\n");
        if (Button(BUTTON_CIRCLE)) printf("CIRCLE pressed\n");
        if (Button(BUTTON_CROSS)) printf("CROSS pressed\n");
        if (Button(BUTTON_SQUARE)) printf("SQUARE pressed\n");
        if (Button(BUTTON_L1)) printf("L1 pressed\n");
        if (Button(BUTTON_R1)) printf("R1 pressed\n");
        if (Button(BUTTON_L2)) printf("L2 pressed\n");
        if (Button(BUTTON_R2)) printf("R2 pressed\n");
        
        // 模拟摇杆值
        if (en_Pressures) {
            printf("LX: %d, LY: %d, RX: %d, RY: %d\n",
                   data[LX], data[LY], 
                   data[RX], data[RY]);
        }
        
        vTaskDelay(READ_DELAY_MS / portTICK_PERIOD_MS);
    }
    
    vTaskDelete(NULL);
}

void PS2Controller::ps2StaticTask(void *pvParameters)
{
    PS2Controller *instance = static_cast<PS2Controller *>(pvParameters);
    instance->ps2Task();

}
void PS2Controller::launch()
{
    xTaskCreate(
        ps2StaticTask, // 静态任务函数
        "ps2Task",     // 任务名称
        4096,          // 堆栈大小
        this,          // 传递 this 指针
        5,             // 优先级
        NULL);
}
