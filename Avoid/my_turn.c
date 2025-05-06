#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringx.h>
#include <math.h>
#include <signal.h>
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
float Kp = 1;   // 比例系数
float Ki = 0.1;  // 积分系数
float Kd = 0.1;   // 微分系数       
int initial_speed=15000,additional_speed=5000;
/*
float Kp = 2.5;   // 比例系数
float Ki = 0.01;  // 积分系数
float Kd = 1.0;   // 微分系数     
int initial_speed=10000,additional_speed=4000;
*/
void my_turn(int speed,int angle){ //负左 正右
    usleep(1000000);
    reset();
    usleep(1000000);
    printf("--------%f\n",read_mpu6050_yaw());
    float last_error = 0.0;      // 上一次误差
    float integral = 0.0;        // 积分项
    float derivative = 0.0;      // 微分项
    float output = 0.0;  
    float error = 0.0;           // 当前误差
    int maxval=angle/2;
    angle*=16.0/9.0;
    // right
    while(1){
        float gyro_data = read_mpu6050_yaw();
        // printf("z_date %f\n",gyro_data);
        usleep(1000);
        error = angle - gyro_data;
        integral += error;
        derivative = error - last_error;
        output = Kp * error + Ki * integral + Kd * derivative;
        // printf("my_turn_output %f\n",output);
        usleep(1000);
        last_error = error;
        if(output>0){
            output = min(output,maxval)/maxval;
            // output=max(output,0.8);
            output=1.0;
            my_move(speed*output,1,speed*output,0);
        }else{
            output = min(-output,maxval)/maxval;
            output=max(output,0.8);
            output=1.0;
            my_move(speed*output,0,speed*output,1);
        }
        if(error<0.5 && error>-0.5){
            my_move(0,1,0,1);
            break;
        }
        usleep(1000);
    }
    my_move(0,0,0,0);

}