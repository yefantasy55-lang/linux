#pragma once

#include <iostream>
#include <vector>
#include <ctime>

typedef void (*task_t)(); // 函数指针

////////////////debug/////////////////////
void PrintLog()
{
    std::cout << "我是一个打印日志的任务" << std::endl;
}

void Download()
{
    std::cout << "我是一个下载的任务" << std::endl;
}

void Upload()
{
    std::cout << "我是一个上传的任务" << std::endl;
}
//////////////////////////////////////

class TaskManager
{
public:
    TaskManager()
    {
        srand((unsigned int)time(nullptr));
    }

    void Register(task_t t)
    {
        _tasks.emplace_back(t);
    }

    int Code()
    {
        return rand() % _tasks.size();
    }

    void Execute(int code)
    {
        if (code >= 0 && code < _tasks.size())
        {
            _tasks[code](); // 调用对应的方法
        }
    }

    ~TaskManager()
    {
    }

private:
    std::vector<task_t> _tasks;
};