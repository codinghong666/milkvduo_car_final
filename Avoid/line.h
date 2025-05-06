#ifndef LINE_H
#define LINE_H

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

// I2C device definitions
#define I2C_NUM     1
#define I2C_ADDR    0x29
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

// PWM_PIN for motor control


// PID coefficients
extern float line_Kp;   // Proportional coefficient
extern float line_Ki;   // Integral coefficient
extern float line_Kd;   // Derivative coefficient

// Initial and additional speed
extern int line_initial_speed;
extern int line_additional_speed;

/**
 * @brief Line-following function
 * 
 * @param speed The speed of the motors.
 * @param cnt The duration to run the line-following logic in seconds.
 */
void my_line(int speed, float cnt);

#endif // LINE_H