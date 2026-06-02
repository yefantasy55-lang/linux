#include "TcpServer.hpp"
#include "Common.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}



int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port = std::stoi(argv[1]);
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port,[](){});

    return 0;
}