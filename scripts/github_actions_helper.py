#!/usr/bin/env python3
"""
GitHub Actions Helper Script for XiaoZhi ESP32 Project
用于辅助 GitHub Actions 构建流程的脚本
"""

import os
import json
import argparse
import subprocess
from pathlib import Path
from datetime import datetime

def discover_boards():
    """发现所有可用的板子配置"""
    boards = []
    boards_dir = Path("main/boards")
    
    print("🔍 发现板子配置...")
    
    for board_dir in boards_dir.iterdir():
        if not board_dir.is_dir():
            continue
            
        config_file = board_dir / "config.json"
        if not config_file.exists():
            continue
            
        try:
            with open(config_file) as f:
                config = json.load(f)
                
            board_info = {
                "name": board_dir.name,
                "target": config["target"],
                "config_path": str(config_file),
                "builds": config.get("builds", [])
            }
            boards.append(board_info)
            print(f"  ✓ {board_info['name']} ({board_info['target']}) - {len(board_info['builds'])} builds")
            
        except Exception as e:
            print(f"  ✗ {board_dir.name}: {e}")
    
    return boards

def generate_build_matrix(boards, filter_target=None):
    """生成 GitHub Actions 构建矩阵"""
    matrix_items = []
    
    for board in boards:
        if filter_target and board["target"] != filter_target:
            continue
            
        # 为每个构建变体创建矩阵项
        for build in board["builds"]:
            matrix_items.append({
                "board_type": build["name"],
                "idf_target": board["target"],
                "config_file": board["config_path"]
            })
    
    return {"include": matrix_items}

def validate_project_structure():
    """验证项目结构"""
    print("🔍 验证项目结构...")
    
    required_files = [
        "CMakeLists.txt",
        "main/CMakeLists.txt", 
        "main/main.cc",
        "main/application.cc"
    ]
    
    required_dirs = [
        "main/boards",
        "main/audio_codecs",
        "main/display",
        "main/protocols"
    ]
    
    errors = []
    warnings = []
    
    # 检查必需文件
    for file_path in required_files:
        if Path(file_path).exists():
            print(f"  ✓ {file_path}")
        else:
            errors.append(f"Missing required file: {file_path}")
            print(f"  ✗ {file_path}")
    
    # 检查必需目录
    for dir_path in required_dirs:
        if Path(dir_path).exists():
            file_count = len(list(Path(dir_path).rglob("*.*")))
            print(f"  ✓ {dir_path} ({file_count} files)")
        else:
            errors.append(f"Missing required directory: {dir_path}")
            print(f"  ✗ {dir_path}")
    
    # 检查 idf_component.yml
    if Path("main/idf_component.yml").exists():
        try:
            import yaml
            with open("main/idf_component.yml") as f:
                deps = yaml.safe_load(f)
                if "dependencies" in deps:
                    dep_count = len(deps["dependencies"])
                    print(f"  ✓ main/idf_component.yml ({dep_count} dependencies)")
        except Exception as e:
            warnings.append(f"Invalid YAML in idf_component.yml: {e}")
    else:
        warnings.append("Missing idf_component.yml")
    
    # 检查至少有一个板子配置文件
    board_configs = list(Path("main/boards").glob("*/config.json"))
    if board_configs:
        print(f"  ✓ 发现 {len(board_configs)} 个板子配置")
    else:
        errors.append("No board configurations found")
    
    return errors, warnings

def check_sensitive_content():
    """检查敏感内容"""
    print("🔒 检查敏感内容...")
    
    sensitive_patterns = [
        "API_KEY", "SECRET_KEY", "PASSWORD", 
        "wifi_password", "token", "auth_key"
    ]
    
    found_issues = []
    
    for root, dirs, files in os.walk("main"):
        for file in files:
            if file.endswith((".cc", ".h", ".c")):
                file_path = Path(root) / file
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        
                    for pattern in sensitive_patterns:
                        if pattern.lower() in content.lower():
                            found_issues.append(f"{file_path}: potential {pattern}")
                            
                except Exception as e:
                    print(f"  ⚠ 无法读取 {file_path}: {e}")
    
    if found_issues:
        print("  ⚠ 发现潜在敏感内容:")
        for issue in found_issues[:5]:  # 只显示前5个
            print(f"    - {issue}")
    else:
        print("  ✓ 未发现明显的敏感内容")
    
    return found_issues

