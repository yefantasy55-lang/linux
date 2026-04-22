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

#define PATH "."        // 默认路径
#define FILENAME "fifo" // 默认文件名

// 错误处理
#define ERR_EXIT(m)         \
    do                      \
    {                       \
        perror(m);          \
        exit(EXIT_FAILURE); \
    } while (0)

// 创建命名管道
class NamedFifo
{
public:
    NamedFifo(const std::string &path, const std::string &name)
        : _path(path), _name(name)
    {
        _fifoname = path + "/" + _name;
        umask(0); // 掩码设置为0
        // 新建管道
        int n = mkfifo(_fifoname.c_str(), 0666);
        if (n < 0)
        {
            ERR_EXIT("mkfifo failure");
        }
        std::cout << "mkfifo success" << std::endl;
    }

    ~NamedFifo()
    {
        // 删除管道文件
        int n = unlink(_fifoname.c_str());
        if (n < 0)
        {
            ERR_EXIT("unlink failure");
        }
        std::cout << "unlink success" << std::endl;
    }

private:
    std::string _path;
    std::string _name;
    std::string _fifoname; // 文件路径
};

// 管道操作
class FileOper
{
public:
    FileOper(const std::string &path, const std::string &name)
        : _path(path), _name(name), _fd(-1)
    {
        _fifoname = path + "/" + _name;
    }

    // 以读模式打开管道文件
    void OpenForRead()
    {
        int fd = open(_fifoname.c_str(), O_RDONLY);
        if (fd < 0)
        {
            ERR_EXIT("open failure");
        }
        _fd = fd;
        std::cout << "open success" << std::endl;
    }

    // 以写模式打开管道文件
    void OpenForWrite()
    {
        int fd = open(_fifoname.c_str(), O_WRONLY);
        if (fd < 0)
        {
            ERR_EXIT("open failure");
        }
        _fd = fd;
        std::cout << "open success" << std::endl;
    }

    // 读取内容
    void Read()
    {
        char buffer[1024] = {0};
        while (true)
        {
            int n = read(_fd, buffer, sizeof(buffer) - 1);
            if (n > 0) // 将读到的字符输出
            {
                buffer[n] = 0;
                std::cout << "Client Say# " << buffer << std::endl;
            }
            else if (n == 0) // 写端退出读端也退出
            {
                std::cout << "client quit! me too!" << std::endl;
                break;
            }
            else
            {
                ERR_EXIT("Read failure");
            }
        }
    }

    // 写入内容
    void Write()
    {
        std::string message;
        int cnt = 1; // 用于信息编号
        pid_t id = getpid();
        while (true)
        {
            std::cout << "Please Enter# ";
            std::getline(std::cin, message);
            message += (", message number: " + std::to_string(cnt++) + ", [" + std::to_string(id) + "]");
            int n = write(_fd, message.c_str(), message.size()); // 写入管道文件
            if (n < 0)
            {
                ERR_EXIT("Write failure");
            }
        }
    }

    // 关闭管道文件
    void Close()
    {
        if (_fd > 0)
        {
            close(_fd);
        }
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