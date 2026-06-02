#!/usr/bin/env python3
import os
import sys
import json
from pathlib import Path


def build_assets() -> None:
    """Build assets.bin (srmodels, fonts, etc.) and place it in build/assets.bin"""
    import shutil
    
    build_assets_script = Path(__file__).parent / "build_default_assets.py"
    if not build_assets_script.exists():
        print(f"Warning: {build_assets_script} not found, skipping assets build")
        return
    
    cmd = f"python3 {build_assets_script} --sdkconfig sdkconfig --output build/assets.bin"
    
    # Try to find ESP-SR model path
    for model_path in [
        Path("managed_components") / "espressif__esp-sr" / "model",
        Path("managed_components") / "espressif_esp-sr" / "model",
        Path("components") / "esp-sr" / "model"
    ]:
        if model_path.exists():
            cmd += f" --esp_sr_model_path {model_path}"
            break
    
    # Try to find xiaozhi-fonts path
    for fonts_path in [
        Path("components") / "xiaozhi-fonts",
        Path("managed_components") / "78__xiaozhi-fonts",
        Path("managed_components") / "xiaozhi-fonts"
    ]:
        if fonts_path.exists():
            cmd += f" --xiaozhi_fonts_path {fonts_path}"
            break
    
    print(f"Running: {cmd}")
    result = os.system(cmd)
    if result != 0:
        print(f"Warning: assets build failed (exit code {result}), continuing without assets partition")
    elif Path("build/assets.bin").exists():
        size = Path("build/assets.bin").stat().st_size
        print(f"assets.bin size: {size} bytes ({size / 1024:.2f} KB)")


def merge_bin() -> None:
    import shutil
    esptool_path = shutil.which("esptool.py")
    if not esptool_path:
        esptool_path = shutil.which("esptool")
    if not esptool_path:
        print("esptool.py not found in PATH", file=sys.stderr)
        sys.exit(1)
    
    required_files = [
        "build/bootloader/bootloader.bin",
        "build/partition_table/partition-table.bin",
        "build/ota_data_initial.bin",
        "build/xiaozhi.bin"
    ]
    
    for f in required_files:
        if not Path(f).exists():
            print(f"Required file not found: {f}", file=sys.stderr)
            sys.exit(1)
    
    # v1.5.2 compatible partition table layout:
    # - bootloader @ 0x0
    # - partition table @ 0x8000
    # - ota data @ 0xd000
    # - ota_0 @ 0x100000
    cmd = f"{esptool_path} --chip esp32s3 merge_bin -o build/merged-binary.bin " \
          "--flash_mode dio --flash_size 8MB --flash_freq 80m " \
          "0x0 build/bootloader/bootloader.bin " \
          "0x8000 build/partition_table/partition-table.bin " \
          "0xd000 build/ota_data_initial.bin " \
          "0x100000 build/xiaozhi.bin"
    
    if Path("build/assets.bin").exists():
        cmd += " 0x10000 build/assets.bin"
        print("Including assets.bin (model partition @ 0x10000)")
    
    print(f"Running: {cmd}")
    if os.system(cmd) != 0:
        print("merge-bin failed", file=sys.stderr)
        sys.exit(1)


def release(board_type: str, name: str):
    print("=" * 80)
    print("Building project")
    print("=" * 80)

    if os.system("idf.py fullclean") != 0:
        print("fullclean failed")
        sys.exit(1)

    config_file = Path(f"main/boards/{board_type}/config.json")
    if not config_file.exists():
        print(f"Config file not found: {config_file}", file=sys.stderr)
        sys.exit(1)

    with open(config_file, encoding="utf-8") as f:
        config = json.load(f)
        print(f"Project Version: {config.get('version', 'unknown')}")

    print("=" * 80)
    print("Building project")
    print("=" * 80)
    print("=" * 80)

    with open("sdkconfig.defaults", "w", encoding="utf-8") as f:
        f.write(f'CONFIG_BOARD_NAME="{name}"\n')
        f.write(f"CONFIG_BOARD_TYPE_{board_type.upper().replace('-', '_')}=y\n")

    if os.path.exists(f"sdkconfig.defaults.{board_type}"):
        with open(f"sdkconfig.defaults.{board_type}", encoding="utf-8") as f:
            f_content = f.read()
        with open("sdkconfig.defaults", "a", encoding="utf-8") as f:
            f.write(f_content)

    # Build with macro BOARD_NAME defined to name
    if os.system(f"idf.py -DBOARD_NAME={name} -DBOARD_TYPE={board_type} build") != 0:
        print("build failed")
        sys.exit(1)

    # Build assets.bin (srmodels, fonts, etc.)
    build_assets()

    # merge-bin
    merge_bin()

    print("=" * 80)
    print("Building project done")
    print("=" * 80)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: release.py <board_type> [--name <name>]", file=sys.stderr)
        sys.exit(1)

    board_type = sys.argv[1]
    name = board_type

    if len(sys.argv) > 2 and sys.argv[2] == "--name":
        if len(sys.argv) < 4:
            print("Usage: release.py <board_type> --name <name>", file=sys.stderr)
            sys.exit(1)
        name = sys.argv[3]

    release(board_type, name)
