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
void LineWalking(int x1,int x2,int x3,int x4,int x5,int x6,int x7,int x8)
{
    
    
    // 1. 读取红外传感器状态
    // 2. 调用 LineWalking 函数进行处理
    // 3. 根据 err 的值进行相应的操作
    
    // 这里假设读取到的红外传感器状态为 x1, x2, ..., x8
    // 调用 LineWalking 函数进行处理
  if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
	{
		err = -1;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
	{
		err = -2;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1101 1111
	{
		err = -2;
	}
	
	else if(x1 == 1 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1011 1111
	{
		err = -3;
	}
	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
	{
		err = -3;
	}
		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
	{
		err = -4;  
	}
	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
	{
		err = -4; 
	}

	
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
	{
		err = 1;
	} 
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
	{
		err = 2;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 1011
	{
		err = 2;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
	{
		err = 3;
	}
	
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 1) // 1111 1101
	{
		err = 3;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
	{
		err = 4; 
	}
		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
	{
		err = 4; 
	}
	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //直走
	{
		err = 0;
	}
	printf("\nerr = %d\n", err);
	//剩下的就保持上一个状态	
}
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
    usleep(10000);  // 10ms

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
    return my_turn(reir_sensor);
}

int main() {
    if(wiringXSetup("milkv_duo", NULL) == -1) {
        wiringXGC();
        return -1;
    }

    wiringXPWMSetPeriod(PWM_PIN, 20000000);  // 20ms
    wiringXPWMSetDuty(PWM_PIN, 1500000);     // 1.5ms stop
    wiringXPWMSetPolarity(PWM_PIN, 0);       // 0-normal, 1-inversed
    wiringXPWMEnable(PWM_PIN, 1);            // 1-enable, 0-disable
    printf("Reading IR sensors from I2C device 0x12, register 0x30...\n");
    while(1){
        error=read_ir_sensors();
        P=Kp*error;
        I=Ki*integral;
        D=Kd*derivative;
        output=P+I+D;
        printf("P: %f, I: %f, D: %f, Output: %f\n", P, I, D, output);
/*
        0:1500000
        左：240000
        右：900000
        [0,4]-[1500000,900000]
        y=-150000x+1500000
        [0,4]-[1500000,240000]
        y=225000x+1500000
*/
        if(output>0){
            printf("turn right\n");
            float ans=fmin(output,4.0);
            // float y=-150000*ans+1500000; 
            float y=125000*ans+1300000;
            printf("y=%f\n",y);
            wiringXPWMSetDuty(PWM_PIN, y);
            delayMicroseconds(50000);

        }
        else {
            printf("turn left\n");
            float ans=fmax(output,-4.0);
            float y=125000*ans+1300000;
            printf("y=%f\n",y);
            wiringXPWMSetDuty(PWM_PIN, y);
            delayMicroseconds(50000);
        }

        last_error=error;
        integral+=error;
        derivative=error-last_error;    
        usleep(100000);  // 10ms
    }
    return 0;
}