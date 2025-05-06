// #include <wiringx.h>

// #include <stdio.h>
// #include <unistd.h>
// #include <wiringx.h>
// #include <stdbool.h>  // Add this line
// int T=100000,duty = 30000;
// int pwm_pin1 = 4,pwm_pin2=5;
// int ain1 = 14, ain2 = 15,bin1=16,bin2=17;
// void my_move(int speed,bool dir){
//     // 0后退 1前进
//     if(dir){
//         printf("dir = 1\n");
//         digitalWrite(bin1,LOW);
//         digitalWrite(bin2,HIGH);
//         digitalWrite(ain1, LOW);   
//         digitalWrite(ain2, HIGH);  
//     }else{
//         printf("dir = 0\n");
//         digitalWrite(bin1,HIGH);
//         digitalWrite(bin2,LOW);
//         digitalWrite(ain1, HIGH);   
//         digitalWrite(ain2, LOW);         
//     }
//     // printf("bin1: %d, bin2: %d, ain1: %d, ain2: %d\n", 
//     //     digitalRead(bin1), digitalRead(bin2), 
//     //     digitalRead(ain1), digitalRead(ain2));
//     wiringXPWMSetPeriod(pwm_pin1, T);  
//     wiringXPWMSetDuty(pwm_pin1, speed); 
//     wiringXPWMSetPolarity(pwm_pin1, 0);   // 极性（0 或 1）
//     wiringXPWMEnable(pwm_pin1, 1);        // 启用 PWM
//     wiringXPWMSetPeriod(pwm_pin2,T);
//     wiringXPWMSetDuty(pwm_pin2,speed);
//     wiringXPWMSetPolarity(pwm_pin2, 0);   // 极性（0 或 1）
//     wiringXPWMEnable(pwm_pin2, 1);        // 启用 PWM
//     usleep(1000);
//     return ;
// }
// int main() {
//    // 初始化 wiringX（确认设备名正确）
//    if(wiringXSetup("milkv_duo", NULL) == -1) {
//        printf("wiringX setup failed\n");
//        return 1;
//    }
//    pinMode(ain1, PINMODE_OUTPUT);
//    pinMode(ain2, PINMODE_OUTPUT);
//    pinMode(bin1,PINMODE_OUTPUT);
//    pinMode(bin2,PINMODE_OUTPUT);
//    my_move(30000,0);
//    sleep(2);
//    my_move(100000,0);
//    // 保持程序运行（防止退出后 PWM 停止）
//    sleep(2);
//    digitalWrite(bin1,LOW);
//    digitalWrite(bin2,LOW);
//    digitalWrite(ain1, LOW);   
//    digitalWrite(ain2, LOW);     
//    return 0;
// }
// #include <wiringx.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdbool.h>

// // 定义参数
// int T = 100000, duty = 30000;
// int pwm_pin1 = 4, pwm_pin2 = 5;
// int ain1 = 14, ain2 = 15, bin1 = 16, bin2 = 17;
// int encoder_a = 26, encoder_b = 27;  // 假设编码器 A、B 信号连接到 GPIO 12 和 GPIO 13
// volatile int pulseCount = 0;  // 电机转动的圈数
// volatile bool lastA = false;
// volatile bool lastB = false;

// // 编码器信号变化的中断服务程序
// void encoderISR() {
//     bool currentA = digitalRead(encoder_a);  // 读取 A 相的当前状态
//     bool currentB = digitalRead(encoder_b);  // 读取 B 相的当前状态

//     // 判断旋转方向
//     if (currentA != lastA) {
//         // A 信号变化时，检查 B 信号来判断旋转方向
//         if (currentB != currentA) {
//             pulseCount++;  // 顺时针旋转
//         } else {
//             pulseCount--;  // 逆时针旋转
//         }
//     }

//     lastA = currentA;  // 更新 A 相的状态
//     lastB = currentB;  // 更新 B 相的状态
// }

// // 电机驱动控制
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

// int main() {
//     // 初始化 wiringX（确认设备名正确）
//     if (wiringXSetup("milkv_duo", NULL) == -1) {
//         printf("wiringX setup failed\n");
//         return 1;
//     }

