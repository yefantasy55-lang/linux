#pragma once

#include <cstdlib>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include "Common.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace yzq;

namespace yzq
{
    const static int gbacklog = 16;

    class Socket
    {
    public:
        virtual ~Socket() {}
        virtual void SocketOrDie() = 0;                                       // 创建套接字
        virtual void BindOrDie(uint16_t port) = 0;                            // 绑定套接字
        virtual void ListenOrDie(int backlog) = 0;                            // 监听套接字
        virtual int Accept(InetAddr *client) = 0;                             // 接受连接
        virtual void Close() = 0;                                             // 关闭对应套接字
        virtual int Recv(std::string *out) = 0;                               // 从套接字中接收数据
        virtual int Send(const std::string &message) = 0;                     // 发送数据
        virtual int Connect(const std::string &server_ip, uint16_t port) = 0; // 链接服务端
        virtual int Fd() = 0;                                                 // 返回套接字

    public:
        void BuildTcpSocketMethod(uint16_t port, int backlog = gbacklog)
        {
            SocketOrDie();
            BindOrDie(port);
            ListenOrDie(backlog);
        }

        void BuildTcpClientSocketMethod()
        {
            SocketOrDie();
        }

        // void BuildUdpSocketMethod(uint16_t port)
        // {
        //     SocketOrDie();
        //     BindOrDie(port);
        // }
    };

    const static int defaultfd = -1;

    class TcpSocket : public Socket
    {
    public:
        TcpSocket()
            : _sockfd(defaultfd)
        {
        }

        TcpSocket(int sockfd)
            : _sockfd(sockfd)
        {
        }

        ~TcpSocket()
        {
        }

        void SocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "socket error";
                exit(SOCKET_ERR);
            }
            LOG(LogLevel::INFO) << "socket success";
        }

        void BindOrDie(uint16_t port) override
        {
            InetAddr localaddr(port);
            int n = bind(_sockfd, localaddr.NetAddrPtr(), localaddr.NetAddrLen());
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "bind error";
                exit(BIND_ERR);
            }
            LOG(LogLevel::INFO) << "bind success";
        }

        void ListenOrDie(int backlog) override
        {
            int n = listen(_sockfd, backlog);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "listen error";
                exit(LISTEN_ERR);
            }
            LOG(LogLevel::INFO) << "listen success";
        }

#define ACCEPT_DONE -1
#define ACCEPT_CONTINUE -2
#define ACCEPT_ERR -3
        int Accept(InetAddr *client) override
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int fd = accept(_sockfd, CONV(peer), &len);
            if (fd < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    return -1;
                }
                else if (errno == EINTR)
                {
                    return -2;
                }
                else
                {
                    LOG(LogLevel::WARNING) << "accept error";
                    return -3;
                }
            }
            return fd;
        }

        void Close() override
        {
            if (_sockfd >= 0)
            {
                close(_sockfd);
            }
        }

        int Recv(std::string *out)
        {
            char inbuffer[4096 * 2];
            int n = recv(_sockfd, inbuffer, sizeof(inbuffer) - 1, 0);
            if (n > 0)
            {
                inbuffer[n] = 0;
                *out += inbuffer;
            }
            return n;
        }

        int Send(const std::string &message)
        {
            return send(_sockfd, message.c_str(), message.size(), 0);
        }

        int Connect(const std::string &server_ip, uint16_t port) override
        {
            InetAddr server(server_ip, port);
            return connect(_sockfd, server.NetAddrPtr(), server.NetAddrLen());
        }

        int Fd()
        {
            return _sockfd;
        }

    private:
        int _sockfd;
    };

    // class UdpSocket : public Socket
    // {
    // };
}
