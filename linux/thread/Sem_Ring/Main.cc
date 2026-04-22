#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include "RingQueue.hpp"

struct threaddata
{
    RingQueue<int> *rq;
    std::string name;
};

void *consumer(void *args)
{
    threaddata *td = static_cast<threaddata*>(args);

    while (true)
    {
        // sleep(3);
        // 1. 消费任务
        int t = 0;
        td->rq->Pop(&t);

        // 2. 处理任务 -- 处理任务的时候，这个任务，已经被拿到线程的上下文中了,不属于队列了
        std::cout << td->name << " 消费者拿到了一个数据:  " << t << std::endl;
        // t();
    }
}

int data = 1;

void *productor(void *args)
{
    threaddata *td = static_cast<threaddata*>(args);
    
    while (true)
    {
        sleep(1);
        // sleep(2);
        // 1. 获得任务
        std::cout << td->name << " 生产了一个任务: " << data << std::endl;

        // 2. 生产任务
        td->rq->Equeue(data);

        data++;
    }
}

int main()
{
    // 申请阻塞队列
    RingQueue<int> *rq = new RingQueue<int>();

    // 构建生产和消费者
    pthread_t c[2], p[3];

    threaddata *td = new threaddata();
    td->name = "cthread-1";
    td->rq = rq;
    pthread_create(c, nullptr, consumer, td);

    threaddata *td2 = new threaddata();
    td2->name = "cthread-2";
    td2->rq = rq;
    pthread_create(c + 1, nullptr, consumer, td2);

    threaddata *td3 = new threaddata();
    td3->name = "pthread-3";
    td3->rq = rq;
    pthread_create(p, nullptr, productor, td3);

    threaddata *td4 = new threaddata();
    td4->name = "pthread-4";
    td4->rq = rq;
    pthread_create(p + 1, nullptr, productor, td4);

    threaddata *td5 = new threaddata();
    td5->name = "pthread-5";
    td5->rq = rq;
    pthread_create(p + 2, nullptr, productor, td5);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);

    return 0;
}