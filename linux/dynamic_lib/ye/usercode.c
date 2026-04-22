#include "a.h"
#include "b.h"
#include <stdio.h>

int main(){
  int cnt = 5;
  while(cnt--){
    Print();
    printf("%d\n",fun(cnt,cnt));
  }
  return 0;
}
