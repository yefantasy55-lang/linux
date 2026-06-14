#pragma once
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "Log.hpp"
#include "Common.hpp"
#include "InetAddr.hpp"
#include "ThreadPool.hpp"

using namespace yzq;

using fun_t = std::function<std::string(const std::string &, InetAddr &)>;

const static int defaultsockfd = -1;
const static int backlog = 8; // 最大的连接数量

class TcpServer : public NoCopy
{
public:
    TcpServer(uint16_t port, fun_t func)
        : _port(port), _listensockfd(defaultsockfd), _isrunning(false), _func(func)
    {
    }

    // 创建子类，使线程可以调用类内方法
    class ThreadData
    {
    public:
    public:
        ThreadData(int fd, InetAddr &ar, TcpServer *s) : sockfd(fd), addr(ar), tsvr(s)
        {
        }

        int sockfd;
        InetAddr addr;
        TcpServer *tsvr;
    };

    static void *Routine(void *args)
    {
        pthread_detach(pthread_self()); // 自我分离，系统回收
        ThreadData *td = static_cast<ThreadData *>(args);

        // 通过内部类调用类方法
        td->tsvr->Service(td->sockfd, td->addr);
        delete td;
        return nullptr;
    }

    void Init()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建TCP套接字
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socket success: " << _listensockfd;

        InetAddr local(_port);
        int n = bind(_listensockfd, local.NetAddrPtr(), local.NetAddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success: " << _listensockfd;
        n = listen(_listensockfd, backlog); // 转化为监听套接字
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen success";
    }

    void Service(int sockfd, InetAddr &peer)
    {
        char buffer[1024];
        // 1. 先读取数据
        // a. n>0: 读取成功
        // b. n<0: 读取失败
        // c. n==0: 对端把链接关闭了，读到了文件的结尾 --- pipe
        while (true)
        {
            ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                LOG(LogLevel::DEBUG) << peer.StringAddr() << " #" << buffer;

                std::string echo_string = _func(buffer, peer); // 回调处理
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                LOG(LogLevel::DEBUG) << peer.StringAddr() << " 退出了...";
                close(sockfd);
                break;
            }
            else
            {
                LOG(LogLevel::DEBUG) << peer.StringAddr() << " 异常...";
                close(sockfd);
                break;
            }
        }
    }

    void Run()
    {
        _isrunning = true;
        while (_isrunning)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, CONV(peer), &len); // 取走信息
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                continue;
            }

            InetAddr addr(peer);
            LOG(LogLevel::INFO) << "accept success,peer addr: " << addr.StringAddr();

            // version 0 (单线程版本)
            // Service(sockfd, addr);

            // version 1 (多进程版本)
            // signal(SIGCHLD, SIG_IGN); // 忽略子进程退出码，系统自动回收
            // pid_t id = fork();  // 让子进程去执行服务，避免被服务器被一个客户端卡住，无法处理其他客户端
            // if (id == 0) // 子进程
            // {
            //     close(_listensockfd); // 关闭监听套接字
            //     Service(sockfd, addr);
            //     exit(OK);
            // }
            // // 父进程
            // else
            // {
            //     close(sockfd); // 关闭读写套接字
            // }

            // version 2 (多线程版本)
            ThreadData *td = new ThreadData(sockfd, addr, this);
            pthread_t pid;
            pthread_create(&pid, nullptr, Routine, td);  // 子线程执行服务

            // version3 (线程池版本)
        }
        _isrunning = false;
    }

    ~TcpServer()
    {
    }

private:
    uint16_t _port;
    int _listensockfd; // 监听套接字
    bool _isrunning;
    fun_t _func; // 回调处理
};
