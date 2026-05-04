#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <pthread.h>
#define NUM 5

int cnt = 1000;

pthread_mutex_t glock = PTHREAD_MUTEX_INITIALIZER; // 全局锁
pthread_cond_t gcond = PTHREAD_COND_INITIALIZER;   // 全局条件变量

void *threadrun(void *args)
{
    std::string name = static_cast<const char *>(args);
    delete[] (char *)args;
    while (true)
    {
        pthread_mutex_lock(&glock);        // 锁住
        pthread_cond_wait(&gcond, &glock); // 阻塞住，等待时会自动把锁解掉，唤醒时自动申请锁
        std::cout << name << " 计算: " << cnt << std::endl;
        cnt++;
        pthread_mutex_unlock(&glock);
    }
}

int main()
{
    std::vector<pthread_t> threads;
    for (int i = 0; i < 5; i++)
    {
        char *name = new char[64];
        snprintf(name, 64, "thread-%d", i);
        pthread_t tid;
        int n = pthread_create(&tid, nullptr, threadrun, name); // 创建线程
        if (n != 0)
            continue;
        threads.push_back(tid);
        sleep(1);
    }

    while (true)
    {
        std::cout << "唤醒所有线程... " << std::endl;
        pthread_cond_broadcast(&gcond); // 唤醒所有线程
        // std::cout << "唤醒一个线程... " << std::endl;
        // pthread_cond_signal(&gcond);
        sleep(1);
    }

    for (auto thread : threads)
    {
        pthread_join(thread, nullptr); // 轮询等待
    }
    return 0;
}