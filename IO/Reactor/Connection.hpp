#pragma once
#include <iostream>

class Connection
{
public:
    Connection()
    {
    }

    ~Connection()
    {
    }

    virtual int GetSockFd() = 0;

private:
    int _sockfd;
};