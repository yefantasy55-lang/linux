
#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>



int main() {

  pid_t pid = fork();

   

  if (pid < 0) {

    // fork失败

    perror("fork failed");

    exit(1);

  } else if (pid == 0) {

    // 子进程

    printf("子进程 PID=%d 开始运行\n", getpid());

    sleep(2); // 子进程运行2秒

    printf("子进程 PID=%d 结束运行\n", getpid());

    exit(0);  // 子进程退出，成为僵尸

  } else {

    // 父进程

    printf("父进程 PID=%d 创建了子进程 PID=%d\n", getpid(), pid);

    printf("父进程不调用wait()，子进程将成为僵尸\n");

     

    // 父进程睡眠30秒，期间子进程为僵尸状态

    for (int i = 1; i <= 30; i++) {

      printf("父进程运行第 %d 秒（子进程为僵尸状态）\n", i);

      sleep(1);

    }

     

    // 最后回收子进程

    int status;

    waitpid(pid, &status, 0);

    printf("父进程回收了子进程\n");

  }
  return 0;
}


//int main(){
//  //if (chdir("/home/yzq") == -1) {
//  //      perror("chdir 失败");
//  //      return 1;
//  //}
//  //fopen("a.txt","w");
//  int pid = fork();
//  if(pid==0){
//    while(1){
//      printf("你好，子进程%d\n",getpid());
//    }
//  }
//  else if(pid>0){
//    printf("你好，父进程%d\n",getppid());
//  }
//  else{
//    perror("error");
//  }
// // while(1){
// //   printf("我是一个进程！我的进程id是%d\n",getpid());
// //   printf("我的父进程是%d\n",getppid());
// //   usleep(1000000);
// // }
//  return 0;
//}
