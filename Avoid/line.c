#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringx.h>
#include <math.h>
#include <signal.h>
#include <time.h> 
#include"my_move.h"
#include"mpu.h"
#include "tof.h"

/* 
   I2C1_SDA -> SDA on VL53L0X Moudle
   I2C1_SCL -> SCL on VL53L0X Moudle
   3.3v -> VCC on VL53L0X Moudle
   GND -> GND on VL53L0X Moudle
*/

// 使用到的I2C编号 The I2C number used
#define I2C_NUM     1

// 器件地址 device address
#define I2C_ADDR    0x29
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
static int PWM_PIN = 6; // 9
float line_Kp = 1;   // 比例系数
float line_Ki = 0.1;  // 积分系数
float line_Kd = 0.1;   // 微分系数       
int line_initial_speed=15000,line_additional_speed=2500;
/*
float Kp = 2.5;   // 比例系数
float Ki = 0.01;  // 积分系数
float Kd = 1.0;   // 微分系数     
int initial_speed=10000,additional_speed=4000;
*/
void my_line(int speed,float cnt){ //负左 正右
    line_initial_speed=speed;
    line_additional_speed=speed/8;
    usleep(100000);
    reset();
    usleep(100000);
    printf("--------%f\n",read_mpu6050_yaw());
    float line_last_error = 0.0;      // 上一次误差
    float line_integral = 0.0;        // 积分项
    float line_derivative = 0.0;      // 微分项
    float line_output = 0.0;  
    float line_error = 0.0;           // 当前误差
    // right
    time_t start_time = time(NULL);
    while(1){
        if (difftime(time(NULL), start_time) > cnt) {
            break;
        }
        float line_gyro_data = read_mpu6050_yaw();
        // printf("z_date %f\n",line_gyro_data);
        usleep(1000);
        line_error = - line_gyro_data;
        line_integral += line_error;
        line_derivative = line_error - line_last_error;
        line_output = line_Kp * line_error + line_Ki * line_integral + line_Kd * line_derivative;
        // printf("my_turn_output %f\n",line_output);
        usleep(1000);
        line_last_error = line_error;
        if(line_output>0){
            my_move(line_initial_speed+line_additional_speed,1,line_initial_speed-line_additional_speed,1);
        }else{
            my_move(line_initial_speed-line_additional_speed,1,line_initial_speed+line_additional_speed,1);
        }
        usleep(1000);
    }
    my_move(0,0,0,0);

}