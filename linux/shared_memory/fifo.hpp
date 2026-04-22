#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "comm.hpp"

#define PATH "."
#define FILENAME "fifo"


class NamedFifo
{
public:
    NamedFifo(const std::string &path, const std::string &name)
        : _path(path), _name(name)
    {
        _fifoname = _path + "/" + _name;
        umask(0);
        // 新建管道
        int n = mkfifo(_fifoname.c_str(), 0666);
        if (n < 0)
        {
            ERR_EXIT("mkfifo");
        }
        else
        {
            std::cout << "mkfifo success" << std::endl;
        }
    }
    ~NamedFifo()
    {
        // 删除管道文件
        int n = unlink(_fifoname.c_str());
        if (n == 0)
        {
            // ERR_EXIT("unlink"); // bug在这里，先析构fifo，导致shm的析构没有被调用
        }
        else
        {
            std::cout << "remove fifo failed" << std::endl;
        }
    }

private:
    std::string _path;
    std::string _name;
    std::string _fifoname;
};

class FileOper
{
public:
    FileOper(const std::string &path, const std::string &name)
        : _path(path), _name(name), _fd(-1)
    {
        _fifoname = _path + "/" + _name;
    }
    void OpenForRead()
    {
        // 打开, write 方没有执行open的时候，read方，就要在open内部进行阻塞
        // 直到有人把管道文件打开了，open才会返回！
        _fd = open(_fifoname.c_str(), O_RDONLY);
        if (_fd < 0)
        {
            ERR_EXIT("open");
        }
        std::cout << "open fifo success" << std::endl;
    }
    void OpenForWrite()
    {
        // write
        _fd = open(_fifoname.c_str(), O_WRONLY);
        if (_fd < 0)
        {
            ERR_EXIT("open");
        }
        std::cout << "open fifo success" << std::endl;
    }
    void Wakeup()
    {
        // 写入操作
        char c = 'c';
        int n = write(_fd, &c, 1);
        printf("尝试唤醒: %d\n", n);
    }
    bool Wait()
    {
        char c;
        int number = read(_fd, &c, 1);
        if(number > 0) 
        {
            printf("醒来: %d\n", number);
            return true;
        }
        return false;
    }
    void Close()
    {
        if (_fd > 0)
            close(_fd);
    }
    ~FileOper()
    {
    }

private:
    std::string _path;
    std::string _name;
    std::string _fifoname;
    int _fd;
};