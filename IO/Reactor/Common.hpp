#pragma once

#include <iostream>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CONV(addr) ((struct sockaddr *)&addr)

enum ExitCode
{
    OK = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    CONNECT_ERR,
    FORK_ERR,
    OPEN_ERR,
    EPOLL_CREATE_ERR,
    EPOLL_CTL_ERR,
    EPOOL_WAIT_ERR
};

void SetNonBlock(int fd)
{
    int f1 = fcntl(fd, F_GETFL);
    if (f1 < 0)
    {
        return;
    }
    fcntl(fd, F_SETFL, f1 | O_NONBLOCK);
}

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
