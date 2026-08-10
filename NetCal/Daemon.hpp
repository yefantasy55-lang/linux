#pragma once

#include <iostream>
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Log.hpp"
#include "Common.hpp"

using namespace yzq;
const std::string dev = "/dev/null";

// 守护进程化
void Daemon(int nochdir, int noclose) // 第一个参数是更改工作路径，第二个参数是决定是否关闭标准输入输出
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN); // 避免产生僵尸进程

    if (fork() > 0)
        exit(0);
    setsid(); // 让子进程开启新会话
    if (nochdir == 0)
    {
        chdir("/"); // 定位到根目录
    }

    if (noclose == 0)
    {
        int fd = open(dev.c_str(), 0);
        if (fd < 0)
        {
            LOG(LogLevel::FATAL) << "open " << dev << " errno";
            exit(OPEN_ERR);
        }
        else
        {
            dup2(fd, 0);
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }
    }
}