#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <functional>
#include <vector>
#include <wait.h>

//-----------------------------非阻塞回收子进程------------------------------------------
// void handler(int n)
// {
//     while (true)
//     {
//         // 因为子进程退出时都会发送信号，但是pending表不会记录退出个数，只会处理一次，所以需要循环检查各个子进程是否退出
//         pid_t n = waitpid(-1, nullptr, WNOHANG); // -1等待子进程,有子进程退出就回收
//         // 只有设置WNOHANG,子进程未回收返回0，成功返回子进程pid
//         if (n == 0)  
//         {
//             break;
//         }
//         else if (n < 0)
//         {
//             std::cout << "waitpid error " << std::endl;
//             break;
//         }
//     }
//     std::cout << "father get a signal: " << n << std::endl;
// }

// int main()
// {
//     signal(SIGCHLD, handler);  // 子进程退出会给父进程发送SIGCHLD信号
//     for (int i = 0; i < 10; i++)
//     {
//         pid_t id = fork();
//         if (id == 0)
//         {
//             sleep(3);
//             std::cout << "I am child, exit" << std::endl;
//             if (i <= 6)
//             {
//                 exit(3);
//             }
//             else
//                 pause();
//         }
//     }

//     while (true)
//     {
//         std::cout << "I am fater, exit" << std::endl;
//         sleep(1);
//     }
// }

//-----------------------------测试优化------------------------------------------
// volatile int flag = 0;  // volatile确保必须从内存读取到寄存器

// void handler(int signu)
// {
//     std::cout << "更改全局变量, " << flag << "-> 1" << std::endl;
//     flag = 1;
// }

// int main()
// {
//     signal(2, handler);
//     std::cout << getpid() << std::endl;
//     // 一旦进行编译优化到一定程度，操作系统就会让flag从已存在的寄存器里面读取，而不是去内存获取到寄存器再进行读取，因为main中没有显示的对flag修改
//     while (!flag);
//     std::cout << "process quit normal!" << std::endl;
//     return 0;
// }

//-----------------------------阻塞信号集------------------------------------------
// void handler(int n)
// {
//     std::cout << "hello signal: " << n << std::endl;
//     while (true)
//     {
//         sigset_t pending;
//         sigpending(&pending);
//         for (int i = 31; i >= 1; i--)
//         {
//             if (sigismember(&pending, i))
//             {
//                 std::cout << "1";
//             }
//             else
//             {
//                 std::cout << "0";
//             }
//         }
//         std::cout << std::endl;
//         sleep(1);
//     }
// }

// int main()
// {
//     struct sigaction act, oact;
//     act.sa_handler = handler; // 修改信号处理函数
//     // sa_mask是handler调用时用来屏蔽的而已,即是局部的block表，当没触发handler调用时，不起屏蔽作用
//     act.sa_flags = 0;
//     sigemptyset(&act.sa_mask);  // 清空信号屏蔽字（阻塞信号集）
//     sigaddset(&act.sa_mask, 3); // 添加信号屏蔽字
//     sigaddset(&act.sa_mask, 4);

//     sigaction(SIGINT, &act, &oact); // 对2号信号进行了捕捉, 2,3,4都屏蔽

//     while (true)
//     {
//         std::cout << "hello world: " << getpid() << std::endl;
//         sleep(1);
//     }
// }

//-----------------------------core dump------------------------------------------
// int main()
// {
//     pid_t id = fork();
//     if (id == 0)
//     {
//         sleep(2);
//         printf("hello bit\n");
//         printf("hello bit\n");
//         printf("hello bit\n");
//         printf("hello bit\n");
//         printf("hello bit\n");
//         int a = 10;
//         a /= 0;
//         printf("hello bit\n");

//         exit(1);
//     }
//     int status = 0;
//     waitpid(id, &status, 0);
//     printf("signal: %d, exit code: %d, core dump: %d\n",
//            (status & 0x7F), (status >> 8) & 0xFF, (status >> 7) & 0x1);
// }

