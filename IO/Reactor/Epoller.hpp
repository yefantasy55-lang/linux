#pragma once
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include "Common.hpp"
#include "Log.hpp"

class Poll
{
};

class Epoller : public Poll
{
public:
    Epoller()
    {
    }
    ~Epoller()
    {
    }

private:
    int _epfd;
};