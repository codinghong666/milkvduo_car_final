
# Smart Car System (CV1800B-based)
[Smart Car System Video](https://www.bilibili.com/video/BV1sfVbzkEiZ/?spm_id_from=333.788.top_right_bar_window_history.content.click&vd_source=c5b468db8f118c8b57a0836611cf900f)
## Hardware Specifications
- **Main Chip**: CV1800B 
- **Memory**: 64MB total (only a few MB available for runtime)
- **Sensors**:
  - MPU6050 (IMU for angle detection)
  - VL53L0X (Time-of-Flight distance sensor)
  - 8-channel infrared array
- **Constraints**: Limited resources require all onboard code to be written in C/C++ (Python not feasible)

## Core Capabilities
1. **Precision Movement**:
   - Accurate straight-line navigation even with imperfect wheels
   - Precise 90° (π/2) turns

2. **Computer Vision** (Offloaded to external computer):
   - Object/person tracking via YOLOv5
   - Note: Run externally due to hardware limitations

3. **Line Following**: Robust black line tracking

## Code Modules
- **MPU**: MPU6050 interface code
- **Line**: Line-following implementation
- **Det**: Infrared sensor processing
- **Track**: Primary line tracking logic
- **Spdsen** (WIP): Speed sensor module
- **TDP**: Test module

## Limitations
- No onboard computer vision processing
- Memory constraints require optimized C/C++ code
- Some modules still in development



