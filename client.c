/**
 * TCP Echo 客户端程序 (Windows 版本)
 *
 * 功能说明：
 * - 连接到指定的 TCP 服务器
 * - 从标准输入读取用户输入
 * - 将输入发送到服务器并接收回显数据
 *
 * 编译命令：
 * gcc client.c -o client.exe -lws2_32
 */

#define _WIN32_WINNT 0x0600 /* 定义 Windows 版本，启用 inet_pton 函数 */

/* 包含必要的头文件 */
#include <winsock2.h> /* Windows Socket 2 主头文件 */
#include <ws2tcpip.h> /* TCP/IP 相关函数和结构体 */
#include <stdio.h>    /* 标准输入输出 */
#include <stdlib.h>   /* 标准库函数 */
#include <string.h>   /* 字符串处理函数 */

#pragma comment(lib, "ws2_32.lib") /* 链接 Winsock 库 */

/* 常量定义 */
#define SERVER_IP "127.0.0.1" /* 服务器 IP 地址（本地回环地址） */
#define SERVER_PORT 9000      /* 服务器端口号 */
#define BUF_SIZE 1024         /* 缓冲区大小 */

/**
 * 主函数 - 客户端入口点
 */
int main(void)
{
    WSADATA wsa; /* Winsock 初始化数据结构 */

    /*
     * 初始化 Winsock 库
     * MAKEWORD(2, 2) 表示请求使用 Winsock 2.2 版本
     * 这是 Windows 网络编程的第一步，必须在使用任何 socket 函数之前调用
     */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    /*
     * 创建客户端套接字
     * AF_INET: 使用 IPv4 地址族
     * SOCK_STREAM: 使用 TCP 协议（面向连接的流式套接字）
     * IPPROTO_TCP: 明确指定 TCP 协议
     */
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        WSACleanup(); /* 清理 Winsock 资源 */
        return 1;
    }

    /*
     * 配置服务器地址结构
     * sockaddr_in 是 IPv4 地址结构
     */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); /* 清零地址结构 */
    server_addr.sin_family = AF_INET;             /* 地址族：IPv4 */
    server_addr.sin_port = htons(SERVER_PORT);    /* 端口号（转换为网络字节序） */

    /*
     * 将点分十进制 IP 地址字符串转换为网络字节序的二进制格式
     * inet_pton: "presentation to network" 的缩写
     * 返回值: 1 表示成功，0 表示格式无效，-1 表示错误
     */
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        fprintf(stderr, "inet_pton failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    /*
     * 连接到服务器
     * connect() 发起 TCP 三次握手，建立与服务器的连接
     * 这是一个阻塞调用，直到连接建立或失败才返回
     */
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "connect failed: %d\n", WSAGetLastError());
        closesocket(sockfd);
        WSACleanup();
        return 1;
    }

    /* 连接成功，显示提示信息 */
    printf("Connected to %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Type messages, Ctrl+Z then Enter to quit.\n");

    /*
     * 主循环：读取用户输入，发送到服务器，接收并显示回显
     * fgets() 从标准输入读取一行，遇到 EOF (Ctrl+Z) 时返回 NULL
     */
    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin))
    {
        size_t len = strlen(buf); /* 获取输入字符串长度（包含换行符） */

        /*
         * 发送数据到服务器
         * send() 将数据写入套接字发送缓冲区
         */
        if (send(sockfd, buf, (int)len, 0) == SOCKET_ERROR)
        {
            fprintf(stderr, "send failed: %d\n", WSAGetLastError());
            break;
        }

        /*
         * 接收服务器的回显数据
         * recv() 从套接字接收缓冲区读取数据
         * 返回值: >0 表示接收的字节数，0 表示连接关闭，-1 表示错误
         */
        int n = recv(sockfd, buf, BUF_SIZE - 1, 0);
        if (n == SOCKET_ERROR)
        {
            fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
            break;
        }
        if (n == 0) /* 服务器关闭了连接 */
        {
            printf("Server closed connection\n");
            break;
        }

        buf[n] = '\0';           /* 添加字符串结束符 */
        printf("Echo: %s", buf); /* 显示服务器回显的数据 */
    }

    /* 清理资源 */
    closesocket(sockfd); /* 关闭套接字，触发 TCP 四次挥手 */
    WSACleanup();        /* 清理 Winsock 库 */
    return 0;
}
