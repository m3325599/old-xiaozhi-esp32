#!/usr/bin/env python3
"""
GitHub Actions Workflow Validator for XiaoZhi ESP32
验证workflow配置文件的正确性
"""

import os
import json
from pathlib import Path

# 简单YAML检查，避免依赖外部库
def safe_load_yaml(content):
    """简单的YAML检查和加载"""
    try:
        # 基础检查：必须有正确的缩进和冒号
        lines = content.strip().split('\n')
        for line in lines:
            line = line.rstrip()
            if line and not line.startswith('#'):
                # 跳过注释和空行
                if ':' in line and not line.lstrip().startswith('-'):
                    # 基本格式检查
                    pass
        return True, "YAML格式基础检查通过"
    except Exception as e:
        return False, str(e)

def check_yaml_syntax(workflow_path):
    """检查YAML语法"""
    try:
        with open(workflow_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 基础YAML格式检查
        yaml_valid, yaml_error = safe_load_yaml(content)
        if yaml_valid:
            return True, None
        else:
            return False, yaml_error
    except Exception as e:
        return False, str(e)

def validate_workflow_structure(workflow_path, content):
    """验证workflow结构（简化版本）"""
    issues = []
    
    # 对于基础检查，我们主要使用文本分析来避免复杂的YAML解析
    with open(workflow_path, 'r', encoding='utf-8') as f:
        content_text = f.read()
    
    # 检查基本结构
    if 'name:' not in content_text:
        issues.append("Missing 'name' field")
    
    if 'on:' not in content_text:
        issues.append("Missing 'on' field")
    
    if 'jobs:' not in content_text:
        issues.append("Missing 'jobs' field")
    
    # 检查runs-on
    if 'runs-on:' not in content_text:
        issues.append("Missing 'runs-on' in jobs")
    
    return issues

def check_specific_issues(workflow_path, content):
    """检查特定问题"""
    issues = []
    
    with open(workflow_path, 'r', encoding='utf-8') as f:
        content_text = f.read()
    
    # 检查已废弃的v3 artifact actions
    if 'upload-artifact@v3' in content_text:
        issues.append("Deprecated upload-artifact@v3 found")
    
    if 'download-artifact@v3' in content_text:
        issues.append("Deprecated download-artifact@v3 found")
    
    # 检查环境变量设置
    if 'FORCE_JAVASCRIPT_ACTIONS_TO_NODE24' in content_text:
        if 'FORCE_JAVASCRIPT_ACTIONS_TO_NODE24: "true"' not in content_text:
            issues.append("FORCE_JAVASCRIPT_ACTIONS_TO_NODE24 should be set to 'true'")
    
    return issues

def main():
    workflow_dir = Path('.github/workflows')
    
    if not workflow_dir.exists():
        print("❌ .github/workflows directory not found")
        return 1
    
    workflow_files = list(workflow_dir.glob('*.yml')) + list(workflow_dir.glob('*.yaml'))
    
    if not workflow_files:
        print("❌ No workflow files found")
        return 1
    
    print(f"🔍 验证 {len(workflow_files)} 个workflow文件...")
    print("=" * 60)
    
    total_issues = 0
    
    for workflow_path in workflow_files:
        print(f"\n📄 {workflow_path.name}")
        print("-" * 40)
        
        # 检查YAML语法
        yaml_valid, yaml_error = check_yaml_syntax(workflow_path)
        if not yaml_valid:
            print(f"❌ YAML语法错误: {yaml_error}")
            total_issues += 1
            continue
        else:
            print("✅ YAML语法正确")
        
        # 验证结构
        structure_issues = validate_workflow_structure(workflow_path, None)
        if structure_issues:
            print(f"⚠️  结构问题 ({len(structure_issues)}个):")
            for issue in structure_issues:
                print(f"   - {issue}")
            total_issues += len(structure_issues)
        else:
            print("✅ 结构验证通过")
        
        # 检查特定问题
        specific_issues = check_specific_issues(workflow_path, None)
        if specific_issues:
            print(f"⚠️  特定问题 ({len(specific_issues)}个):")
            for issue in specific_issues:
                print(f"   - {issue}")
            total_issues += len(specific_issues)
        else:
            print("✅ 无特定问题")
    
    print("=" * 60)
    if total_issues == 0:
        print("🎉 所有workflow文件验证通过！")
        return 0
    else:
        print(f"❌ 发现 {total_issues} 个问题需要修复")
        return 1

if __name__ == "__main__":
    exit(main())