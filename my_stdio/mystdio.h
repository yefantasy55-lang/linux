#pragma once 

#include <stdio.h>

#define MAX 1024
#define NONE_FLUSH (1<<0)  // 立即刷新
#define LINE_FLUSH (1<<1)  // 行刷新
#define FULL_FLUSH (1<<2)  // 满刷新

typedef struct IO_FILE{
    int fileno;  // 文件描述符
    int flag;  // 打开方式
    char outbuffer[MAX];  // 输出缓冲区
    int bufferlen;  // 有效字符个数
    int flush_mode;  // 刷新模式
}MyFile;

MyFile *MyFopen(const char *path, const char *mode);
void MyFclose(MyFile *);
int MyFwrite(MyFile *, void *str, int len);
void MyFFlush(MyFile *);
