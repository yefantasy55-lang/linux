#pragma once
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include "Common.hpp"
#include "Log.hpp"

using namespace yzq;

class Poll
{
};

class Epoller : public Poll
{
private:
    void ModEventHelper(int sockfd, uint32_t events, int oper)
    {
        struct epoll_event ev;
        ev.events = events;
        ev.data.fd = sockfd;
        int n = epoll_ctl(_epfd, oper, sockfd, &ev);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_ctl error";
            return;
        }
        LOG(LogLevel::INFO) << "epoll_ctl success: " << sockfd;
    }

public:
    Epoller()
        : _epfd(-1)
    {
        _epfd = epoll_create(256);
        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error!!!";
            exit(EPOLL_CREATE_ERR);
        }
        LOG(LogLevel::INFO) << "create epoll success: " << _epfd;
    }

    void AddEvent(int sockfd, uint32_t events)
    {
        ModEventHelper(sockfd, events, EPOLL_CTL_ADD);
    }

    void ModEvent(int sockfd, uint32_t events)
    {
        ModEventHelper(sockfd, events, EPOLL_CTL_MOD);
    }

    void DelEvent(int sockfd)
    {
        int n = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
        if (n < 0)
        {
            LOG(LogLevel::ERROR) << "epoll_ctl error";
            return;
        }
        LOG(LogLevel::INFO) << "epoll_ctl success: " << sockfd;
    }

    int WaitEvents(struct epoll_event revs[], int maxnum, int timeout)
    {
        int n = epoll_wait(_epfd, revs, maxnum, timeout);
        if (n < 0)
        {
            LOG(LogLevel::WARNING) << "epoll_wait error";
        }
        else if (n == 0)
        {
            LOG(LogLevel::WARNING) << "epoll_wait timeout";
        }
        else
        {
            LOG(LogLevel::WARNING) << "epoll_wait ready num: " << n;
        }
        return n;
    }

    ~Epoller()
    {
        if (_epfd >= 0)
        {
            close(_epfd);
        }
    }

private:
    int _epfd;
};