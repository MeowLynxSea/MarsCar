#include "uartTool.h"
#include "esp_log.h"
int UartTool::sendData(const std::string logName, const std::string data)
{
    const int txBytes = uart_write_bytes(UART_NUM, data.c_str(), data.length());
    ESP_LOGI(logName.c_str(), "Wrote %d bytes", txBytes); // log打印
    return txBytes;
}