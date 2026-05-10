#include <iostream>
#include <memory>
#include "UdpServer.hpp"

std::string defaulthandler(const std::string &message)
{
    std::string hello = "hello, ";
    hello += message;
    return hello;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]); // 将传进来的字符串端口转为int
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port, defaulthandler);
    usvr->Init();  // 初始化套接字
    usvr->Start(); // 启动套接字
    return 0;
}