//     // 设置引脚模式
//     pinMode(ain1, PINMODE_OUTPUT);
//     pinMode(ain2, PINMODE_OUTPUT);
//     pinMode(bin1, PINMODE_OUTPUT);
//     pinMode(bin2, PINMODE_OUTPUT);
//     pinMode(encoder_a, PINMODE_INPUT);
//     pinMode(encoder_b, PINMODE_INPUT);

//     // 设置编码器中断
//     wiringXISR(encoder_a, ISR_MODE_RISING);  // 在 A 相上升沿触发中断
//     wiringXISR(encoder_b, ISR_MODE_RISING);  // 在 B 相上升沿触发中断

//     // 启动电机
//     my_move(30000, 0);  // 逆时针旋转
//     sleep(2);

//     my_move(100000, 0);  // 逆时针旋转
//     sleep(2);

//     // 输出转动的圈数
//     printf("当前旋转圈数: %d\n", pulseCount);

//     // 停止电机
//     digitalWrite(bin1, LOW);
//     digitalWrite(bin2, LOW);
//     digitalWrite(ain1, LOW);
//     digitalWrite(ain2, LOW);

//     return 0;
// }


// #include <wiringx.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdbool.h>

// // 定义参数
// int T = 100000, duty = 30000;
// int pwm_pin1 = 4, pwm_pin2 = 5;
// int ain1 = 14, ain2 = 15, bin1 = 16, bin2 = 17;
// int encoder_a = 26, encoder_b = 27;  // 假设编码器 A、B 信号连接到 GPIO 26 和 GPIO 27
// volatile int pulseCount = 0;  // 电机转动的圈数
// volatile bool lastA = false;
// volatile bool lastB = false;

// // 编码器信号变化的中断服务程序
// void encoderISR() {
//     bool currentA = digitalRead(encoder_a);  // 读取 A 相的当前状态
//     bool currentB = digitalRead(encoder_b);  // 读取 B 相的当前状态

//     // 判断旋转方向
//     if (currentA != lastA) {
//         // A 信号变化时，检查 B 信号来判断旋转方向
//         if (currentB != currentA) {
//             pulseCount++;  // 顺时针旋转
//         } else {
//             pulseCount--;  // 逆时针旋转
//         }
//     }

//     lastA = currentA;  // 更新 A 相的状态
//     lastB = currentB;  // 更新 B 相的状态
// }

// // 电机驱动控制
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

// int main() {
//     // 初始化 wiringX（确认设备名正确）
//     if (wiringXSetup("milkv_duo", NULL) == -1) {
//         printf("wiringX setup failed\n");
//         return 1;
//     }

//     // 设置引脚模式
//     pinMode(ain1, PINMODE_OUTPUT);
//     pinMode(ain2, PINMODE_OUTPUT);
//     pinMode(bin1, PINMODE_OUTPUT);
//     pinMode(bin2, PINMODE_OUTPUT);
//     pinMode(encoder_a, PINMODE_INPUT);
//     pinMode(encoder_b, PINMODE_INPUT);

//     // 设置编码器中断
//     wiringXISR(encoder_a, ISR_MODE_RISING, encoderISR);  // 在 A 相上升沿触发中断，调用 encoderISR
//     wiringXISR(encoder_b, ISR_MODE_RISING, encoderISR);  // 在 B 相上升沿触发中断，调用 encoderISR

//     // 启动电机
//     my_move(30000, 0);  // 逆时针旋转
//     sleep(2);

//     my_move(100000, 0);  // 逆时针旋转
//     sleep(2);

//     // 输出转动的圈数
//     printf("当前旋转圈数: %d\n", pulseCount);

//     // 停止电机
//     digitalWrite(bin1, LOW);
//     digitalWrite(bin2, LOW);
//     digitalWrite(ain1, LOW);
//     digitalWrite(ain2, LOW);

//     return 0;
// }

// #include <wiringx.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdbool.h>

// // 定义参数
// int T = 100000, duty = 30000;
// int pwm_pin1 = 4, pwm_pin2 = 5;
// int ain1 = 14, ain2 = 15, bin1 = 16, bin2 = 17;
// int encoder_a = 26, encoder_b = 27;  // 假设编码器 A、B 信号连接到 GPIO 26 和 GPIO 27
// volatile int pulseCount = 0;  // 电机转动的圈数
// volatile bool lastA = false;
// volatile bool lastB = false;

