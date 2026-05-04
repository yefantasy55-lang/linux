#pragma once
// 阻塞队列的实现(生产者-消费者模型)

#include <iostream>
#include <string>
#include <queue>
#include <pthread.h>
#include "Mutex.hpp"
#include "Cond.hpp"

using namespace yzq;

const int defaultcap = 5; // 默认队列容量

template <typename T>
class BlockQueue
{
private:
    bool IsEmpty()
    {
        return _q.empty();
    }
    bool IsFull()
    {
        return _q.size() >= _cap;
    }

public:
    BlockQueue(int cap = defaultcap)
        : _cap(cap), _psleep_num(0), _csleep_num(0)
    {
    }

    ~BlockQueue()
    {
    }

    // 生产者生产
    void Equeue(const T &in)
    {
        {
            LockGuard lg(_mutex);
            while (IsFull()) // 队满休眠
            {
                _psleep_num++; // 生产者进来的休眠个数加一
                std::cout << "生产者，进入休眠了: _psleep_num" << _psleep_num << std::endl;
                _producer_cond.Wait(_mutex);
                _psleep_num--;
            }
            _q.push(in);         // 确保队列一定有空间
            if (_csleep_num > 0) // 唤醒休眠的消费者
            {
                std::cout << "唤醒消费者..." << std::endl;
                _consumer_cond.Signal();
            }
        }
    }

    // 消费者消费
    T Pop()
    {
        T data;
        {
            LockGuard lg(_mutex);
            while (IsEmpty()) // 队空休眠
            {
                _csleep_num++;
                std::cout << "生产者，进入休眠了: _csleep_num" << _csleep_num << std::endl;
                _consumer_cond.Wait(_mutex);
                _csleep_num--;
            }
            data = _q.front(); // 取出队头
            _q.pop();
            if (_psleep_num > 0) // 唤醒休眠的生产者
            {
                _producer_cond.Signal();
                std::cout << "唤醒生产者..." << std::endl;
            }
        }
        return data;
    }

private:
    std::queue<T> _q;    // 临界资源(超市)
    int _cap;            // 队列容量
    Mutex _mutex;        // 互斥锁
    Cond _producer_cond; // 生产者条件变量
    Cond _consumer_cond; // 消费者条件变量
    int _psleep_num;     // 生产者休眠个数
    int _csleep_num;     // 消费者休眠个数
};