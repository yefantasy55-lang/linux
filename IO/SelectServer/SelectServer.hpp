#pragma once

#include <iostream>
#include "Log.hpp"
#include "Socket.hpp"
#include <unistd.h>
#include <memory>

using namespace yzq;

class SelectServer
{

    const static int size = sizeof(fd_set) * 8;
    const static int defaultfd = -1;

public:
    SelectServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildTcpSocketMethod(port);
        for (int i = 0; i < size; i++)
        {
            _fds[i] = defaultfd;
        }
        _fds[0] = _listensock->Fd(); // 默认数组第一个为监听套接字
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            fd_set rfds;
            FD_ZERO(&rfds); // 将位图置为0
            int maxfd = defaultfd;
            for (int i = 0; i < size; i++)
            {
                if (_fds[i] == defaultfd)
                    continue;
                if (maxfd < _fds[i])
                {
                    maxfd = _fds[i];
                }
                FD_SET(_fds[i], &rfds);
            }

            PrintFd();
            int n = select(maxfd + 1, &rfds, nullptr, nullptr, nullptr); // timeout为nullptr为无限阻塞等待
            switch (n)
            {
            case -1:
                LOG(LogLevel::ERROR) << "select error";
                break;
            case 0:
                LOG(LogLevel::INFO) << "time out...";
                break;
            default:
                LOG(LogLevel::DEBUG) << "有事件就绪了..., n : " << n;
                Dispatcher(rfds); // 处理就绪的事件啊！
                break;
            }
        }
        _isrunning = false;
    }

    // 事件派发器
    void Dispatcher(fd_set &rfds)
    {
        for (int i = 0; i < size; i++)
        {
            if (_fds[i] == defaultfd)
                continue;
            if (FD_ISSET(_fds[i], &rfds))
            {
                if (_fds[i] == _listensock->Fd())
                {
                    Accepter(); // 监听套接字管理连接任务
                }
                else
                {
                    Recver(_fds[i], i);
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
                if (_fds[pos] == defaultfd)
                    break;
            }

            // 管理的文件描述符已满
            if (pos == size)
            {
                LOG(LogLevel::WARNING) << "select server full";
                sockfd->Close();
            }
            else
            {
                _fds[pos] = sockfd->Fd();
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
            _fds[pos] = defaultfd;
        }
        else
        {
            LOG(LogLevel::ERROR) << "recv error";
            close(fd);
            _fds[pos] = defaultfd;
        }
    }

    void PrintFd()
    {
        std::cout << "_fds[]: ";
        for (int i = 0; i < size; i++)
        {
            if (_fds[i] == defaultfd)
                continue;
            std::cout << _fds[i] << " ";
        }
        std::cout << "\r\n";
    }

    void Stop()
    {
        _isrunning = false;
    }

    ~SelectServer()
    {
    }

private:
    std::unique_ptr<TcpSocket> _listensock;
    bool _isrunning;
    int _fds[size]; // 辅助数组，用于记录历史需要等待的套接字
};