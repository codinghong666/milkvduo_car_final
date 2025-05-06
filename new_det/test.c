#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringx.h>
#include <math.h>
// 定义红外传感器状态变量
static int PWM_PIN = 4; // PWM5@GP4
int ir_sensor[8],reir_sensor[8];  // ir_sensor[0]对应最低位，ir_sensor[7]对应最高位
int err = 0;
float error = 0.0;           // 当前误差
float last_error = 0.0;      // 上一次误差
float integral = 0.0;        // 积分项
float derivative = 0.0;      // 微分项
float output = 0.0;  
float Kp = 1.0;   // 比例系数
float Ki = 0.01;  // 积分系数
float Kd = 0.1;   // 微分系数       
float P = 0.0;   // 比例项
float I = 0.0;   // 积分项
float D = 0.0;   // 微分项

int my_turn(int a[]){
    int sum=0,tot=0;
    for(int i=0;i<8;i++){
        sum+=a[i]*(-4+i+(i>=4?1:0));
        tot+=a[i];
    }
    printf("%d %d %d\n",sum,tot,sum/tot);
    return sum/tot;

}
int read_ir_sensors() {
    int i2c_fd;
    unsigned char reg_addr = 0x30;  // 要读取的寄存器地址
    unsigned char data = 0;

    // 1. 打开 I2C 总线设备
    i2c_fd = open("/dev/i2c-0", O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open /dev/i2c-0");
        exit(EXIT_FAILURE);
    }

    // 2. 设置从设备地址 (0x12)
    if (ioctl(i2c_fd, I2C_SLAVE, 0x12) < 0) {
        perror("Failed to set I2C address");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }

    // 3. 写入寄存器地址 (0x30)
    if (write(i2c_fd, &reg_addr, 1) != 1) {
        perror("Failed to write register address");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }

    // 4. 延迟等待传感器准备数据
    usleep(100000);  // 10ms

    // 5. 读取1字节数据
    if (read(i2c_fd, &data, 1) != 1) {
        perror("Failed to read data");
        close(i2c_fd);
        exit(EXIT_FAILURE);
    }

    // 6. 解析8个红外传感器状态
    for (int i = 0; i < 8; i++) {
        ir_sensor[i] = (data >> i) & 0x01;  // 从低位到高位依次存储
        reir_sensor[i] = ir_sensor[7-i];
    }
    
    // 7. 打印结果
    printf("Raw Data: 0x%02X\n", data);
    printf("IR Sensors Status:\n");
    for (int i = 7; i >= 0; i--) {
        printf("IR%d: %d  ", i + 1, ir_sensor[i]);
    }
    printf("\n");

    // 8. 关闭 I2C 设备
    close(i2c_fd);
}

int main() {
    if(wiringXSetup("milkv_duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }
    while(1){
        read_ir_sensors();
        sleep(1);
    }

    return 0;
}