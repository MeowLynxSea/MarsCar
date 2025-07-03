#include "wifiServer.h"
#include <algorithm>
#include <cstdlib> // 包含 atoi 和 strtok_r 函数
#include <cstring> // 包含 strtok_r 函数
#include <string>
static const char *TAG = "WIFI_UDP_SERVER";

WifiServer::WifiServer(const Config &config) : config(config)
{
    // 初始化数据
    for (int i = 0; i < 4; i++)
        carData[i] = 127.5;
    for (int i = 0; i < 6; i++)
        catcherData[i] = 0;
    reset = 0;
    pawData = 0;
    getBall=0;
    // 初始化 NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void WifiServer::start()
{
    // 初始化 WiFi
    wifi_init_sta();

    // 启动 UDP 服务器任务
    xTaskCreate(udp_server_task, "udp_server", 8192, this, 5, &server_task_handle);
}

void WifiServer::stop()
{
    if (server_task_handle)
    {
        vTaskDelete(server_task_handle);
        server_task_handle = nullptr;
    }
}

void WifiServer::wifi_init_sta()
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {};
    strcpy((char *)wifi_config.sta.ssid, config.wifi_ssid);
    strcpy((char *)wifi_config.sta.password, config.wifi_pass);
    // wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK; // 此处配置项已不推荐使用

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Disabling WiFi power save mode for low latency.");
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "正在连接到 WiFi...");

    // 等待连接
    int retry = 0;
    while (true)
    {
        // 使用 event loop 来等待连接成功事件，而不是轮询
        // 这里为了简单，暂时保留轮询，但在生产环境中建议使用事件处理
        wifi_ap_record_t ap;
        if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK)
        {
            ESP_LOGI(TAG, "WiFi 连接成功!");
            esp_netif_ip_info_t ip_info;
            esp_netif_get_ip_info(sta_netif, &ip_info);
            ESP_LOGI(TAG, "设备 IP 地址: " IPSTR, IP2STR(&ip_info.ip));
            break;
        }
        if (retry++ > 20)
        {
            ESP_LOGE(TAG, "连接 WiFi 失败!");
            esp_restart();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, ".");
    }
}

void WifiServer::udp_server_task(void *arg)
{
    WifiServer *server = static_cast<WifiServer *>(arg);
    const Config &config = server->config;

    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t socklen = sizeof(client_addr);
    char rx_buffer[1024];

    // 创建 UDP socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        ESP_LOGE(TAG, "创建 socket 失败");
        vTaskDelete(NULL);
    }

    // 绑定地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(config.web_port); // 使用相同的端口号

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        ESP_LOGE(TAG, "绑定端口失败");
        close(server_fd);
        vTaskDelete(NULL);
    }

    ESP_LOGI(TAG, "UDP 服务器正在监听端口 %d...", config.web_port);

    while (true)
    {
        // 接收 UDP 数据
        int len = recvfrom(server_fd, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&client_addr, &socklen);

        if (len > 0)
        {
            rx_buffer[len] = '\0';

            char client_ip[16];
            inet_ntoa_r(client_addr.sin_addr, client_ip, sizeof(client_ip));

            for (int i = 0; i < 4; i++)
                server->carData[i] = 127.5;
            for (int i = 0; i < 6; i++)
                server->catcherData[i] = 0;
            server->reset = 0;
            server->pawData = 0;
            server->getBall=0;

            std::string testTimeValue;
            char *saveptr;
            char *pair = strtok_r(rx_buffer, "&", &saveptr);
            while (pair != NULL)
            {
                char *eq_pos = strchr(pair, '=');
                if (eq_pos != NULL)
                {
                    *eq_pos = '\0';
                    const char *key = pair;
                    const char *value = eq_pos + 1;

                    if (strcmp(key, "TESTTIME") == 0)
                    {
                        testTimeValue = value;
                    }
                    else if (strcmp(key, "LX") == 0)
                        server->carData[W_LX] = std::atof(value);
                    else if (strcmp(key, "LY") == 0)
                        server->carData[W_LY] = std::atof(value);
                    else if (strcmp(key, "RX") == 0)
                        server->carData[W_RX] = std::atof(value);
                    else if (strcmp(key, "RY") == 0)
                        server->carData[W_RY] = std::atof(value);
                    else if (strcmp(key, "UP") == 0)
                        server->catcherData[W_UP] = std::atof(value);
                    else if (strcmp(key, "DOWN") == 0)
                        server->catcherData[W_DOWN] = std::atof(value);
                    else if (strcmp(key, "FORWARD") == 0)
                        server->catcherData[W_FORWARD] = std::atof(value);
                    else if (strcmp(key, "BACKWARD") == 0)
                        server->catcherData[W_BACKWARD] = std::atof(value);
                    else if (strcmp(key, "RIGHT") == 0)
                        server->catcherData[W_RIGHT] = std::atof(value);
                    else if (strcmp(key, "LEFT") == 0)
                        server->catcherData[W_LEFT] = std::atof(value);
                    else if (strcmp(key, "RESET") == 0)
                        server->reset = std::atof(value);
                    else if (strcmp(key, "PAW") == 0)
                        server->pawData = std::atof(value);
                    else if (strcmp(key, "GETBALL") == 0)
                        server->getBall = std::atoi(value);
                }
                pair = strtok_r(NULL, "&", &saveptr);
            }
            //server->print();
            if (!testTimeValue.empty())
            {
                std::string responseStr = "TESTTIME=" + testTimeValue;
                sendto(server_fd, responseStr.c_str(), responseStr.length(), 0, (struct sockaddr *)&client_addr, socklen);
            }
        }
    }

    close(server_fd);
    vTaskDelete(NULL);
}
void WifiServer::print()
{
    printf("LX=%.2f, LY=%.2f, RX=%.2f, RY=%.2f, FORWARD=%.2f, BACKWARD=%.2f, LEFT=%.2f, RIGHT=%.2f, UP=%.2f, DOWN=%.2f\n, RESET=%d\n", carData[W_LX], carData[W_LY], carData[W_RX], carData[W_RY], catcherData[W_FORWARD], catcherData[W_BACKWARD], catcherData[W_LEFT], catcherData[W_RIGHT], catcherData[W_UP], catcherData[W_DOWN], reset);
}
void WifiServer::normlized()
{

    for (int i = 0; i < 4; i++)
        carData[i] = 127.5;
    for (int i = 0; i < 6; i++)
        catcherData[i] = 0;
    reset = 0;
}