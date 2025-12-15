# Socket 套接字通信项目

基于 Windows Winsock2 实现的 TCP Echo 服务器与客户端程序。

## 📋 项目简介

本项目是操作系统课程分组讨论 5 的作业，实现了基于 Socket 套接字接口的客户端与服务器进程通信。

### 功能特性

- ✅ TCP 可靠连接通信
- ✅ Echo 回显服务（服务器将收到的数据原样返回）
- ✅ 支持多次消息交互
- ✅ 正确处理连接建立与断开
- ✅ 完整的错误处理机制

## 🛠️ 环境要求

- **操作系统**: Windows 7/10/11
- **编译器**: GCC (MinGW) 或 MSVC
- **依赖库**: Winsock2 (ws2_32.lib)

## 📦 文件结构

```
.
├── server.c              # 服务器端源代码
├── client.c              # 客户端源代码
├── server.exe            # 服务器可执行文件
├── client.exe            # 客户端可执行文件
├── README.md             # 项目说明文档
└── 分组讨论5.md          # 任务要求说明
```

## 🚀 快速开始

### 编译

使用 GCC 编译：

```bash
# 编译服务器
gcc server.c -o server.exe -lws2_32

# 编译客户端
gcc client.c -o client.exe -lws2_32
```

使用 MSVC 编译：

```bash
# 编译服务器
cl server.c ws2_32.lib

# 编译客户端
cl client.c ws2_32.lib
```

### 运行

1. **启动服务器**（终端 A）：
   ```bash
   ./server.exe
   ```
   输出：`Server listening on port 9000`

2. **启动客户端**（终端 B）：
   ```bash
   ./client.exe
   ```
   输出：`Connected to 127.0.0.1:9000`

3. **发送消息**：
   在客户端终端输入任意文本，按回车发送。服务器会将消息原样返回。

4. **退出**：
   - 客户端：按 `Ctrl+Z` 然后回车（Windows）
   - 服务器：按 `Ctrl+C`

## 📖 使用示例

### 服务器端输出
```
Server listening on port 9000
Client connected: 127.0.0.1:52341
Received: Hello World
Received: Socket通信测试
Client disconnected
```

### 客户端输出
```
Connected to 127.0.0.1:9000
Type messages, Ctrl+Z then Enter to quit.
Hello World
Echo: Hello World
Socket通信测试
Echo: Socket通信测试
```

## 🔧 配置说明

### 修改端口号

服务器端口默认为 `9000`，如需修改：

**server.c:**
```c
#define PORT 9000  // 修改为其他端口
```

**client.c:**
```c
#define SERVER_PORT 9000  // 修改为相同端口
```

### 修改服务器地址

客户端默认连接本地回环地址，如需连接其他服务器：

**client.c:**
```c
#define SERVER_IP "127.0.0.1"  // 修改为目标服务器 IP
```

### 修改缓冲区大小

```c
#define BUF_SIZE 1024  // 可根据需要调整
```

## 📚 技术原理

### TCP 通信流程

```
┌─────────────────┐                    ┌─────────────────┐
│     Server      │                    │     Client      │
├─────────────────┤                    ├─────────────────┤
│  1. socket()    │                    │  1. socket()    │
│       ↓         │                    │       ↓         │
│  2. bind()      │                    │                 │
│       ↓         │                    │                 │
│  3. listen()    │                    │                 │
│       ↓         │                    │       ↓         │
│  4. accept() ←──┼── TCP 三次握手 ───┼── 2. connect()  │
│       ↓         │                    │       ↓         │
│  5. recv()  ←───┼──── 数据传输 ─────┼── 3. send()     │
│       ↓         │                    │       ↓         │
│  6. send()  ────┼──── 回显数据 ─────┼→  4. recv()     │
│       ↓         │                    │       ↓         │
│  7. close()     │                    │  5. close()     │
└─────────────────┘                    └─────────────────┘
```

### 核心 API

| 函数 | 作用 |
|------|------|
| `WSAStartup()` | 初始化 Winsock 库 |
| `socket()` | 创建套接字 |
| `bind()` | 绑定地址和端口 |
| `listen()` | 开始监听连接 |
| `accept()` | 接受客户端连接 |
| `connect()` | 连接到服务器 |
| `send()` | 发送数据 |
| `recv()` | 接收数据 |
| `closesocket()` | 关闭套接字 |
| `WSACleanup()` | 清理 Winsock 库 |

## ⚠️ 常见问题

### Q: 端口被占用怎么办？

A: 修改 `PORT` 和 `SERVER_PORT` 为其他未被占用的端口（如 9001、8080 等）。

### Q: 客户端连接失败？

A: 
1. 确保服务器已启动
2. 检查防火墙设置
3. 确认 IP 地址和端口配置正确

### Q: 如何支持多个客户端？

A: 当前实现是单线程的，一次只能处理一个客户端。可以通过以下方式扩展：
- 多线程：每个连接创建一个线程
- I/O 多路复用：使用 `select()` 或 IOCP

## 📄 参考资料

- [Winsock 官方文档](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
- [RFC 862 - Echo Protocol](https://tools.ietf.org/html/rfc862)
- [TCP/IP 详解](https://en.wikipedia.org/wiki/Transmission_Control_Protocol)

## 👥 作者

操作系统课程分组讨论 5 小组

## 📝 许可证

本项目仅用于教学目的。