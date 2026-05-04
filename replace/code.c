#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>

int main(){
   pid_t id = fork();
   if(id==0){
     //execl("/usr/bin/ls","ls","-l","-a",NULL); // 最后必须以NULL结尾
     //execlp("./code_cc","code_cc",NULL); // 自动从系统环境变量去找该命令
     //char* const argv[]={
     //  (char* const)"ls",
     //  (char* const)"-ll",
     //  (char* const)"-a",
     //  NULL 
     //};
     //execv("/usr/bin/ls",argv);
     char* const argv[]={
       (char* const)"./code_cc",
       NULL
     };
     //char* const env[]={
     //  (char* const)"USER=YZQ&&XRF",
     //   NULL 
     //};
     //execle("/usr/bin/ls","ls","-ll","-a",NULL,env);
     //exe1cve("/usr/bin/ls",argv,env);  // 唯一的系统调用接口，其他都是c包装的库
     //execvp(argv[0],argv);
     setenv("USER","YZQ&&XRF",1);
     extern char** environ;  // 显示传给环境变量表指针
     execvpe(argv[0],argv,environ);
     exit(1);  // 执行失败返回非零码
   }
   waitpid(-1,NULL,0);
   printf("进程执行结束\n");
}
