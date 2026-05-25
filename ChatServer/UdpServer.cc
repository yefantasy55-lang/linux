#include <iostream>
#include <memory>
#include "Dict.hpp"      // 翻译的功能
#include "UdpServer.hpp" // 网络通信的功能

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

    Enable_Console_Log_Strategy();

    Dict dict;
    dict.LoadDict(); // 加载词典

    uint16_t port = std::stoi(argv[1]); // 将传进来的字符串端口转为int
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(port, [&dict](const std::string word, InetAddr &client)
                                                                  { return dict.Translate(word, client); });
    usvr->Init();  // 初始化套接字
    usvr->Start(); // 启动套接字
    return 0;
}