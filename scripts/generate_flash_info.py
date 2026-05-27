#!/usr/bin/env python3
import datetime
import sys

sha = sys.argv[1] if len(sys.argv) > 1 else "unknown"
sha_short = sha[:7]
now = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
ver = datetime.datetime.now().strftime('%Y%m%d_%H%M%S') + '_' + sha_short

content = f"""========================================
微五 X1 WiFi 版固件 - 绿板配置
========================================

版本: {ver}
提交: {sha}
构建时间: {now}

文件说明:
- merged-binary.bin: 一键刷入固件（推荐）
- xiaozhi.bin: 应用程序固件
- bootloader.bin: 引导程序
- partition-table.bin: 分区表
- ota_data_initial.bin: OTA初始数据

========================================
一键刷入命令（推荐）:
========================================
esptool.py --chip esp32s3 --port COMx --baud 921600 write_flash 0x0 merged-binary.bin

或刷入单个文件:
esptool.py --chip esp32s3 --port COMx --baud 921600 write_flash \
  0x0 bootloader/bootloader.bin \
  0x8000 partition-table.bin \
  0xd000 ota_data_initial.bin \
  0x10000 srmodels/srmodels.bin \
  0x60000 xiaozhi.bin

========================================
硬件配置:
========================================
- 麦克风: I2S Simplex (GPIO42/40/2)
- 扬声器: I2S (GPIO15/16/17/18)
- BOOT按键: GPIO0 (绿板)
- 触摸按键: GPIO5
- LED: GPIO8/48
- Flash: 8MB
- 无 OLED 显示屏
========================================
"""

with open('flash_info.txt', 'w', encoding='utf-8') as f:
    f.write(content)

print("flash_info.txt generated successfully")
