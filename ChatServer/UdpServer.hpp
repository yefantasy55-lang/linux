#include <iostream>
#include <string>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <functional>
#include "Log.hpp"
#include "InetAddr.hpp"
#include "Dict.hpp"

using namespace yzq;

const int defaultfd = -1;
using func_t = std::function<std::string(int sockfd, const std::string &, InetAddr &)>;

class UdpServer
{
public:
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
                InetAddr client(peer);
                buffer[s] = 0;
                _fun(_sockfd,buffer, client);
            }
        }
    }

    ~UdpServer()
    {
    }

private:
    int _sockfd;
    uint16_t _port;
    func_t _fun;
    bool _isrunning; // 运行状态
};