// // 编码器信号变化的中断服务程序
// void encoderISR() {
//     printf("ininin\n");
//     bool currentA = digitalRead(encoder_a);  // 读取 A 相的当前状态
//     bool currentB = digitalRead(encoder_b);  // 读取 B 相的当前状态

//     // 判断旋转方向
//     if (currentA != lastA) {
//         // A 信号变化时，检查 B 信号来判断旋转方向
//         if (currentB != currentA) {
//             pulseCount++;  // 顺时针旋转
//         } else {
//             pulseCount--;  // 逆时针旋转
//         }
//     }

//     lastA = currentA;  // 更新 A 相的状态
//     lastB = currentB;  // 更新 B 相的状态
// }

// // 电机驱动控制
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

// int main() {
//     // 初始化 wiringX（确认设备名正确）
//     if (wiringXSetup("milkv_duo", NULL) == -1) {
//         printf("wiringX setup failed\n");
//         return 1;
//     }

//     // 设置引脚模式
//     pinMode(ain1, PINMODE_OUTPUT);
//     pinMode(ain2, PINMODE_OUTPUT);
//     pinMode(bin1, PINMODE_OUTPUT);
//     pinMode(bin2, PINMODE_OUTPUT);
//     pinMode(encoder_a, PINMODE_INPUT);
//     pinMode(encoder_b, PINMODE_INPUT);

//     // 设置编码器中断（不传递回调函数，只传递引脚和中断模式）
//     wiringXISR(encoder_a, ISR_MODE_RISING);  // 在 A 相上升沿触发中断
//     wiringXISR(encoder_b, ISR_MODE_RISING);  // 在 B 相上升沿触发中断

//     // 启动电机
//     my_move(30000, 0);  // 逆时针旋转
//     sleep(2);

//     my_move(100000, 0);  // 逆时针旋转
//     sleep(2);

//     // 输出转动的圈数
//     printf("当前旋转圈数: %d\n", pulseCount);

//     // 停止电机
//     digitalWrite(bin1, LOW);
//     digitalWrite(bin2, LOW);
//     digitalWrite(ain1, LOW);
//     digitalWrite(ain2, LOW);

//     return 0;
// }


// #include <wiringx.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdbool.h>

// // 定义参数
// int T = 100000, duty = 30000;
// int pwm_pin1 = 4, pwm_pin2 = 5;
// int ain1 = 14, ain2 = 15, bin1 = 16, bin2 = 17;
// int encoder_a = 26, encoder_b = 27;  // 假设编码器 A、B 信号连接到 GPIO 26 和 GPIO 27
// volatile int pulseCount = 0;  // 电机转动的圈数
// volatile bool lastA = false;
// volatile bool lastB = false;

// // 电机驱动控制
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

// int main() {
//     // 初始化 wiringX（确认设备名正确）
//     if (wiringXSetup("milkv_duo", NULL) == -1) {
//         printf("wiringX setup failed\n");
//         return 1;
//     }

//     // 设置引脚模式
//     pinMode(ain1, PINMODE_OUTPUT);
//     pinMode(ain2, PINMODE_OUTPUT);
//     pinMode(bin1, PINMODE_OUTPUT);
//     pinMode(bin2, PINMODE_OUTPUT);
//     pinMode(encoder_a, PINMODE_INPUT);
//     pinMode(encoder_b, PINMODE_INPUT);

//     // 主循环，轮询读取编码器信号
//     while (1) {
//         bool currentA = digitalRead(encoder_a);  // 读取 A 相的当前状态
//         bool currentB = digitalRead(encoder_b);  // 读取 B 相的当前状态

//         // 判断旋转方向
//         if (currentA != lastA) {
//             // A 信号变化时，检查 B 信号来判断旋转方向
//             if (currentB != currentA) {
//                 pulseCount++;  // 顺时针旋转
//             } else {
//                 pulseCount--;  // 逆时针旋转
//             }
//         }

//         lastA = currentA;  // 更新 A 相的状态
//         lastB = currentB;  // 更新 B 相的状态

//         // 每隔一段时间输出圈数
//         printf("当前旋转圈数: %d\n", pulseCount);
//         usleep(1);  // 每100ms打印一次结果

//         // 停止条件，可以根据实际需求设置
//         // if (pulseCount >= 100) {
//         //     break;  // 假设转动了100圈就退出
//         // }
//     }

