# Weiwu X1 WiFi v2.2.6 移植说明

## 移植状态

✅ **已完成基础移植**

## 移植内容

### 1. 板子定义 (`main/boards/weiwu-x1-wifi/`)
- `config.h` - GPIO配置（基于绿板）
- `config.json` - 构建配置
- `weiwu_x1_wifi.cc` - 板子实现

### 2. 关键特性
- ✅ Simplex音频模式（麦克风/扬声器分离）
- ✅ 功放使能引脚控制（GPIO18）
- ✅ 关机功能（长按BOOT键3秒）
- ✅ 触摸按键支持
- ✅ LED控制（GPIO8/48）
- ✅ 无显示屏（使用NoDisplay）

### 3. 分区表
使用 v2 分区表：`partitions/v2/8m.csv`
- ota_0: 3MB
- ota_1: 3MB
- assets: 2MB

### 4. 配置文件
`sdkconfig.defaults.weiwu-x1-wifi`

## 与v1的主要差异

| 特性 | v1.5.2 | v2.2.6 |
|------|--------|--------|
| 架构 | 直接调用 | 音频服务层 |
| 分区 | 单OTA | 双OTA + assets |
| 资源 | 静态编译 | 动态下载 |
| 状态管理 | 简单 | 状态机 |
| MCP协议 | ❌ | ✅ |
| 自定义唤醒词 | ❌ | ✅ |

## 已知问题

1. **assets下载**：首次启动需要从网络下载资源
2. **唤醒词**：需要下载模型到assets分区
3. **内存使用**：v2架构更复杂，内存占用更大

## 测试建议

1. 首次烧录后，设备会自动下载assets（需要WiFi）
2. 如果assets下载失败，可以手动刷入默认assets
3. 建议测试：
   - 基础语音对话
   - 关机功能（长按BOOT键3秒）
   - 触摸按键
   - LED状态
   - OTA升级

## 构建命令

```bash
# 设置目标
idf.py set-target esp32s3

# 配置（选择Weiwu X1 WiFi板子）
idf.py menuconfig

# 构建
idf.py build

# 烧录
idf.py flash

# 监控
idf.py monitor
```

## 注意事项

1. **必须重新烧录**：v1和v2分区表不兼容，无法OTA升级
2. **备份配置**：烧录前备份WiFi配置等信息
3. **assets分区**：首次启动需要联网下载资源
4. **回滚**：如果v2不稳定，可以刷回v1.5.2备份

## 后续优化

- [ ] 测试音频质量（Simplex模式）
- [ ] 验证关机功能稳定性
- [ ] 测试MCP协议
- [ ] 优化内存使用
- [ ] 添加自定义唤醒词支持

## 参考

- 原作者仓库：https://github.com/78/xiaozhi-esp32
- v2分区表说明：`partitions/v2/README.md`
- 自定义板子文档：`docs/custom-board_zh.md`
