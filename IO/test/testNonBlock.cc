#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

void SetNonBlock(int fd)
{
    int f1 = fcntl(fd, F_GETFL); // 获取当前文件描述符的状态
    if (f1 < 0)
    {
        perror("fcntl error");
        return;
    }
    fcntl(fd, F_SETFL, f1 | O_NONBLOCK); // 将fd设置为非阻塞
}

int main()
{
    SetNonBlock(0); // 将标准输入设置为非阻塞

    char buffer[1024];
    while (true)
    {
        ssize_t n = read(0, buffer, sizeof(buffer));
        if (n > 0)
        {
            buffer[n - 1]; // 去掉换行符
            std::cout << buffer << std::endl;
        }
        else if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                std::cout << "数据没有准备好..." << std::endl;
                sleep(1);
                continue;
            }
            else if (errno == EINTR) // 其他信号中断
            {
                continue;
            }
        }
        else
        {
            break;
        }
    }

    return 0;
}