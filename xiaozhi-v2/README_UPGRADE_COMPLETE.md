# Weiwu X1 WiFi 板子 - 升级到 xiaozhi-esp32 v2 完整指南

## 📋 项目概述

本项目将 Weiwu X1 WiFi 开发板从 xiaozhi-esp32 v1.5.2 升级到 v2.2.6。

### 硬件配置

| 项目 | 配置 |
|------|------|
| 芯片 | ESP32-S3 |
| Flash | 8 MB |
| PSRAM | 64 MB Octal |
| 麦克风 | I2S (GPIO 42/40/2) |
| 扬声器 | I2S + MAX98357 (GPIO 15/16/17/18) |
| BOOT按钮 | GPIO 0 |
| 功能键 | GPIO 5 |
| LED | GPIO 8 + GPIO 48 |

### v1 vs v2 主要区别

| 功能 | v1.5.2 | v2.2.6 |
|------|--------|--------|
| 分区表 | 单应用分区 | 双应用分区 (OTA) |
| 资源存储 | model 分区 (320KB) | assets 分区 (2MB) |
| 资源更新 | 固件更新 | 网络下载 |
| 固件大小限制 | 7.625 MB | 3 MB |

## 📁 项目结构

```
xiaozhi-v2/
├── main/
│   ├── boards/weiwu-x1-wifi/     # 板子配置
│   │   ├── config.h               # GPIO 配置
│   │   ├── config.json            # 构建配置
│   │   └── weiwu-x1-wifi.cc      # 驱动代码
│   ├── CMakeLists.txt             # ✅ 已添加板子支持
│   └── Kconfig.projbuild          # ✅ 已添加板子选项
├── partitions/v2/
│   └── 8m.csv                    # ✅ v2 8MB 分区表
├── sdkconfig.defaults.weiwu-x1-wifi  # ✅ 板子配置
├── .github/workflows/
│   ├── build.yml                 # 主编译流程
│   ├── build-weiwu-x1-wifi.yml   # ✅ 专门编译流程
│   └── README_GITHUB_ACTIONS.md   # ✅ GitHub Actions 使用指南
├── scripts/
│   ├── test_weiwu_config.py      # ✅ 配置测试脚本
│   └── release.py                 # 编译脚本
├── README_WEIWU_X1_WIFI.md       # ✅ 编译刷机说明
└── README.md                      # 项目主文档
```

## 🚀 快速开始

### 方式一：使用 GitHub Actions（推荐）

1. **推送代码到 GitHub**
   ```bash
   cd /workspace/xiaozhi-v2
   git init
   git add .
   git commit -m "feat: 升级到 xiaozhi-esp32 v2.2.6"
   git remote add origin https://github.com/你的用户名/old-xiaozhi-esp32.git
   git push -u origin main
   ```

2. **触发编译**
   - 访问仓库的 **Actions** 页面
   - 选择 **Build Weiwu X1 WiFi Firmware**
   - 点击 **Run workflow**
   - 选择资源类型，点击 **Run workflow**

3. **下载固件**
   - 编译完成后在 Artifacts 下载 `merged-binary.bin`

### 方式二：本地编译

1. **设置 ESP-IDF 环境**
   ```bash
   cd /workspace/xiaozhi-v2
   source $IDF_PATH/export.sh
   ```

2. **配置和编译**
   ```bash
   idf.py set-target esp32s3
   idf.py menuconfig
   # 选择 Board Type: Weiwu X1 WiFi
   
   idf.py build
   idf.py merge-bin
   ```

3. **刷写到设备**
   ```bash
   idf.py -p /dev/ttyUSB0 flash monitor
   ```

## 📖 详细文档

- **[编译刷机说明](README_WEIWU_X1_WIFI.md)** - 详细的编译和刷机步骤
- **[GitHub Actions 使用指南](.github/workflows/README_GITHUB_ACTIONS.md)** - 自动化编译说明
- **[官方文档](https://xiaozhi.dev/)** - 小智AI官方文档

## 🔧 配置文件说明

### 分区表 (partitions/v2/8m.csv)

```
nvs:      16 KB    - WiFi配置、系统设置
otadata:   8 KB    - OTA升级数据
phy_init:  4 KB    - 射频初始化
ota_0:     3 MB    - 应用程序主分区
ota_1:     3 MB    - OTA备用分区
assets:    2 MB    - 资源文件（唤醒词、字体等）
```

### SDK 配置 (sdkconfig.defaults.weiwu-x1-wifi)

主要配置项：
- Flash 大小: 8MB
- Flash 模式: DIO
- PSRAM: 64MB Octal @ 80MHz
- CPU 频率: 240MHz
- 显示屏: 禁用
- 唤醒词: 你好小智

## ⚠️ 重要提示

1. **数据备份**: 升级前建议备份原有配置
2. **分区表变更**: v1 和 v2 分区表不兼容，必须完整刷写
3. **首次启动**: v2 首次启动需要联网下载资源文件
4. **OTA**: v2 支持 OTA 升级，但不支持从 v1 OTA 升级

## 🐛 故障排查

### 编译失败

1. 检查 ESP-IDF 环境是否正确设置
2. 查看 Actions 中的编译日志
3. 确保 GitHub Actions 有足够的权限

### 刷写失败

1. 确保设备进入下载模式（BOOT + RESET）
2. 检查串口驱动是否安装
3. 尝试降低刷写波特率

### 运行时问题

1. 检查 WiFi 连接
2. 确认服务器地址配置正确
3. 查看串口日志输出

## 📞 获取帮助

- **GitHub Issues**: https://github.com/m3325599/old-xiaozhi-esp32/issues
- **官方 Discord**: https://discord.gg/C759fGMBcZ
- **官方文档**: https://xiaozhi.dev/

## 📝 更新日志

### v2.2.6 (当前版本)

- ✅ 基于官方 xiaozhi-esp32 v2.2.6
- ✅ 支持 8MB Flash 设备
- ✅ 双应用分区支持 OTA
- ✅ 新增 assets 分区管理资源
- ✅ GitHub Actions 自动化编译支持

### v1.5.2 (原始版本)

- 位于 `/workspace_backup_v1.5.2`
- 可作为备份恢复

## 📄 许可证

本项目遵循 MIT 许可证，与 xiaozhi-esp32 官方项目一致。
