#include <iostream>
#include "Common.hpp"
#include "InetAddr.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(SOCKET_ERR);
    }

    InetAddr serveraddr(server_ip, server_port);
    int n = connect(sockfd, serveraddr.NetAddrPtr(), serveraddr.NetAddrLen()); // 将套接字与目标服务器建立连接
    if (n < 0)
    {
        std::cerr << "connect error" << std::endl;
        exit(CONNECT_ERR);
    }

    // 收发消息
    while (true)
    {
        std::string line;
        std::cout << "Please Enter@ ";
        std::getline(std::cin, line);

        // 写（发送消息）
        write(sockfd, line.c_str(), line.size());

        // 读（接收消息）
        char buffer[1024];
        ssize_t size = read(sockfd, buffer, sizeof(buffer) - 1);
        if (size > 0)
        {
            buffer[n] = 0;
            std::cout << "server echo# " << buffer << std::endl;
        }
    }

    close(sockfd);
    return 0;
}