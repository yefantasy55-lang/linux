#include <iostream>

int main(int argc,char* argv[],char* env[]){
  int i=0;
  for(i=0;env[i];i++){
    std::cout<<env[i]<<std::endl;
  }
  std::cout<<"hello world"<<std::endl;
  return 0;
}

