import socket

SERVER_IP = "192.168.42.1"  # MilkvDuo 的 IP 地址
PORT = 12345                # 服务器端口
BUFFER_SIZE = 1024

# 创建 TCP 套接字
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    # 连接到服务器
    sock.connect((SERVER_IP, PORT))
    print("Connected to server")

    # 发送数据
    while True:
        message = input("Enter message: ")  # 获取用户输入
        message = message + '\n'  # 添加换行符作为分隔符
        sock.sendall(message.encode())  # 发送数据
except Exception as e:
    print(f"Error: {e}")
finally:
    # 关闭套接字
    sock.close()
    print("Connection closed")