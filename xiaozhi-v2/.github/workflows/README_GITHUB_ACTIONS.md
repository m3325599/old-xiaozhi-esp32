# GitHub Actions 自动编译指南

本项目已配置 GitHub Actions 工作流，可以自动编译 Weiwu X1 WiFi 固件。

## 编译方式

### 方式一：手动触发编译（推荐）

1. 访问项目仓库: https://github.com/m3325599/old-xiaozhi-esp32
2. 点击 **Actions** 标签
3. 选择 **Build Weiwu X1 WiFi Firmware** 工作流
4. 点击 **Run workflow**
5. 选择 Flash Assets 类型：
   - `FLASH_DEFAULT_ASSETS`: 包含默认资源（唤醒词、字体等）
   - `FLASH_NONE_ASSETS`: 不包含资源，首次启动需要联网下载
   - `FLASH_EXPRESSION_ASSETS`: 表情动画资源
6. 点击 **Run workflow** 开始编译

### 方式二：自动编译

- **push 到 main 分支**: 自动编译所有板子的固件
- **提交 PR**: 只编译受影响的板子

## 下载编译产物

编译完成后，在 **Actions** 页面找到对应的运行记录：

1. 点击运行记录
2. 在 Artifacts 部分下载 `weiwu-x1-wifi-*` 文件
3. 下载的是 `merged-binary.bin`，可以直接刷写到设备

## 刷写固件

### 使用 esptool

```bash
# 擦除 flash（可选）
esptool.py --chip esp32s3 --port /dev/ttyUSB0 erase_flash

# 刷写固件
esptool.py --chip esp32s3 --port /dev/ttyUSB0 --baud 921600 \
  write_flash 0x0 weiwu-x1-wifi-*.bin
```

### 使用 ESP Flash Download Tool

1. 下载 ESP Flash Download Tool
2. 选择芯片: ESP32-S3
3. 选择固件文件
4. 晶振频率: 40MHz
5. Flash 模式: DIO
6. Flash 大小: 8MB
7. COM 端口: 你的设备端口
8. 波特率: 921600

## 查看编译日志

如果编译失败，可以查看 build logs：

1. 在 Actions 运行记录中
2. 找到对应的编译任务
3. 点击进入
4. 下载 `build-logs-weiwu-x1-wifi-*` artifact

## GitHub Actions 配置说明

配置文件位于: `.github/workflows/build-weiwu-x1-wifi.yml`

主要配置项：
- **容器镜像**: `espressif/idf:v5.5.2`
- **目标芯片**: ESP32-S3
- **Flash 大小**: 8MB
- **PSRAM**: 64MB Octal
- **分区表**: v2 8MB (partitions/v2/8m.csv)

## 常见问题

### Q: 编译失败怎么办？

1. 查看 Actions 中的编译日志
2. 检查是否有依赖下载失败
3. 尝试重新触发编译

### Q: 如何下载特定版本的固件？

GitHub Actions 会保留每次编译的产物。访问对应的 Actions 运行记录即可下载。

### Q: 编译产物在哪里？

编译产物是 `merged-binary.bin`，包含所有分区数据，可以直接刷写到设备。

## 自动编译触发条件

| 事件 | 行为 |
|------|------|
| push 到 main | 编译所有板子 |
| PR 到 main | 只编译受影响的板子 |
| 手动触发 | 编译 Weiwu X1 WiFi |

## 本地编译（可选）

如果需要在本地编译：

```bash
# 1. 设置环境
cd /workspace/xiaozhi-v2
source $IDF_PATH/export.sh

# 2. 配置项目
idf.py set-target esp32s3
idf.py menuconfig
# 选择 Board Type: Weiwu X1 WiFi

# 3. 编译
idf.py build

# 4. 合并固件
idf.py merge-bin
```

## 技术支持

- 项目 Issues: https://github.com/m3325599/old-xiaozhi-esp32/issues
- 官方文档: https://xiaozhi.dev/
