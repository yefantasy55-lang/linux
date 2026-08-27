#pragma once
#include <iostream>
#include "Connection.hpp"
#include "Socket.hpp"

class Listener : public Connection
{
public:
    Listener()
    {
        _listensock = std::make_unique<TcpSocket>();
        _listensock->BuildTcpSocketMethod(_port);
    }

    ~Listener()
    {
    }

    int GetSockFd()
    {
        return _listensock->Fd();
    }

    void SetEvent(uint32_t events)
    {
        _events = events;
    }

    uint32_t GetEvent()
    {
        return _events;
    }

private:
    int _port;
    std::unique_ptr<Socket> _listensock;
    uint32_t _events;
};
