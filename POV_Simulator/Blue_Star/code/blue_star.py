#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Blue Star POV Simulator
16 LEDs, 12 frames version
Following the same structure as POV_Simulator
"""

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Circle
import cv2
import os
from datetime import datetime


class BlueStarPOV:
    """
    Blue Star POV Display Simulator - 16 LEDs
    """

    def __init__(self, num_leds=16, radius_min=2, radius_max=30,
                 img_width=64, img_height=64):
        """Initialize parameters - 16 LEDs"""
        self.num_leds = num_leds
        self.radius_min = radius_min
        self.radius_max = radius_max
        self.led_radii = np.linspace(radius_min, radius_max, num_leds)
        self.img_width = img_width
        self.img_height = img_height

        # Get the correct paths
        self.base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        self.images_dir = os.path.join(self.base_dir, 'images')
        self.output_dir = os.path.join(self.base_dir, 'output')
        self.code_output_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'output')
        self.frames_dir = os.path.join(self.code_output_dir, 'frames')

        # Create all necessary folders
        os.makedirs(self.images_dir, exist_ok=True)
        os.makedirs(self.output_dir, exist_ok=True)
        os.makedirs(self.code_output_dir, exist_ok=True)
        os.makedirs(self.frames_dir, exist_ok=True)

        print(f"\n{'=' * 50}")
        print(f"Yellow Star POV Simulator - 16 LEDs")
        print(f"{'=' * 50}")
        print(f"Number of LEDs: {num_leds}")
        print(f"LED radii: {self.led_radii}")
        print(f"Display radius: {radius_min}mm ~ {radius_max}mm")
        print(f"Image resolution: {img_width}x{img_height}")
        print(f"Number of frames: 12")
        print(f"\nFolder Structure:")
        print(f"  Root: {self.base_dir}")
        print(f"  Images: {self.images_dir}")
        print(f"  Output: {self.output_dir}")
        print(f"  Frames: {self.frames_dir}")
        print(f"{'=' * 50}\n")

    def draw_star(self, img, center, outer_radius, inner_radius, num_points=5, color=(0, 255, 255)):
        """Draw a blue star"""
        cx, cy = center
        points = []

        for i in range(num_points * 2):
            radius = outer_radius if i % 2 == 0 else inner_radius
            angle = (i * 180 / num_points) - 90
            rad = np.deg2rad(angle)

            x = int(cx + radius * np.cos(rad))
            y = int(cy + radius * np.sin(rad))
            points.append([x, y])

        points = np.array(points, np.int32)
        points = points.reshape((-1, 1, 2))
        cv2.fillPoly(img, [points], color)
        cv2.polylines(img, [points], True, (255, 255, 255), 1)

        return img

    def generate_star_pattern(self):
        """Generate blue star pattern"""
        print("[1/4] Generating blue star pattern...")

        img = np.zeros((self.img_height, self.img_width, 3), dtype=np.uint8)
        center_y, center_x = self.img_height // 2, self.img_width // 2

        # Draw main star
        img = self.draw_star(img, (center_x, center_y), 25, 12,
                             num_points=5, color=(0, 255, 255))

        # Add center point
        cv2.circle(img, (center_x, center_y), 2, (255, 255, 255), -1)

        # Add small decorative stars
        small_star_positions = [
            (center_x - 20, center_y - 15),
            (center_x + 22, center_y + 10),
            (center_x - 18, center_y + 20),
            (center_x + 15, center_y - 22)
        ]

        for pos_x, pos_y in small_star_positions:
            if 0 <= pos_x < self.img_width and 0 <= pos_y < self.img_height:
                self.draw_star(img, (pos_x, pos_y), 8, 4,
                               num_points=5, color=(0, 200, 200))

        # Save images
        star_path = os.path.join(self.images_dir, 'blue_star.png')
        star_hires_path = os.path.join(self.images_dir, 'blue_star_highres.png')

        cv2.imwrite(star_path, img)

        high_res = cv2.resize(img, (256, 256), interpolation=cv2.INTER_NEAREST)
        cv2.imwrite(star_hires_path, high_res)

        print(f"   ✓ Star pattern saved to: {star_path}")
        print(f"   ✓ High-res version saved to: {star_hires_path}")

        return img

    def cartesian_to_polar(self, image):
        """Convert Cartesian image to polar sampling data"""
        print("[2/4] Converting to polar coordinates...")

        h, w = image.shape[:2]
        center_y, center_x = h // 2, w // 2
        num_angles = 360
        polar_data = np.zeros((num_angles, self.num_leds, 3), dtype=np.uint8)

        print(f"   Angle samples: {num_angles}")
        print(f"   Radial samples: {self.num_leds}")

        for angle_idx, angle_deg in enumerate(range(num_angles)):
            angle_rad = np.deg2rad(angle_deg)
            for led_idx, radius in enumerate(self.led_radii):
                x = int(center_x + radius * np.cos(angle_rad))
                y = int(center_y + radius * np.sin(angle_rad))
                if 0 <= x < w and 0 <= y < h:
                    polar_data[angle_idx, led_idx] = image[y, x]

        print(f"   ✓ Conversion complete")
        return polar_data

    def render_frame(self, polar_data, angle, frame_num, total_frames):
        """Render a single frame"""
        fig = plt.figure(figsize=(14, 6))

        # Left: Polar sampling
        ax1 = fig.add_subplot(121, projection='polar')
        ax1.set_title(f'Polar Sampling - {angle:.0f}° (16 LEDs)', fontsize=12, pad=20)

        num_angles = polar_data.shape[0]
        theta = np.linspace(0, 2 * np.pi, num_angles, endpoint=False)

        # 绘制16个LED的采样点
        for angle_idx, t in enumerate(theta):
            for led_idx, r in enumerate(self.led_radii):
                color = polar_data[angle_idx, led_idx] / 255.0
                ax1.scatter(t, r, c=[color], s=30, marker='s', alpha=0.8)

        ax1.set_theta_zero_location('E')
        ax1.set_theta_direction(-1)
        ax1.set_ylim(0, self.radius_max + 5)
        ax1.grid(True, alpha=0.3)

        # Right: POV effect
        ax2 = fig.add_subplot(122)
        ax2.set_title(f'POV Effect - {angle:.0f}° (16 LEDs)', fontsize=12, pad=20)
        ax2.set_aspect('equal')

        # Draw circles for all 16 LEDs
        for r in self.led_radii:
            circle = Circle((0, 0), r, fill=False, linestyle='--', alpha=0.3, color='gray')
            ax2.add_patch(circle)

        # Draw 16 LEDs at current angle
        current_angle = np.deg2rad(angle)
        for led_idx, r in enumerate(self.led_radii):
            x = r * np.cos(current_angle)
            y = r * np.sin(current_angle)
            angle_idx = int(angle) % 360
            color = polar_data[angle_idx, led_idx] / 255.0
            led = Circle((x, y), 2.0, color=color, ec='none', alpha=0.9)  # 稍微小一点的圆点
            ax2.add_patch(led)

        ax2.plot(0, 0, 'ko', markersize=8, label='Rotation Axis')
        ax2.set_xlim(-self.radius_max - 5, self.radius_max + 5)
        ax2.set_ylim(-self.radius_max - 5, self.radius_max + 5)
        ax2.legend(loc='upper right', fontsize=8)
        ax2.axis('off')

        plt.suptitle(f'Blue Star POV - 16 LEDs - Frame {frame_num}/{total_frames} ({angle:.0f}°)',
                     fontsize=14, fontweight='bold')
        plt.tight_layout()

        # Save frame
        filename = os.path.join(self.frames_dir, f'frame_{frame_num:02d}_{int(angle):03d}deg.png')
        plt.savefig(filename, dpi=150, bbox_inches='tight')
        plt.close()

        return filename

    def generate_frames(self, polar_data, num_frames=12):
        """Generate animation frames"""
        print(f"[3/4] Generating {num_frames} frames...")

        angles = np.linspace(0, 360, num_frames, endpoint=False)
        frame_files = []

        for i, angle in enumerate(angles):
            angle_int = int(angle)
            filename = self.render_frame(polar_data, angle_int, i + 1, num_frames)
            frame_files.append(filename)
            print(f"   Frame {i + 1:2d}/{num_frames}: {angle_int:3d}°")

        print(f"   ✓ Frames saved to: {self.frames_dir}")
        return frame_files

    def create_summary(self, frame_files):
        """Create summary image of all frames"""
        print("[4/4] Creating summary image...")

        try:
            fig, axes = plt.subplots(3, 4, figsize=(16, 12))
            fig.suptitle('Blue Star POV - 16 LEDs - 12 Key Frames', fontsize=16, fontweight='bold')

            angles = [0, 30, 60, 90, 120, 150, 180, 210, 240, 270, 300, 330]

            for idx, (ax, angle) in enumerate(zip(axes.flat, angles)):
                frame_file = os.path.join(self.frames_dir, f'frame_{idx + 1:02d}_{angle:03d}deg.png')
                if os.path.exists(frame_file):
                    img = plt.imread(frame_file)
                    ax.imshow(img)
                ax.set_title(f'{angle}°', fontsize=12)
                ax.axis('off')

            plt.tight_layout()
            summary_path = os.path.join(self.output_dir, 'summary_16leds_12frames.png')
            plt.savefig(summary_path, dpi=200, bbox_inches='tight')
            plt.close()
            print(f"   ✓ Summary saved to: {summary_path}")
        except Exception as e:
            print(f"   Note: Summary not created - {e}")

    def simulate_stm32_control(self, polar_data, output_file=None):
        """Generate C array file for STM32"""
        if output_file is None:
            output_file = os.path.join(self.code_output_dir, 'led_data_16leds.h')

        num_angles, num_leds, _ = polar_data.shape

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("/*\n")
            f.write(" * POV LED Display Data - Blue Star Pattern\n")
            f.write(" * 16 LEDs Version\n")
            f.write(" * Generated: {}\n".format(datetime.now().strftime("%Y-%m-%d %H:%M:%S")))
            f.write(" * Number of LEDs: {}\n".format(num_leds))
            f.write(" * Angle samples: {}\n".format(num_angles))
            f.write(" */\n\n")

            f.write("#ifndef __LED_DATA_16LEDS_H\n")
            f.write("#define __LED_DATA_16LEDS_H\n\n")
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
            f.write("};\n\n#endif // __LED_DATA_16LEDS_H\n")

        print(f"   ✓ STM32 data saved to: {output_file}")

    def run(self):
        """Run the complete simulation"""
        print("\n" + "=" * 50)
        print("Starting Blue Star POV Simulation - 16 LEDs")
        print("=" * 50)

        # Step 1: Generate star pattern
        star_img = self.generate_star_pattern()

        # Step 2: Convert to polar data
        polar_data = self.cartesian_to_polar(star_img)

        # Step 3: Generate STM32 data
        self.simulate_stm32_control(polar_data)

        # Step 4: Generate 12 frames
        frame_files = self.generate_frames(polar_data, num_frames=12)

        # Step 5: Create summary
        self.create_summary(frame_files)

        print("\n" + "=" * 50)
        print("✅ Simulation Complete!")
        print("=" * 50)
        print(f"\nGenerated files:")
        print(f"  📁 {self.images_dir}")
        print(f"     - blue_star.png")
        print(f"     - blue_star_highres.png")
        print(f"  📁 {self.frames_dir}")
        for i in range(12):
            angle = i * 30
            print(f"     - frame_{i + 1:02d}_{angle:03d}deg.png")
        print(f"  📁 {self.code_output_dir}")
        print(f"     - led_data_16leds.h")
        print(f"  📁 {self.output_dir}")
        print(f"     - summary_16leds_12frames.png")
        print("=" * 50)


def main():
    """Main function"""
    print("""
    ╔══════════════════════════════════════════╗
    ║     Blue Star POV Simulator            ║
    ║     16 LEDs - 12 Frames                   ║
    ║     Following POV_Simulator Structure    ║
    ╚══════════════════════════════════════════╝
    """)

    # Create and run simulator with 16 LEDs
    sim = BlueStarPOV(num_leds=16, radius_min=2, radius_max=30,
                        img_width=64, img_height=64)
    sim.run()


if __name__ == "__main__":
    main()