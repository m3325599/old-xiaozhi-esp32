# Weiwu X1 WiFi 板子 - xiaozhi-esp32 v2 升级说明

## 项目概述

本项目基于 xiaozhi-esp32 v2.2.6，为 Weiwu X1 WiFi 开发板（8MB Flash + 64MB Octal PSRAM）专门配置。

## 硬件配置

- **芯片**: ESP32-S3
- **Flash**: 8 MB
- **PSRAM**: 64 MB Octal
- **音频**: I2S 麦克风 + MAX98357 功放
- **按钮**: BOOT (GPIO 0) + 功能键 (GPIO 5)
- **LED**: GPIO 8 (简单LED) + GPIO 48 (内置LED)

## 分区表 (v2 8MB)

```
nvs:      16 KB    (WiFi配置等)
otadata:   8 KB    (OTA数据)
phy_init:  4 KB    (PHY初始化)
ota_0:     3 MB    (应用程序)
ota_1:     3 MB    (OTA备用)
assets:    2 MB    (资源文件 - 唤醒词、主题等)
```

## 与 v1 的主要区别

1. **新增 assets 分区**: 资源可从网络下载
2. **双应用分区**: 支持 OTA 升级
3. **固件大小限制**: 应用程序限制在 3MB 以内
4. **资源管理**: 唤醒词、主题等可动态更新

## 编译步骤

### 1. 设置 ESP-IDF 环境

```bash
cd /workspace/xiaozhi-v2
source $IDF_PATH/export.sh
```

### 2. 配置项目

```bash
idf.py set-target esp32s3
idf.py menuconfig
```

在 menuconfig 中选择:
- **Board Type**: Weiwu X1 WiFi
- **Flash Assets**: 根据需要选择

### 3. 编译固件

```bash
idf.py build
```

### 4. 刷写到设备

确保设备处于下载模式（按住 BOOT 按钮，按下 RESET），然后：

```bash
idf.py flash monitor
```

或者分步操作：

```bash
# 擦除 flash
esptool.py --chip esp32s3 --port /dev/ttyUSB0 erase_flash

# 烧录固件
idf.py -p /dev/ttyUSB0 -b 921600 flash
```

## 首次启动说明

1. 固件首次启动会自动下载资源文件到 assets 分区
2. 设备会创建 WiFi 热点 "Xiaozhi-Setup"
3. 连接热点后，浏览器访问 192.168.4.1 配置 WiFi
4. 配置完成后，设备会连接服务器

## 常见问题

### 1. 编译错误：找不到头文件

确保正确设置了 ESP-IDF 环境：
```bash
source $IDF_PATH/export.sh
```

### 2. 刷写失败

检查设备是否进入下载模式：
- 按住 BOOT 按钮
- 按一下 RESET 按钮
- 松开 BOOT 按钮

### 3. 资源下载失败

设备需要能够访问互联网才能下载资源。如果网络不可用，可以：
- 使用预编译的资源文件手动刷写
- 参考官方文档配置离线资源

## 备份说明

- 原始 v1.5.2 项目已备份到 `/workspace_backup_v1.5.2`
- 如需回退，可以恢复该备份

## 参考资料

- [xiaozhi-esp32 官方仓库](https://github.com/78/xiaozhi-esp32)
- [ESP-IDF 文档](https://docs.espressif.com/projects/esp-idf/)
- [小智AI文档中心](https://xiaozhi.dev/)
