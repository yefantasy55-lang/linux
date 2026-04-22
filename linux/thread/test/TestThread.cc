#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <thread>
#include <string>
#include <vector>

// -----------------------------------线程局部存储---------------------------------------------------
// __thread  int cnt = 0;   // __thread让变量转化为线程局部存储

// void *test(void *args)
// {
//     for (int i = 0; i < 10; i++)
//     {
//         cnt++;
//         sleep(1);
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, test, nullptr);
//     while (true)
//     {
//         std::cout << cnt << std::endl;
//         sleep(1);
//     }
//     return 0;
// }

// -----------------------------------多线程---------------------------------------------------
// void *test(void *args)
// {
//     sleep(1);
//     std::string name = static_cast<const char *>(args);
//     delete[] (char*)args;
//     std::cout << "new线程名字: " << name << std::endl;
//     return nullptr;
// }

// int main()
// {
//     std::vector<pthread_t> tids; // 管理线程
//     // 创建线程
//     for (int i = 0; i < 10; i++)
//     {
//         pthread_t tid;
//         char *ch = new char[100];
//         snprintf(ch, 100, "thread-%d", i);
//         int n = pthread_create(&tid, nullptr, test, ch);
//         if (n == 0)
//             tids.push_back(tid);
//         else
//             continue;
//         sleep(1);
//     }
//     // 回收线程(一个个等待)
//     for (int i = 0; i < 10; i++)
//     {
//         pthread_join(tids[i], nullptr);
//     }
//     return 0;
// }

// -----------------------------------线程分离---------------------------------------------------
// void *test(void *args)
// {
//     // pthread_cancel(pthread_self());  // 新线程自己分离
//     std::string name = (const char *)args;
//     int cnt = 5;
//     while (cnt--)
//     {
//         sleep(1);
//         std::cout << "我是新线程: name: " << name << ", pid: " << getpid() << std::endl;
//     }
//     return (void *)123;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, test, (void *)"hello");
//     std::cout << "新线程被分离" << std::endl;
//     pthread_detach(tid);  // 分离新线程后，主线程就不用join阻塞等待子进程退出了
//     printf("主线程码：%lx\n", pthread_self());
//     printf("次线程码：%lx\n", tid);
//     int cnt = 5;
//     while (cnt--)
//     {
//         std::cout << "我是主线程..." << ", pid: " << getpid() << std::endl;
//         sleep(1);
//     }
//     void *ret = nullptr;
//     std::cout << "ret is : " << (long long int)ret << std::endl;
//     return 0;
// }

// -----------------------------------线程退出---------------------------------------------------
// class Task
// {
// public:
//     Task(int a, int b) : _a(a), _b(b)
//     {
//     }
//     int Execute()
//     {
//         return _a + _b;
//     }
//     ~Task() {}

// private:
//     int _a;
//     int _b;
// };

// class Result
// {
// public:
//     Result(int result) : _result(result)
//     {
//     }
//     void RET_VAL()
//     {
//         std::cout << _result << std::endl;
//     }
//     ~Result() {}

// private:
//     int _result;
// };

// void *routine(void *args)
// {
//     Task *t = static_cast<Task *>(args);
//     sleep(100);
//     Result *r = new Result(t->Execute());
//     return (void *)r;
// }

// int main()
// {
//     pthread_t tid;
//     Task *t = new Task(10, 20);
//     pthread_create(&tid, nullptr, routine, (void *)t);
//     sleep(3);
//     pthread_cancel(tid); // 取消新线程
//     // Result *r = nullptr;
//     void *result = nullptr;
//     // pthread_join(tid, (void **)&r);
//     // r->RET_VAL();
//     pthread_join(tid, &result);
//     std::cout << (long long)result << std::endl;  // 线程取消后返回码是-1
//     delete t;
//     // delete r;
//     return 0;
// }

// -----------------------------------线程码和返回码---------------------------------------------------
// void *test(void *args)
// {
//     std::string name = (const char *)args;
//     int cnt = 5;
//     while (cnt--)
//     {
//         sleep(1);
//         std::cout << "我是新线程: name: " << name << ", pid: " << getpid() << std::endl;
//     }
//     return (void *)123;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, test, (void *)"hello");
//     printf("主线程码：%lx\n", pthread_self());
//     printf("次线程码：%lx\n", tid);
//     int cnt = 5;
//     while (cnt--)
//     {
//         std::cout << "我是主线程..." << ", pid: " << getpid() << std::endl;
//         sleep(1);
//     }
//     void *ret = nullptr;
//     pthread_join(tid, &ret);
//     std::cout << "ret is : " << (long long int)ret << std::endl;
//     return 0;
// }

// -----------------------------------C++11线程封装---------------------------------------------------
// void test()
// {
//     while (true)
//     {
//         sleep(1);
//         std::cout << "我是新线程: pid: " << getpid() << std::endl;
//     }
// }

// int main()
// {
//     std::thread t(test);
//     while (true)
//     {
//         std::cout << "我是主线程..." << ", pid: " << getpid() << std::endl;
//         sleep(1);
//     }
//     t.join();
//     return 0;
// }

// -----------------------------------线程原生库---------------------------------------------------
// void *test(void *args)
// {
//     std::string name = (const char *)args;
//     while (true)
//     {
//         sleep(1);
//         std::cout << "我是新线程: name: " << name << ", pid: " << getpid() << std::endl;
//     }
//     return nullptr;
// }

// int main()
// {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, test, (void *)"hello");
//     while (true)
//     {
//         std::cout << "我是主线程..." << ", pid: " << getpid() << std::endl;
//         sleep(1);
//     }
//     return 0;
// }