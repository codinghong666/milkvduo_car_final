// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PORT 12345
// #define BUFFER_SIZE 1024

// int main() {
//     int server_fd, client_fd;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t client_len = sizeof(client_addr);
//     char buffer[BUFFER_SIZE];

//     // 创建 TCP 套接字
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//         perror("Socket creation failed");
//         exit(1);
//     }

//     // 设置服务器地址结构
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网卡
//     server_addr.sin_port = htons(PORT);

//     // 绑定套接字到地址
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
//         perror("Binding failed");
//         close(server_fd);
//         exit(1);
//     }

//     // 监听连接
//     if (listen(server_fd, 5) == -1) {
//         perror("Listen failed");
//         close(server_fd);
//         exit(1);
//     }

//     printf("Server listening on port %d...\n", PORT);

//     // 接受客户端连接
//     if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
//         perror("Accept failed");
//         close(server_fd);
//         exit(1);
//     }

//     printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

//     // 接收客户端发送的数据
//     while (1) {
//         int n = read(client_fd, buffer, BUFFER_SIZE);
//         if (n <= 0) {
//             printf("Connection closed or error\n");
//             break;
//         }
//         printf("Received data: %s\n", buffer);
//         // 处理数据或回复客户端
//     }

//     // 关闭套接字
//     close(client_fd);
//     close(server_fd);

//     return 0;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 创建 TCP 套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // 设置服务器地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 监听所有网卡
    server_addr.sin_port = htons(PORT);

    // 绑定套接字到地址
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(server_fd);
        exit(1);
    }

    // 监听连接
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    // 接受客户端连接
    if ((client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len)) == -1) {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

    // 接收客户端发送的数据
    while (1) {
        int n = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (n <= 0) {
            printf("Connection closed or error\n");
            break;
        }
        buffer[n] = '\0';  // 使字符串正确结束
        printf("Received data: %s", buffer);  // 不需要打印多余的换行符
    }

    // 关闭套接字
    close(client_fd);
    close(server_fd);

    return 0;
}