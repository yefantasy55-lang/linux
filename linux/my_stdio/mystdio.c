#include "mystdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static MyFile* BuyNode(int fd,int flag){
   MyFile* f = (MyFile*)malloc(sizeof(MyFile));
   if(f == NULL) return NULL;
   f->flush_mode = LINE_FLUSH;  // 默认行刷新
   f->flag = flag;
   f->fileno = fd;
   f->bufferlen = 0;
   memset(f->outbuffer,0,sizeof(f->outbuffer));
   return f;
}


MyFile* MyFopen(const char *path, const char *mode){
   int fp = -1;
   int flag = 0;
   if(strcmp(mode,"r") == 0){
     // 读模式
     flag = O_RDONLY; 
     fp = open(path,flag);
   }
   else if(strcmp(mode,"w") == 0){
     // 写模式
     flag = O_CREAT | O_WRONLY | O_TRUNC;
     fp = open(path,flag,0666);
   }
   else if(strcmp(mode,"a") == 0){
     // 追加模式
     flag = O_CREAT | O_WRONLY | O_APPEND;
     fp = open(path,flag,0666);
   }
   else{
     // ...
   }
   if(fp < 0){
     return NULL;
   }
   return BuyNode(fp,flag);
}

void MyFclose(MyFile* file){
   if(file->fileno < 0) return;
   MyFFlush(file);  // 关闭前把缓冲区刷新
   close(file->fileno);
   free(file);
}

int MyFwrite(MyFile* file, void* str, int len){
   memcpy(file->outbuffer+file->bufferlen,str,len); // 把用户输入的数据拷贝到缓冲区
   file->bufferlen += len;
   // 判断是否满足刷新条件
   if((file->flag & LINE_FLUSH)==LINE_FLUSH && file->outbuffer[file->bufferlen-1]=='\n'){
      MyFFlush(file);
   }
   return 0;
}

void MyFFlush(MyFile* file){
   if(file->bufferlen <= 0) return;
   // 将用户数据刷新到系统缓冲区
   write(file->fileno,file->outbuffer,file->bufferlen);
   fsync(file->fileno);  // 刷新系统缓冲区
   file->bufferlen = 0;
}
