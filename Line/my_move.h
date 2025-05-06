#ifndef MY_MOVE_H
#define MY_MOVE_H

#include <stdbool.h>  // 为了使用 bool 类型

// 全局变量声明（如需在其他 .c 文件中使用）
extern int pwm_pin1, pwm_pin2;
extern int duty;
extern int ain1, ain2, bin1, bin2;
extern int T;

// 函数声明
void my_move(int speed1, bool dir1,int speed2, bool dir2);

#endif // MY_MOVE_H
