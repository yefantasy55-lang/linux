#include "mystdio.h"
#include <string.h>
#include <unistd.h>

int main()
{
    MyFile *filep = MyFopen("./log.txt", "a");
    if(!filep)
    {
        printf("fopen error!\n");
        return 1;
    }

    int cnt = 10;
    while(cnt--)
    {
        //char *msg = (char*)"hello myfile!!!\n";  // 行刷新
        char *msg = (char*)"hello myfile!!!";
        MyFwrite(filep, msg, strlen(msg));
        //MyFFlush(filep);
        printf("buffer: %s\n", filep->outbuffer);
        sleep(1);
    }
    MyFclose(filep); // FILE *fp
    return 0;
}
