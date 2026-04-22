#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"

using namespace yzq;

const int defaultfd = -1;

class UdpServer
{
public:
    UdpServer(uint16_t port, const std::string &ip)
        : _sockfd(defaultfd), _port(port), _ip(ip)
    {
    }

    // 初始化套接字
    void Init()
    {
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 创建UPD套接字，0表示系统自动选择对应协议
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error!";
            exit(1);
        }
        LOG(LogLevel::INFO) << "socket success, sockfd : " << _sockfd;
        struct sockaddr_in local;   // 创建套接字结构体
        bzero(&local,sizeof(local));  // 将结构体清零
        local.sin_family=htons(_port);  // 将端口统一转成大端形式
        local.sin_addr.s_addr=
        int n = bind(_sockfd,);

    }

    // 启动套接字
    void Start()
    {
    }

    ~UdpServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    std::string _ip; // 用的是字符串风格，点分十进制
};