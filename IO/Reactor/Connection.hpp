#pragma once
#include <iostream>
#include <functional>
#include <iostream>

class Reactor;

using handler_t = std::function<std::string(std::string &)>;

class Connection
{
public:
    Connection()
        : _events(0), _owner(nullptr)
    {
    }

    ~Connection()
    {
    }

    virtual int GetSockFd() = 0;
    virtual void Recver() = 0;
    virtual void Sender() = 0;
    virtual void Excepter() = 0;
    void SetEvent(uint32_t events)
    {
        _events = events;
    }

    uint32_t GetEvent()
    {
        return _events;
    }

    void SetOwner(Reactor *owner)
    {
        _owner = owner;
    }

    Reactor *GetOwner()
    {
        return _owner;
    }

    void RegisterHandler(handler_t handler)
    {
        _handler = handler;
    }

private:
    uint32_t _events;
    Reactor *_owner;

public:
    handler_t _handler;
};