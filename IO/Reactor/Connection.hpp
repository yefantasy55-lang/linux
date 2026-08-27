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
    virtual uint32_t GetEvent() = 0;
    virtual void SetEvent() = 0;

private:
    int _sockfd;
};