#pragma once
#include <iostream>
#include "Connection.hpp"
#include "Socket.hpp"

class Listener : public Connection
{
public:
    Listener()
    {
    }

    ~Listener()
    {
    }

    int GetSockFd()
    {
        return _listensock->Fd();
    }

private:
    int _port;
    std::unique_ptr<Socket> _listensock;
};
