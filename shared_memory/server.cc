#include "Shm.hpp"
#include "fifo.hpp"

int main()
{
    Shm shm(PATH, projid, CREATER); // 创建共享内存
    NamedFifo fifo(PATH, FILENAME); // 创建命名管道
    // 文件操作
    FileOper readerfile(PATH, FILENAME);
    readerfile.OpenForRead();
    // 取到虚拟内存
    char *mem = (char *)shm.VirtualAddr();
    while (true)
    {
        if (readerfile.Wait()) // 唤醒成功才继续读取
        {
            printf("%s\n", mem);
        }
        else  // 客户端的命名管道关闭，服务端也关闭
        {
            break;   
        }
    }
    readerfile.Close();
    std::cout << "server end normal!" << std::endl;
    return 0;
}