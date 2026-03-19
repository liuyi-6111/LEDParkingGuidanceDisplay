#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
POV Rotating LED Display Simulator
For Underground Parking Lot Intelligent Management System
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Circle
import cv2
import os
from datetime import datetime
import imageio


class POVSimulator:
    """
    POV Display Simulator
    Simulates 8 LEDs on a rotating blade
    """

    def __init__(self, num_leds=8, radius_min=2, radius_max=30,
                 img_width=64, img_height=64):
        """Initialize parameters"""
        self.num_leds = num_leds
        self.radius_min = radius_min
        self.radius_max = radius_max
        self.led_radii = np.linspace(radius_min, radius_max, num_leds)
        self.img_width = img_width
        self.img_height = img_height

        # Create output folders
        os.makedirs('images', exist_ok=True)
        os.makedirs('output', exist_ok=True)
        os.makedirs('output/frames', exist_ok=True)

        print(f"=== POV Simulator Initialized ===")
        print(f"Number of LEDs: {num_leds}")
        print(f"Display radius: {radius_min}mm ~ {radius_max}mm")
        print(f"Image resolution: {img_width}x{img_height}")
        print("=" * 30)

    def generate_test_pattern(self):
        """Generate test pattern (up arrow + blue circle only)"""
        img = np.zeros((self.img_height, self.img_width, 3), dtype=np.uint8)
        center_y, center_x = self.img_height // 2, self.img_width // 2

        # Draw blue circle boundary (BGR: 255,0,0 for blue)
        cv2.circle(img, (center_x, center_y), 20, (255, 0, 0), 2)

        # Draw white up arrow
        cv2.arrowedLine(img,
                        (center_x, center_y + 15),  # start point (below center)
                        (center_x, center_y - 15),  # end point (above center)
                        (255, 255, 255), 2, tipLength=0.3)

        cv2.imwrite('images/up_arrow_blue_circle.png', img)
        print(f"[✓] Test pattern generated: images/up_arrow_blue_circle.png")
        return img

    def cartesian_to_polar(self, image):
        """Convert Cartesian image to polar sampling data"""
        h, w = image.shape[:2]
        center_y, center_x = h // 2, w // 2
        num_angles = 360
        polar_data = np.zeros((num_angles, self.num_leds, 3), dtype=np.uint8)

        print(f"[*] Converting image...")
        print(f"    Angle samples: {num_angles}")
        print(f"    Radial samples: {self.num_leds}")

        for angle_idx, angle_deg in enumerate(range(num_angles)):
            angle_rad = np.deg2rad(angle_deg)
            '''POV核心：极坐标转直角坐标'''
            for led_idx, radius in enumerate(self.led_radii):
                x = int(center_x + radius * np.cos(angle_rad))
                y = int(center_y + radius * np.sin(angle_rad))
                if 0 <= x < w and 0 <= y < h:
                    polar_data[angle_idx, led_idx] = image[y, x]

        print(f"[✓] Conversion complete!")
        return polar_data

    def simulate_stm32_control(self, polar_data, output_file='output/led_data.h'):
        """Generate C array file for STM32"""
        num_angles, num_leds, _ = polar_data.shape

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("/*\n")
            f.write(" * POV LED Display Data\n")
            f.write(" * Generated: {}\n".format(datetime.now().strftime("%Y-%m-%d %H:%M:%S")))
            f.write(" * Number of LEDs: {}\n".format(num_leds))
            f.write(" * Angle samples: {}\n".format(num_angles))
            f.write(" */\n\n")

            f.write("#ifndef __LED_DATA_H\n")
            f.write("#define __LED_DATA_H\n\n")
            f.write("#include <stdint.h>\n\n")
            f.write("#define POV_NUM_LEDS {}\n".format(num_leds))
            f.write("#define POV_NUM_ANGLES {}\n\n".format(num_angles))

            f.write("const uint8_t pov_image_data[POV_NUM_ANGLES][POV_NUM_LEDS][3] = {\n")
            for angle_idx in range(num_angles):
                f.write("    {{ // Angle {} }}\n".format(angle_idx))
                for led_idx in range(num_leds):
                    r, g, b = polar_data[angle_idx, led_idx]
                    f.write("        {{{:3d}, {:3d}, {:3d}}}".format(r, g, b))
                    if led_idx < num_leds - 1:
                        f.write(",")
                    f.write("\n")
                f.write("    }")
                if angle_idx < num_angles - 1:
                    f.write(",")
                f.write("\n")
            f.write("};\n\n#endif // __LED_DATA_H\n")

        print(f"[✓] STM32 data generated: {output_file}")

    def render_pov_frame(self, polar_data, angle_offset=0, filename='output/frame.png'):
        """Render single frame"""
        fig = plt.figure(figsize=(14, 6))

        # Left: Polar coordinate sampling
        ax1 = fig.add_subplot(121, projection='polar')
        ax1.set_title('Polar Coordinate Sampling', fontsize=12, pad=20)

        num_angles = polar_data.shape[0]
        theta = np.linspace(0, 2 * np.pi, num_angles, endpoint=False)

        for angle_idx, angle in enumerate(theta):
            for led_idx, radius in enumerate(self.led_radii):
                color = polar_data[angle_idx, led_idx] / 255.0
                ax1.scatter(angle, radius, c=[color], s=40, marker='s', alpha=0.8, edgecolors='none')

        ax1.set_theta_zero_location('E')
        ax1.set_theta_direction(-1)
        ax1.set_ylim(0, self.radius_max + 5)
        ax1.grid(True, alpha=0.3, linestyle='--')

        # Right: Simulated POV effect
        ax2 = fig.add_subplot(122)
        ax2.set_title('Simulated POV Effect', fontsize=12, pad=20)
        ax2.set_aspect('equal')

        # Remove all borders
        ax2.spines['top'].set_visible(False)
        ax2.spines['right'].set_visible(False)
        ax2.spines['bottom'].set_visible(False)
        ax2.spines['left'].set_visible(False)
        ax2.tick_params(left=False, bottom=False, labelleft=False, labelbottom=False)

        # Draw radius circles
        for r in self.led_radii:
            circle = Circle((0, 0), r, fill=False,
                            linestyle='--', linewidth=0.5, alpha=0.3, edgecolor='gray')
            ax2.add_patch(circle)

        # Draw LEDs at current angle
        current_angle = np.deg2rad(angle_offset)
        for led_idx, r in enumerate(self.led_radii):
            x = r * np.cos(current_angle)
            y = r * np.sin(current_angle)
            angle_idx = int(angle_offset) % 360
            color = polar_data[angle_idx, led_idx] / 255.0
            circle = Circle((x, y), 2.5, color=color, ec='none', alpha=0.9)
            ax2.add_patch(circle)

        # Draw rotation axis
        ax2.plot(0, 0, 'ko', markersize=8, label='Rotation Axis', markeredgecolor='none')

        # Draw rotation trajectory
        theta_line = np.linspace(0, 2 * np.pi, 100)
        x_line = self.radius_max * 0.7 * np.cos(theta_line)
        y_line = self.radius_max * 0.7 * np.sin(theta_line)
        ax2.plot(x_line, y_line, '--', color='gray', alpha=0.5, linewidth=1)

        ax2.set_xlim(-self.radius_max - 5, self.radius_max + 5)
        ax2.set_ylim(-self.radius_max - 5, self.radius_max + 5)
        ax2.legend(loc='upper right', fontsize=8, frameon=False)
        ax2.set_xlabel('X (mm)', fontsize=10)
        ax2.set_ylabel('Y (mm)', fontsize=10)

        plt.suptitle(f'POV Rotating LED Display (Angle: {angle_offset:.0f}°)',
                     fontsize=14, fontweight='bold')
        plt.tight_layout()
        plt.savefig(filename, dpi=150, bbox_inches='tight', transparent=False)
        plt.close()

        print(f"[✓] Frame saved: {filename}")

    def generate_animation_frames(self, polar_data, num_frames=12):
        """Generate animation frames"""
        frame_dir = 'output/frames'
        angles = np.linspace(0, 360, num_frames, endpoint=False)

        print(f"[*] Generating {num_frames} animation frames...")
        frame_files = []
        for i, angle in enumerate(angles):
            filename = f'{frame_dir}/frame_{i:03d}_{int(angle)}deg.png'
            self.render_pov_frame(polar_data, angle, filename)
            frame_files.append(filename)

        print(f"[✓] {num_frames} frames generated")
        return frame_files

    def create_gif(self, frame_files, gif_filename='output/pov_animation.gif', duration=0.01):
        """
        创建高速无限循环GIF动画
        duration: 每帧显示时间（秒）
        - 0.03 = 30ms (33 fps) - 较慢
        - 0.01 = 10ms (100 fps) - 推荐，有视觉暂留效果
        - 0.005 = 5ms (200 fps) - 极快，适合POV演示
        """
        try:
            # 使用imageio.v2避免版本兼容问题
            import imageio.v2 as iio

            frames = []
            total_frames = len(frame_files)

            print(f"[*] Loading {total_frames} frames...")
            for i, file in enumerate(frame_files):
                frames.append(iio.imread(file))
                # 显示进度
                if (i + 1) % 4 == 0 or i == total_frames - 1:
                    print(f"    Loaded {i + 1}/{total_frames} frames")

            print(f"[*] Creating GIF (duration={duration * 1000:.0f}ms per frame)...")

            # 保存GIF
            iio.mimsave(
                gif_filename,frames,duration=duration,
                loop=0,  # 无限循环
                subrectangles=True
            )

            # 计算帧率并显示信息
            fps = 1 / duration
            file_size = os.path.getsize(gif_filename) / 1024

            print(f"[✓] GIF animation created: {gif_filename}")
            print(f"    Frame rate: {fps:.1f} fps ({duration * 1000:.0f}ms per frame)")
            print(f"    Loop: Infinite")
            print(f"    File size: {file_size:.1f} KB")
            print(f"    Frames: {total_frames}")

            return True
        except Exception as e:
            print(f"[!] GIF creation failed: {e}")
            print(f"    Try installing: pip install imageio[ffmpeg]")
            return False

    def generate_intensity_profile(self, polar_data):
        """Generate brightness compensation curve"""
        avg_intensity = np.mean(polar_data, axis=(0, 2))

        plt.figure(figsize=(10, 8))

        # Original brightness
        plt.subplot(2, 1, 1)
        plt.plot(self.led_radii, avg_intensity, 'o-', color='blue', linewidth=2, markersize=8)
        plt.title('Original Brightness (Center Bright, Edge Dark)', fontsize=12, fontweight='bold')
        plt.xlabel('Radius (mm)')
        plt.ylabel('Average Brightness (0-255)')
        plt.grid(True, alpha=0.3)
        plt.xlim(0, self.radius_max + 5)

        # Compensated brightness
        compensation_factor = self.led_radii / self.radius_min
        compensated = avg_intensity * compensation_factor
        compensated = np.clip(compensated, 0, 255)

        plt.subplot(2, 1, 2)
        plt.plot(self.led_radii, compensated, 'o-', color='red', linewidth=2, markersize=8)
        plt.title('Compensated Brightness (With Rotation Compensation)', fontsize=12, fontweight='bold')
        plt.xlabel('Radius (mm)')
        plt.ylabel('Compensated Brightness')
        plt.grid(True, alpha=0.3)
        plt.xlim(0, self.radius_max + 5)

        plt.tight_layout()
        plt.savefig('output/intensity_compensation.png', dpi=150)
        plt.close()
        print(f"[✓] Brightness compensation curve: output/intensity_compensation.png")

    def export_for_ppt(self):
        """Export all materials for PPT"""
        print("\n" + "=" * 50)
        print("📊 Exporting PPT Materials")
        print("=" * 50)

        test_img = self.generate_test_pattern()
        polar_data = self.cartesian_to_polar(test_img)
        self.simulate_stm32_control(polar_data)

        self.generate_intensity_profile(polar_data)

        print("\n[*] Generating animation frames...")
        frame_files = self.generate_animation_frames(polar_data, num_frames=480)

        print("\n" + "=" * 50)
        print("✅ All materials generated successfully!")
        print("=" * 50)
        print("\nOutput files:")
        for root, dirs, files in os.walk('output'):
            for file in files:
                print(f"  📄 {os.path.join(root, file)}")
        print("\nThese files can be directly inserted into your PPT.")


def main():
    print("""
    ╔══════════════════════════════════════════╗
    ║  Rotating LED POV Display Simulator      ║
    ║  For Underground Parking Lot Management  ║
    ║  Version: English (No Chinese Font Issues)║
    ╚══════════════════════════════════════════╝
    """)

    sim = POVSimulator(num_leds=8, radius_min=2, radius_max=30,
                       img_width=64, img_height=64)
    sim.export_for_ppt()
    print("\n✨ Done! Check the 'output' folder for all generated files.")


if __name__ == "__main__":
    main()