//     // 停止电机
//     digitalWrite(bin1, LOW);
//     digitalWrite(bin2, LOW);
//     digitalWrite(ain1, LOW);
//     digitalWrite(ain2, LOW);

//     return 0;
// }


// #include <wiringx.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdbool.h>
// #include <stdlib.h>

// // 引脚定义
// #define PWM_PIN1 4
// #define PWM_PIN2 5
// #define AIN1 14
// #define AIN2 15
// #define BIN1 16
// #define BIN2 17
// #define ENCODER_A 26
// #define ENCODER_B 27

// // 编码器参数
// const int PPR = 13;                      // 编码器线数
// const int PULSES_PER_REV = PPR * 4;      // 四倍频后每转脉冲数
// volatile int pulseCount = 0;             // 脉冲计数器
// volatile bool lastA = false, lastB = false; // 编码器状态缓存

// // 正交编码器解码表（四倍频）
// const int8_t TRANSITIONS[] = {
//     0,  // 00→00
//     +1, // 00→01 (顺时针)
//     -1, // 00→10 (逆时针)
//     0,  // 00→11
//     -1, // 01→00 (逆时针)
//     0,  // 01→01
//     0,  // 01→10
//     +1, // 01→11 (顺时针)
//     +1, // 10→00 (顺时针)
//     0,  // 10→01
//     0,  // 10→10
//     -1, // 10→11 (逆时针)
//     0,  // 11→00
//     -1, // 11→01 (逆时针)
//     +1, // 11→10 (顺时针)
//     0   // 11→11
// };

// // 中断服务程序
// void encoderISR() {
//     bool currentA = digitalRead(ENCODER_A);
//     bool currentB = digitalRead(ENCODER_B);
    
//     // 计算状态变化
//     uint8_t oldState = (lastA << 1) | lastB;
//     uint8_t newState = (currentA << 1) | currentB;
//     int8_t transition = (oldState << 2) | newState;
    
//     if(transition < 16) {
//         pulseCount += TRANSITIONS[transition];
//     }
    
//     // 更新状态缓存
//     lastA = currentA;
//     lastB = currentB;
// }

// // 电机控制函数
// void motor_control(int speed, bool direction) {
//     // 设置电机方向
//     digitalWrite(AIN1, direction ? LOW : HIGH);
//     digitalWrite(AIN2, direction ? HIGH : LOW);
//     digitalWrite(BIN1, direction ? LOW : HIGH);
//     digitalWrite(BIN2, direction ? HIGH : LOW);

//     // 配置PWM
//     wiringXPWMSetPeriod(PWM_PIN1, 100000);
//     wiringXPWMSetDuty(PWM_PIN1, speed);
//     wiringXPWMEnable(PWM_PIN1, 1);

//     wiringXPWMSetPeriod(PWM_PIN2, 100000);
//     wiringXPWMSetDuty(PWM_PIN2, speed);
//     wiringXPWMEnable(PWM_PIN2, 1);
// }

// int main() {
//     // 初始化wiringX
//     if(wiringXSetup("milkv_duo", NULL) == -1) {
//         fprintf(stderr, "wiringX初始化失败\n");
//         return 1;
//     }

//     // 初始化GPIO
//     pinMode(AIN1, PINMODE_OUTPUT);
//     pinMode(AIN2, PINMODE_OUTPUT);
//     pinMode(BIN1, PINMODE_OUTPUT);
//     pinMode(BIN2, PINMODE_OUTPUT);
//     pinMode(PWM_PIN1, PINMODE_PWM);
//     pinMode(PWM_PIN2, PINMODE_PWM);
    
//     // 初始化编码器输入
//     pinMode(ENCODER_A, PINMODE_INPUT);
//     pinMode(ENCODER_B, PINMODE_INPUT);
//     lastA = digitalRead(ENCODER_A);
//     lastB = digitalRead(ENCODER_B);

//     // 注册中断处理
//     if(wiringXISR(ENCODER_A, INT_EDGE_BOTH, encoderISR) < 0 ||
//        wiringXISR(ENCODER_B, INT_EDGE_BOTH, encoderISR) < 0) {
//         fprintf(stderr, "中断注册失败\n");
//         return 1;
//     }

//     // 启动电机
//     motor_control(30000, true);  // 以50%占空比正转
    
