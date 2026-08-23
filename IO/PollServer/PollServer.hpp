#pragma once

#include <iostream>
#include "Log.hpp"
#include "Socket.hpp"
#include <unistd.h>
#include <memory>
#include <sys/poll.h>

using namespace yzq;

class PollServer
{

    const static int size = 4096;
    const static int defaultfd = -1;

public:
    PollServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildTcpSocketMethod(port);
        for (int i = 0; i < size; i++)
        {
            _fds[i].fd = defaultfd;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }
        _fds[0].fd = _listensock->Fd();
        _fds[0].events = POLLIN;
    }

    void Start()
    {
        _isrunning = true;
        int timeout = -1; // 无限等待(阻塞)
        while (_isrunning)
        {
            PrintFd();
            int n = poll(_fds, size, timeout);
            switch (n)
            {
            case -1:
                LOG(LogLevel::ERROR) << "poll error";
                break;
            case 0:
                LOG(LogLevel::INFO) << "time out...";
                break;
            default:
                LOG(LogLevel::DEBUG) << "有事件就绪了..., n : " << n;
                Dispatcher(); // 处理就绪的事件啊！
                break;
            }
        }
        _isrunning = false;
    }

    // 事件派发器
    void Dispatcher()
    {
        for (int i = 0; i < size; i++)
        {
            if (_fds[i].fd == defaultfd)
                continue;
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _listensock->Fd())
                {
                    Accepter(); // 监听套接字管理连接任务
                }
                else
                {
                    Recver(_fds[i].fd, i);
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
            int pos;
            for (pos = 0; pos < size; pos++)
            {
                if (_fds[pos].fd == defaultfd)
                    break;
            }

            // 管理的文件描述符已满
            if (pos == size)
            {
                LOG(LogLevel::WARNING) << "poll server full";
                sockfd->Close();
            }
            else
            {
                _fds[pos].fd = sockfd->Fd();
                _fds[pos].events = POLLIN;
                _fds[pos].revents = 0;
            }
        }
    }

    // IO处理器
    void Recver(int fd, int pos)
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
            close(fd);
            _fds[pos].fd = defaultfd;
        }
        else
        {
            LOG(LogLevel::ERROR) << "recv error";
            close(fd);
            _fds[pos].fd = defaultfd;
        }
    }

    void PrintFd()
    {
        std::cout << "_fds[]: ";
        for (int i = 0; i < size; i++)
        {
            if (_fds[i].fd == defaultfd)
                continue;
            std::cout << _fds[i].fd << " ";
        }
        std::cout << "\r\n";
    }

    void Stop()
    {
        _isrunning = false;
    }

    ~PollServer()
    {
    }

private:
    std::unique_ptr<TcpSocket> _listensock;
    bool _isrunning;
    struct pollfd _fds[size]; // 辅助数组，用于记录历史需要等待的套接字
};