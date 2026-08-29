#pragma once
#include <iostream>
#include <sys/epoll.h>
#include "Connection.hpp"
#include "Socket.hpp"
#include "Channel.hpp"
#include "Common.hpp"

class Listener : public Connection
{
public:
    Listener(int port)
        : _port(port), _listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildTcpSocketMethod(_port);
        SetEvent(EPOLLIN | EPOLLET);
        SetNonBlock(_listensock->Fd());
    }

    ~Listener()
    {
    }

    int GetSockFd() override
    {
        return _listensock->Fd();
    }

    void Recver() override
    {
        InetAddr client;
        while (true)
        {
            int sockfd = _listensock->Accept(&client);

            if (sockfd == ACCEPT_ERR)
                break;
            else if (sockfd == ACCEPT_CONTINUE)
                continue;
            else if (sockfd == ACCEPT_DONE)
                break;
            else
            {
                std::shared_ptr<Connection> conn = std::make_shared<Channel>(sockfd, client);
                conn->SetEvent(EPOLLIN | EPOLLET); // ET模式
                if (_handler != nullptr)
                    conn->RegisterHandler(_handler);
                GetOwner()->AddConnection(conn);
            }
        }
    }

    void Sender() override
    {
    }

    void Excepter() override {};

private:
    int _port;
    std::unique_ptr<Socket> _listensock;
};
