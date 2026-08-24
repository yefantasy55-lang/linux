#pragma once

#include <iostream>
#include "Log.hpp"
#include "Socket.hpp"
#include <unistd.h>
#include <memory>
#include <sys/epoll.h>
#include "Command.hpp"

using namespace yzq;

class EpollServer
{

    const static int size = 64;
    const static int defaultfd = -1;

public:
    EpollServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildTcpSocketMethod(port);
        _epfd = epoll_create(256); 
        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error";
            exit(EPOLL_CREATE_ERR);
        }
        LOG(LogLevel::INFO) << "epoll_create success: " << _epfd;

        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Fd();
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock->Fd(), &ev);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "add listensockfd failed";
            exit(EPOLL_CTL_ERR);
        }
    }

    void Start()
    {
        _isrunning = true;
        int timeout = -1; // 无限等待(阻塞)
        while (_isrunning)
        {
            int n = epoll_wait(_epfd, _revs, size, timeout); 
            switch (n)
            {
            case -1:
                LOG(LogLevel::ERROR) << "epoll error";
                break;
            case 0:
                LOG(LogLevel::INFO) << "time out...";
                break;
            default:
                LOG(LogLevel::DEBUG) << "有事件就绪了..., n : " << n;
                Dispatcher(n); // 处理就绪的事件啊！
                break;
            }
        }
        _isrunning = false;
    }

    // 事件派发器
    void Dispatcher(int rnum)
    {
        LOG(LogLevel::DEBUG) << "event ready ..."; // LT: 水平触发模式--epoll默认
        for (int i = 0; i < rnum; i++)
        {
            int sockfd = _revs[i].data.fd;
            uint32_t revent = _revs[i].events;
            if (revent & EPOLLIN) // 读事件就绪
            {
                if (sockfd == _listensock->Fd()) // 监听套接字
                {
                    Accepter();
                }
                else // 普通套接字
                {
                    Recver(sockfd);
                }
            }
        }
    }

    // 链接管理器
    void Accepter()
    {
        InetAddr client;
        auto sockfd = _listensock->Accept(&client);
        if (sockfd->Fd() >= 0)
        {
            LOG(LogLevel::INFO) << "get a new link, sockfd: "
                                << sockfd->Fd() << ", client is: " << client.StringAddr();
            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = sockfd->Fd();
            int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd->Fd(), &ev);
            if (n < 0)
            {
                LOG(LogLevel::WARNING) << "add listensockfd failed";
            }
            else
            {
                LOG(LogLevel::INFO) << "epoll_ctl add sockfd success: " << sockfd;
            }
        }
    }

    // IO处理器
    void Recver(int fd)
    {
        char buffer[1024];
        int n = recv(fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client say@ " << buffer << std::endl;
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "clien quit...";
            int res = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            if (res > 0)
            {
                LOG(LogLevel::INFO) << "epoll_ctl remove sockfd success: " << fd;
            }
            close(fd);
        }
        else
        {
            LOG(LogLevel::ERROR) << "recv error";
            int res = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr);
            if (res > 0)
            {
                LOG(LogLevel::INFO) << "epoll_ctl remove sockfd success: " << fd;
            }
            close(fd);
        }
    }

    void Stop()
    {
        _isrunning = false;
    }

    ~EpollServer()
    {
        _listensock->Close();
        if (_epfd > 0)
            close(_epfd);
    }

private:
    std::unique_ptr<TcpSocket> _listensock;
    bool _isrunning;

    int _epfd;                      // 创建的epoll句柄
    struct epoll_event _revs[size]; // epoll_wait输出已就绪的事件
};