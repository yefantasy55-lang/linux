#pragma once
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"
#include "ThreadPool.hpp"

using namespace yzq;

using fun_t = std::function<void(const std::string &, InetAddr &)>;

const static int defaultsockfd = -1;
const static int backlog = 8; // 最大的连接数量

class TcpServer : public NoCopy
{
public:
    TcpServer(uint16_t port, fun_t func)
        : _port(port), _listensockfd(defaultsockfd), _isrunning(false), _func(func)
    {
    }

    void Init()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
        if (_listensockfd)
        {
            LOG(LogLevel::FATAL) << "socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket success: " << _listensockfd;

        InetAddr local(_port);
        int n = bind(_listensockfd, local.NetAddrPtr(), local.NetAddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success: " << _listensockfd;
        n = listen(_listensockfd, backlog); // 转化为监听套接字
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen success";
    }

    void Service(int sockfd, InetAddr &peer)
    {
    }

    void Run()
    {
        _isrunning = true;
        while (_isrunning)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, CONV(peer), &len); // 取走信息
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                continue;
            }
        }
        _isrunning = false;
    }

    ~TcpServer()
    {
    }

private:
    uint16_t _port;
    int _listensockfd; // 监听套接字
    bool _isrunning;
    fun_t _func;
};
