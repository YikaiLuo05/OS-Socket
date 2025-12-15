/**
 * TCP Echo 服务器程序 (Windows 版本)
 *
 * 功能说明：
 * - 创建一个 TCP 服务器，监听指定端口
 * - 接受客户端连接
 * - 将收到的数据原样返回给客户端（Echo 功能）
 *
 * 编译命令：
 * gcc server.c -o server.exe -lws2_32
 */

#define _WIN32_WINNT 0x0600 
/* 定义 Windows 版本，启用 InetNtop 函数（需要 Vista 及以上版本） */

#include <winsock2.h>   //Windows Socket 2 主头文件
#include <ws2tcpip.h>   // TCP/IP 相关函数和结构体
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib") //链接 Winsock 库

/* 常量定义 */
#define PORT 9000      //服务器监听端口号
#define BACKLOG 5      //等待连接队列的最大长度
#define BUF_SIZE 1024  //接收缓冲区大小

//主函数 - 服务器入口点
int main(void)
{
    WSADATA wsa; // Winsock 初始化数据结构

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
     * 创建监听套接字
     * AF_INET: 使用 IPv4 地址族
     * SOCK_STREAM: 使用 TCP 协议（面向连接的流式套接字）
     * IPPROTO_TCP: 明确指定 TCP 协议
     */
    SOCKET listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_fd == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed: %d\n", WSAGetLastError());
        WSACleanup(); /* 清理 Winsock 资源 */
        return 1;
    }

    /*
     * 设置套接字选项：允许地址重用
     * SO_REUSEADDR: 允许在 TIME_WAIT 状态下重新绑定地址
     * 这样服务器重启时可以立即重新绑定到同一端口
     */
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) == SOCKET_ERROR)
    {
        fprintf(stderr, "setsockopt failed: %d\n", WSAGetLastError());
        closesocket(listen_fd);
        WSACleanup();
        return 1;
    }

    /*
     * 配置服务器地址结构
     * sockaddr_in 是 IPv4 地址结构
     */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));           /* 清零地址结构 */
    addr.sin_family = AF_INET;                /* 地址族：IPv4 */
    addr.sin_port = htons(PORT);              /* 端口号（需要转换为网络字节序） */
    addr.sin_addr.s_addr = htonl(INADDR_ANY); /* 绑定到所有可用网络接口 */

    /*
     * 将套接字绑定到指定地址和端口
     * bind() 将套接字与本地地址关联
     */
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        fprintf(stderr, "bind failed: %d\n", WSAGetLastError());
        closesocket(listen_fd);
        WSACleanup();
        return 1;
    }

    /*
     * 开始监听连接请求
     * BACKLOG: 指定等待连接队列的最大长度
     * 超过此数量的连接请求将被拒绝
     */
    if (listen(listen_fd, BACKLOG) == SOCKET_ERROR)
    {
        fprintf(stderr, "listen failed: %d\n", WSAGetLastError());
        closesocket(listen_fd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    /*
     * 主循环：持续接受和处理客户端连接
     * 这是一个简单的单线程服务器，一次只能处理一个客户端
     */
    for (;;)
    {
        struct sockaddr_in client_addr; /* 存储客户端地址信息 */
        int client_len = sizeof(client_addr);

        /*
         * 接受客户端连接
         * accept() 会阻塞直到有客户端连接
         * 返回一个新的套接字用于与该客户端通信
         */
        SOCKET conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (conn_fd == INVALID_SOCKET)
        {
            int err = WSAGetLastError();
            if (err == WSAEINTR) /* 被信号中断，继续等待 */
            {
                continue;
            }
            fprintf(stderr, "accept failed: %d\n", err);
            break;
        }

        /*
         * 将客户端 IP 地址转换为可读字符串
         * InetNtopA: 将网络字节序的 IP 地址转换为点分十进制字符串
         */
        char client_ip[INET_ADDRSTRLEN];
        if (!InetNtopA(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip)))
        {
            strcpy(client_ip, "unknown");
        }
        printf("Client connected: %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        /*
         * Echo 循环：接收数据并原样返回
         * 持续处理直到客户端断开连接或发生错误
         */
        char buf[BUF_SIZE];
        int n;
        while ((n = recv(conn_fd, buf, BUF_SIZE - 1, 0)) > 0)
        {
            buf[n] = '\0'; /* 添加字符串结束符 */
            printf("Received: %s\n", buf);

            /* 将收到的数据原样发送回客户端 */
            if (send(conn_fd, buf, n, 0) == SOCKET_ERROR)
            {
                fprintf(stderr, "send failed: %d\n", WSAGetLastError());
                break;
            }
        }

        /* 检查接收是否因错误而终止 */
        if (n == SOCKET_ERROR)
        {
            fprintf(stderr, "recv failed: %d\n", WSAGetLastError());
        }

        printf("Client disconnected\n");
        closesocket(conn_fd); /* 关闭与客户端的连接 */
    }

    /* 清理资源 */
    closesocket(listen_fd); /* 关闭监听套接字 */
    WSACleanup();           /* 清理 Winsock 库 */
    return 0;
}
