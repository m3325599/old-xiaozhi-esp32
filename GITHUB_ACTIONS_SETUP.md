# GitHub Actions 编译配置指南

本项目现在支持通过 GitHub Actions 进行自动化编译和发布。以下是配置和使用说明。

## 🚀 快速开始

### 1. 启用 GitHub Actions

1. 确保您的仓库已启用 GitHub Actions 功能
2. 将以下 workflow 文件保存到 `.github/workflows/` 目录：
   - `build.yml` - 主要编译 workflow
   - `quick-test.yml` - 快速测试 workflow 
   - `release-all.yml` - 全板子发布 workflow

### 2. 触发方式

#### 自动触发条件：
- **推送到 main/devel 分支** → 触发快速测试
- **创建 pull request** → 触发代码检查和快速编译
- **推送版本标签** (如 v1.6.0) → 触发完整发布流程
- **手动运行** → 通过 GitHub Actions 页面手动触发

#### 手动触发：
1. 访问仓库的 Actions 页面
2. 选择相应的 workflow
3. 点击 "Run workflow"
4. （可选）输入参数如板子类型、IDF 目标等

## 🔧 Workflow 说明

### 1. Build Workflow (`build.yml`)

**功能：**
- 支持多板子并行编译
- 自动设置 ESP-IDF 环境
- 生成固件和构建信息
- 打包发布制品

**支持的触发方式：**
```yaml
on:
  push:           # 代码推送
  pull_request:   # PR 合并
  workflow_dispatch:  # 手动运行
  tags:           # 版本标签
```

**可配置参数：**
- `board_type`: 指定编译特定的板子类型
- `idf_target`: 指定 IDF 目标芯片 (esp32/esp32s3)

### 2. Quick Test Workflow (`quick-test.yml`)

**功能：**
- 代码质量检查
- 配置验证
- 快速编译测试
- 依赖检查
- 安全扫描
- 生成测试报告

**使用场景：**
- 每次 PR 时验证代码质量
- 开发过程中快速验证构建

### 3. Release All Workflow (`release-all.yml`)

**功能：**
- 自动发现所有支持的板子
- 并行编译所有固件
- 生成刷写脚本
- 创建发布包
- 自动生成发布说明
- 发布到 GitHub Releases

**高级特性：**
- 自动从 `config.json` 探测板子配置
- 支持为每个板子生成独立发布包
- 包含 Windows/Linux 刷写脚本
- 自动更新 README 版本信息

## 📋 支持的板子类型

当前自动支持的板子（可在 workflow 矩阵中配置）：

| 板子名称 | 目标芯片 | 说明 |
|---------|----------|------|
| bread-compact-wifi | esp32s3 | 基础版小智 |
| bread-compact-esp32 | esp32 | ESP32 版本 |
| m5stack-core-s3 | esp32s3 | M5Stack CoreS3 |
| atoms3r-echo-base | esp32s3 | AtomS3R + Echo 底座 |
| kevin-box-1 | esp32s3 | Kevin Box 版本 |
| esp-box-3 | esp32s3 | Espressif ESP32-S3-BOX3 |

## 🛠️ 自定义配置

### 添加新板子支持

1. **在 workflow 矩阵中添加：**
```yaml
- {"board_type":"your-board-name","idf_target":"esp32s3"}
```

2. **确保板子有正确的配置：**
- `main/boards/your-board-name/config.json`
- 包含正确的 target 和 builds 配置

### 自定义编译参数

在 workflow 中修改编译命令：
```yaml
- name: Build firmware  
  run: |
    idf.py -DOPTIONAL_DEFINE=value build
```

### 配置缓存

- **ESP-IDF 缓存：** 30天，基于操作系统和IDF版本
- **构建缓存：** 基于文件哈希，加速增量构建
- **制品保留：** 固件30天，构建信息7天

## 📦 输出制品

每个 build job 会生成：

### 1. 固件文件
- `merged-binary.bin` - 完整固件包（一键刷写）
- `xiaozhi.bin` - 应用固件
- `bootloader.bin` - 引导程序
- `partition-table.bin` - 分区表
- `ota_data_initial.bin` - OTA 数据

### 2. 刷写脚本
- `flash.sh` - Linux/macOS 刷写脚本
- `flash.bat` - Windows 刷写脚本

### 3. 构建信息
- `build_info.json` - 包含版本、时间、提交等信息
- `sdkconfig` - 编译配置文件

## 🔄 发布流程

### 版本发布
1. 创建版本标签：`git tag v1.6.0`
2. 推送标签：`git push origin v1.6.0`
3. GitHub Actions 自动执行完整发布流程
4. 发布包自动上传到 GitHub Releases

### 发布包含内容
- 所有支持板子的固件包
- 完整发布包（包含所有板子）
- 自动生成的发布说明
- 刷写工具脚本

## 🐛 故障排除

### 常见构建问题

**1. 编译超时**
- 问题：默认 6 小时超时可能不够
- 解决：在 workflow 中调整 `timeout-minutes`

**2. 内存不足** 
- 问题：大型固件可能超出 GitHub runner 内存
- 解决：请求更大规格的 runner 或优化编译选项

**3. 依赖下载失败**
- 问题：网络问题导致 IDF 组件下载失败
- 解决：增加重试机制或使用缓存

### 调试技巧

1. **查看详细日志：**
   - 在 Actions 页面点击 job 查看完整构建日志

2. **本地复现问题：**
   - 使用相同的 ESP-IDF 版本
   - 检查本地的构建环境

3. **检查制品：**
   - 下载构建产物验证完整性
   - 检查 build_info.json 中的构建信息

## 🔒 安全考虑

- **敏感信息：** workflow 不存储密码或密钥
- **第三方 Action：** 仅使用官方或经过验证的 Action
- **依赖检查：** 自动扫描依赖中的安全问题
- **代码扫描：** 检查硬编码的敏感信息

## 🚀 性能优化

### 缓存策略
- ESP-IDF 工具链：基于版本缓存
- 构建目录：基于源文件哈希
- Python 依赖：自动管理

### 并行构建
- 支持同时构建多个板子
- 可配置最大并行数
- 使用构建矩阵优化资源利用

## 📚 相关资源

- [ESP-IDF 文档](https://docs.espressif.com/projects/esp-idf/)
- [GitHub Actions 文档](https://docs.github.com/actions)
- [项目发布手册](main/README.md)

---

通过这些 workflow 配置，您可以实现：

✅ **自动化构建** - 代码提交自动触发构建  
✅ **多平台支持** - 支持多种 ESP32 开发板  
✅ **质量保障** - 自动代码检查和安全扫描  
✅ **自动发布** - 一键生成完整版发布包  
✅ **持续集成** - 确保代码质量和构建稳定性  
