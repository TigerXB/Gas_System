# Gas_System — 室内有害气体监测与物联网报警系统

基于 STM32F103 的嵌入式物联网项目，实时采集 MQ-2（烟雾）、MQ-135（空气质量）传感器数据与 DHT11 温湿度，通过 OLED 屏幕本地显示，并通过 ESP8266 WiFi 模块上传至**巴法云**，支持微信小程序远程监控与声光报警。

## 功能特性

| 功能 | 说明 |
|------|------|
| **多参数采集** | MQ-2（烟雾）、MQ-135（空气质量/有害气体）、DHT11（温度+湿度） |
| **本地 OLED 显示** | SSD1306 0.96 寸 OLED，四行实时刷新 |
| **声光报警** | 气体浓度超标 → 蜂鸣器间断鸣响 + 继电器触发排气风扇 |
| **WiFi 联网** | ESP8266 连接巴法云，支持微信小程序远程查看 |
| **定时上传** | 每 5 秒推送一次传感器数据到云端 |

## 硬件清单

| 模块 | 型号 | 数量 | 参考购买渠道 |
|------|------|------|------------|
| 主控 | STM32F103C8T6 最小系统板（Blue Pill） | 1 | 淘宝 / 嘉立创 |
| 烟雾传感器 | MQ-2 | 1 | |
| 空气质量传感器 | MQ-135 | 1 | |
| 温湿度传感器 | DHT11 | 1 | |
| OLED 显示屏 | SSD1306 0.96 寸 128x64 I2C | 1 | |
| WiFi 模块 | ESP8266-01S | 1 | |
| 继电器模块 | 5V 单路继电器（低电平触发） | 1 | |
| 有源蜂鸣器 | 3.3V / 5V | 1 | |
| 面包板 + 杜邦线 | — | 若干 | |

> MQ-2 和 MQ-135 出厂后需要**预热 24-48 小时**才能稳定读数，首次上电建议先让设备持续运行一段时间再校准。

## 引脚接线表

| STM32 引脚 | 连接外设 | 功能 | 备注 |
|-----------|---------|------|------|
| **PA0** | MQ-2 AO | ADC1_IN0 | 模拟输入，烟雾浓度 |
| **PA1** | MQ-135 AO | ADC1_IN1 | 模拟输入，空气质量 |
| **PA2** | ESP8266 TX | USART2_TX | 串口发送 |
| **PA3** | ESP8266 RX | USART2_RX | 串口接收 |
| **PA4** | DHT11 DATA | GPIO (单总线) | 需 4.7kΩ 上拉电阻到 3.3V |
| **PB6** | OLED SCL | GPIO (模拟 I2C) | |
| **PB7** | OLED SDA | GPIO (模拟 I2C) | |
| **PB12** | 蜂鸣器 | GPIO | 低电平触发 |
| **PB13** | 继电器 IN | GPIO | 接排气风扇电源回路 |

**供电注意**：

- STM32 通过 USB 5V 供电，板上 LDO 提供 3.3V
- ESP8266 瞬时电流可达 300mA，**严禁从 STM32 板载 3.3V 取电**，需外接独立 3.3V 稳压模块
- MQ 传感器加热丝功耗较大（5V / 约 150mA），建议从面包板电源模块取 5V
- STM32、MQ 传感器、ESP8266、继电器等 **GND 必须共地**

## 软件环境

| 工具 | 版本/说明 |
|------|----------|
| IDE | Keil MDK-ARM 5.x |
| HAL 库 | STM32Cube_FW_F1_V1.8.x |
| 编译器 | ARM Compiler 5 / ARMCC |
| 烧录工具 | ST-Link / J-Link / USB-TTL 串口下载 |

> 项目由 STM32CubeMX 生成框架，用户代码位于 `USER CODE BEGIN` / `USER CODE END` 区块内。如需修改引脚分配，建议在 CubeMX 中打开 `Gas_System.ioc` 重新生成，CubeMX 不会覆盖用户自定义代码区块。

