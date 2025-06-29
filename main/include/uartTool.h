#ifndef UART_H
#define UART_H

#include "driver/gpio.h"
#include <string>
#include <driver/uart.h>
#include <stdio.h>

static const int RX_BUF_SIZE = 1024;
class UartTool
{
public:
    UartTool(uart_port_t UART_NUM, gpio_num_t TXD_PIN, gpio_num_t RXD_PIN,
             int b_rate = 115200,
             uart_word_length_t d_bits = UART_DATA_8_BITS,
             uart_parity_t p = UART_PARITY_DISABLE,
             uart_stop_bits_t s_bits = UART_STOP_BITS_1,
             uart_hw_flowcontrol_t f_ctrl = UART_HW_FLOWCTRL_DISABLE,
             uint8_t rx = 0,
             uart_sclk_t s_clk = UART_SCLK_APB)
        : UART_NUM(UART_NUM),TXD_PIN(TXD_PIN), RXD_PIN(RXD_PIN)
    {
        config = {
            .baud_rate = b_rate,
            .data_bits = d_bits,
            .parity = p,
            .stop_bits = s_bits,
            .flow_ctrl = f_ctrl,
            .rx_flow_ctrl_thresh = rx,
            .source_clk = s_clk,
        };
        // 安装串口驱动 串口编号、接收buff、发送buff、事件队列、分配中断的标志
        uart_driver_install(UART_NUM, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
        // 串口参数配置 串口号、串口配置参数
        uart_param_config(UART_NUM, &config);
        // 设置串口引脚号 串口编号、tx引脚、rx引脚、rts引脚、cts引脚
        uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        printf("串口通信已创建！\n");
    }
    int sendData(const std::string logName, const std::string data);
    
private:
    uart_port_t UART_NUM;
    gpio_num_t TXD_PIN;
    gpio_num_t RXD_PIN;
    uart_config_t config;
};
#endif // UART_H