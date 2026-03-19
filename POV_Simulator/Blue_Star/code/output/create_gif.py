#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
生成16 LED蓝色星星GIF - 12帧动画
增强版 - 自动打开文件夹
"""

import imageio.v2 as iio
import os
import subprocess
import sys

# 设置参数
frame_dir = 'D:\\大创\\POV_Simulator\\Blue_Star\\code\\output\\frames'
output_gif = 'D:\\大创\\POV_Simulator\\Blue_Star\\blue_star_16leds.gif'

# 12帧动画参数
duration = 0.001  # 每帧0.5秒
expected_frames = 12

print("=" * 50)
print("16 LEDs 蓝色星星GIF生成器")
print("=" * 50)
print(f"当前工作目录: {os.getcwd()}")
print(f"Python解释器: {sys.executable}")
print("-" * 50)

# 检查文件夹
if not os.path.exists(frame_dir):
    print(f"❌ 错误: 找不到文件夹")
    print(f"   预期路径: {frame_dir}")
    # 尝试查找frames文件夹
    alternative_paths = [
        'D:\\大创\\POV_Simulator\\Blue_Star\\code\\output\\frames',
        'D:\\大创\\POV_Simulator\\Blue_Star\\output\\frames',
        '.\\output\\frames',
        '..\\output\\frames'
    ]
    for alt_path in alternative_paths:
        if os.path.exists(alt_path):
            print(f"   找到替代路径: {alt_path}")
            frame_dir = alt_path
            break
    else:
        exit(1)

print(f"✅ 找到文件夹: {frame_dir}")

# 获取所有帧文件
frame_files = [f for f in os.listdir(frame_dir)
               if f.endswith('.png') and f.startswith('frame_')]
frame_files.sort()  # 按文件名排序

print(f"✅ 找到 {len(frame_files)} 帧图片")

if len(frame_files) != expected_frames:
    print(f"⚠️ 警告: 预期{expected_frames}帧，实际{len(frame_files)}帧")

# 显示前3帧和后3帧
print("\n📋 帧文件列表:")
for i, f in enumerate(frame_files[:3]):
    print(f"   {i + 1}: {f}")
if len(frame_files) > 6:
    print("   ...")
for i, f in enumerate(frame_files[-3:]):
    print(f"   {len(frame_files) - 3 + i + 1}: {f}")

# 加载帧
print("\n📂 加载帧图片中...")
frames = []
for i, f in enumerate(frame_files):
    filepath = os.path.join(frame_dir, f)
    try:
        frames.append(iio.imread(filepath))
        print(f"   ✅ 加载 {i + 1}/{len(frame_files)}: {f}")
    except Exception as e:
        print(f"   ❌ 加载失败 {f}: {e}")

if len(frames) == 0:
    print("❌ 没有成功加载任何帧！")
    exit(1)

# 生成GIF
print("\n🎬 生成GIF中...")
try:
    iio.mimsave(output_gif, frames, duration=duration, loop=0)
    print(f"   ✅ GIF生成成功")
except Exception as e:
    print(f"   ❌ GIF生成失败: {e}")
    exit(1)

# 结果显示
if os.path.exists(output_gif):
    size_bytes = os.path.getsize(output_gif)
    size_kb = size_bytes / 1024
    size_mb = size_kb / 1024

    print("\n" + "=" * 50)
    print("✅ GIF生成成功!")
    print("=" * 50)
    print(f"📁 文件路径: {output_gif}")
    print(f"📊 文件大小: {size_bytes:,} 字节")
    print(f"            {size_kb:.1f} KB")
    print(f"            {size_mb:.2f} MB")
    print(f"🎞️  帧数: {len(frames)}")
    print(f"⏱️  每帧时长: {duration * 1000:.0f}ms")
    print(f"⏱️  总时长: {len(frames) * duration:.1f}秒")
    print("=" * 50)
