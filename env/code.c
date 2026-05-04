#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[],char* env[]){
  (void)argc;
  (void)argv;
  (void)env;
  char* user=getenv("USER");
  if(strcmp(user,"yzq")==0){
    printf("%s主人你好~\n",user);
  }
  else {
    printf("你不是yzq!\n");
  }
  //int i=0;
  //for(i=0;env[i];i++){
  //  printf("%s\n",env[i]);
  //}
  //for(i=0;i<argc;i++){
  //  printf("argv[%d]:%s\n",i,argv[i]);
  //}
  return 0;
}
