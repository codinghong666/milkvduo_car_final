#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringx.h>
#include <math.h>
#include"my_move.h"
#include"mpu.h"
#include <signal.h>
#include "tof.h"
#include "my_turn.h"
#include "line.h"
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
float error = 0.0;           // 当前误差
float last_error = 0.0;      // 上一次误差
float integral = 0.0;        // 积分项
float derivative = 0.0;      // 微分项
float output = 0.0;     
float P = 0.0, I = 0.0, D = 0.0;  
/*
float Kp = 2.5;   // 比例系数
float Ki = 0.01;  // 积分系数
float Kd = 1.0;   // 微分系数     
int initial_speed=10000,additional_speed=4000;
*/
void cleanup(void) {
    printf("程序退出，执行清理操作...\n");
    if (i2c_fd >= 0) {
        close(i2c_fd);
    }
    digitalWrite(ain1, LOW);
    digitalWrite(ain2, LOW);
    digitalWrite(bin1, LOW);
    digitalWrite(bin2, LOW);
    // 关闭PWM，释放资源等
}

void sigint_handler(int sig) {
    cleanup();
    exit(0);
}
int main() {
    signal(SIGINT, sigint_handler); 
    if(wiringXSetup("milkv_duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }
    pinMode(ain1, PINMODE_OUTPUT);
    pinMode(ain2, PINMODE_OUTPUT);
    pinMode(bin1, PINMODE_OUTPUT);
    pinMode(bin2, PINMODE_OUTPUT);
    mpu6050_init();
    calibrate_gyro();
    tofInit(I2C_NUM, I2C_ADDR, 0);
    // my_turn(initial_speed,90);
    // my_turn(initial_speed,-90);
    while(1){
        my_move(initial_speed,1,initial_speed,1);
        int dis=tofReadDistance();
        printf("%dmm\n",dis);
        if(dis<=150){
            my_move(0,0,0,0);
            my_turn(initial_speed,90);
            usleep(10000);
            // my_move(initial_speed,1,initial_speed,1);
            my_line(initial_speed,2.0);
            usleep(500000);
            my_turn(initial_speed,-90);            
            usleep(10000);
            // my_move(initial_speed,1,initial_speed,1);

            my_line(initial_speed,2.0);
            usleep(2000000);
            // float data_z=read_mpu6050_yaw();
            // printf("%f\n",data_z);
            my_turn(initial_speed,-95);
            usleep(10000);
            // my_move(initial_speed,1,initial_speed,1);

            my_line(initial_speed,2.0);
            usleep(500000);
            my_turn(initial_speed,90);
            printf("finished");
            my_move(0,0,0,0);
            usleep(1000000);
            break;
        }
        usleep(1000);
    }
    // my_line(15000,2.0);
    my_move(0,0,0,0);
    cleanup();
    return 0;
}