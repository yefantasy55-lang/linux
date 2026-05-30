#include <iostream>
#include <memory>
#include "Route.hpp"
#include "UdpServer.hpp" // 网络通信的功能
#include "ThreadPool.hpp"

using namespace yzq;

std::string defaulthandler(const std::string &message)
{
    std::string hello = "hello, ";
    hello += message;
    return hello;
};

using task_t = std::function<void()>;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        exit(1);
    }

    Enable_Console_Log_Strategy();

    Route r;                                     // 路由对象，负责转发
    auto tp = ThreadPool<task_t>::GetInstance(); // 建立线程池

    uint16_t port = std::stoi(argv[1]); // 将传进来的字符串端口转为int
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port, [&r, &tp](int sockfd, const std::string &message, InetAddr &peer)
                                                                  {
        task_t t = std::bind(&Route::MessageRoute, &r, sockfd, message, peer);
        tp->Enqueue(t); });
    usvr->Init();  // 初始化套接字
    usvr->Start(); // 启动套接字
    return 0;
}