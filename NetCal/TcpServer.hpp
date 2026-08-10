#pragma once

#include "Socket.hpp"
#include "Log.hpp"
#include <iostream>
#include <memory>
#include <sys/wait.h>
#include <functional>

using namespace yzq;
using ioservice_t = std::function<void(std::shared_ptr<Socket> &, InetAddr &)>;

class TcpServer
{
public:
    TcpServer(uint16_t port, ioservice_t service)
        : _port(port), _listensockptr(std::make_unique<TcpSocket>()), _isrunning(false), _service(service)
    {
        _listensockptr->BuildTcpSocketMethod(_port);
    }

    ~TcpServer()
    {
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            InetAddr client; // 接受客户端信息
            auto sockfd = _listensockptr->Accept(&client);
            if (sockfd == nullptr)
            {
                continue;
            }
            pid_t id = fork(); // 多进程
            if (id == 0)       // 子进程（负责接收）
            {
                _listensockptr->Close();
                if (fork() > 0)
                    exit(OK);
                _service(sockfd, client); // 孙子进程执行服务
                sockfd->Close();
                exit(OK);
            }
            else if (id > 0) // 父进程（负责监听）
            {
                sockfd->Close();
                pid_t rid = waitpid(id, nullptr, 0);
                (void)rid;
            }
            else
            {
                LOG(LogLevel::FATAL) << "fork error ...";
                exit(FORK_ERR);
            }
        }
        _isrunning = false;
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensockptr;
    bool _isrunning;
    ioservice_t _service;
};
