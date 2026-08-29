#include <iostream>
#include <string>
#include "Reactor.hpp"
#include "Common.hpp"
#include "Protocol.hpp"
#include "Listener.hpp"
#include "Connection.hpp"
#include "Log.hpp"
#include "NetCal.hpp"

static void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    Enable_Console_Log_Strategy();
    uint16_t port = std::stoi(argv[1]);

    // 1.应用层(业务模块)
    std::shared_ptr<NetCal> cal = std::make_shared<NetCal>();

    // 2.协议层(处理数据)
    std::shared_ptr<Protocol> protocol = std::make_shared<Protocol>([&cal](Request &req) -> Response
                                                                    { return cal->Execute(req); });

    // 3.创建监听套接字
    std::shared_ptr<Connection> listensock = std::make_shared<Listener>(port);
    listensock->RegisterHandler([&protocol](std::string &inbuffer) -> std::string
                                {
        LOG(LogLevel::DEBUG) << "进入到匿名函数中...";
        std::string response_str;
        while (true)
        {
            std::string package;
            if (!protocol->Decode(inbuffer, &package))
                break;

            // 完整报文
            response_str = protocol->Execute(package);
        } 
        LOG(LogLevel::DEBUG) << "结束匿名函数中...: " << response_str;
        return response_str; });

    // 4.添加到反应堆
    std::unique_ptr<Reactor>
        R = std::make_unique<Reactor>();
    R->AddConnection(listensock);
    R->Loop();

    return 0;
}