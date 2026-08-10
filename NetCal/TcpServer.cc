#include "TcpServer.hpp"
#include <memory>
#include "Common.hpp"
#include "Protocol.hpp"
#include "NetCal.hpp"
#include "Daemon.hpp"

void Usage(std::string proc)
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

    std::cout << "服务器已经启动，已经是一个守护进程了" << std::endl;
    Daemon(1, 0);

    Enable_File_Log_Strategy();  // 将日志写进文件

    // 1.应用层--网络计算器
    std::unique_ptr<NetCal> cal = std::make_unique<NetCal>();

    // 2.协议层
    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>(
        [&cal](Request &req) -> Response
        {
            return cal->Execute(req);
        });

    // 3.服务层
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(std::stoi(argv[1]),
                                                                  [&protocol](std::shared_ptr<Socket> &sock, InetAddr &client)
                                                                  {
                                                                      protocol->GetRequest(sock, client);
                                                                  });
    tsvr->Start();

    return 0;
}