# MarsCar - 火星车项目

MarsCar 是一个基于 ESP32 的火星车项目，带有一个机械臂，可以通过 Flutter 移动应用或 PS2 手柄进行远程控制。

## ✨ 项目亮点

  * **双模控制**: 可通过专属的 Flutter App 或 PS2 游戏手柄进行控制，满足不同场景下的操作需求。
  * **高级机械臂控制**: 采用了逆运动学算法，通过牛顿迭代法精确控制机械臂的末端执行器，实现复杂抓取任务。
  * **实时视频图传**: Flutter 应用集成了低延迟的实时视频流，让您身临其境，精准操控。
  * **功能丰富的用户界面**:
      * **多功能遥感**: 提供了多种虚拟摇杆，用于控制车辆移动和机械臂操作。
      * **数据可视化**: 在 App 中实时显示图传和控制的延迟数据，帮助您监控连接状态。
      * **精细化操作**: 支持多种机械臂操控速度模式（精确、缓慢、默认、狂暴），以适应不同任务需求。
  * **跨平台控制器**: 基于 Flutter 开发的 App，可轻松在 Android 和 iOS 设备上运行。
  * **模块化代码**: C++ 源码结构清晰，将不同功能模块化（PS2 控制器、WiFi 服务、机械臂、向量运算等），易于维护和扩展。

## 🛠️ 技术栈

### **硬件**

  * **主控**: ESP32
  * **电机驱动**: Servo

### **固件 (Embedded)**

  * **框架**: ESP-IDF, FreeRTOS
  * **语言**: C++
  * **通信**:
      * WiFi (UDP)
      * UART
  * **核心算法**:
      * 逆运动学 (牛顿迭代法)
      * 2D/3D 向量和矩阵运算

### **移动端 (Mobile App)**

  * **框架**: Flutter
  * **语言**: Dart
  * **视频播放**: `media_kit` (RTSP 协议)
  * **状态管理**: `provider`
  * **图表**: `fl_chart`
## 🚀 如何开始

### **硬件要求**

  * ESP32 开发板
  * 直流电机及驱动模块
  * 舵机 (用于机械臂和转向)
  * 摄像头模块 (支持 RTSP 视频流)
  * PS2 游戏手柄 (可选)

### **软件环境**

1.  **ESP-IDF**: 参照 [官方文档](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) 进行安装和配置。
2.  **Flutter**: 参照 [官方文档](https://flutter.dev/docs/get-started/install) 进行安装。

3.  **App**: [下载](https://cloud.meowdream.cn/)
### **编译与烧录**

1.  克隆本仓库。
2.  使用 VSCode 和 ESP-IDF 插件打开项目，编译并烧录固件到 ESP32。
3.  在 `main/main.cpp` 中配置您的 WiFi SSID 和密码。
4.  在 Flutter 项目中，根据需要修改 `home_page.dart` 中的默认 IP 地址，然后运行 App。

## 协议

本项目基于 Apache-2.0 协议。详情请见 [LICENSE](https://www.google.com/search?q=rain-kotsuzui/marscar/MarsCar-979f33becefb45504f5706dfb15daa39fc480306/managed_components/espressif__servo/license.txt) 文件。

## 友链

[flutter项目地址](https://git.meowdream.cn/MeowLynxSea/MarsCarController)
