#include "comm.hpp"

int main()
{
    NamedFifo fifo(PATH, FILENAME); // 创建命名管道
    // 文件操作
    FileOper fo_read(PATH,FILENAME);
    fo_read.OpenForRead();
    fo_read.Read();
    fo_read.Close();
}