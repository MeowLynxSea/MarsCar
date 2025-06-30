#ifndef PS2_CONTROLLER_H
#define PS2_CONTROLLER_H

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

#define LX 7
#define LY 8
#define RX 5
#define RY 6

#include <stdio.h>
#include "driver/gpio.h"
class PS2Controller
{
public:
    PS2Controller() = delete;
    PS2Controller(gpio_num_t dat_pin, gpio_num_t cmd_pin, gpio_num_t sel_pin, gpio_num_t clk_pin);
    ~PS2Controller() = default;

    int getLx() { return data[LX]; }
    int getLy() { return data[LY]; }
    int getRx() { return data[RX]; }
    int getRy() { return data[RY]; }
    uint8_t *getData() { return data; }

    bool ButtonPressed(uint16_t button) const;
    bool Button(uint16_t button) const;
    bool updateStatic();
    void launch();

private:
    uint8_t sendAndReceive(uint8_t const &byte) const;
    bool readState(bool motor1, uint8_t motor2);
    int config(bool pressures, bool rumble);
    void reconfig();
    void sendCommandString(uint8_t *cmd, int length) const;
    bool NewButtonState(uint16_t button) const;
    bool ButtonReleased(uint16_t button) const;

    void ps2Task();
    static void ps2StaticTask(void *pvParameters);

    gpio_num_t DAT;
    gpio_num_t CMD;
    gpio_num_t SEL;
    gpio_num_t CLK;

    uint8_t data[21];
    uint16_t last_buttons;
    uint16_t buttons;
    int64_t last_read;
    uint8_t read_delay;
    uint8_t controller_type;
    bool en_Rumble;
    bool en_Pressures;
};

#endif // PS2_CONTROLLER_H