def generate_build_info(board_type, target, version):
    """生成构建信息"""
    info = {
        "project": "xiaozhi",
        "board_type": board_type,
        "target": target,
        "version": version,
        "build_time": datetime.utcnow().isoformat() + "Z",
        "build_environment": {
            "idf_version": os.environ.get("IDF_VERSION", "v5.3"),
            "runner_os": os.environ.get("RUNNER_OS", "Linux")
        }
    }
    
    # 添加 Git 信息（如果在 GitHub Actions 中）
    if "GITHUB_SHA" in os.environ:
        info["git"] = {
            "commit": os.environ["GITHUB_SHA"],
            "ref": os.environ.get("GITHUB_REF", ""),
            "repository": os.environ.get("GITHUB_REPOSITORY", "")
        }
    
    return info

def create_binary_summary(build_dir):
    """创建固件摘要"""
    binary_files = {
        "firmware": "xiaozhi.bin",
        "merged": "merged-binary.bin",
        "bootloader": "bootloader/bootloader.bin",
        "partitions": "partition_table/partition-table.bin"
    }
    
    summary = {}
    build_path = Path(build_dir)
    
    for name, file_path in binary_files.items():
        full_path = build_path / file_path
        if full_path.exists():
            summary[name] = {
                "path": str(full_path),
                "size": full_path.stat().st_size,
                "modified": datetime.fromtimestamp(full_path.stat().st_mtime).isoformat()
            }
        else:
            summary[name] = {"error": "File not found"}
    
    return summary

def main():
    parser = argparse.ArgumentParser(description="GitHub Actions Helper for XiaoZhi ESP32")
    subparsers = parser.add_subparsers(dest="command", help="可用命令")
    
    # 发现板子
    discover_parser = subparsers.add_parser("discover", help="发现可用板子")
    discover_parser.add_argument("--json", action="store_true", help="输出JSON格式")
    discover_parser.add_argument("--target", help="过滤特定目标芯片")
    
    # 验证项目
    validate_parser = subparsers.add_parser("validate", help="验证项目结构")
    
    # 安全检查  
    security_parser = subparsers.add_parser("security", help="安全检查")
    
    # 生成构建矩阵
    matrix_parser = subparsers.add_parser("matrix", help="生成构建矩阵")
    matrix_parser.add_argument("--target", help="过滤特定目标芯片")
    
    # 生成构建信息
    info_parser = subparsers.add_parser("build-info", help="生成构建信息")
    info_parser.add_argument("--board", required=True, help="板子类型")
    info_parser.add_argument("--target", required=True, help="目标芯片")
    info_parser.add_argument("--version", required=True, help="版本号")
    info_parser.add_argument("--output", help="输出文件")
    
    # 二进制摘要
    summary_parser = subparsers.add_parser("summary", help="创建二进制摘要")
    summary_parser.add_argument("--build-dir", default="build", help="构建目录")
    summary_parser.add_argument("--output", help="输出文件")
    
    args = parser.parse_args()
    
    if args.command == "discover":
        boards = discover_boards()
        
        if args.json:
            if args.target:
                boards = [b for b in boards if b["target"] == args.target]
            print(json.dumps(boards, indent=2))
        else:
            print(f"\n发现 {len(boards)} 个板子:")
            for board in boards:
                if not args.target or board["target"] == args.target:
                    print(f"  - {board['name']} ({board['target']})")
    
    elif args.command == "validate":
        errors, warnings = validate_project_structure()
        
        print(f"\n结果: {len(errors)} 错误, {len(warnings)} 警告")
        
        if errors:
            print("\n❌ 错误:")
            for error in errors:
                print(f"  - {error}")
            return 1
        
        if warnings:
            print("\n⚠️ 警告:")
            for warning in warnings:
                print(f"  - {warning}")
        
        if not errors:
            print("\n✅ 项目结构验证通过")
    
    elif args.command == "security":
        issues = check_sensitive_content()
        
        if issues:
            print(f"\n⚠️ 发现 {len(issues)} 个潜在安全问题")
            return 1
        else:
            print("\n✅ 安全检查通过")
    
    elif args.command == "matrix":
        boards = discover_boards()
        matrix = generate_build_matrix(boards, args.target)
        print(json.dumps(matrix, indent=2))
    
    elif args.command == "build-info":
        info = generate_build_info(args.board, args.target, args.version)
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(info, f, indent=2)
            print(f"构建信息已保存到 {args.output}")
        else:
            print(json.dumps(info, indent=2))
    
    elif args.command == "summary":
        summary = create_binary_summary(args.build_dir)
        
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(summary, f, indent=2)
            print(f"二进制摘要已保存到 {args.output}")
        else:
            print(json.dumps(summary, indent=2))
    
    else:
        parser.print_help()
        return 1

if __name__ == "main":
    exit(main())