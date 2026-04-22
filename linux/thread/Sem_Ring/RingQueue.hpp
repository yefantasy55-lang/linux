#pragma once

#include <iostream>
#include <vector>
#include "Sem.hpp"
#include "Mutex.hpp"

using namespace yzq;

static const int gcap = 5;

template <typename T>
class RingQueue
{
public:
    RingQueue(int cap = gcap)
        : _rq(cap), _cap(cap), _blank_sem(cap), _p_step(0), _data_sem(0), _c_step(0)
    {
    }

    ~RingQueue()
    {
    }

    void Equeue(const T &in)
    {
        _blank_sem.P(); // 申请信号量,如果申请的信号量没了，线程就会被阻塞住
        {
            LockGuard lg(_pmutex);
            _rq[_p_step] = in; // 存放资源
            _p_step++;         // 前往下一个空位准备存放数据
            _p_step %= _cap;   // 维持环形队列
        }
        _data_sem.V(); // 释放信号量
    }

    void Pop(T *out)
    {
        _data_sem.P(); // 申请信号量,如果申请的信号量没了，线程就会被阻塞住
        {
            LockGuard lg(_cmutex);
            *out = _rq[_c_step];
            _c_step++;       // 前往下一个空位准备消耗数据
            _c_step %= _cap; // 维持环形队列
        }
        _blank_sem.V(); // 释放信号量
    }

private:
    std::vector<T> _rq;
    int _cap;

    // 生产者
    Sem _blank_sem; // 空位置
    int _p_step;
    // 消费者
    Sem _data_sem; // 数据
    int _c_step;

    // 维护多生产，多消费, 2把锁
    Mutex _cmutex;
    Mutex _pmutex;
};