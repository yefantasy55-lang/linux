#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "Log.hpp"
#include "Thread.hpp"
#include "Cond.hpp"
#include "Mutex.hpp"

namespace yzq
{
    using namespace yzq;
    static const int gnum = 15;
    template <typename T>
    class ThreadPool
    {
    private:
        void WakeUpAllThread()
        {
            LockGuard lockguard(_mutex);
            if (_sleepernum) // 存在休眠的进程就可以被唤醒
                _cond.Broadcast();
            LOG(LogLevel::INFO) << "唤醒所有的休眠线程";
        }

        void WakeUpOne()
        {
            _cond.Signal();
            LOG(LogLevel::INFO) << "唤醒单个休眠线程";
        }

        ThreadPool(int num = gnum)
            : _num(num), _isrunning(false), _sleepernum(0)
        {
            // 各个线程依次处理分配处理任务
            for (int i = 0; i < num; i++)
            {
                _threads.emplace_back(
                    [this]
                    {
                        HandlerTask(); // 相当于this->HandlerTask();
                    });
            }
        }

        void Start()
        {
            if (_isrunning) // 已经运行了的直接返回，反正重复启动
                return;
            _isrunning = true;
            for (auto &thread : _threads)
            {
                thread.Start();
                LOG(LogLevel::INFO) << "start new thread success: " << thread.Name();
            }
        }

        // 禁用拷贝构造
        ThreadPool(const ThreadPool<T> &) = delete;
        ThreadPool<T> &operator=(const ThreadPool<T> &) = delete;

    public:
        // 懒汉模式，用单例指针实现
        static ThreadPool *GetInstance() // 用static保证了不用创建对象后，才能调用单例来创建对象
        {
            LockGuard lockguard(_lock);
            LOG(LogLevel::DEBUG) << "获取单例....";
            if (inc == nullptr)
            {
                LOG(LogLevel::DEBUG) << "首次使用单例, 创建之....";
                inc = new ThreadPool<T>(); // 调用构造函数
                inc->Start();
            }
            return inc;
        }

        void Stop()
        {
            if (!_isrunning)
                return;
            _isrunning = false;
            WakeUpAllThread(); // 唤醒所有线程处理剩下未完成的任务
        }

        void Join()
        {
            for (auto &thread : _threads)
            {
                thread.Join();
            }
        }

        void HandlerTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));
            while (true)
            {
                T t;
                {
                    LockGuard lockguard(_mutex);
                    if (_taskq.empty() && _isrunning) // 当任务队列为空且线程仍在运行时，进行阻塞等待
                    {
                        _sleepernum++;      // 休眠线程加一
                        _cond.Wait(_mutex); // 阻塞等待
                        _sleepernum--;      // 被唤醒后休眠线程减一
                    }
                    if (_taskq.empty() && !_isrunning)
                    {
                        LOG(LogLevel::INFO) << name << " 退出了, 线程池退出&&任务队列为空";
                        break;
                    }
                    // 走到这里必定存在任务
                    t = _taskq.front();
                    _taskq.pop();
                }
                t(); // 在锁外处理任务
            }
        }

        bool Enqueue(const T &in)
        {
            LockGuard lockguard(_mutex);
            if (_isrunning)
            {
                _taskq.push(in);
                if (_threads.size() == _sleepernum) // 唤醒线程来处理任务
                    WakeUpOne();
                return true;
            }
            return false;
        }

        ~ThreadPool()
        {
        }

    private:
        std::vector<Thread> _threads;
        int _num; // 线程池中，线程的个数
        std::queue<T> _taskq;
        Cond _cond;
        Mutex _mutex; // 线程锁

        bool _isrunning;
        int _sleepernum;

        static ThreadPool<T> *inc; // 单例指针
        static Mutex _lock;        // 外部锁
    };

    template <typename T>
    ThreadPool<T> *ThreadPool<T>::inc = nullptr;

    template <typename T>
    Mutex ThreadPool<T>::_lock; // 用来确保单例指针只有一个

};
