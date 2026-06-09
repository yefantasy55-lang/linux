#include "TcpServer.hpp"
#include "Common.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

std::string defaulthandler(const std::string &word, InetAddr &addr)
{
    LOG(LogLevel::DEBUG) << "回调到了defaulthandler";
    std::string s = "haha, ";
    s += word;
    return s;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port = std::stoi(argv[1]);
    Enable_Console_Log_Strategy();
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port, defaulthandler);

    tsvr->Init();
    tsvr->Run();

    return 0;
}