//     // 主控制循环
//     const int TARGET_REVS = 10;
//     while(1) {
//         int current_revs = pulseCount / PULSES_PER_REV;
//         printf("当前圈数: %d (脉冲数: %d)\n", current_revs, pulseCount);
        
//         if(abs(current_revs) >= TARGET_REVS) {
//             break;
//         }
//         usleep(100000);  // 100ms更新一次
//     }

//     // 停止电机
//     motor_control(0, true);  // 占空比设为0停止
//     printf("已到达目标圈数\n");
    
//     return 0;
// }


#include <wiringx.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
typedef unsigned char uint8_t;
typedef signed char int8_t;


// 引脚定义
#define PWM_PIN1 4
#define PWM_PIN2 5
#define AIN1 14
#define AIN2 15
#define BIN1 16
#define BIN2 17
#define ENCODER_A 26
#define ENCODER_B 27

// 编码器参数
const int PPR = 13;                      // 编码器线数
const int PULSES_PER_REV = PPR * 4;      // 四倍频后每转脉冲数
volatile int pulseCount = 0,current_revs=0;             // 脉冲计数器
volatile bool lastA = false, lastB = false; // 编码器状态缓存
const int8_t TRANSITIONS[16] = {
    0,  // 00 -> 00
    +1, // 00 -> 01
    -1, // 00 -> 10
    0,  // 00 -> 11
    -1, // 01 -> 00
    0,  // 01 -> 01
    0,  // 01 -> 10
    +1, // 01 -> 11
    +1, // 10 -> 00
    0,  // 10 -> 01
    0,  // 10 -> 10
    -1, // 10 -> 11
    0,  // 11 -> 00
    -1, // 11 -> 01
    +1, // 11 -> 10
    0   // 11 -> 11
};
int abs(int x){
    return x>0?x:-x;
}
void cleanup(){
    printf("%d\n",current_revs);
}
void sigint_handler(int sig) {
    cleanup();
    exit(0);
}

// 中断服务程序
void encoderISR() {
    bool currentA = digitalRead(ENCODER_A);
    bool currentB = digitalRead(ENCODER_B);

    // 计算状态变化
    uint8_t oldState = (lastA << 1) | lastB;
    uint8_t newState = (currentA << 1) | currentB;
    int8_t transition = (oldState << 2) | newState;

    if (transition < 16) {
        pulseCount += TRANSITIONS[transition];
    }

    // 更新状态缓存
    lastA = currentA;
    lastB = currentB;
    // printf("in");
}

