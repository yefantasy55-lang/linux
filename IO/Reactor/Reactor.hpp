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

    int LoopOnce(int timeout)
    {
        return _epoller_ptr->WaitEvents(_revs, revs_num, timeout);
    }

    // 事件派发器
    void Dispatcher(int n)
    {
        for (int i = 0; i < n; i++)
        {
            int sockfd = _revs[i].data.fd;
            uint32_t events = _revs[i].events;

            // 套接字错误或者对端链接关闭，统一转化为读写错误
            if ((events & EPOLLERR) || (events & EPOLLHUP))
            {
                events |= (EPOLLIN | EPOLLOUT);
            }

            if (events & EPOLLIN) // 读事件就绪
            {
                if (IsConnectionExists(sockfd))
                {
                    _connections[sockfd]->Recver();
                }
            }

            if (events & EPOLLOUT) // 写事件就绪
            {
                if (IsConnectionExists(sockfd))
                {
                    _connections[sockfd]->Sender();
                }
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
            PrintConnection();
            int n = LoopOnce(timeout);
            Dispatcher(n);
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
        conn->SetOwner(this);
        _connections[sockfd] = conn;
    }

    void DelConnection(int sockfd)
    {

        _epoller_ptr->DelEvent(sockfd);
        _connections.erase(sockfd);
        close(sockfd);

        LOG(LogLevel::INFO) << "client quit: " << sockfd;
    }

    void EnableReadWrite(int sockfd, bool enableread, bool enablewrite)
    {
        if (!IsConnectionExists(sockfd))
        {
            LOG(LogLevel::WARNING) << "EnableReadWrite, conn is exists: " << sockfd;
            return;
        }

        uint32_t new_event = EPOLLET | (enableread ? EPOLLIN : 0 | enablewrite ? EPOLLOUT
                                                                               : 0);

        _connections[sockfd]->SetEvent(new_event);
        _epoller_ptr->ModEvent(sockfd, new_event);
    }

    void PrintConnection()
    {
        std::cout << "当前Reactor正在进行管理的fd List:";
        for (auto &conn : _connections)
        {
            std::cout << conn.second->GetSockFd() << " ";
        }
        std::cout << "\r\n";
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