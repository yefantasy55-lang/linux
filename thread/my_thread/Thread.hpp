#pragma once
#include <iostream>
#include <pthread.h>
#include <cstring>
#include <string>
#include <functional>

namespace yzq
{
    static uint32_t number = 1;
    class thread
    {
        using func_t = std::function<void()>;
        void EnableDetach()
        {
            std::cout << "线程被分离了" << std::endl;
            _isdetach = true;
        }

        void EnableRunning()
        {
            _isrunning = true;
        }

        static void *Routine(void *args) // 用static修饰，因为如果是类内函数其第一个参数为this
        {
            thread *self = static_cast<thread *>(args);
            self->EnableRunning(); // 创建后直接运行
            if (self->_isdetach)
            {
                self->Detach();
            }
            pthread_setname_np(self->_tid, self->_name.c_str());
            self->_func(); // 回调
            return nullptr;
        }

    public:
        thread(func_t func)
            : _tid(0), _isdetach(false), _isrunning(false), _res(nullptr), _func(func)
        {
            _name = "thread-" + std::to_string(number++);
        }

        // 启动线程
        bool Start()
        {
            if (_isrunning)
            {
                return false;
            }
            int n = pthread_create(&_tid, nullptr, Routine, this);
            if (n != 0) // 创建失败
            {
                std::cerr << "create thread error: " << strerror(n) << std::endl;
                return false;
            }
            else
            {
                std::cout << _name << " create success" << std::endl;
                return true;
            }
        }

        // 分离线程
        bool Detach()
        {
            if (_isdetach)
                return false;
            if (_isrunning)
                pthread_detach(_tid);
            EnableDetach();
            return true;
        }

        // 中止线程
        bool Stop()
        {
            if (!_isrunning)
                return false;
            int n = pthread_cancel(_tid);
            if (n != 0)
            {
                std::cerr << "create thread error: " << strerror(n) << std::endl;
                return false;
            }
            else
            {
                _isrunning = false;
                std::cout << _name << " stop" << std::endl;
                return true;
            }
        }

        // 回收线程
        bool Join()
        {
            if (_isdetach)
            {
                std::cout << "你的线程已经是分离的了,不能进行join" << std::endl;
                return false;
            }
            int n = pthread_join(_tid, &_res);
            if (n != 0)
            {
                std::cerr << "create thread error: " << strerror(n) << std::endl;
                return false;
            }
            else
            {
                std::cout << "join success" << std::endl;
                return true;
            }
        }

        ~thread() {}

    private:
        pthread_t _tid;    // 线程号
        std::string _name; // 线程名
        bool _isdetach;    // 分离状态
        bool _isrunning;   // 运行状态
        void *_res;        // 线程返回值
        func_t _func;      // 线程执行的函数
    };
}