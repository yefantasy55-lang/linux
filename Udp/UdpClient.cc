#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        return 1;
    }
    std::string server_ip = argv[1];             // 服务器ip
    uint16_t server_port = std::stoi(argv[2]);   // 端口
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 网络套接字(UDP)
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 2;
    }
    struct sockaddr_in server; // 服务器的套接字信息
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;                           // 网络套接字
    server.sin_port = htons(server_port);                  // 将端口转化为大端网络序列
    server.sin_addr.s_addr = inet_addr(server_ip.c_str()); // 将ip转化为网络序列

    // 发送消息
    while (true)
    {
        std::string input; // 消息
        std::cout << "Please Enter# ";
        std::getline(std::cin, input); // 从键盘输入消息
        sendto(sockfd, input.c_str(), input.size(), 0, (struct sockaddr *)&server, sizeof(server));

        // 接送服务器处理完的结果
        char buffer[1024];
        struct sockaddr_in peer;      // 服务器套接字信息
        socklen_t len = sizeof(peer); // 服务器套接字大小
        int n = recvfrom(sockfd, &buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
        if (n < 0)
        {
            std::cerr << "client rec error" << std::endl;
            return 3;
        }
        buffer[n] = 0;                    // 字符串结束
        std::cout << buffer << std::endl; // 打印返回来的结果
    }
    return 0;
}