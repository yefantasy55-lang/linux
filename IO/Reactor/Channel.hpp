#pragma once
#include <iostream>
#include <string>
#include "Connection.hpp"
#include "InetAddr.hpp"
#include "Log.hpp"
#include "Common.hpp"

using namespace yzq;

#define SIZE 1024

class Channel : public Connection
{
public:
    Channel(int sockfd, InetAddr &client)
        : _sockfd(sockfd), _client(client)
    {
        SetNonBlock(_sockfd);
    }

    int GetSockFd() override
    {
        return _sockfd;
    }

    void Recver() override
    {
        char buffer[SIZE];
        while (true)
        {
            buffer[0] = 0;
            LOG(LogLevel::DEBUG) << "enter recver: " << _sockfd;
            ssize_t n = recv(_sockfd, buffer, sizeof(buffer) - 1, 0);
            LOG(LogLevel::DEBUG) << "n: " << n;
            if (n > 0)
            {
                buffer[n] = 0;
                _inbuffer += buffer;
            }
            else if (n == 0)
            {
                Excepter();
                return;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    break;
                }
                else if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    Excepter();
                    return;
                }
            }
        }
        LOG(LogLevel::DEBUG) << "Channel: Inbuffer:\n"
                             << _inbuffer;

        if (!_inbuffer.empty())
            _outbuffer += _handler(_inbuffer);
        else
            return;

        if (!_outbuffer.empty())
            Sender();
    }

    void Sender() override
    {
        while (true)
        {
            int n = send(_sockfd, _outbuffer.c_str(), _outbuffer.size(), 0);
            if (n > 0)
            {
                _outbuffer.erase(0, n);
                if (_outbuffer.empty())
                    return;
            }
            else if (n == 0)
            {
                break;
            }
            else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                if (errno == EINTR)
                    continue;
                else
                {
                    Excepter();
                    return;
                }
            }

            if (!_outbuffer.empty()) // EAGAIN || EWOULDDBLOCK
            {
                GetOwner()->EnableReadWrite(_sockfd, true, true);
            }
            else
            {
                GetOwner()->EnableReadWrite(_sockfd, true, false);
            }
        }
    }

    std::string &Inbuffer()
    {
        return _inbuffer;
    }

    void AppendOutBuffer(const std::string &out)
    {
        _outbuffer += out;
    }

    void Excepter() override
    {
        GetOwner()->DelConnection(_sockfd);
    }

    ~Channel()
    {
    }

private:
    int _sockfd;
    InetAddr _client;
    std::string _inbuffer;
    std::string _outbuffer;
};