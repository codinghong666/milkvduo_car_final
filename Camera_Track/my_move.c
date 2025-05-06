#include <stdio.h>
#include <unistd.h>
#include <wiringx.h>
#include <stdbool.h>
int pwm_pin1 = 4,pwm_pin2=5;
int duty = 30000;
int ain1 = 14, ain2 = 15,bin1=16,bin2=17;
int T=100000;
// void my_move(int speed, bool dir) {
//     // 0 后退 1 前进
//     if (dir) {
//         printf("dir = 1\n");
//         digitalWrite(bin1, LOW);
//         digitalWrite(bin2, HIGH);
//         digitalWrite(ain1, LOW);
//         digitalWrite(ain2, HIGH);
//     } else {
//         printf("dir = 0\n");
//         digitalWrite(bin1, HIGH);
//         digitalWrite(bin2, LOW);
//         digitalWrite(ain1, HIGH);
//         digitalWrite(ain2, LOW);
//     }

//     wiringXPWMSetPeriod(pwm_pin1, T);
//     wiringXPWMSetDuty(pwm_pin1, speed);
//     wiringXPWMSetPolarity(pwm_pin1, 0);  // 极性（0 或 1）
//     wiringXPWMEnable(pwm_pin1, 1);       // 启用 PWM

//     wiringXPWMSetPeriod(pwm_pin2, T);
//     wiringXPWMSetDuty(pwm_pin2, speed);
//     wiringXPWMSetPolarity(pwm_pin2, 0);  // 极性（0 或 1）
//     wiringXPWMEnable(pwm_pin2, 1);       // 启用 PWM

//     usleep(1000);
//     return;
// }


void my_move(int speed1, bool dir1, int speed2, bool dir2) {
    // 0 后退 1 前进
    if (dir1) {
        // printf("dir1 = 1\n");
        digitalWrite(ain1,LOW);
        digitalWrite(ain2, HIGH);
    } else {
        // printf("dir1 = 0\n");
        digitalWrite(ain1, HIGH);
        digitalWrite(ain2, LOW);
    }
    if (dir2) {
        // printf("dir = 1\n");
        digitalWrite(bin1, LOW);
        digitalWrite(bin2, HIGH);
    } else {
        // printf("dir = 0\n");
        digitalWrite(bin1, HIGH);
        digitalWrite(bin2, LOW);
    }
    wiringXPWMSetPeriod(pwm_pin1, T);
    wiringXPWMSetDuty(pwm_pin1, speed1);
    wiringXPWMSetPolarity(pwm_pin1, 0);  // 极性（0 或 1）
    wiringXPWMEnable(pwm_pin1, 1);       // 启用 PWM

    wiringXPWMSetPeriod(pwm_pin2, T);
    wiringXPWMSetDuty(pwm_pin2, speed2);
    wiringXPWMSetPolarity(pwm_pin2, 0);  // 极性（0 或 1）
    wiringXPWMEnable(pwm_pin2, 1);       // 启用 PWM

    usleep(1000);
    return;
}