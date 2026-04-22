
#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>

#include <errno.h>

#include <string.h>

#define NUM 3

typedef void (*func)();

// 上传
void UPLOAD(){
  printf("正在上载...\n");
}  

// 下载
void DOWNLOAD(){
  printf("正在下载...\n");
}

// 更新
void UPDATE(){
  printf("正在更新...\n");
}

func register_fun[NUM+1];

int main(){
   pid_t id = fork();
   if(id == 0){
    //int cnt = 5;
    while(1){
      printf("我是子进程:%d\n",getpid());
      sleep(2);
    }
    exit(5);
   }
   else{
     int status = 0;
     //pid_t rid = waitpid(id,&status,0);  // 默认阻塞父进程
    // pid_t rid = waitpid(id,&status,WNOHANG);  // 不阻塞父进程
     waitpid(id,&status,WNOHANG);
     register_fun[0]=UPLOAD;
     register_fun[1]=DOWNLOAD;
     register_fun[2]=UPDATE;
     register_fun[3]=NULL;

     for(int i = 0; i < NUM + 1; i++){
       if(register_fun[i]==NULL) break;
       register_fun[i](); // 通过函数指针调用函数
       sleep(5);
     }
     wait(&status); // 阻塞，继续等待子进程完成
    // status = WEXITSTATUS(status); // 退出码
    //  printf("进程成功退出，退出码为:%d,退出的子程序为:%d\n",status,rid);
   }
   return 0;
}

// 僵尸进程
//int main() {
//
//  pid_t pid = fork();
//
//   
//
//  if (pid < 0) {
//
//    // fork失败
//
//    perror("fork failed");
//
//    exit(1);
//
//  } else if (pid == 0) {
//
//    // 子进程
//
//    printf("子进程 PID=%d 开始运行\n", getpid());
//
//    sleep(2); // 子进程运行2秒
//
//    printf("子进程 PID=%d 结束运行\n", getpid());
//
//    exit(0);  // 子进程退出，成为僵尸
//
//  } else {
//
//    // 父进程
//
//    printf("父进程 PID=%d 创建了子进程 PID=%d\n", getpid(), pid);
//
//    printf("父进程不调用wait()，子进程将成为僵尸\n");
//
//     
//
//    // 父进程睡眠30秒，期间子进程为僵尸状态
//
//    for (int i = 1; i <= 30; i++) {
//
//      printf("父进程运行第 %d 秒（子进程为僵尸状态）\n", i);
//
//      sleep(1);
//
//    }
//
//     
//
//    // 最后回收子进程
//
//    int status;
//
//    waitpid(pid, &status, 0);
//
//    printf("父进程回收了子进程\n");
//
//  }
//  return 0;
//}

// 进程
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
