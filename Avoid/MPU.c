#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include <wiringx.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>
#define MPU6050_ADDR 0x68
#define PWR_MGMT_1   0x6B
#define GYRO_ZOUT_H  0x47

#define GYRO_SENS    131.0f
#define DT           0.01f

float yaw_angle = 0;
int i2c_fd = -1;
float gyro_z_offset = 0.0f;
/*
GP7	10 SCL
GP6 9 SDA
*/
// void cleanup() {
//     printf("释放资源...\n");
//     if (i2c_fd >= 0) {
//         close(i2c_fd);
//     }
// }

// void sigint_handler(int sig) {
//     cleanup();
//     exit(0);
// }

int16_t read_word(uint8_t reg_high) {
    unsigned char buf[2];
    // 写入要读取的寄存器地址
    if (write(i2c_fd, &reg_high, 1) != 1) {
        perror("Failed to write register address");
        return 0;
    }
    
    // 读取两个字节的数据
    if (read(i2c_fd, buf, 2) != 2) {
        perror("Failed to read data");
        return 0;
    }
    
    return (int16_t)((buf[0] << 8) | buf[1]);
}

void mpu6050_init() {
    yaw_angle = 0;
    // 1. 打开 I2C 总线设备 (注意根据实际使用修改为i2c-0或i2c-3)
    i2c_fd = open("/dev/i2c-3", O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open I2C device");
        exit(EXIT_FAILURE);
    }

    // 2. 设置从设备地址 (0x68)
    if (ioctl(i2c_fd, I2C_SLAVE, MPU6050_ADDR) < 0) {
        perror("Failed to set I2C address");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }

    // 3. 验证设备ID
    uint8_t check;
    uint8_t reg = 0x75; // WHO_AM_I寄存器
    if (write(i2c_fd, &reg, 1) != 1) {
        perror("Failed to write WHO_AM_I register");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }
    if (read(i2c_fd, &check, 1) != 1) {
        perror("Failed to read device ID");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }
    if (check != 0x68) {
        printf("错误的设备ID: 0x%02X (应为0x68)\n", check);
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }

    // 4. 复位设备
    unsigned char reset_buf[2] = {PWR_MGMT_1, 0x80};
    if (write(i2c_fd, reset_buf, 2) != 2) {
        perror("Failed to reset MPU6050");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }
    usleep(100000); // 等待复位完成

    // 5. 唤醒设备并设置时钟源
    unsigned char wake_buf[2] = {PWR_MGMT_1, 0x01}; // 使用陀螺仪X轴PLL
    if (write(i2c_fd, wake_buf, 2) != 2) {
        perror("Failed to wake up MPU6050");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }

    // 6. 配置传感器
    unsigned char config_buf[2];
    
    // 设置采样率 (50Hz)
    config_buf[0] = 0x19; // SMPLRT_DIV
    config_buf[1] = 0x13;
    write(i2c_fd, config_buf, 2);
    
    // 配置加速度计 ±2g
    config_buf[0] = 0x1C; // ACCEL_CONFIG
    config_buf[1] = 0x00;
    write(i2c_fd, config_buf, 2);
    
    // 配置陀螺仪 ±250°/s
    config_buf[0] = 0x1B; // GYRO_CONFIG
    config_buf[1] = 0x00;
    write(i2c_fd, config_buf, 2);
    
    // 启用所有传感器
    config_buf[0] = 0x6C; // PWR_MGMT_2
    config_buf[1] = 0x00;
    write(i2c_fd, config_buf, 2);

    usleep(100000); // 等待配置生效
}
void calibrate_gyro() {
    printf("正在校准陀螺仪，请保持静止...\n");
    int32_t sum = 0;
    int samples = 500; // 校准采样500次
    for (int i = 0; i < samples; i++) {
        sum += read_word(GYRO_ZOUT_H);
        usleep(1000); // 1ms
    }
    gyro_z_offset = (float)sum / samples;
    printf("陀螺仪Z轴零偏: %.2f\n", gyro_z_offset);
}
void reset(){
    yaw_angle = 0;
    gyro_z_offset = 0.0f;
    calibrate_gyro();
}
float read_mpu6050_yaw() {
    int16_t gz_raw = read_word(GYRO_ZOUT_H);
    float gz = (gz_raw-gyro_z_offset)/ GYRO_SENS;
    yaw_angle += gz * DT;
    
    // 限制角度范围在 -180° 到 180° 之间
    if (yaw_angle > 180.0f) {
        yaw_angle -= 360.0f;
    } else if (yaw_angle < -180.0f) {
        yaw_angle += 360.0f;
    }
    
    return yaw_angle;
}
