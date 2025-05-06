#ifndef MY_SENSOR_H
#define MY_SENSOR_H

// 定义外部传感器数组（如果别的文件要访问的话）
extern int ir_sensor[8];
extern int reir_sensor[8];

// 函数声明
int read_ir_sensors();        // 读取传感器并返回偏移值
int my_turn(int a[]);         // 计算偏移量

#endif // MY_SENSOR_H
