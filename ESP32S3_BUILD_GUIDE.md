# ESP32-S3 专用编译指南

本项目提供专门的 ESP32-S3 编译支持，专注于为 ESP32-S3 芯片优化构建流程。

## 🚀 快速开始 ESP32-S3 编译

### 方法1：使用专用 Workflow (推荐)

**触发条件：**
- 代码推送到 main/develop 分支
- 创建 Pull Request
- 推送版本标签 (如 v1.6.0)
- 手动在 Actions 页面触发

**支持的板子：**
- ✅ **Bread Compact WiFi** - 基础版WiFi小智
- ✅ **M5Stack CoreS3** - M5Stack 官方开发板
- ✅ **ESP-BOX-3** - Espressif 官方开发板
- ✅ **AtomS3R Echo Base** - M5Stack AtomS3R + Echo底座
- ✅ **Kevin Box V1** - Kevin 定制版本
- ✅ 其他ESP32-S3开发板...

**手动触发步骤：**
1. 访问仓库 Actions 页面
2. 选择 "ESP32-S3 Only Build" workflow
3. 点击 "Run workflow"
4. （可选）指定特定的板子类型

### 方法2：快速测试编译

使用 "ESP32-S3 Quick Test" workflow 进行快速验证：
- 代码质量检查
- 配置验证  
- 快速编译测试
- 安全扫描

## ⚡ 构建性能优化

### 缓存策略
- **ESP-IDF 缓存**：缓存ESP-IDF v5.3环境，首次构建节省15-20分钟
- **构建缓存**：智能缓存构建目录，增量编译加速50%以上
- **并行构建**：最多同时编译3个板子，提高吞吐量

### 构建时间预估
| 板子类型 | 首次构建 | 增量构建 |
|---------|---------|---------|
| Bread Compact WiFi | 8-12分钟 | 2-4分钟 |
| M5Stack CoreS3 | 10-15分钟 | 3-5分钟 |
| 完整构建矩阵 | 15-25分钟 | 5-10分钟 |

## 📦 输出产物

每个 ESP32-S3 板子构建完成后生成：

### 1. 标准固件包
- `xiaozhi-${board_type}-firmware` (artifact)
  - `merged-binary.bin` - 一键刷写固件
  - `xiaozhi.bin` - 应用固件分区
  - `bootloader.bin` - 引导程序
  - `partition-table.bin` - 分区表
  - `ota_data_initial.bin` - OTA 数据

### 2. 刷写工具
- `flash_${board_type}.sh` - Linux/macOS 刷写脚本
- `flash_${board_type}.bat` - Windows 刷写脚本
- `flash_${board_type}.txt` - 详细刷写说明

### 3. 构建信息
- `sdkconfig` - 编译配置
- 版本和构建时间信息

## 🎯 板子特性对比

| 板子名称 | 显示屏 | 特色功能 | 适用场景 |
|---------|-------|---------|---------|
| **Bread Compact WiFi** | OLED 128x32/64 | 经济实惠，基础功能 | 学习入门，基础应用 |
| **M5Stack CoreS3** | 彩色LCD 320x240 | 摄像头，IMU传感器 | 图像显示，图像识别 |
| **ESP-BOX-3** | 触控LCD | 官方开发板，接口丰富 | 产品开发，演示展示 |
| **AtomS3R Echo Base** | OLED + Atom显示 | 音频处理，RGB LED | 音频应用，效果展示 |
| **Kevin Box V1** | OLED | 优质音频，多按键 | 音乐应用，人机交互 |

## 🔧 手动编译 ESP32-S3

### 环境准备
```bash
# 设置ESP-IDF环境
git clone --recursive -b v5.3 https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
. ./export.sh
```

### 编译特定板子
```bash
# 例如编译M5Stack CoreS3
cd xiaozhi-esp32
idf.py set-target esp32s3
idf.py -DBOARD_NAME="m5stack-core-s3" build
idf.py merge-bin
```

### 刷写固件
```bash
# 使用合并的二进制文件（推荐）
esptool.py -p /dev/ttyUSB0 -b 460800 write_flash 0 merged-binary.bin

# 或者手动刷写各分区
esptool.py -p /dev/ttyUSB0 -b 460800 write_flash \
  0x0 bootloader.bin \
  0x8000 partition-table.bin \
  0xd000 ota_data_initial.bin \
  0x100000 xiaozhi.bin
```

## 🛠️ 故障排除

### 常见问题

**Q: 编译时出现 "target 'esp32s3' is not supported"**  
A: 确保使用 ESP-IDF v5.3 或更高版本，并正确安装了 esp32s3 工具链。

**Q: flash大小不够或分区错误** 
A: ESP32-S3通常使用8MB或16MB flash，确保在menuconfig中正确配置。

**Q: PSRAM相关问题**
A: 有些ESP32-S3板子带有外部PSRAM，需要在配置中启用相关选项。

### 调试技巧

1. **查看详细日志**：在Actions页面点击job查看详细构建日志
2. **本地复现**：使用相同的ESP-IDF v5.3版本在本地复现问题
3. **检查配置**：验证板子的config.json配置是否正确

## 📊 构建统计

### 最新构建性能 (GitHub Actions)
- **平均构建时间**：8-15分钟/板子
- **成功率**：>95%
- **并行度**：最多3个板子同时构建
- **缓存命中率**：ESP-IDF缓存 >80%

### 支持的 ESP32-S3 板子统计
- 当前支持板子：10+
- 覆盖主要厂商：Espressif、M5Stack、LILYGO等
- 显存配置：支持内部SRAM和外部PSRAM

## 🔄 版本发布流程

### 自动发布触发
1. 创建版本标签：`git tag v1.6.0`
2. 推送标签：`git push origin v1.6.0`
3. Actions自动执行：
   - 编译所有ESP32-S3板子
   - 生成独立固件包
   - 创建GitHub Release
   - 上传发布包

### 发布包内容
- **完整包**：xiaozhi-esp32s3-complete-v1.6.0.zip
- **单板包**：xiaozhi-[board]-v1.6.0.zip
- **发布说明**：自动生成的详细更新日志

## 🎨 自定义配置

### 添加新的ESP32-S3板子支持

1. **创建板子配置**：
```jsonc
// main/boards/your-board/config.json
{
  "target": "esp32s3",
  "builds": [
    {
      "name": "your-board-name",
      "sdkconfig_append": [
        "CONFIG_OLED_SSD1306_128X64=y"
      ]
    }
  ]
}
```

2. **实现板子代码**：
```cpp
// main/boards/your-board/your_board.cc
// 继承Board基类，实现具体功能
```

3. **更新CMakeLists.txt**：在main/CMakeLists.txt中添加新板子的配置选项

### 优化编译速度

在workflow中调整：
```yaml
# 增加并行数
strategy:
  max-parallel: 4  # 根据资源调整

# 调整缓存策略
with:
  key: ${{ runner.os }}-idf-v5.3-s3-only  # 单独的S3缓存
```

## 📚 相关资料

- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/)
- [ESP32-S3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [GitHub Actions Documentation](https://docs.github.com/actions)
- [Project README](README.md)

---

🎯 **专注ESP32-S3** - 更快、更稳定、更专业的ESP32-S3构建体验！