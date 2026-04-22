#pragma once

#include <iostream>
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include "comm.hpp"

const int gdefaultid = -1;
const int gsize = 4096;
const std::string pathname = ".";
const int projid = 0x66;
const int gmode = 0666;
#define CREATER "creater"
#define USER "user"

class Shm
{
private:
    // 创建的一定要是一个全新的共享内存
    void CreateHelper(int flg)
    {
        printf("key: 0x%x\n", _key);
        umask(0);
        _shmid = shmget(_key, _size, flg);
        if (_shmid < 0)
        {
            ERR_EXIT("shmid");
        }
        printf("shmid: %d\n", _shmid);
    }

    // 创建一个全新的共享内存
    void Create()
    {
        CreateHelper(IPC_CREAT | IPC_EXCL | gmode);
    }

    // 获取已经存在的共享内存
    void Get()
    {
        CreateHelper(IPC_CREAT);
    }

    // 映射共享内存到虚拟空间
    void Attach()
    {
        _start_mem = shmat(_shmid, nullptr, 0);
        if ((long long)_start_mem < 0)
        {
            ERR_EXIT("shmat");
        }
        printf("attach success\n");
    }

    // 解除与共享内存的映射
    void Detach()
    {
        int n = shmdt(_start_mem);
        if (n == 0)
        {
            printf("detach success\n");
        }
    }

    // 释放对应的共享内存
    void Destory()
    {
        Detach();
        if (_usertype == CREATER)
        {
            int n = shmctl(_shmid, IPC_RMID, nullptr);
            if (n > 0)
            {
                printf("shmctl delete shm: %d success!\n", _shmid);
            }
            else
            {
                ERR_EXIT("shmctl");
            }
        }
    }

public:
    Shm(const std::string &pathname, int projid, const std::string &usertype)
        : _usertype(usertype), _size(gsize)
    {
        _key = ftok(pathname.c_str(), projid);
        if (_key < 0)
        {
            ERR_EXIT("key");
        }
        if (_usertype == CREATER)
        {
            Create(); // 创建共享内存
        }
        else if (_usertype == USER)
        {
            Get(); // 获取共享内存
        }
        else
        {
            ERR_EXIT("usertype");
        }
        Attach(); // 映射到虚拟内存
    }

    void *VirtualAddr()
    {
        printf("VirtualAddr: %p\n", _start_mem);
        return _start_mem;
    }

    int Size()
    {
        return _size;
    }

    void Attr()
    {
        struct shmid_ds ds;                    // 接收共享内存信息的结构体
        int n = shmctl(_shmid, IPC_STAT, &ds); // ds:输出型参数
        printf("shm_segsz: %ld\n", ds.shm_segsz);
        printf("key: 0x%x\n", ds.shm_perm.__key);
    }

    ~Shm()
    {
        Destory();
    }

private:
    int _shmid;
    key_t _key;
    int _size;
    void *_start_mem;
    std::string _usertype;
};