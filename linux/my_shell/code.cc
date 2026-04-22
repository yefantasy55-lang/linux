#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <wait.h>
#include <fcntl.h>

#define COM_MAXSIZE 1024 // 命令行最大长度
#define COM_FORMAT "[%s@%s %s]# "  // 命令行样式

#define MAXARGC 128  // 输入命令参数最多个数

// 重定向
#define NONE_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3
int redir = NONE_REDIR;
std::string filename;


char* env[1024];  // 当前环境变量
extern char** environ;  // 获取系统的环境变量表
int envs = 0;  // 环境变量个数

int argc = 0;  // 输入命令参数个数
char* argv[MAXARGC] = {0};  // 命令行参数

int lastcode = 0;  // 程序退出码

// 获取主机名
const char* GetHostName(){
  char* HostName = getenv("HOSTNAME");
  return HostName != NULL ? HostName : "";
}

// 获取用户名
const char* GetUser(){
  char* User = getenv("USER");
  return User != NULL ? User : "";
}

// 获取当前目录
const char* GetPwd() {
 // const char* Pwd = getenv("PWD");  // 使用chdir只获取当前环境变量的值，而如果用chdir更新，它并不回更新到环境变量，导致显示上工作目录一直不变
  const char* Pwd = getcwd(nullptr,0); 
  if (!Pwd) return "";    
  const char* last_slash = strrchr(Pwd, '/');
  if (!last_slash) return "BUG?";
  if (strcmp(Pwd, "/") == 0) return "/";
  return last_slash + 1;  // 注意：这个指针仍然指向原字符串
}

// 获取用户当前家目录
const char* GetHome(){
  const char* Home = getenv("HOME");
  return Home != NULL ? Home : "";
}

// cd 
void Cd(){
  const char* new_path = nullptr;
  if(argc == 1){  // 只输入cd
     chdir(GetHome());  // 切换为当前家目录
  }
  else{
    std::string where = argv[1];
    // cd ~ - 
    if(where == "~"){
      new_path = GetHome();
      if (chdir(new_path) == 0) {
          setenv("PWD", new_path, 1);
      } else {
          perror("cd");
      }
    }
    else if(where == "-"){
      // 切换到上一个目录
      char* oldpwd = getenv("OLDPWD");
      char* current = getcwd(nullptr, 0);
      if (oldpwd && chdir(oldpwd) == 0) {
          setenv("OLDPWD", current, 1);
          setenv("PWD", oldpwd, 1);
          std::cout << oldpwd << std::endl;  // 显示新目录
      } else {
          std::cerr << "cd: OLDPWD not set" << std::endl;
      }
      free(current);
    }
    else{
      chdir(where.c_str());
    }
  }
}

// echo 
void Echo(){
  if(argc == 2){
    std::string opt = argv[1];
    if(opt == "$?"){
      std::cout << lastcode << std::endl;
      lastcode=0;
    }
    else if(opt[0] == '$')  // 第一个为$,打印对应变量
    {
      std::string env_name = opt.substr(1);  // 截取从第二个位置后的串，即$后的字符串
      const char* env_value = getenv(env_name.c_str());  // 从环境变量获取对应的值
      std::cout << env_value << std::endl;
    }
    else{
      std::cout << opt << std::endl;  // 正常输入
    }
  }
}

// 制作命令行
void MakeComLine(char cmd_prompt[], int size){
  snprintf(cmd_prompt,size,COM_FORMAT,GetHostName(),GetUser(),GetPwd());
}

// 打印命令行
void PrintComLine(){
  char cmd_prompt[COM_MAXSIZE];
  MakeComLine(cmd_prompt,sizeof(cmd_prompt));
  printf("%s",cmd_prompt);
  fflush(stdout);
}

// 获取用户输入命令
bool GetComLine(char* out,int size){
  char* c = fgets(out,size,stdin);
  if(c == NULL) return false;  // 空指针返回
  out[strlen(out)-1]=0;  // 清理掉\n
  if(strlen(out) == 0) return false;  // 长度为0返回
  return true;
}

