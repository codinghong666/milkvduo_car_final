// mpu6050.h

#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

// MPU6050 I2C 地址和寄存器定义
#define MPU6050_ADDR 0x68
#define PWR_MGMT_1   0x6B
#define GYRO_ZOUT_H  0x47

#define GYRO_SENS    131.0f
#define DT           0.01f

#ifdef __cplusplus
extern "C" {
#endif

extern float yaw_angle;
extern int i2c_fd;

// 初始化 MPU6050 传感器
void mpu6050_init();

// 读取陀螺仪 Z 轴角速度并更新 yaw 角
float read_mpu6050_yaw();

void calibrate_gyro();
void reset();
// 信号处理函数（可用于 Ctrl+C 时释放资源）

#ifdef __cplusplus
}
#endif

#endif // MPU6050_H