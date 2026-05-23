#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddr
{
public:
    InetAddr(struct sockaddr_in addr)
    :_addr(addr)
    {
        _ip=_addr.sin_addr.s_addr;
        _port=ntohs(_addr.sin_port);
        _ip=inet_ntoa(_addr.sin_addr);
    }

    std::string Ip()
    {
        return _ip;
    }

    uint16_t Port()
    {
        return _port;
    }

    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _addr;   // 套接字信息
    std::string _ip;
    uint16_t _port;
};