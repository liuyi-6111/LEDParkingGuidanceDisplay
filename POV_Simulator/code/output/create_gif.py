#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
生成高速GIF - 读取frames文件夹内的480帧图片
duration=0.01s (10ms/帧 = 100fps，总时长4.8秒)
"""

import imageio.v2 as iio
import os
import re

# 设置参数
frame_dir = 'frames'  # 你的480帧图片目录
output_gif = 'pov_animation_100fps.gif'

# duration=0.01s = 10ms/帧 = 100fps，480帧总时长4.8秒
duration = 0.01  # 10ms/帧（推荐，平衡速度和可观看性）

# 只读取frames文件夹内的文件
print(f"[*] Loading frames from {frame_dir}...")
print(f"[*] Target: 100fps (10ms/frame), {duration*1000:.1f}ms per frame")

# 读取所有png文件
frame_files = sorted([
    os.path.join(frame_dir, f)
    for f in os.listdir(frame_dir)
    if f.endswith('.png')  # 读取所有png文件
])

print(f"    Found {len(frame_files)} frames")

if len(frame_files) == 0:
    print("[!] No frame files found! Check the folder path.")
    exit(1)

# 按文件名排序（确保顺序正确）
frame_files = sorted(frame_files)

# 加载所有帧
frames = []
for i, file in enumerate(frame_files):
    frames.append(iio.imread(file))
    # 每48帧显示一次进度（480帧显示10次）
    if (i+1) % 48 == 0 or i == len(frame_files)-1:
        progress = (i+1) / len(frame_files) * 100
        print(f"    Loaded {i+1}/{len(frame_files)} frames ({progress:.0f}%)")

# 计算总时长
total_time = len(frame_files) * duration

print(f"[*] Creating GIF at 100fps (total duration: {total_time:.2f}s)...")

# 生成GIF
iio.mimsave(
    output_gif,
    frames,
    duration=duration,
    loop=0,  # 无限循环
    subrectangles=True  # 优化文件大小
)

# 显示结果
fps = 1 / duration
file_size = os.path.getsize(output_gif) / 1024  # KB
file_size_mb = file_size / 1024  # MB

print(f"[✓] GIF created successfully!")
print(f"    Output file: {output_gif}")
print(f"    Frame rate: {fps:.0f} fps ({duration*1000:.1f}ms per frame)")
print(f"    Total duration: {total_time:.2f} seconds")
print(f"    Total frames: {len(frame_files)}")
print(f"    File size: {file_size:.1f} KB ({file_size_mb:.2f} MB)")