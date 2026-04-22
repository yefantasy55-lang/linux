#include "Thread.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace yzq;

int main()
{
    std::vector<thread> threads;
    int cnt = 5;
    for (int i = 0; i < 10; i++)
    {
        threads.emplace_back([cnt]() mutable
                             {
        while(cnt--)
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            std::cout << "我是一个新线程: " << name << std::endl; // 我的线程的名字是什么呀？debug
            sleep(1);
        } });
    }
    for (auto &thread : threads)
    {
        thread.Start();
    }

    for (auto &thread : threads)
    {
        thread.Join();
    }
// ----------------------------------------------------------------------------------
    // thread t([]()
    //          {
    //     while(true)
    //     {
    //         char name[128];
    //         pthread_getname_np(pthread_self(), name, sizeof(name));
    //         std::cout << "我是一个新线程: " << name << std::endl; // 我的线程的名字是什么呀？debug
    //         sleep(1);
    //     } });
    // // t.Detach();
    // t.Start();
    // t.Detach();
    // sleep(5);

    // t.Stop();

    // sleep(5);

    // t.Join();
    return 0;
}