//-----------------------------block表 pending表------------------------------------------
// void PrintPending(sigset_t &pending)
// {
//     printf("我是一个进程(%d), pending: ", getpid());

//     for (int signo = 31; signo >= 1; signo--)
//     {
//         if (sigismember(&pending, signo))
//         {
//             std::cout << "1";
//         }
//         else
//         {
//             std::cout << "0";
//         }
//     }
//     std::cout << std::endl;
// }

// void handler(int n)
// {
//     std::cout << "获得了一个信号: " << n << "., " << "pid: " << getpid() << std::endl;
// }

// int main()
// {
//     signal(SIGINT, handler);   // 自定义二号信号,如果信号没被屏蔽会正常走到handler
//     sigset_t block, old_block; // 创建block和pending表
//     sigemptyset(&block);       // 清空block表
//     sigemptyset(&old_block);

//     sigaddset(&block, SIGINT);                    // 对二号信号屏蔽
//     sigprocmask(SIG_SETMASK, &block, &old_block); // 将该表更新到系统
//     for (int i = 0; i < 32; i++) // 屏蔽所有信号
//     {
//         sigaddset(&block, i);
//     }
//     sigprocmask(SIG_SETMASK, &block, &old_block);
//     int cnt = 0;
//     while (true)
//     {
//         // 2. 获取pending信号集合
//         sigset_t pending;
//         sigpending(&pending);

//         // 3. 打印
//         PrintPending(pending);
//         if (cnt == 10)
//         {
//             // 5. 恢复对2号信号的block情况
//             std::cout << "解除对2号的屏蔽" << std::endl;
//             sigprocmask(SIG_SETMASK, &old_block, nullptr);
//         }

//         sleep(1);
//         cnt++;
//     }
//     return 0;
// }

//-----------------------------系统闹钟------------------------------------------
// int timestamp = 0;
// //////////func////////////
// void Sched()
// {
//     std::cout << "我是进程调度" << std::endl;
// }
// void MemManger()
// {
//     std::cout << "我是周期性的内存管理，正在检查有没有内存问题" << std::endl;
// }
// void Fflush()
// {
//     std::cout << "我是刷新程序，我在定期刷新内存数据，到磁盘" << std::endl;
// }
// /////////////////////////
// using fun_c = std::function<void()>;

// std::vector<fun_c> funcs;

// void signal_handler(int n)
// {
//     // std::cout << "获得了一个信号: " << n << "., " << "pid: " << getpid() << std::endl;
//     timestamp++; // 10000
//     std::cout << "##############################" << std::endl;
//     for (auto f : funcs)
//         f();
//     std::cout << "##############################" << std::endl;

//     alarm(1);
// }

// int main()
// {
//     funcs.push_back(Sched);
//     funcs.push_back(MemManger);
//     funcs.push_back(Fflush);
//     signal(SIGALRM, signal_handler); // 捕捉闹钟信号
//     alarm(1);                       // 一秒钟中止程序
//     // int cnt = 0;
//     while (true)
//     {
//         // std::cout << "hello world, " << cnt++ << " ,pid: " << getpid() << std::endl;
//         pause(); // 等到有信号才继续执行，否则就阻塞住
//     }
// }

//-----------------------------信号自定义处理------------------------------------------
// void signal_handler(int n)
// {
//     std::cout << "获得了一个信号: " << n << std::endl;
// }

// int main()
// {
//     for (int i = 1; i < 32; i++)
//     {
//         signal(i, signal_handler); // 对信号自定义处理
//     }

//     // for (int i = 1; i < 32; i++)
//     // {
//     //     if (i == 9 || i == 19)
//     //         continue;
//     //     raise(i); // 主动抛出信号
//     //     sleep(1);
//     // }

//     while (true)
//     {
//         std::cout << "hello world" << "myid:" << getpid() << std::endl;
//         abort();  // 虽然不是内核级信号，但是在调用abort还是会使用默认的6号信号
//         sleep(2);
//     }
// }