// 重定向分析
void RedirCheck(char cmd[])
{
    redir = NONE_REDIR;
    filename.clear();
    
    int len = strlen(cmd);
    for(int i = 0; i < len; i++){
        if(cmd[i] == '<' || cmd[i] == '>'){
            char redir_char = cmd[i];
            
            // 检查是否是 >>
            bool is_append = false;
            if(redir_char == '>' && i + 1 < len && cmd[i+1] == '>'){
                is_append = true;
                redir = APPEND_REDIR;
                cmd[i] = '\0';
                cmd[i+1] = '\0';
                i += 2;  // 跳过两个字符
            } else if(redir_char == '<'){
                redir = INPUT_REDIR;
                cmd[i] = '\0';
                i++;
            } else {
                redir = OUTPUT_REDIR;
                cmd[i] = '\0';
                i++;
            }
            
            // 跳过空格找文件名
            while(i < len && isspace(cmd[i])) i++;
            
            if(i < len){
                filename = &cmd[i];
                
                // 清空文件名部分，避免被 ParseComLine 解析
                int j = i;
                while(j < len){
                    cmd[j] = '\0';
                    j++;
                }
            }
            break;
        }
    }
}

// 命令行分析
bool ParseComLine(char* comline){
#define SEP " "  // 默认空格为分隔符
  argc = 0;
  argv[argc++] = strtok(comline,SEP);
  while((bool)(argv[argc++] = strtok(nullptr,SEP))); // 后续分割只需传nullptr
  argc--;
  return argc > 0 ? true : false;
}

// 打印输入命令
void PrintArgv()
{
    for(int i = 0; argv[i]; i++)
    {
        printf("argv[%d]->%s\n", i, argv[i]);
    }
    printf("argc: %d\n", argc);
}

bool CheckAndExecBuiltin(){
  std::string cmd = argv[0];  // 使用string方便直接比较
  if(cmd == "cd"){
    Cd();
    return true;
  }
  else if(cmd == "echo"){
    Echo();
    return true;
  }
  return false;
}

// 执行命令行
void ExecuteComLine(){
  pid_t id = fork();
  // 子程序执行命令
  if(id == 0){
    int fd = -1; // 文件描述符
    if(redir == INPUT_REDIR){
       fd = open(filename.c_str(),O_RDONLY); 
       if(fd<0) exit(1);
       dup2(fd,0);  // 重定向到stdin
       close(fd);
    } 
    else if(redir == OUTPUT_REDIR){
       fd = open(filename.c_str(),O_CREAT | O_WRONLY,0666); 
       if(fd<0) exit(2);
       dup2(fd,1);  // 重定向到stdout
       close(fd);
    }
    else if(redir == APPEND_REDIR){
      fd = open(filename.c_str(),O_CREAT | O_WRONLY | O_APPEND,0666);
      dup2(fd,1);  // 重定向到stdout
      close(fd);
    }
    else{}
    execvp(argv[0],argv);
    exit(1);  // 执行失败退出程序
  }
  int status;
  pid_t rid = waitpid(id,&status,0);  // 阻塞等待
  if(rid > 0)
  {
    lastcode = WEXITSTATUS(status);
  }
}

// 初始化环境变量
void InitEnv(){
   envs = 0;
   memset(env,0,sizeof(env));
   // 1.获取环境变量（本应该得去到系统配置文件获取，这里就直接获取shell的了
   for(int i = 0;environ[i];i++){
     env[i] = new char[strlen(environ[i])+1];
     strcpy(env[i],environ[i]);  // 拷贝到环境变量
     envs++;
   }
    env[envs++] = (char*)"HAHA=for_test"; //for_test
    env[envs] = NULL;

    // 2.导入环境变量
    for(int i = 0;env[i];i++){
      putenv(env[i]);  // 将其导入到environ上，键相同更新
    }
}



int main(){
  while(true){

    InitEnv();  // 初始化环境变量

  // 1.打印命令行
    PrintComLine();

  // 2.获取用户输入命令
    char comline[COM_MAXSIZE] = {0};
    if(!GetComLine(comline,sizeof(comline))){  // 输入无效命令进入下一轮循环
       continue;
    };

  // 3.重定向分析
    RedirCheck(comline);

  // 4.命令行分析
    if (!ParseComLine(comline)){
      continue;
    }

  // (可选)打印输入命令
  //  PrintArgv();
  
  // 5. 检测并处理内键命令
    if(CheckAndExecBuiltin()){
       continue;
    }

  // 6.执行命令行
    ExecuteComLine();
  }
  delete environ;  // 异常导致退出循环释放environ
  return 0;
}
