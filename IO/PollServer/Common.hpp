#pragma once

#include <iostream>
#include <functional>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CONV(addr) ((struct sockaddr*)&addr)

enum ExitCode
{
    OK = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    CONNECT_ERR,
    FORK_ERR,
    OPEN_ERR
};

class NoCopy
{
public:
    NoCopy() {};
    ~NoCopy() {};
    // 禁用拷贝构造
    NoCopy(const NoCopy &) = delete;
    const NoCopy &operator=(const NoCopy &) = delete;
private:
};
