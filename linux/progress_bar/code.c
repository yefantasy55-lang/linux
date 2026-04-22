#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define NUM 101
#define STYLE '#'


// version 2
void FlushProcess(double total, double current){
  char buffer[NUM];
  memset(buffer,0,sizeof(buffer));
  const char *lable="|/-\\";
  int len = strlen(lable);
  int num=(int)(current*100/total);
  static int cnt = 0;  //用于计算调用的次数，从而去改变lable的样式
  int i;
  // 根据当前的内容来填充进度条
  for(i=0;i<num;i++){
    buffer[i]='#';
  }
  cnt%=len;
  printf("[%-100s][%d%%][%c]\r",buffer,num,lable[cnt]);
  fflush(stdout);
  cnt++;
}
















// version 1
//void process_v1(){
//  char buffer[NUM];
//  memset(buffer,0,sizeof(buffer));
//  const char *lable="|/-\\";
//  int len = strlen(lable);
//  int i;
//  int cnt=0;
//  for(i=0;i<=100;i++){
//    printf("[%-100s][%d%%][%c]\r",buffer,cnt,lable[i%len]);
//    fflush(stdout); // 刷新缓冲区 
//    buffer[i]=STYLE;
//    cnt++;
//    usleep(50000);
//  }
//  printf("\n");
//}
