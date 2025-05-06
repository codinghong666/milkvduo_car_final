#ifndef _MY_TURN_H_
#define _MY_TURN_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringx.h>
#include <math.h>
#include <signal.h>
#include "my_move.h"
#include "mpu.h"
#include "tof.h"

#ifdef __cplusplus
extern "C" {
#endif

// I2C number used
#define I2C_NUM     1
// Device address
#define I2C_ADDR    0x29

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

extern float Kp;
extern float Ki;
extern float Kd;
extern int initial_speed;
extern int additional_speed;

void my_turn(int speed, int angle);

#ifdef __cplusplus
}
#endif

#endif // _MY_TURN_H_
