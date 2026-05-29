#!/usr/bin/env python3
"""
测试脚本：验证 Weiwu X1 WiFi 板子配置是否正确
"""

import json
from pathlib import Path

def test_board_config():
    """测试板子配置"""
    
    print("=" * 80)
    print("测试 Weiwu X1 WiFi 板子配置")
    print("=" * 80)
    
    # 1. 检查 config.json
    print("\n1. 检查 config.json...")
    config_path = Path("main/boards/weiwu-x1-wifi/config.json")
    if not config_path.exists():
        print("   ❌ config.json 不存在")
        return False
    
    with config_path.open(encoding='utf-8') as f:
        config = json.load(f)
    
    if config.get("target") != "esp32s3":
        print(f"   ❌ target 配置错误: {config.get('target')}")
        return False
    print(f"   ✅ target: {config.get('target')}")
    
    builds = config.get("builds", [])
    if not builds:
        print("   ❌ 没有找到 builds 配置")
        return False
    
    for build in builds:
        if build.get("name") != "weiwu-x1-wifi":
            print(f"   ❌ build name 错误: {build.get('name')}")
            return False
        print(f"   ✅ build name: {build.get('name')}")
    
    # 2. 检查 config.h
    print("\n2. 检查 config.h...")
    config_h_path = Path("main/boards/weiwu-x1-wifi/config.h")
    if not config_h_path.exists():
        print("   ❌ config.h 不存在")
        return False
    print("   ✅ config.h 存在")
    
    # 3. 检查板子驱动文件
    print("\n3. 检查板子驱动文件...")
    board_cc_path = Path("main/boards/weiwu-x1-wifi/weiwu-x1-wifi.cc")
    if not board_cc_path.exists():
        print("   ❌ weiwu-x1-wifi.cc 不存在")
        return False
    print("   ✅ weiwu-x1-wifi.cc 存在")
    
    # 4. 检查分区表
    print("\n4. 检查分区表...")
    partition_path = Path("partitions/v2/8m.csv")
    if not partition_path.exists():
        print("   ❌ partitions/v2/8m.csv 不存在")
        return False
    print("   ✅ partitions/v2/8m.csv 存在")
    
    # 5. 检查 sdkconfig 配置
    print("\n5. 检查 sdkconfig 配置...")
    sdkconfig_path = Path("sdkconfig.defaults.weiwu-x1-wifi")
    if not sdkconfig_path.exists():
        print("   ⚠️ sdkconfig.defaults.weiwu-x1-wifi 不存在（可选）")
    else:
        print("   ✅ sdkconfig.defaults.weiwu-x1-wifi 存在")
    
    # 6. 检查 CMakeLists.txt
    print("\n6. 检查 CMakeLists.txt...")
    cmake_path = Path("main/CMakeLists.txt")
    cmake_content = cmake_path.read_text(encoding='utf-8')
    
    if 'CONFIG_BOARD_TYPE_WEIWU_X1_WIFI' not in cmake_content:
        print("   ❌ CMakeLists.txt 中未找到 WEIWU_X1_WIFI 配置")
        return False
    print("   ✅ CMakeLists.txt 包含 WEIWU_X1_WIFI 配置")
    
    if 'set(BOARD_TYPE "weiwu-x1-wifi")' not in cmake_content:
        print("   ❌ CMakeLists.txt 中未设置 BOARD_TYPE")
        return False
    print("   ✅ CMakeLists.txt 设置了 BOARD_TYPE")
    
    # 7. 检查 Kconfig
    print("\n7. 检查 Kconfig.projbuild...")
    kconfig_path = Path("main/Kconfig.projbuild")
    kconfig_content = kconfig_path.read_text(encoding='utf-8')
    
    if 'config BOARD_TYPE_WEIWU_X1_WIFI' not in kconfig_content:
        print("   ❌ Kconfig.projbuild 中未找到 BOARD_TYPE_WEIWU_X1_WIFI")
        return False
    print("   ✅ Kconfig.projbuild 包含 BOARD_TYPE_WEIWU_X1_WIFI")
    
    # 8. 检查 GitHub Actions workflow
    print("\n8. 检查 GitHub Actions workflow...")
    workflow_path = Path(".github/workflows/build-weiwu-x1-wifi.yml")
    if not workflow_path.exists():
        print("   ⚠️ build-weiwu-x1-wifi.yml 不存在（可选）")
    else:
        print("   ✅ build-weiwu-x1-wifi.yml 存在")
    
    print("\n" + "=" * 80)
    print("✅ 所有检查通过！Weiwu X1 WiFi 板子配置正确")
    print("=" * 80)
    print("\n下一步:")
    print("1. 推送代码到 GitHub 仓库")
    print("2. 在 Actions 页面手动触发编译")
    print("3. 或等待 push 到 main 分支时自动编译")
    print("\n详细说明请查看:")
    print("- .github/workflows/README_GITHUB_ACTIONS.md")
    print("- README_WEIWU_X1_WIFI.md")
    print("=" * 80)
    
    return True

if __name__ == "__main__":
    import os
    os.chdir(Path(__file__).resolve().parent.parent)
    test_board_config()
