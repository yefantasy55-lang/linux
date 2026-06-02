#pragma once

#include "Common.hpp"

class InetAddr
{
public:
    // 网络转主机
    InetAddr(struct sockaddr_in addr)
        : _addr(addr)
    {
        _port = ntohs(_addr.sin_port);
        char ipbuffer[64];
        inet_ntop(AF_INET, &_addr.sin_addr, ipbuffer, sizeof(ipbuffer));
        _ip = ipbuffer;
    }

    // 主机转网络
    InetAddr(const std::string ip, uint16_t port)
        : _ip(ip), _port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET; // IPV4
        _addr.sin_port = htons(_port);
        inet_pton(AF_INET, _ip.c_str(), &_addr.sin_addr);
    }

    // 主机转网络（port)
    InetAddr(uint16_t port)
        : _ip(0), _port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = INADDR_ANY;
        _addr.sin_port = htons(_port);
    }

    std::string Ip()
    {
        return _ip;
    }

    uint16_t Port()
    {
        return _port;
    }

    const struct sockaddr_in &NetAddr()
    {
        return _addr;
    }

    struct sockaddr *NetAddrPtr()
    {
        return CONV(_addr);
    }

    socklen_t NetAddrLen()
    {
        return sizeof(_addr);
    }

    std::string StringAddr()
    {
        return _ip + ":" + std::to_string(_port);
    }

    bool operator==(InetAddr &addr)
    {
        return _ip == addr._ip && _port == addr._port;
    }

    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _addr; // 套接字信息
    std::string _ip;
    uint16_t _port;
};