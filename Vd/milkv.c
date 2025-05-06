#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CAMERA_DEVICE "/dev/cvi-vi"
#define WIDTH 640
#define HEIGHT 480
#define FMT V4L2_PIX_FMT_MJPEG
#define PORT 8888
#define MAX_CLIENTS 1

typedef struct {
    void *start;
    size_t length;
} Buffer;

Buffer *buffers;
unsigned int n_buffers;

void errno_exit(const char *s) {
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void *arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
    } while (-1 == r && EINTR == errno);
    return r;
}

void init_camera(int fd) {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n", CAMERA_DEVICE);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }
    
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf(stderr, "%s is not video capture device\n", CAMERA_DEVICE);
        exit(EXIT_FAILURE);
    }
    
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming\n", CAMERA_DEVICE);
        exit(EXIT_FAILURE);
    }
    
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = FMT;
    fmt.fmt.pix.field = V4L2_FIELD_ANY;
    
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
        errno_exit("VIDIOC_S_FMT");
    
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    
    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support memory mapping\n", CAMERA_DEVICE);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_REQBUFS");
        }
    }
    
    buffers = calloc(req.count, sizeof(*buffers));
    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");
        
        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED,
                                      fd, buf.m.offset);
        
        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

void start_capturing(int fd) {
    unsigned int i;
    enum v4l2_buf_type type;
    
    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        
        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
    }
    
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
}

void send_frame(int sockfd, const void *p, int size) {
    // 先发送帧大小
    uint32_t net_size = htonl(size);
    if (write(sockfd, &net_size, sizeof(net_size)) < sizeof(net_size)) {
        perror("write size failed");
        return;
    }
    
    // 发送帧数据
    int sent = 0;
    while (sent < size) {
        int n = write(sockfd, (char*)p + sent, size - sent);
        if (n < 0) {
            perror("write frame failed");
            return;
        }
        sent += n;
    }
}

void read_frame(int fd, int sockfd) {
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    
    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
            case EAGAIN:
                return;
            case EIO:
                // 可忽略
            default:
                errno_exit("VIDIOC_DQBUF");
        }
    }
    
    send_frame(sockfd, buffers[buf.index].start, buf.bytesused);
    
    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");
}

void stop_capturing(int fd) {
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
        errno_exit("VIDIOC_STREAMOFF");
}

void uninit_camera(int fd) {
    unsigned int i;
    for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap(buffers[i].start, buffers[i].length))
            errno_exit("munmap");
    free(buffers);
}

int setup_tcp_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Waiting for client connection...\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    printf("Client connected\n");
    return new_socket;
}

int main() {
    int camera_fd, sockfd;
    
    camera_fd = open(CAMERA_DEVICE, O_RDWR | O_NONBLOCK, 0);
    if (camera_fd == -1) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                CAMERA_DEVICE, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    sockfd = setup_tcp_server();
    init_camera(camera_fd);
    start_capturing(camera_fd);
    
    while (1) {
        for (;;) {
            fd_set fds;
            struct timeval tv;
            int r;
            
            FD_ZERO(&fds);
            FD_SET(camera_fd, &fds);
            
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            
            r = select(camera_fd + 1, &fds, NULL, NULL, &tv);
            
            if (-1 == r) {
                if (EINTR == errno)
                    continue;
                errno_exit("select");
            }
            
            if (0 == r) {
                fprintf(stderr, "select timeout\n");
                exit(EXIT_FAILURE);
            }
            
            read_frame(camera_fd, sockfd);
        }
    }
    
    stop_capturing(camera_fd);
    uninit_camera(camera_fd);
    close(camera_fd);
    close(sockfd);
    return 0;
}