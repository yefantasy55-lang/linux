// 操作共享变量会有问题的售票系统代码
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "Mutex.hpp"
using namespace yzq;

// --------------------------------------封装锁---------------------------------------------
class ThreadData // 管理锁
{
public:
    ThreadData(const std::string &n, Mutex &lock)
        : name(n), lockp(&lock)
    {
    }

    ~ThreadData()
    {
    }

    std::string name;       // 锁名
    Mutex *lockp; // 锁
};

int ticket = 1000;

void *route(void *arg)
{
    ThreadData *td = static_cast<ThreadData *>(arg);
    while (1)
    {
        // td->lockp->Lock();
        LockGuard lg(*td->lockp);
        if (ticket > 0)                // 1. 判断
        {
            usleep(1000);                                             // 模拟抢票花的时间
            printf("%s sells ticket:%d\n", td->name.c_str(), ticket); // 2. 抢到了票
            ticket--;                                                 // 3. 票数--
            // td->lockp->UnLock();                        // 出临界区解锁
        }
        else
        {
            // td->lockp->UnLock(); // 出临界区解锁
            break;
        }
        usleep(1000);
    }
    return nullptr;
}

int main(void)
{
    pthread_t t1, t2, t3, t4;

    Mutex lock; // 局部锁需要初始化和释放
    ThreadData td1("thread 1", lock);
    ThreadData td2("thread 2", lock);
    ThreadData td3("thread 3", lock);
    ThreadData td4("thread 4", lock);

    pthread_create(&t1, NULL, route, &td1);
    pthread_create(&t1, NULL, route, &td2);
    pthread_create(&t1, NULL, route, &td3);
    pthread_create(&t1, NULL, route, &td4);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
}

// --------------------------------------局部锁---------------------------------------------
// class ThreadData // 管理锁
// {
// public:
//     ThreadData(const std::string &n, pthread_mutex_t &lock)
//         : name(n), lockp(&lock)
//     {
//     }

//     ~ThreadData()
//     {
//     }

//     std::string name;       // 锁名
//     pthread_mutex_t *lockp; // 锁
// };

// int ticket = 1000;

// void *route(void *arg)
// {
//     ThreadData *td = static_cast<ThreadData *>(arg);
//     while (1)
//     {
//         pthread_mutex_lock(td->lockp); // 临界区加锁
//         if (ticket > 0)                // 1. 判断
//         {
//             usleep(1000);                                             // 模拟抢票花的时间
//             printf("%s sells ticket:%d\n", td->name.c_str(), ticket); // 2. 抢到了票
//             ticket--;                                                 // 3. 票数--
//             pthread_mutex_unlock(td->lockp);                          // 出临界区解锁
//         }
//         else
//         {
//             pthread_mutex_unlock(td->lockp); // 出临界区解锁
//             break;
//         }
//         usleep(1000);
//     }
//     return nullptr;
// }

// int main(void)
// {
//     pthread_t t1, t2, t3, t4;

//     pthread_mutex_t lock; // 局部锁需要初始化和释放
//     ThreadData td1("thread 1", lock);
//     ThreadData td2("thread 2", lock);
//     ThreadData td3("thread 3", lock);
//     ThreadData td4("thread 4", lock);

//     pthread_mutex_init(&lock, nullptr);

//     pthread_create(&t1, NULL, route, &td1);
//     pthread_create(&t1, NULL, route, &td2);
//     pthread_create(&t1, NULL, route, &td3);
//     pthread_create(&t1, NULL, route, &td4);

//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
//     pthread_join(t3, NULL);
//     pthread_join(t4, NULL);

//     pthread_mutex_destroy(&lock); // 释放锁
// }

// --------------------------------------全局锁---------------------------------------------
// int ticket = 1000;
// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  // 全局锁程序结束会自动释放

// void *route(void *arg)
// {
//     char *id = (char *)arg;
//     while (1)
//     {
//         pthread_mutex_lock(&lock); // 临界区加锁
//         if (ticket > 0)            // 1. 判断
//         {
//             usleep(1000);                               // 模拟抢票花的时间
//             printf("%s sells ticket:%d\n", id, ticket); // 2. 抢到了票
//             ticket--;                                   // 3. 票数--
//             pthread_mutex_unlock(&lock);                // 出临界区解锁
//         }
//         else
//         {
//             pthread_mutex_unlock(&lock); // 出临界区解锁

//             break;
//         }
//     }
//     return nullptr;
// }

// int main(void)
// {
//     pthread_t t1, t2, t3, t4;

//     pthread_create(&t1, NULL, route, (void *)"thread 1");
//     pthread_create(&t2, NULL, route, (void *)"thread 2");
//     pthread_create(&t3, NULL, route, (void *)"thread 3");
//     pthread_create(&t4, NULL, route, (void *)"thread 4");

//     pthread_join(t1, NULL);
//     pthread_join(t2, NULL);
//     pthread_join(t3, NULL);
//     pthread_join(t4, NULL);
// }