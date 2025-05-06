// spd_sensor.h

#ifndef SPD_SENSOR_H
#define SPD_SENSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringx.h>
#include <math.h>
#include <signal.h>

// 定义结构体 Spd
struct Spd {
    int arg;    // 速度值
    int flag;   // 状态标识
};

// 函数原型
struct Spd my_spd_sensor(int a[]);           // 计算速度传感器数据
struct Spd read_spd_sensors(void);           // 读取速度传感器数据

#endif // SPD_SENSOR_H