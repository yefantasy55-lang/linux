#include "Log.hpp"
#include "ThreadPool.hpp"
#include "Task.hpp"
#include <memory>

using namespace yzq;

int main()
{
    Enable_Console_Log_Strategy();
    // ThreadPool<task_t> tp;
    // ThreadPool<task_t> tp1 = tp;
    // ThreadPool<task_t> *tp = new ThreadPool<task_t>();

    // tp->Start();
    // 有一个单例了! 如果线程池本身，会被多线程获取呢？？
    int count = 10;
    while (count)
    {
        sleep(1);
        ThreadPool<task_t>::GetInstance()->Enqueue(Download);
        count--;
    }

    ThreadPool<task_t>::GetInstance()->Stop();
    ThreadPool<task_t>::GetInstance()->Join();

    // tp->Stop();
    // Enable_Console_Log_Strategy();
    // LOG(LogLevel::DEBUG) << "hello world" << 3.141;
    // LOG(LogLevel::DEBUG) << "hello world" << 3.142;

    // Enable_File_Log_Strategy();
    // LOG(LogLevel::DEBUG) << "hello world" << 3.143;
    // LOG(LogLevel::DEBUG) << "hello world" << 3.144;

    // std::unique_ptr<LogStrategy> strategy = std::make_unique<ConsoleLogStrategy>(); // C++14
    // std::unique_ptr<LogStrategy> strategy = std::make_unique<FileLogStrategy>(); // C++14
    // strategy->SyncLog("hello log!");
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world," << 3.14 << " " << 8899 << "aaaa";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    // logger(LogLevel::DEBUG, "main.cc", 10) << "hello world";
    return 0;
}