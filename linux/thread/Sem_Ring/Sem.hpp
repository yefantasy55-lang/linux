#include <iostream>
#include <semaphore.h>
#include <pthread.h>

namespace yzq
{
    const int defaultvalue = 1; // 默认信号量为1，即互斥

    class Sem
    {
        
    public:
        Sem(unsigned int  value = defaultvalue)
        {
            sem_init(&_sem,0,value);   // 初始化信号量
        }   

        void P()
        {
            sem_wait(&_sem);
        }

        void V()
        {
            sem_post(&_sem);
        }

        ~Sem()
        {
            sem_destroy(&_sem);
        }

    private:
        sem_t _sem; // 定义信号量
    };
};
