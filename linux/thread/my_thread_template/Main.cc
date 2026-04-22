#include "thread_template.hpp"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace yzq;

class ThreadData
{
public:
    pthread_t tid;
    std::string name;
};

void Count(ThreadData td)
{
    int cnt = 5;
    while (cnt--)
    {
        std::cout << "我是一个新线程" << std::endl;
        sleep(1);
    }
}

int main()
{
    ThreadData td;
    thread<ThreadData> t(Count, td);

    t.Start();

    t.Join();
    return 0;
}