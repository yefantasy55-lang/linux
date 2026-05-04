#ifndef __PROCESS_POOL_HPP__
#define __PROCESS_POOL_HPP__

#include <iostream>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include "Task.hpp"

// 管道
class Channel
{
public:
    Channel(int wfd, int subid) : _wfd(wfd), _subid(subid)
    {
        _name = "channel-" + std::to_string(_wfd) + "-" + std::to_string(_subid);
    }
    ~Channel()
    {
    }

    // 发送信号
    void Send(int code)
    {
        write(_wfd, &code, sizeof(code));
    }

    // 关闭写端
    void Close()
    {
        close(_wfd);
    }

    // 回收子进程
    void Wait()
    {
        waitpid(_subid, nullptr, 0); // 阻塞等待
    }

    int Fd()
    {
        return _wfd;
    }

    pid_t SubId()
    {
        return _subid;
    }

    std::string Name()
    {
        return _name;
    }

private:
    int _wfd;
    pid_t _subid;
    std::string _name;
    // int _loadnum;  // 负载因子
};

// 管道统一管理
class ChannelManager
{
public:
    ChannelManager()
    {
    }

    // 插入管道
    void Insert(int wfd, int subid)
    {
        _channels.emplace_back(wfd, subid);
    }

    void PrintChannel()
    {
        for (auto &channel : _channels)
        {
            std::cout << channel.Name() << std::endl;
        }
    }

    // 采用轮询分配资源
    Channel &Select()
    {
        auto &c = _channels[_next];
        _next++;
        _next %= _channels.size();
        return c;
    }

    // 停止所有子进程写端
    void StopSubProcess()
    {
        for (auto &channel : _channels)
        {
            channel.Close();
            std::cout << "关闭: " << channel.Name() << std::endl;
        }
    }

    // 回收所有子进程
    void WaitSubProcess()
    {
        for (auto &channel : _channels)
        {
            channel.Wait();
            std::cout << "回收: " << channel.Name() << std::endl;
        }
    }

    // 关闭所有子进程的写端
    void CloseAll()
    {
        for (auto &channel : _channels)
        {
            channel.Close();
        }
    }

    // 停止并回收
    void CloseAndWait()
    {
        // 错误例子(写端有多个指向)
        /*
        for(auto &channel:_channels)
        {
            channel.Close();
            channel.Wait();
        }
        */

        // 解决办法1(倒着关闭)
        /*
        for (int i = _channels.size() - 1; i >= 0; i--)
        {
            _channels[i].Close();
            _channels[i].Wait();
            std::cout << "关闭: " << _channels[i].Name() << std::endl;
            std::cout << "回收: " << _channels[i].Name() << std::endl;
        }
        */

        // 解决方法2(子进程在继承时关闭指向所有指向的写端，确保写端唯一，CloseAll函数实现)
        for (auto &channel : _channels)
        {
            channel.Close();
            std::cout << "关闭: " << channel.Name() << std::endl;
            channel.Wait();
            std::cout << "回收: " << channel.Name() << std::endl;
        }
    }

    ~ChannelManager()
    {
    }

private:
    std::vector<Channel> _channels;
    int _next = 0;
};

const int gdefaultnum = 5; // 默认创建进程数

// 进程池
class ProcessPool
{
public:
    ProcessPool(int num) : _process_num(num)
    {
        _tm.Register(Download); // 下载任务
        _tm.Register(Upload);   // 上传任务
        _tm.Register(PrintLog); // 打印日志
    }

    // 子进程工作
    void Work(int rfd)
    {
        while (true)
        {
            int code = 0; // 任务码
            ssize_t n = read(rfd, &code, sizeof(code));
            if (n > 0)
            {
                if (n != sizeof(code))
                {
                    continue;
                }
                std::cout << "子进程[" << getpid() << "]收到一个任务码: " << code << std::endl;
                _tm.Execute(code);
            }
            else if (n == 0)
            {
                std::cout << "子进程退出" << std::endl;
                break;
            }
            else
            {
                std::cout << "读取错误" << std::endl;
                break;
            }
        }
    }

    void Debug()
    {
        _cm.PrintChannel();
    }

    // 开始
    bool Start()
    {
        for (int i = 0; i < _process_num; i++)
        {
            int fd[2] = {0};    // fd[0]为读端,fd[1]为写端
            pipe(fd);           // 创建管道
            int subid = fork(); // 创建子进程
            if (subid < 0)
            {
                perror("fork error");
                return false;
            }
            else if (subid == 0)
            { // 子进程负责读
                close(fd[1]);
                _cm.CloseAll(); // 关闭历史打开的所有~~写端
                Work(fd[0]);
                close(fd[0]);
                exit(0);
            }
            else
            { // 父进程负责写入
                close(fd[0]);
                _cm.Insert(fd[1], subid); // 插入管道
            }
        }
        return true;
    }

    // 运行
    void Run()
    {
        // 1. 选择一个任务
        int taskcode = _tm.Code();

        // 2. 选择一个信道[子进程],负载均衡的选择一个子进程，完成任务
        auto &c = _cm.Select();
        std::cout << "选择了一个子进程: " << c.Name() << std::endl;
        // 3. 发送任务
        std::cout << "发送了一个任务码: " << taskcode << std::endl;
        c.Send(taskcode);
    }

    // 停止一切操作
    void Stop()
    {
        // _cm.StopSubProcess();
        // _cm.WaitSubProcess();
        _cm.CloseAndWait();
    }

    ~ProcessPool()
    {
    }

private:
    ChannelManager _cm;
    int _process_num;
    TaskManager _tm;
};

#endif