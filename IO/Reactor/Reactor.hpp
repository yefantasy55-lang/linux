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

    bool IsConnectionEmpty()
    {
        return _connections.empty();
    }

    // 事件派发器
    void Dispatcher(int n)
    {
        for (int i = 0; i < n; i++)
        {
            int sockfd = _revs[i].data.fd;
            uint32_t events = _revs[i].events;

            if (events & EPOLLIN) // 读事件就绪
            {
            }
            else if (events & EPOLLOUT) // 写事假就绪
            {
            }
        }
    }

public:
    Reactor()
        : _epoller_ptr(std::make_unique<Epoller>()), _isrunning(false)
    {
    }
    ~Reactor()
    {
    }

    void Loop()
    {
        if (IsConnectionEmpty())
            return;
        _isrunning = true;

        while (true)
        {
            int timeout = -1;
            int n = _epoller_ptr->WaitEvents(_revs, revs_num, timeout);
        }

        _isrunning = false;
    }

    void AddConnection(std::shared_ptr<Connection> &conn)
    {
        if (IsConnectionExists(conn))
        {
            LOG(LogLevel::WARNING) << "conn is exists: " << conn->GetSockFd();
            return;
        }

        int sockfd = conn->GetSockFd();
        uint32_t events = conn->GetEvent();

        _epoller_ptr->AddEvent(sockfd, events);
        // ?
        _connections[sockfd] = conn;
    }

    void DelConnection(std::shared_ptr<Connection> &conn)
    {
        if (!IsConnectionExists(conn))
        {
            LOG(LogLevel::WARNING) << "conn is no exists: " << conn->GetSockFd();
            return;
        }

        int sockfd = conn->GetSockFd();

        _epoller_ptr->DelEvent(sockfd);
        _connections.erase(sockfd);
        close(sockfd);

        LOG(LogLevel::INFO) << "client quit: " << sockfd;
    }

    void Stop()
    {
        _isrunning = false;
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