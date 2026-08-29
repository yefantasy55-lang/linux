#include <iostream>
#include "Protocol.hpp"
#include "InetAddr.hpp"
#include "Common.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " server_ip server_port" << std::endl;
}

void GetDataFromStdin(int &x, int &y, char &oper)
{
    std::cout << "Please Enter x: ";
    std::cin >> x;
    std::cout << "Please Enter y: ";
    std::cin >> y;
    std::cout << "Please Enter oper: ";
    std::cin >> oper;
}

// ./tcpclient server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);

    std::shared_ptr<Socket> client = std::make_shared<TcpSocket>();
    client->BuildTcpClientSocketMethod(); // 创建tcp套接字

    if (client->Connect(server_ip, server_port) != 0) // 连接失败
    {
        std::cerr << "server connect error" << std::endl;
        exit(CONNECT_ERR);
    }

    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>();
    std::string resp_buffer; // 客户端的接收缓冲区

    while (true)
    {
        // 1. 从标准输入当中获取数据
        int x, y;
        char oper;
        GetDataFromStdin(x, y, oper);

        // 2. 构建一个请求-> 可以直接发送的字符串
        std::string req_str = protocol->BuildRequestString(x, y, oper);

        // 3. 发送请求
        client->Send(req_str);

        // 4. 获取应答
        Response resp;
        bool ret = protocol->GetResponse(client, resp_buffer, &resp);
        if (ret == false)
            break;

        // 5.展示结果
        resp.ShowResult();
    }

    client->Close();
}