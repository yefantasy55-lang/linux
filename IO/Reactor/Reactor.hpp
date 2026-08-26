#pragma once

#include <iostream>
#include <memory>
#include <unordered_map>
#include "Log.hpp"
#include "Epoller.hpp"
#include "Connection.hpp"

using namespace yzq;

class Reactor
{
    static const int revs_num = 128;

private:
    bool IsConnectionExistsHelper(int sockfd)
    {
        auto iter = _connections.find(sockfd);
        if (iter == _connections.end())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    bool IsConnectionExists(std::shared_ptr<Connection> &conn)
    {
        return IsConnectionExistsHelper(conn->GetSockFd());
    }

    bool IsConnectionExists(int sockfd)
    {
        return IsConnectionExistsHelper(sockfd);
    }

public:
    Reactor()
        : _epoller_ptr(std::make_unique<Epoller>()), _isrunning(false)
    {
    }
    ~Reactor()
    {
    }

    void Start()
    {
    }

    void AddConnection(std::shared_ptr<Connection> &conn)
    {
        if (IsConnectionExists(conn))
        {
            LOG(LogLevel::WARNING) << "conn is exists: " << conn->GetSockFd();
            return;
        }
        
    }

private:
    // 1.epoll模型
    std::unique_ptr<Epoller> _epoller_ptr;
    // 2.管理所有connection，本质是管理未来所有获取到的fd
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
    // 3.就绪事件
    struct epoll_event _revs[revs_num];
    // 4.是否运行
    bool _isrunning;
};