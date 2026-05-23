#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include "Log.hpp"

using namespace yzq;

const int defaultfd = -1;
using func_t = std::function<std::string(const std::string &)>;

class UdpServer
{
public:
    // UdpServer(uint16_t port, const std::string &ip)
    //     : _sockfd(defaultfd), _port(port), _ip(ip)
    // {
    // }

    UdpServer(uint16_t port, func_t fun)
        : _sockfd(defaultfd), _port(port), _fun(fun)
    {
    }

    // 初始化套接字
    void Init()
    {
        // 创建UPD套接字，AF_INET是网络套接字，SOCK_DGRAM数据报形式，0表示系统自动选择对应协议
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error!";
            exit(1);
        }
        LOG(LogLevel::INFO) << "socket success, sockfd : " << _sockfd;
        struct sockaddr_in local;      // 创建套接字结构体
        bzero(&local, sizeof(local));  // 将结构体清零
        local.sin_family = AF_INET;    // 网络套接字
        local.sin_port = htons(_port); // 将端口统一转成大端形式
        // local.sin_addr.s_addr = inet_addr(_ip.c_str());                  // 将点分十进制转化为网络序列
        local.sin_addr.s_addr = INADDR_ANY;                              // 无限制绑定的ip
        int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local)); // 显示绑定套接字
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(2); // 设置退出码
        }
        LOG(LogLevel::INFO) << "bind success, sockfd : " << _sockfd;
    }

    // 启动套接字~
    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            char buffer[1024];       // 信息缓冲区，用于存储信息
            struct sockaddr_in peer; // 用来存储发送过来消息的套接字信息
            socklen_t len = sizeof(peer);
            ssize_t s = recvfrom(_sockfd, &buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if (s > 0)
            {
                // int peer_port = ntohs(peer.sin_port); // 将网络序列转化为点分十进制
                // std::string peer_ip = inet_ntoa(peer.sin_addr);
                // LOG(LogLevel::DEBUG) << "[" << peer_ip << ":" << peer_port<< "]# " << buffer;
                buffer[s] = 0;
                std::string result = _fun(buffer);
                sendto(_sockfd, result.c_str(), result.size(), 0, (struct sockaddr *)&peer, len); // 发送消息
            }
        }
    }

    ~UdpServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    // std::string _ip; // 用的是字符串风格，点分十进制
    func_t _fun;
    bool _isrunning; // 运行状态
};