#include "code.h"
#include <unistd.h>
#include <stdio.h>

double total=1024.52;
double speed=1.25;

void Download(){
  double current=0;
  while(current<total){
    FlushProcess(total,current);
    current+=speed;
    usleep(5000);
  }
  FlushProcess(total,total);
  printf("\ndownload %.2lfMB Done\n", total);
}



int main(){
  // version 1
  //process_v1();
  
  // version 2
  Download();
  Download();
  Download();
  Download();
}