## 快速开始

### 第一步：克隆仓库

```bash
git clone https://github.com/<your-username>/Gas_System.git
```

### 第二步：注册巴法云（Bemfa Cloud）

1. 打开 [巴法云控制台](https://cloud.bemfa.com/)
2. 微信扫码登录
3. 进入 **TCP 创客云** → 创建一个主题，主题名可自定义（例如 `GasMonitor001`）
4. 复制你的 **用户私钥 UID**（32 位十六进制字符串）

### 第三步：配置 WiFi 和云凭证

打开 `Core/Inc/esp8266.h`，修改以下四个宏：

```c
#define WIFI_SSID   "你的WiFi名称"
#define WIFI_PWD    "你的WiFi密码"

#define BEMFA_UID   "你的巴法云UID"
#define BEMFA_TOPIC "你在巴法云创建的主题名"
```

> ⚠️ **安全提示**：WiFi 密码和巴法云 UID 属于敏感信息，请勿将真实值提交到公开仓库。

### 第四步：编译烧录

1. 用 Keil MDK 打开 `MDK-ARM/Gas_System.uvprojx`
2. 确认编译目标为 `Gas_System`
3. `Project` → `Build Target` (F7)
4. 通过 ST-Link 烧录

### 第五步：微信小程序绑定

1. 微信搜索小程序 **"巴法云"**
2. 登录后绑定你的 UID
3. 在设备列表中找到你的主题，即可查看实时数据

## 代码结构

```
Gas_System/
├── Core/
│   ├── Inc/
│   │   ├── main.h              # 主头文件
│   │   ├── dht11.h             # DHT11 驱动头文件
│   │   ├── esp8266.h           # ESP8266 + 巴法云配置（需修改凭证）
│   │   ├── oled.h              # SSD1306 OLED 驱动头文件
│   │   ├── oledfont.h          # OLED 字库数据
│   │   ├── stm32f1xx_hal_conf.h
│   │   └── stm32f1xx_it.h
│   └── Src/
│       ├── main.c              # 主程序（初始化、主循环、ADC、报警逻辑）
│       ├── dht11.c             # DHT11 温湿度传感器驱动
│       ├── esp8266.c           # ESP8266 AT 指令驱动 + 巴法云上传
│       ├── oled.c              # SSD1306 OLED 驱动（模拟 I2C）
│       ├── stm32f1xx_hal_msp.c # HAL 外设初始化
│       ├── stm32f1xx_it.c      # 中断服务函数
│       └── system_stm32f1xx.c  # 系统时钟配置
├── Drivers/
│   ├── CMSIS/                  # ARM CMSIS 内核文件
│   └── STM32F1xx_HAL_Driver/   # STM32 HAL 驱动库
├── MDK-ARM/                    # Keil 工程文件
├── Gas_System.ioc              # CubeMX 项目配置
└── README.md
```

## 常见问题

**Q: OLED 白屏不显示？**
A: 检查 PB6/PB7 连接，确认 OLED 模块供电 3.3V，I2C 地址通常是 `0x3C`。部分模块背面有地址选择电阻，短接后变为 `0x3D`。

**Q: ESP8266 连不上 WiFi？**
A: 路由器必须为 **2.4GHz**（ESP8266 不支持 5GHz）。`esp8266.c` 中 WiFi 连接等待了 8 秒超时，如果路由器响应慢可适当增大 `HAL_Delay()`。

**Q: DHT11 读数始终为 0？**
A: 检查 PA4 是否有 4.7kΩ 上拉电阻。DHT11 初始化后会有一小段稳定时间，尝试增加启动延迟。

**Q: 继电器频繁切换咔哒响？**
A: 代码中报警状态下继电器保持常开（`WritePin SET`），不会跟随蜂鸣器周期翻转。

## 许可证

MIT License — STM32 HAL 库部分遵循 STMicroelectronics 原始许可。