// 主函数
int main() {
    // 初始化wiringX
    signal(SIGINT, sigint_handler); 
    if (wiringXSetup("milkv_duo", NULL) == -1) {
        fprintf(stderr, "wiringX初始化失败\n");
        return 1;
    }
    pinMode(AIN1, PINMODE_OUTPUT);
    pinMode(AIN2, PINMODE_OUTPUT);
    pinMode(BIN1, PINMODE_OUTPUT);
    pinMode(BIN2, PINMODE_OUTPUT);
    // pinMode(PWM_PIN1, PINMODE_PWM);
    // pinMode(PWM_PIN2, PINMODE_PWM);
    // 初始化GPIO
    pinMode(ENCODER_A, PINMODE_INPUT);
    pinMode(ENCODER_B, PINMODE_INPUT);
    lastA = digitalRead(ENCODER_A);
    lastB = digitalRead(ENCODER_B);

    // 注册中断处理
    // if (wiringXISR(ENCODER_A, ISR_MODE_BOTH) < 0 ||
    //     wiringXISR(ENCODER_B, ISR_MODE_BOTH) < 0) {
    //     fprintf(stderr, "中断注册失败\n");
    //     return 1;
    // }

    // 主控制循环
    while (1) {
        current_revs = pulseCount / (PULSES_PER_REV);
        // printf("当前圈数: %d (脉冲数: %d)\n", current_revs, pulseCount);
        encoderISR();
        // if (abs(current_revs) >= TARGET_REVS) {
        //     break;
        // }
        usleep(100);  // 100ms更新一次
    }

    printf("已到达目标圈数\n");
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <stdatomic.h>
// #include <wiringx.h>

// // 硬件引脚定义（根据实际接线修改）
// #define ENCODER_A 14   // GPIO14（物理引脚号）
// #define ENCODER_B 15   // GPIO15
// #define MOTOR_DIR 16
// #define MOTOR_PUL 17
// #define MOTOR_ENA 18

// volatile atomic_long encoder_count = 0;

// // 电机控制结构体
// typedef struct {
//     int dir_pin;
//     int pul_pin;
//     int ena_pin;
//     long step_time;
//     long current_step;
//     long target_step;
// } Motor;

// void motor_init(Motor* motor, int dir, int pul, int ena) {
//     motor->dir_pin = dir;
//     motor->pul_pin = pul;
//     motor->ena_pin = ena;
//     motor->step_time = 1000;
//     motor->current_step = 0;
//     motor->target_step = 0;

//     wiringXGPIO(pinMode, motor->dir_pin, PINMODE_OUTPUT);
//     wiringXGPIO(pinMode, motor->pul_pin, PINMODE_OUTPUT);
//     wiringXGPIO(pinMode, motor->ena_pin, PINMODE_OUTPUT);

//     wiringXGPIO(digitalWrite, motor->dir_pin, LOW);
//     wiringXGPIO(digitalWrite, motor->pul_pin, LOW);
//     wiringXGPIO(digitalWrite, motor->ena_pin, LOW);
// }

// int motor_is_running(Motor* motor) {
//     return motor->current_step != motor->target_step;
// }

// void motor_run_step(Motor* motor, int direction) {
//     if(direction) {
//         // 正向脉冲
//         wiringXGPIO(digitalWrite, motor->pul_pin, HIGH);
//         usleep(motor->step_time / 2);
//         wiringXGPIO(digitalWrite, motor->pul_pin, LOW);
//         usleep(motor->step_time / 2);
//         motor->current_step++;
//     } else {
//         // 反向脉冲
//         wiringXGPIO(digitalWrite, motor->dir_pin, HIGH);
//         usleep(motor->step_time / 2);
//         wiringXGPIO(digitalWrite, motor->dir_pin, LOW);
//         usleep(motor->step_time / 2);
//         motor->current_step--;
//     }
// }

// void motor_run_to(Motor* motor, long target, long step_time) {
//     motor->target_step = target;
//     motor->step_time = step_time;
    
//     while(motor_is_running(motor)) {
//         motor_run_step(motor, motor->current_step < target);
//     }
    
//     motor->current_step = 0;
// }

// // 编码器中断回调
// void encoder_callback(int pin, void *data) {
//     static int last_state = 0;
//     int current_a = wiringXGPIO(digitalRead, ENCODER_A);
//     int current_b = wiringXGPIO(digitalRead, ENCODER_B);
    
//     if(current_a != last_state) {
//         if(current_a == current_b) {
//             atomic_fetch_add(&encoder_count, 1);
//         } else {
//             atomic_fetch_sub(&encoder_count, 1);
//         }
//         last_state = current_a;
//     }
// }

// int main() {
//     // 初始化WiringX
//     if(wiringXSetup("duo", NULL) == -1) {
//         printf("WiringX setup failed!\n");
//         return -1;
//     }

//     // 初始化编码器
//     wiringXGPIO(pinMode, ENCODER_A, PINMODE_INPUT);
//     wiringXGPIO(pinMode, ENCODER_B, PINMODE_INPUT);
//     wiringXGPIO(pullUpDnControl, ENCODER_A, PUD_UP);
//     wiringXGPIO(pullUpDnControl, ENCODER_B, PUD_UP);
    
//     // 注册编码器中断
//     if(wiringXISR(ENCODER_A, ISR_MODE_BOTH, encoder_callback, NULL) < 0) {
//         printf("Failed to register ISR for encoder\n");
//         return -1;
//     }

//     // 初始化电机
//     Motor my_motor;
//     motor_init(&my_motor, MOTOR_DIR, MOTOR_PUL, MOTOR_ENA);

//     while(1) {
//         // 运行电机2圈（假设800步=2圈）
//         motor_run_to(&my_motor, 800, 5000);
        
//         // 获取并显示编码器数据
//         long pulses = atomic_load(&encoder_count);
//         float rotations = pulses / 400.0f; // 假设每圈200脉冲（4倍频后）
        
//         printf("Encoder: %ld pulses, %.2f rotations\n", pulses, rotations);
        
//         // 重置计数器
//         atomic_store(&encoder_count, 0);
        
//         sleep(1);
//     }

//     return 0;
// }
