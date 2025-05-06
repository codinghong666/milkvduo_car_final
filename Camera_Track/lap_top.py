

import torch
import cv2
import numpy as np
import socket

SERVER_IP = "192.168.42.1"  # MilkvDuo 的 IP 地址
PORT = 12345                # 服务器端口
BUFFER_SIZE = 1024

# 创建 TCP 套接字
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 导入 YOLOv5 模型（使用 torch.hub 加载预训练模型）
model = torch.hub.load('ultralytics/yolov5', 'yolov5s', pretrained=True)

# 初始化全局变量，用于记录选定目标的边界框和类别
selected_box = None
selected_class = None

# 定义计算两个边界框 IoU 的函数
def compute_iou(boxA, boxB):
    """
    计算两个边界框的交并比（IoU）
    boxA 和 boxB 格式为 (x1, y1, x2, y2)
    """
    xA = max(boxA[0], boxB[0])
    yA = max(boxA[1], boxB[1])
    xB = min(boxA[2], boxB[2])
    yB = min(boxA[3], boxB[3])

    interWidth = max(0, xB - xA)
    interHeight = max(0, yB - yA)
    interArea = interWidth * interHeight

    boxAArea = (boxA[2] - boxA[0]) * (boxA[3] - boxA[1])
    boxBArea = (boxB[2] - boxB[0]) * (boxB[3] - boxB[1])
    if boxAArea + boxBArea - interArea == 0:
        return 0
    iou = interArea / float(boxAArea + boxBArea - interArea)
    return iou

# 鼠标事件回调函数，用于选取目标
def on_mouse(event, x, y, flags, param):
    global selected_box, selected_class, boxes, classes
    # 只处理左键单击事件
    if event == cv2.EVENT_LBUTTONUP:
        # 如果当前有检测框，则判断点击位置是否在框内
        if len(boxes) > 0:
            for i, box in enumerate(boxes):
                x1, y1, x2, y2 = box
                # 如果点击位置在某个检测框内，则选定该目标
                if x >= x1 and x <= x2 and y >= y1 and y <= y2:
                    selected_box = box
                    selected_class = classes[i]
                    print(f"选定目标：类别 {selected_class}, 边界框 {selected_box}")
                    break

# 打开摄像头视频流
cap = cv2.VideoCapture(0)
cv2.namedWindow('YOLOv5 Detection')
cv2.setMouseCallback('YOLOv5 Detection', on_mouse)

# IoU 阈值：将原先的 0.15 调整到 0.10，以便目标微小移动后仍能持续追踪
iou_threshold = 0.10

try:
    sock.connect((SERVER_IP, PORT))
#     print("Connected to server")
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        height, width = frame.shape[:2]
        # 运行 YOLOv5 模型进行检测
        results = model(frame)
        # 提取检测结果中的边界框和类别
        boxes = []
        classes = []
        if results is not None and len(results) > 0:
            # 处理模型输出，获取每个检测框的坐标和类别
            detections = results.xyxy[0].cpu().numpy()
            # detections 中每行格式: [x1, y1, x2, y2, confidence, class]
            for det in detections:
                x1, y1, x2, y2, conf, cls = det
                # 仅保留高置信度检测结果（阈值可以根据需要调整）
                if conf < 0.3:
                    continue
                boxes.append((int(x1), int(y1), int(x2), int(y2)))
                classes.append(int(cls))

        # 将检测到的边界框画在帧上
        for i, box in enumerate(boxes):
            x1, y1, x2, y2 = box
            cls = classes[i]
            label = f"{cls}"
            cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
            cv2.putText(frame, label, (x1, y1 - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.9, (36,255,12), 2)

        # 如果已有选定目标，则寻找与其 IoU 最大的检测框
        if selected_box is not None:
            best_iou = 0
            best_box = None
            best_cls = None
            for i, box in enumerate(boxes):
                iou = compute_iou(selected_box, box)
                if iou > best_iou:
                    best_iou = iou
                    best_box = box
                    best_cls = classes[i]
            # 匹配检测框不仅要求 IoU 最大且大于阈值，还要求类别与初选目标类别一致
            # 新增类别(class)一致性判断
            if best_iou > iou_threshold and best_cls == selected_class:
                # 更新跟踪到的目标边界框
                selected_box = best_box
                x1, y1, x2, y2 = selected_box

                # 计算目标位置中心点和尺寸的百分比(mid_percent, S_percent)
                h, w = frame.shape[:2]
                x_center = (x1 + x2) / 2
                y_center = (y1 + y2) / 2
                mid_x = (x1 + x2) / 2
                mid_percent = mid_x/width
                S=(x2-x1)*(y2-y1)
                S_percent = S/(width*height)
                message= f"{mid_percent:.2f}"
                message=message+" "
                message=message+f"{S_percent:.2f}"
                message=message+"\n"
                sock.sendall(message.encode())            

                # 在这里发送 mid_percent 和 S_percent（保持原逻辑不变）
                # 示例打印，可以替换为实际发送逻辑
                print(f"mid_percent: {mid_percent}, S_percent: {S_percent}")
                
                # 可视化：用红色框标记跟踪到的目标
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)
            else:
                # 如果当前帧没有找到匹配目标（例如 IoU 过小或类别不一致），可根据需要清除选定目标或其他处理
                pass

        # 显示图像
        cv2.imshow('YOLOv5 Detection', frame)
        # 按 'q' 键退出循环
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
except socket.error as e:
    print(f"Socket error: {e}")
    sock.close()
    exit(1)
finally:
    # 关闭套接字
    sock.close()
    print("Connection closed")
# 释放资源
cap.release()
cv2.destroyAllWindows()
