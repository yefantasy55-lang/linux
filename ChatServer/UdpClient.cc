#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Thread.hpp"

using namespace yzq;

int sockfd = 0;
std::string server_ip;    // 服务器ip
uint16_t server_port = 0; // 服务器端口
pthread_t id;             // 接消息进程的pid

void Recv()
{
    while (true)
    {
        char buffer[1024];
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (sockaddr *)&peer, &len);
        if (n > 0)
        {
            buffer[n] = 0;
            std::cerr << buffer << std::endl; // 2
        }
    }
}

void Send()
{
    // 发送消息
    struct sockaddr_in server; // 服务器的套接字信息
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return;
    }
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;                           // 网络套接字
    server.sin_port = htons(server_port);                  // 将端口转化为大端网络序列
    server.sin_addr.s_addr = inet_addr(server_ip.c_str()); // 将ip转化为网络序列
    const std::string online = "inline";
    sendto(sockfd, online.c_str(), online.size(), 0, (sockaddr *)&server, sizeof(server));
    while (true)
    {
        std::string input; // 消息
        std::cout << "Please Enter# ";
        std::getline(std::cin, input); // 从键盘输入消息
        sendto(sockfd, input.c_str(), input.size(), 0, (struct sockaddr *)&server, sizeof(server));
        if (input == "QUIT")
        {
            pthread_cancel(id); // 取消该进程
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        return 1;
    }
    server_ip = argv[1];                     // 服务器ip
    server_port = std::stoi(argv[2]);        // 端口
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // 网络套接字(UDP)
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 2;
    }

    // 接收服务器处理完的结果
    Thread recver(Recv); // 负责收消息
    Thread sender(Send); // 负责发消息

    recver.Start();
    sender.Start();

    id = recver.Id();

    recver.Join();
    sender.Join();

    return 0;
}